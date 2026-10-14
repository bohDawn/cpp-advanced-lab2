#pragma once

#include <vector>

// Custom parallel merge of two sorted sequences.
class ParallelMerge {
public:
    // Merges sorted `a` and `b` into `out` (must have size a.size() + b.size())
    // using K threads.
    
    // Algorithm: `a` is split into K nearly equal index ranges. For every
    // boundary a[i] the matching boundary in `b` is found with lower_bound,
    // so all elements of the pair of ranges (a_i, b_i) are not greater than
    // all elements of the next pair. Each thread merges its pair of ranges
    // directly into its own slice of `out` (offset = a_begin + b_begin),
    // so no final sequential merge is needed.
    static void customParallelMerge(const std::vector<int>& a, const std::vector<int>& b,
                                    std::vector<int>& out, int k);

    // Convenience overload that allocates the result.
    static std::vector<int> customParallelMerge(const std::vector<int>& a,
                                                const std::vector<int>& b, int k);
};
