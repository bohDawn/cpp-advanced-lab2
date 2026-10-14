#include "ExperimentRunner.h"

#include "DataGenerator.h"
#include "ParallelMerge.h"
#include "Timer.h"

#include <algorithm>
#include <execution>
#include <iomanip>
#include <set>
#include <stdexcept>
#include <thread>

namespace {

// Tolerance used to decide where the speed "stops growing":
// the smallest K whose time is within 5% of the best time.
constexpr double kSaturationTolerance = 1.05;
constexpr std::size_t kLargeInputThreshold = 4'000'000;
constexpr int kRepeatsSmall = 15;
constexpr int kRepeatsLarge = 5;

struct PolicyResults {
    double plainMs = 0;
    double seqMs = 0;
    double parMs = 0;
    double parUnseqMs = 0;
    bool correct = true;
};

struct CustomRow {
    int k = 0;
    double ms = 0;
    bool correct = true;
};

struct DatasetSummary {
    std::size_t total = 0;
    PolicyResults policies;
    int bestK = 1;
    double bestMs = 0;
    int saturationK = 1;
    double saturationMs = 0;
    bool correct = true;
};

const char* yesNo(bool ok) { return ok ? "yes" : "NO"; }

// K = 1 .. 2*threads, then 3*threads and 4*threads to see the saturation.
std::vector<int> makeKValues(int threads) {
    std::set<int> values;
    for (int k = 1; k <= 2 * threads; ++k) values.insert(k);
    values.insert(3 * threads);
    values.insert(4 * threads);
    return {values.begin(), values.end()};
}

template <class Merge>
double timeAndCheck(Merge&& mergeFn, std::vector<int>& result,
                    const std::vector<int>& reference, int repeats, bool& correct) {
    std::fill(result.begin(), result.end(), 0);  // a no-op "merge" cannot pass the check
    const double ms = measureMedianMs(mergeFn, repeats);
    correct = correct && (result == reference);
    return ms;
}

PolicyResults runLibraryMerges(const std::vector<int>& v1, const std::vector<int>& v2,
                               std::vector<int>& result, std::vector<int>& reference,
                               int repeats) {
    PolicyResults r;
    std::merge(v1.begin(), v1.end(), v2.begin(), v2.end(), reference.begin());

    r.plainMs = timeAndCheck(
        [&] { std::merge(v1.begin(), v1.end(), v2.begin(), v2.end(), result.begin()); },
        result, reference, repeats, r.correct);
    r.seqMs = timeAndCheck(
        [&] {
            std::merge(std::execution::seq, v1.begin(), v1.end(), v2.begin(), v2.end(),
                       result.begin());
        },
        result, reference, repeats, r.correct);
    r.parMs = timeAndCheck(
        [&] {
            std::merge(std::execution::par, v1.begin(), v1.end(), v2.begin(), v2.end(),
                       result.begin());
        },
        result, reference, repeats, r.correct);
    r.parUnseqMs = timeAndCheck(
        [&] {
            std::merge(std::execution::par_unseq, v1.begin(), v1.end(), v2.begin(), v2.end(),
                       result.begin());
        },
        result, reference, repeats, r.correct);
    return r;
}

std::vector<CustomRow> runCustomMerge(const std::vector<int>& v1, const std::vector<int>& v2,
                                      std::vector<int>& result,
                                      const std::vector<int>& reference, int threads,
                                      int repeats) {
    std::vector<CustomRow> rows;
    for (int k : makeKValues(threads)) {
        CustomRow row;
        row.k = k;
        row.ms = timeAndCheck([&] { ParallelMerge::customParallelMerge(v1, v2, result, k); },
                              result, reference, repeats, row.correct);
        rows.push_back(row);
    }
    return rows;
}

void printPolicies(std::ostream& out, const PolicyResults& r) {
    out << "Standard merge (library):\n";
    out << std::left << std::setw(28) << "Mode" << std::setw(14) << "Time (ms)"
        << "Speedup vs plain\n";
    const auto line = [&](const char* name, double ms) {
        out << std::left << std::setw(28) << name << std::setw(14) << ms
            << std::setprecision(2) << r.plainMs / ms << std::setprecision(4) << "\n";
    };
    line("no policy", r.plainMs);
    line("execution::seq", r.seqMs);
    line("execution::par", r.parMs);
    line("execution::par_unseq", r.parUnseqMs);
    out << "Result correct: " << yesNo(r.correct) << "\n\n";
}

DatasetSummary printCustom(std::ostream& out, const std::vector<CustomRow>& rows, int threads) {
    DatasetSummary s;
    const double k1 = rows.front().ms;
    const auto best = std::min_element(rows.begin(), rows.end(),
                                       [](const CustomRow& a, const CustomRow& b) {
                                           return a.ms < b.ms;
                                       });
    s.bestK = best->k;
    s.bestMs = best->ms;
    for (const auto& row : rows) {
        if (row.ms <= best->ms * kSaturationTolerance) {
            s.saturationK = row.k;
            s.saturationMs = row.ms;
            break;
        }
    }

    out << "Custom parallel merge:\n";
    out << std::left << std::setw(8) << "K" << std::setw(14) << "Time (ms)" << std::setw(18)
        << "Speedup vs K=1" << "Correct\n";
    for (const auto& row : rows) {
        s.correct = s.correct && row.correct;
        out << std::left << std::setw(8) << row.k << std::setw(14) << row.ms << std::setw(18)
            << std::setprecision(2) << k1 / row.ms << std::setprecision(4)
            << yesNo(row.correct) << "\n";
    }
    out << "Best K: " << s.bestK << " (" << s.bestMs << " ms), K/threads = "
        << static_cast<double>(s.bestK) / threads << "\n";
    out << "Speed stops growing at K = " << s.saturationK << " (" << s.saturationMs
        << " ms, within 5% of the best), K/threads = "
        << static_cast<double>(s.saturationK) / threads << "\n\n";
    return s;
}

DatasetSummary runDataset(const DatasetSpec& spec, int threads, std::ostream& out) {
    const std::vector<int> v1 = DataGenerator::loadFromFile(spec.file1);
    const std::vector<int> v2 = DataGenerator::loadFromFile(spec.file2);
    if (v1.size() != spec.size1 || v2.size() != spec.size2 ||
        !std::is_sorted(v1.begin(), v1.end()) || !std::is_sorted(v2.begin(), v2.end())) {
        throw std::runtime_error("Input files are damaged or not sorted: " + spec.file1 +
                                 ", " + spec.file2 + " (run with --regen)");
    }

    const std::size_t total = v1.size() + v2.size();
    const int repeats = total >= kLargeInputThreshold ? kRepeatsLarge : kRepeatsSmall;
    std::vector<int> result(total);
    std::vector<int> reference(total);

    out << "============================================================\n";
    out << "Sequences: " << v1.size() << " + " << v2.size() << " elements (" << spec.file1
        << ", " << spec.file2 << "), median of " << repeats << " runs\n";
    out << "============================================================\n";

    const PolicyResults policies = runLibraryMerges(v1, v2, result, reference, repeats);
    printPolicies(out, policies);

    const auto rows = runCustomMerge(v1, v2, result, reference, threads, repeats);
    DatasetSummary summary = printCustom(out, rows, threads);
    summary.total = total;
    summary.policies = policies;
    summary.correct = summary.correct && policies.correct;
    return summary;
}

void printSummary(std::ostream& out, const std::vector<DatasetSummary>& summaries, int threads) {
    out << "============================================================\n";
    out << "SUMMARY (hardware threads: " << threads << ")\n";
    out << "============================================================\n";
    out << std::left << std::setw(12) << "Total N" << std::setw(11) << "plain" << std::setw(11)
        << "seq" << std::setw(11) << "par" << std::setw(11) << "par_unseq" << std::setw(8)
        << "bestK" << std::setw(11) << "K/thr" << std::setw(11) << "custom" << std::setw(8)
        << "satK" << std::setw(9) << "OK" << "\n";
    for (const auto& s : summaries) {
        out << std::left << std::setw(12) << s.total << std::setw(11) << s.policies.plainMs
            << std::setw(11) << s.policies.seqMs << std::setw(11) << s.policies.parMs
            << std::setw(11) << s.policies.parUnseqMs << std::setw(8) << s.bestK
            << std::setw(11) << static_cast<double>(s.bestK) / threads << std::setw(11)
            << s.bestMs << std::setw(8) << s.saturationK << std::setw(9) << yesNo(s.correct)
            << "\n";
    }
    out << "(all times in ms; plain/seq/par/par_unseq - std::merge, custom - best K)\n";
}

}  // namespace

void ExperimentRunner::runAll(const std::vector<DatasetSpec>& datasets, std::ostream& out) {
    // hardware_concurrency() may legally return 0.
    const int threads = static_cast<int>(std::max(1u, std::thread::hardware_concurrency()));

    out << std::fixed << std::setprecision(4);
    out << "Hardware threads: " << threads << "\n\n";

    std::vector<DatasetSummary> summaries;
    for (const auto& spec : datasets) {
        summaries.push_back(runDataset(spec, threads, out));
    }
    printSummary(out, summaries, threads);
}
