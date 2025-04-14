#pragma once
#include <vector>
#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <map>
#include <unordered_set>
#include <sstream>
#include <sys/resource.h>
#include <algorithm>

using namespace std;

extern ofstream csvOut; // Global CSV output stream

// Check if the array is sorted
inline bool isSorted(const vector<pair<int, int>>& arr) {
    for (size_t i = 1; i < arr.size(); ++i) {
        if (arr[i - 1].first > arr[i].first) return false;
    }
    return true;
}

// Merge step with inversion counting (used in Merge Sort)
inline long long mergeAndCount(vector<pair<int, int>>& arr, vector<pair<int, int>>& temp, int left, int mid, int right) {
    long long inv_count = 0;
    int i = left, j = mid, k = left;
    while (i < mid && j <= right) {
        if (arr[i].first <= arr[j].first) temp[k++] = arr[i++];
        else {
            temp[k++] = arr[j++];
            inv_count += mid - i; // All remaining elements in left are inversions
        }
    }
    while (i < mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    for (int l = left; l <= right; ++l) arr[l] = temp[l];
    return inv_count;
}

// Recursive inversion count utility
inline long long countInversionsUtil(vector<pair<int, int>>& arr, vector<pair<int, int>>& temp, int left, int right) {
    long long inv_count = 0;
    if (left < right) {
        int mid = (left + right) / 2;
        inv_count += countInversionsUtil(arr, temp, left, mid);
        inv_count += countInversionsUtil(arr, temp, mid + 1, right);
        inv_count += mergeAndCount(arr, temp, left, mid + 1, right);
    }
    return inv_count;
}

// Count total inversions in the array
inline long long countInversions(const vector<pair<int, int>>& original) {
    vector<pair<int, int>> arr = original;
    vector<pair<int, int>> temp(arr.size());
    return countInversionsUtil(arr, temp, 0, arr.size() - 1);
}

// Measure execution time in seconds
template <typename Func>
double measureExecutionTime(Func sortFunc, vector<pair<int, int>>& data) {
    auto start = chrono::high_resolution_clock::now();
    sortFunc(data);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double>(end - start).count();
}

// Estimate memory usage of the algorithm
inline size_t estimateMemoryUsage(const vector<pair<int, int>>& data, const string& algorithm) {
    size_t base = data.capacity() * sizeof(pair<int, int>);
    static unordered_map<string, double> overhead = {
        {"Bubble Sort", 0.0}, {"Insertion Sort", 0.0}, {"Selection Sort", 0.0},
        {"Cocktail Shaker Sort", 0.0}, {"Comb Sort", 0.0},
        {"Quick Sort", 0.01}, {"Merge Sort", 1.0}, {"Heap Sort", 0.0},
        {"Tournament Sort", 4.0}, {"Library Sort", 1.0},
        {"Tim Sort", 1.0}, {"Introsort", 0.5}
    };
    return base + static_cast<size_t>(base * overhead[algorithm]);
}

// Get peak resident set size (VmHWM) in KB
inline size_t getPeakRSS() {
    ifstream status("/proc/self/status");
    string line;
    while (getline(status, line)) {
        if (line.rfind("VmHWM:", 0) == 0) {
            istringstream iss(line);
            string key, unit;
            size_t value;
            iss >> key >> value >> unit;
            return value;
        }
    }
    return 0;
}

// Run sorting experiment (10 runs average)
inline void runExperiment(const string& name, function<void(vector<pair<int, int>>&)> sortFunc,
                          const vector<pair<int, int>>& input, const string& inputType, int size) {
    double totalTime = 0;
    const int runs = 10;
    vector<pair<int, int>> result;

    long long beforeInv = countInversions(input); // Inversions before sort
    size_t peakBefore = getPeakRSS();             // Memory before sort

    for (int i = 0; i < runs; ++i) {
        vector<pair<int, int>> copy = input;
        totalTime += measureExecutionTime(sortFunc, copy);
        if (i == 0) result = copy; // Save first result
    }

    size_t peakAfter = getPeakRSS();
    size_t peakDiff = (peakAfter > peakBefore) ? (peakAfter - peakBefore) : 0;
    double avgTime = totalTime / runs;
    size_t memEstimate = estimateMemoryUsage(input, name);
    long long afterInv = countInversions(result);
    string sortStatus = isSorted(result) ? "Sorted" : "Sort Failed";

    cout << "\n[" << name << "]\n";
    cout << "Average Time: " << avgTime << " sec\n";
    cout << "Inversion Count (before → after): " << beforeInv << " → " << afterInv << "\n";
    cout << "Actual Peak Memory Increase (VmHWM): " << peakDiff << " KB\n";

    csvOut << name << "," << size << "," << inputType << "," << avgTime << "," << memEstimate
           << "," << peakDiff << "," << beforeInv << "," << afterInv << "," << sortStatus << ",,-\n";
}

// Run a single experiment (one trial)
inline void runSingleExperiment(const string& name, function<void(vector<pair<int, int>>&)> sortFunc,
                                const vector<pair<int, int>>& input, const string& inputType, int size) {
    vector<pair<int, int>> copy = input;
    size_t peakBefore = getPeakRSS();
    long long beforeInv = countInversions(copy);
    double time = measureExecutionTime(sortFunc, copy);
    long long afterInv = countInversions(copy);
    size_t peakAfter = getPeakRSS();
    size_t peakDiff = (peakAfter > peakBefore) ? (peakAfter - peakBefore) : 0;
    size_t memEstimate = estimateMemoryUsage(input, name);
    string sortStatus = isSorted(copy) ? "Sorted" : "Sort Failed";

    cout << "\n[" << name << "]\n";
    cout << "Time: " << time << " sec\n";
    cout << "Inversion Count (before → after): " << beforeInv << " → " << afterInv << "\n";
    cout << "Actual Peak Memory Increase (VmHWM): " << peakDiff << " KB\n";

    csvOut << name << "," << size << "," << inputType << "," << time << "," << memEstimate
           << "," << peakDiff << "," << beforeInv << "," << afterInv << "," << sortStatus << ",,-\n";
}

// Structure for stability test result
struct StabilityResult {
    bool stable;
    double ratio;
};

// Check stability of sorting algorithm
inline StabilityResult checkStability(function<void(vector<pair<int, int>>&)> sortFunc, const vector<pair<int, int>>& original) {
    vector<pair<int, int>> sorted = original;
    sortFunc(sorted);

    map<int, vector<int>> before, after;
    for (const auto& p : original) before[p.first].push_back(p.second);
    for (const auto& p : sorted)   after[p.first].push_back(p.second);

    long long total = 0, preserved = 0;
    for (const auto& [val, bvec] : before) {
        const auto& avec = after[val];
        for (size_t i = 0; i < bvec.size(); ++i)
            for (size_t j = i + 1; j < bvec.size(); ++j) {
                if ((bvec[i] < bvec[j]) == (avec[i] < avec[j])) preserved++;
                total++;
            }
    }

    StabilityResult result;
    result.stable = (total == preserved);
    result.ratio = (total > 0 ? static_cast<double>(preserved) / total : 1.0);
    return result;
}

// Run stability test multiple times
inline void runStabilityTest(const string& name, function<void(vector<pair<int, int>>&)> sortFunc, const vector<int>& input, int trials = 10) {
    double totalRatio = 0.0;
    bool allStable = true;

    for (int t = 0; t < trials; ++t) {
        vector<pair<int, int>> data;
        for (size_t i = 0; i < input.size(); ++i)
            data.emplace_back(input[i], i); // Second = unique ID

        StabilityResult result = checkStability(sortFunc, data);
        totalRatio += result.ratio;
        if (!result.stable) allStable = false;
    }

    double avgRatio = totalRatio / trials;
    cout << "Stability: " << (allStable ? "Stable" : "Not Stable") << " (" << avgRatio * 100 << "%)\n";

    csvOut << name << "," << input.size() << "," << "StabilityTest" << "," 
           << "-,-,-,-,-,-," << (allStable ? "Stable" : "Not Stable") << "," << avgRatio * 100 << "\n";
}

// Generate input with many duplicates
inline vector<int> generateDuplicateRichInput(int size, int mod = 10) {
    vector<pair<int, int>> pairs;
    for (int i = 0; i < size; ++i)
        pairs.emplace_back(i % mod, i);
    random_shuffle(pairs.begin(), pairs.end());

    vector<int> result;
    for (auto& [val, _] : pairs)
        result.push_back(val);
    return result;
}
