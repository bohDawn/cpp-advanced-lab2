#include "ParallelMerge.h"
#include <algorithm>
#include <thread>
#include <iterator>

using namespace std;

vector<int> ParallelMerge::customParallelMerge(const vector<int>& v1, const vector<int>& v2, int K) {
    if (K <= 0) K = 1;

    size_t chunkSize1 = v1.size() / K;
    size_t chunkSize2 = v2.size() / K;

    vector<vector<int>> partialResults(K);
    vector<thread> threads;

    for (int i = 0; i < K; ++i) {
        threads.emplace_back([&, i]() {
            auto start1 = v1.begin() + i * chunkSize1;
            auto end1 = (i == K - 1) ? v1.end() : start1 + chunkSize1;

            auto start2 = v2.begin() + i * chunkSize2;
            auto end2 = (i == K - 1) ? v2.end() : start2 + chunkSize2;

            partialResults[i].resize(distance(start1, end1) + distance(start2, end2));
            merge(start1, end1, start2, end2, partialResults[i].begin());
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    vector<int> finalResult = move(partialResults[0]);
    for (int i = 1; i < K; ++i) {
        vector<int> temp(finalResult.size() + partialResults[i].size());
        merge(finalResult.begin(), finalResult.end(), 
              partialResults[i].begin(), partialResults[i].end(), 
              temp.begin());
        finalResult = move(temp);
    }

    return finalResult;
}