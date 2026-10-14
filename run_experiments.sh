set -e
for opt in O0 O1 O2 O3; do
    cmake -S . -B build_$opt -DOPT_LEVEL=$opt -DCMAKE_BUILD_TYPE=Release
    cmake --build build_$opt
    ./build_$opt/MergeExperiment
done
