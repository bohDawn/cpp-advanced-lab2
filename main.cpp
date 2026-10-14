// Lab work: merge of two sorted sequences (variant 9 - std::merge).
//
// Compiler : GCC 15.2.0 (MSYS2 MinGW-w64), C++20, CMake, Intel TBB
// Build    : cmake -S . -B build -DOPT_LEVEL=O3 && cmake --build build
//            (Release mode, OPT_LEVEL = O0 / O1 / O2 / O3 / Ofast).
// Run      : MergeExperiment [--regen]
//            Test data is generated once into text files (fixed seed) and then
//            re-used by every run, so all optimization levels see the same data.
//            Output goes to the console and to results_<OPT_LEVEL>.txt.
// No precompiled headers are used.

#include "DataGenerator.h"
#include "ExperimentRunner.h"
#include "Tee.h"

#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifndef BUILD_TYPE_STR
#define BUILD_TYPE_STR "unknown"
#endif

#ifndef OPT_LEVEL_STR
#define OPT_LEVEL_STR "unknown"
#endif

namespace {

constexpr unsigned kBaseSeed = 20240909;
constexpr std::size_t kSizes[] = {10'000, 100'000, 1'000'000, 5'000'000};

// The second sequence is 1.5 times longer than the first one (different lengths).
std::size_t secondSize(std::size_t first) { return first + first / 2; }

std::vector<DatasetSpec> prepareDatasets(bool regenerate, std::ostream& out) {
    std::vector<DatasetSpec> specs;
    unsigned seed = kBaseSeed;
    for (std::size_t n : kSizes) {
        DatasetSpec spec{n, secondSize(n), "seq_" + std::to_string(n) + "_a.txt",
                         "seq_" + std::to_string(n) + "_b.txt"};
        if (DataGenerator::ensureDataset(spec.size1, spec.size2, spec.file1, spec.file2, seed,
                                         regenerate)) {
            out << "Generated " << spec.file1 << ", " << spec.file2 << "\n";
        }
        specs.push_back(spec);
        seed += 10;
    }
    return specs;
}

}

int main(int argc, char* argv[]) {
    const bool regenerate = argc > 1 && std::strcmp(argv[1], "--regen") == 0;

    try {
        std::ofstream file(std::string("results_") + OPT_LEVEL_STR + ".txt");
        TeeStreambuf tee(std::cout.rdbuf(), file.rdbuf());
        std::ostream out(&tee);

        out << "Build type: " << BUILD_TYPE_STR << ", optimization level: -" << OPT_LEVEL_STR << "\n";
#ifdef __VERSION__
        out << "Compiler: " << __VERSION__ << "\n";
#endif
        out << "Preparing data...\n";
        const auto datasets = prepareDatasets(regenerate, out);
        out << "Data is ready.\n\n";

        ExperimentRunner::runAll(datasets, out);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}