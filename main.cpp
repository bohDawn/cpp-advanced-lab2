#include "DataGenerator.h"
#include "ExperimentRunner.h"
#include <iostream>

using namespace std;

int main() {
    size_t size1 = 1000000; 
    size_t size2 = 1000000;

    string file1 = "seq1.txt";
    string file2 = "seq2.txt";

    cout << "Generating data...\n";
    DataGenerator::generateAndSave(size1, size2, file1, file2);
    cout << "Data generated and saved to files.\n\n";

    cout << "Starting experiments...\n";
    ExperimentRunner::runAllExperiments(file1, file2);

    return 0;
}