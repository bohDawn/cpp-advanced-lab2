#include "DataGenerator.h"

#include <algorithm>
#include <fstream>
#include <random>
#include <stdexcept>

namespace {
constexpr int kMinValue = 1;
constexpr int kMaxValue = 100'000'000;
} 

std::vector<int> DataGenerator::makeSortedSequence(std::size_t size, unsigned seed) {
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> dist(kMinValue, kMaxValue);

    std::vector<int> values(size);
    for (int& value : values) {
        value = dist(gen);
    }
    std::sort(values.begin(), values.end());
    return values;
}

void DataGenerator::generateAndSave(std::size_t size1, std::size_t size2,
                                    const std::string& file1, const std::string& file2,
                                    unsigned seed) {
    save(makeSortedSequence(size1, seed), file1);
    save(makeSortedSequence(size2, seed + 1), file2);
}

bool DataGenerator::ensureDataset(std::size_t size1, std::size_t size2,
                                  const std::string& file1, const std::string& file2,
                                  unsigned seed, bool forceRegenerate) {
    if (!forceRegenerate && fileExists(file1) && fileExists(file2)) {
        return false;
    }
    generateAndSave(size1, size2, file1, file2, seed);
    return true;
}

std::vector<int> DataGenerator::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("Cannot open file for reading: " + filename);
    }
    std::vector<int> data;
    int value = 0;
    while (in >> value) {
        data.push_back(value);
    }
    return data;
}

void DataGenerator::save(const std::vector<int>& values, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    for (int value : values) {
        out << value << ' ';
    }
}

bool DataGenerator::fileExists(const std::string& filename) {
    return std::ifstream(filename).good();
}
