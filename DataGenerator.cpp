#include "DataGenerator.h"
#include <fstream>
#include <random>
#include <algorithm>

using namespace std;

void DataGenerator::generateAndSave(size_t size1, size_t size2, const string& file1, const string& file2) {
    vector<int> v1(size1);
    vector<int> v2(size2);
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 1000000);

    for (size_t i = 0; i < size1; ++i) {
        v1[i] = dist(gen);
    }
    for (size_t i = 0; i < size2; ++i) {
        v2[i] = dist(gen);
    }

    sort(v1.begin(), v1.end());
    sort(v2.begin(), v2.end());

    save(v1, file1);
    save(v2, file2);
}

vector<int> DataGenerator::loadFromFile(const string& filename) {
    vector<int> data;
    ifstream in(filename);
    int val;
    while (in >> val) {
        data.push_back(val);
    }
    return data;
}

void DataGenerator::save(const vector<int>& v, const string& filename) {
    ofstream out(filename);
    for (int val : v) {
        out << val << " ";
    }
}