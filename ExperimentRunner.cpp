#include "ExperimentRunner.h"
#include "DataGenerator.h"
#include "ParallelMerge.h"
#include <iostream>
#include <chrono>
#include <execution>
#include <algorithm>
#include <thread>
#include <iomanip>

using namespace std;
using namespace chrono;

void ExperimentRunner::runAllExperiments(const string& file1, const string& file2) {
    vector<int> v1 = DataGenerator::loadFromFile(file1);
    vector<int> v2 = DataGenerator::loadFromFile(file2);
    vector<int> result(v1.size() + v2.size());

    cout << "--- Standard merge (No Policy) ---\n";
    auto start = high_resolution_clock::now();
    merge(v1.begin(), v1.end(), v2.begin(), v2.end(), result.begin());
    auto end = high_resolution_clock::now();
    cout << "Time: " << duration_cast<milliseconds>(end - start).count() << " ms\n\n";

    cout << "--- Standard merge (Execution Policies) ---\n";
    
    start = high_resolution_clock::now();
    merge(execution::seq, v1.begin(), v1.end(), v2.begin(), v2.end(), result.begin());
    end = high_resolution_clock::now();
    cout << "seq Time: " << duration_cast<milliseconds>(end - start).count() << " ms\n";

    start = high_resolution_clock::now();
    merge(execution::par, v1.begin(), v1.end(), v2.begin(), v2.end(), result.begin());
    end = high_resolution_clock::now();
    cout << "par Time: " << duration_cast<milliseconds>(end - start).count() << " ms\n";

    start = high_resolution_clock::now();
    merge(execution::par_unseq, v1.begin(), v1.end(), v2.begin(), v2.end(), result.begin());
    end = high_resolution_clock::now();
    cout << "par_unseq Time: " << duration_cast<milliseconds>(end - start).count() << " ms\n\n";

    cout << "--- Custom Parallel Merge ---\n";
    int maxThreads = thread::hardware_concurrency();
    cout << "Hardware threads: " << maxThreads << "\n";
    cout << left << setw(10) << "K" << "Time (ms)\n";

    long long bestTime = -1;
    int bestK = 1;

    for (int K = 1; K <= maxThreads * 2; ++K) {
        start = high_resolution_clock::now();
        vector<int> customResult = ParallelMerge::customParallelMerge(v1, v2, K);
        end = high_resolution_clock::now();
        
        long long time = duration_cast<milliseconds>(end - start).count();
        cout << left << setw(10) << K << time << "\n";

        if (bestTime == -1 || time < bestTime) {
            bestTime = time;
            bestK = K;
        }
    }

    cout << "\nBest K: " << bestK << " (" << bestTime << " ms)\n";
    cout << "Ratio (K / threads): " << (double)bestK / maxThreads << "\n";
}