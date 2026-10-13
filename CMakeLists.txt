cmake_minimum_required(VERSION 3.10)
project(MergeExperiment)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_PREFIX_PATH "C:/msys64/mingw64" "C:/msys64/ucrt64")

find_package(Threads REQUIRED)
find_package(TBB REQUIRED)

set(SOURCES
    main.cpp
    DataGenerator.cpp
    ParallelMerge.cpp
    ExperimentRunner.cpp
)

add_executable(MergeExperiment ${SOURCES})
target_link_libraries(MergeExperiment PRIVATE Threads::Threads TBB::tbb)