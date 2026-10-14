#pragma once

#include <algorithm>
#include <chrono>
#include <vector>

// Runs `func` once as a warm-up (cold cache, thread-pool start-up), then
// `repeats` timed runs. Returns the median time in milliseconds (double).
template <class Func>
double measureMedianMs(Func&& func, int repeats) {
    using Clock = std::chrono::steady_clock;
    func();  // warm-up, not measured

    std::vector<double> samples;
    samples.reserve(static_cast<std::size_t>(repeats));
    for (int i = 0; i < repeats; ++i) {
        const auto start = Clock::now();
        func();
        const auto end = Clock::now();
        samples.push_back(std::chrono::duration<double, std::milli>(end - start).count());
    }
    std::sort(samples.begin(), samples.end());
    return samples[samples.size() / 2];
}
