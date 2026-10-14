cmake_minimum_required(VERSION 3.10)
project(MergeExperiment LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    message(FATAL_ERROR "Build type is '${CMAKE_BUILD_TYPE}', but this project must be built in Release mode. "
                        "Use -DCMAKE_BUILD_TYPE=Release (in VS Code: CMake: Select Variant -> Release).")
endif()

set(OPT_LEVEL "O3" CACHE STRING "Optimization level: O0, O1, O2, O3, Ofast")
set(CMAKE_CXX_FLAGS_RELEASE "-${OPT_LEVEL} -DNDEBUG")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(WIN32)
    set(MSYS2_ROOT "C:/msys64" CACHE PATH "MSYS2 installation root")
    list(APPEND CMAKE_PREFIX_PATH "${MSYS2_ROOT}/mingw64" "${MSYS2_ROOT}/ucrt64")
endif()

find_package(Threads REQUIRED)
find_package(TBB REQUIRED)

add_executable(MergeExperiment
    main.cpp
    DataGenerator.cpp
    ParallelMerge.cpp
    ExperimentRunner.cpp
)
target_compile_definitions(MergeExperiment PRIVATE OPT_LEVEL_STR="${OPT_LEVEL}" BUILD_TYPE_STR="${CMAKE_BUILD_TYPE}")
target_link_libraries(MergeExperiment PRIVATE Threads::Threads TBB::tbb)