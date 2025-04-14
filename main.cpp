#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <unordered_set>
#include <sys/resource.h>

#include "input_generator.h"
#include "test.h"
#include "basic_sorts.h"
#include "advanced_sorts.h"

using namespace std;

// Global CSV output stream
ofstream csvOut("results.csv");

// Wrapper functions (used instead of lambdas for stable function pointers)
void MergeSortWrapper(vector<pair<int, int>>& arr) { MergeSort(arr, 0, arr.size() - 1); }
void QuickSortWrapper(vector<pair<int, int>>& arr) { QuickSort(arr, 0, arr.size() - 1); }
void IntroSortWrapper(vector<pair<int, int>>& arr) { IntroSort(arr); }
void TimSortWrapper(vector<pair<int, int>>& arr) { TimSort(arr); }

int main() {
    srand(time(0)); // Seed RNG
    csvOut << "Algorithm,Size,InputType,Time,Memory,ActualMemoryKB,InversionBefore,InversionAfter,SortStatus,Stability,StablePercent\n";

    vector<int> sizes = {1000, 10000, 100000, 1000000};
    vector<string> inputTypes = {"Sorted", "ReverseSorted", "Random", "PartiallySorted"};

    // Algorithms considered too slow for large input sizes
    unordered_set<string> slowAlgorithms = {
        "Bubble Sort", "Insertion Sort", "Selection Sort",
        "Cocktail Shaker Sort", "Comb Sort"
    };

    // List of sorting functions to test
    vector<pair<string, function<void(vector<pair<int, int>>&)>>> sortFuncs = {
        {"Bubble Sort", BubbleSort},
        {"Insertion Sort", InsertionSort},
        {"Selection Sort", SelectionSort},
        {"Cocktail Shaker Sort", CocktailShakerSort},
        {"Comb Sort", CombSort},
        {"Merge Sort", MergeSortWrapper},
        {"Quick Sort", QuickSortWrapper},
        {"Heap Sort", HeapSort},
        {"Tournament Sort", TournamentSort},
        {"Library Sort", LibrarySort},
        {"Tim Sort", TimSortWrapper},
        {"Intro Sort", IntroSortWrapper},
    };

    // Stability test using duplicate-rich input
    const int STABILITY_TEST_SIZE = 1000;
    for (const auto& [name, func] : sortFuncs) {
        auto freshInput = generateDuplicateRichInput(STABILITY_TEST_SIZE);
        runStabilityTest(name, func, freshInput);
    }

    // Run experiments for each input type and size
    for (int size : sizes) {
        cout << "\n==============================\n";
        cout << "Running experiments on size: " << size << "\n";

        for (const auto& type : inputTypes) {
            cout << "\n-- Input Type: " << type << " --\n";

            vector<int> raw;
            if (type == "Sorted") raw = generateSorted(size);
            else if (type == "ReverseSorted") raw = generateReverseSorted(size);
            else if (type == "Random") raw = generateRandom(size);
            else if (type == "PartiallySorted") raw = generatePartiallySorted(size);

            // Convert to (value, original index) pairs
            vector<pair<int, int>> input(raw.size());
            for (int i = 0; i < raw.size(); ++i) {
                input[i] = {raw[i], i};
            }

            // Run selected experiment depending on algorithm speed and input size
            for (const auto& [name, sortFunc] : sortFuncs) {
                if (slowAlgorithms.count(name) && size > 10000) {
                    runSingleExperiment(name, sortFunc, input, type, size);
                } else {
                    runExperiment(name, sortFunc, input, type, size);
                }
            }
        }
    }

    csvOut.close();
    return 0;
}
