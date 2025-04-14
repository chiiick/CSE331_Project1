#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>
using namespace std;


// Returns a modulo value to control duplicate frequency
int getDuplicateMod(int size) {
    if (size <= 1000) return 100;
    else if (size <= 10000) return 500;
    else if (size <= 100000) return 2000;
    else return 10000;
}

// Generate sorted data (ascending, with duplicates)
vector<int> generateSorted(int size) {
    vector<int> data(size);
    for (int i = 0; i < size; ++i) data[i] = i % getDuplicateMod(size);
    sort(data.begin(), data.end());
    return data;
}

// Generate reverse sorted data (descending, with duplicates)
vector<int> generateReverseSorted(int size) {
    vector<int> data(size);
    for (int i = 0; i < size; ++i) data[i] = i % getDuplicateMod(size);
    sort(data.rbegin(), data.rend());
    return data;
}

// Generate completely random data (with duplicates)
vector<int> generateRandom(int size) {
    vector<int> data(size);
    for (int i = 0; i < size; ++i) data[i] = i % getDuplicateMod(size);
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(data.begin(), data.end(), default_random_engine(seed));
    return data;
}

// Generate partially sorted data (with duplicates)
vector<int> generatePartiallySorted(int size, double sortedRatio = 0.7) {
    int sortedSize = size * sortedRatio;
    vector<int> data(size);
    for (int i = 0; i < sortedSize; ++i) data[i] = i % getDuplicateMod(size);;

    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rng(seed);
    for (int i = sortedSize; i < size; ++i)
        data[i] = rng() % getDuplicateMod(size);

    return data;
}


// Utility: print first few values (for testing)
void printPreview(const vector<int>& data, int limit = 10) {
    for (int i = 0; i < min((int)data.size(), limit); ++i) {
        cout << data[i] << " ";
    }
    cout << "...\n";
}
