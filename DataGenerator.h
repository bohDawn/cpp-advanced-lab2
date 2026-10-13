#pragma once

#include <string>
#include <vector>

using namespace std;

class DataGenerator {
public:
    static void generateAndSave(size_t size1, size_t size2, const string& file1, const string& file2);
    static vector<int> loadFromFile(const string& filename);

private:
    static void save(const vector<int>& v, const string& filename);
};