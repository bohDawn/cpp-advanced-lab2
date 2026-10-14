#pragma once

#include <cstddef>
#include <string>
#include <vector>

class DataGenerator {
public:
    static void generateAndSave(std::size_t size1, std::size_t size2,
                                const std::string& file1, const std::string& file2,
                                unsigned seed);

    static bool ensureDataset(std::size_t size1, std::size_t size2,
                              const std::string& file1, const std::string& file2,
                              unsigned seed, bool forceRegenerate);

    static std::vector<int> loadFromFile(const std::string& filename);

private:
    static std::vector<int> makeSortedSequence(std::size_t size, unsigned seed);
    static void save(const std::vector<int>& values, const std::string& filename);
    static bool fileExists(const std::string& filename);
};
