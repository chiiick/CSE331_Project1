#pragma once
#include <vector>
#include <algorithm>
#include <climits>
#include <cmath>

using namespace std;

// ------------------------------
// Merge Sort (Stable)
// ------------------------------
inline void MergeSort(vector<pair<int, int>>& arr, int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;

    // Recursively divide left and right halves
    MergeSort(arr, left, mid);
    MergeSort(arr, mid + 1, right);

    vector<pair<int, int>> merged;
    int i = left, j = mid + 1;

    // Merge: pick the smaller element each time
    while (i <= mid && j <= right) {
        if (arr[i].first <= arr[j].first) {
            merged.push_back(arr[i++]);
        } else {
            merged.push_back(arr[j++]);
        }
    }

    // Merge remaining elements
    while (i <= mid) merged.push_back(arr[i++]);
    while (j <= right) merged.push_back(arr[j++]);

    // Copy back to original array
    for (int k = 0; k < merged.size(); ++k) {
        arr[left + k] = merged[k];
    }
}

// ------------------------------
// Heap Sort (Unstable)
// ------------------------------
inline void Heapify(vector<pair<int, int>>& arr, int n, int i) {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    // Compare with left child
    if (l < n && arr[l].first > arr[largest].first)
        largest = l;

    // Compare with right child
    if (r < n && arr[r].first > arr[largest].first)
        largest = r;

    // Swap with larger child and heapify again
    if (largest != i) {
        swap(arr[i], arr[largest]);
        Heapify(arr, n, largest);
    }
}

inline void HeapSort(vector<pair<int, int>>& arr) {
    int n = arr.size();

    // Build max heap
    for (int i = n / 2 - 1; i >= 0; --i)
        Heapify(arr, n, i);

    // Extract root and place at end
    for (int i = n - 1; i > 0; --i) {
        swap(arr[0], arr[i]);
        Heapify(arr, i, 0);
    }
}

// ------------------------------
// Bubble Sort (Stable)
// ------------------------------
inline void BubbleSort(vector<pair<int, int>>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            // Swap if elements are out of order
            if (arr[j].first > arr[j + 1].first) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// ------------------------------
// Insertion Sort (Stable)
// ------------------------------
inline void InsertionSort(vector<pair<int, int>>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        auto key = arr[i];
        int j = i - 1;

        // Shift larger elements to the right
        while (j >= 0 && arr[j].first > key.first) {
            arr[j + 1] = arr[j];
            --j;
        }

        // Insert key at correct position
        arr[j + 1] = key;
    }
}

// ------------------------------
// Selection Sort (Unstable)
// ------------------------------
inline void SelectionSort(vector<pair<int, int>>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        int minIdx = i;

        // Find the minimum in the rest of the array
        for (int j = i + 1; j < n; ++j) {
            if (arr[j].first < arr[minIdx].first) {
                minIdx = j;
            }
        }

        // Swap with the current position (not stable)
        swap(arr[i], arr[minIdx]);
    }
}

// ------------------------------
// Quick Sort (Median-of-Three, Unstable)
// ------------------------------
inline int partition(vector<pair<int, int>>& arr, int low, int high) {
    int mid = low + (high - low) / 2;

    // Median-of-three pivot selection
    if (arr[low].first > arr[mid].first) swap(arr[low], arr[mid]);
    if (arr[low].first > arr[high].first) swap(arr[low], arr[high]);
    if (arr[mid].first > arr[high].first) swap(arr[mid], arr[high]);
    swap(arr[mid], arr[high]); // Move pivot to the end

    auto pivot = arr[high];
    int i = low - 1;

    // Move elements smaller than pivot to the left
    for (int j = low; j < high; ++j) {
        if (arr[j].first <= pivot.first) {
            ++i;
            swap(arr[i], arr[j]);
        }
    }

    // Place pivot in correct position
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

inline void QuickSort(vector<pair<int, int>>& arr, int low, int high) {
    while (low < high) {
        int pi = partition(arr, low, high);

        // Recur on smaller side first (tail recursion optimization)
        if (pi - low < high - pi) {
            QuickSort(arr, low, pi - 1);
            low = pi + 1;
        } else {
            QuickSort(arr, pi + 1, high);
            high = pi - 1;
        }
    }
}
