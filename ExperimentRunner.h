#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

// One pair of input sequences stored in text files.
struct DatasetSpec {
    std::size_t size1;
    std::size_t size2;
    std::string file1;
    std::string file2;
};

class ExperimentRunner {
public:
    // Runs all experiments for every dataset and prints tables to `out`.
    static void runAll(const std::vector<DatasetSpec>& datasets, std::ostream& out);
};
