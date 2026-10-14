#include "ParallelMerge.h"

#include <algorithm>
#include <cstddef>
#include <thread>

void ParallelMerge::customParallelMerge(const std::vector<int>& a, const std::vector<int>& b,
                                        std::vector<int>& out, int k) {
    const std::size_t parts = static_cast<std::size_t>(std::max(k, 1));
    out.resize(a.size() + b.size());

    // Boundaries of the parts in `a` and the corresponding ones in `b`.
    std::vector<std::size_t> aBound(parts + 1);
    std::vector<std::size_t> bBound(parts + 1);
    for (std::size_t i = 0; i <= parts; ++i) {
        aBound[i] = a.size() * i / parts;
        if (i == 0) {
            bBound[i] = 0;
        } else if (i == parts || aBound[i] >= a.size()) {
            bBound[i] = b.size();
        } else {
            bBound[i] = static_cast<std::size_t>(
                std::lower_bound(b.begin(), b.end(), a[aBound[i]]) - b.begin());
        }
    }

    std::vector<std::thread> threads;
    threads.reserve(parts);
    for (std::size_t i = 0; i < parts; ++i) {
        threads.emplace_back([&, i]() {
            std::merge(a.begin() + static_cast<std::ptrdiff_t>(aBound[i]),
                       a.begin() + static_cast<std::ptrdiff_t>(aBound[i + 1]),
                       b.begin() + static_cast<std::ptrdiff_t>(bBound[i]),
                       b.begin() + static_cast<std::ptrdiff_t>(bBound[i + 1]),
                       out.begin() + static_cast<std::ptrdiff_t>(aBound[i] + bBound[i]));
        });
    }
    for (auto& t : threads) {
        t.join();
    }
}

std::vector<int> ParallelMerge::customParallelMerge(const std::vector<int>& a,
                                                    const std::vector<int>& b, int k) {
    std::vector<int> result;
    customParallelMerge(a, b, result, k);
    return result;
}
