#pragma once
#include <vector>
#include <climits>
#include <cmath>

using namespace std;

// ------------------------------
// Library Sort (Stable, Gap-based)
// ------------------------------
inline void LibrarySort(vector<pair<int, int>>& arr) {
    const double INITIAL_GAP_RATIO = 1.5; // Initial table size is set to 1.5x the input
    const int HARD_LIMIT = 100'000'000;   // Maximum memory limit (prevents unbounded growth)

    // Function to check if a slot is empty (INT_MAX represents empty)
    auto isEmpty = [](const pair<int, int>& p) {
        return p.first == INT_MAX;
    };

    // Rebuilds the table with gaps inserted between elements
    auto rebuildWithGap = [&](const vector<pair<int, int>>& active, int newCap) {
        vector<pair<int, int>> newTable(newCap, {INT_MAX, -1});
        if (active.empty()) return newTable;

        int slots = active.size();
        int gapCount = newCap - slots;         // Total number of gaps to insert
        vector<int> gaps(slots + 1, 0);        // Gaps to insert at each position

        for (int i = 0; i < gapCount; ++i)
            gaps[i % (slots + 1)]++;           // Distribute gaps evenly

        int pos = 0;
        for (int i = 0; i < slots; ++i) {
            pos += gaps[i];                    
            newTable[pos++] = active[i];       
        }
        return newTable;
    };

    // Initialize table
    int cap = static_cast<int>(arr.size() * INITIAL_GAP_RATIO + 1);
    vector<pair<int, int>> table(cap, {INT_MAX, -1}); // Empty table

    // Insertion loop
    for (const auto& val : arr) {
        int pos = 0;

        // Find position to insert (linear search from the front)
        for (; pos < table.size(); ++pos) {
            if (isEmpty(table[pos])) break;
            if (val.first < table[pos].first) break;
            if (val.first == table[pos].first && val.second < table[pos].second) break;
        }

        // Linear probing to find an empty slot
        while (pos < table.size() && !isEmpty(table[pos])) ++pos;

        // If table is full, expand and rebuild with new gaps
        if (pos >= table.size()) {
            vector<pair<int, int>> active;
            for (auto& x : table)
                if (!isEmpty(x)) active.push_back(x);

            cap = cap * 2 + 1; // Increase capacity
            if (cap > HARD_LIMIT) return; // Prevent excessive memory use

            table = rebuildWithGap(active, cap);

            // Retry insertion after rebuild
            pos = 0;
            for (; pos < table.size(); ++pos) {
                if (isEmpty(table[pos])) break;
                if (val.first < table[pos].first) break;
                if (val.first == table[pos].first && val.second < table[pos].second) break;
            }
            while (pos < table.size() && !isEmpty(table[pos])) ++pos;
            if (pos >= table.size()) return; // Still no room, give up
        }

        table[pos] = val;
    }

    // ---------- Collect results and stably sort ----------
    vector<pair<int, int>> result;
    for (auto& p : table)
        if (!isEmpty(p)) result.push_back(p);

    // Replace STL stable sort with manual stable insertion sort
    auto stableSortByKey = [](vector<pair<int, int>>& a) {
        for (int i = 1; i < a.size(); ++i) {
            auto key = a[i];
            int j = i - 1;
            while (j >= 0 && (a[j].first > key.first ||
                         (a[j].first == key.first && a[j].second > key.second))) {
                a[j + 1] = a[j];
                --j;
            }
            a[j + 1] = key;
        }
    };
    stableSortByKey(result);

    arr = result;
}

// ------------------------------
// Tim Sort (Stable)
// ------------------------------
inline void TimSort(vector<pair<int, int>>& arr) {
    const int RUN = 32; // Minimum run size (similar to Python's Timsort)
    int n = arr.size();

    // Sort each run using insertion sort (efficient for small segments)
    auto insertionSort = [&](int left, int right) {
        for (int i = left + 1; i <= right; ++i) {
            auto key = arr[i];
            int j = i - 1;

            // Shift larger elements to the right
            while (j >= left &&
                   (arr[j].first > key.first ||
                   (arr[j].first == key.first && arr[j].second > key.second))) {
                arr[j + 1] = arr[j];
                --j;
            }
            arr[j + 1] = key;
        }
    };

    // Merge two sorted runs 
    auto merge = [&](int l, int m, int r) {
        vector<pair<int, int>> left(arr.begin() + l, arr.begin() + m + 1);
        vector<pair<int, int>> right(arr.begin() + m + 1, arr.begin() + r + 1);

        int i = 0, j = 0, k = l;

        while (i < left.size() && j < right.size()) {
            if (left[i].first < right[j].first ||
               (left[i].first == right[j].first && left[i].second <= right[j].second)) {
                arr[k++] = left[i++];
            } else {
                arr[k++] = right[j++];
            }
        }

        while (i < left.size()) arr[k++] = left[i++];
        while (j < right.size()) arr[k++] = right[j++];
    };

    // Sort small runs using insertion sort
    for (int i = 0; i < n; i += RUN) {
        insertionSort(i, min(i + RUN - 1, n - 1));
    }

    // Merge runs in increasing size: RUN, 2*RUN, 4*RUN, ...
    for (int size = RUN; size < n; size *= 2) {
        for (int left = 0; left < n; left += 2 * size) {
            int mid = min(left + size - 1, n - 1);
            int right = min(left + 2 * size - 1, n - 1);

            if (mid < right)
                merge(left, mid, right);
        }
    }
}

// ------------------------------
// Cocktail Shaker Sort (Stable)
// ------------------------------
inline void CocktailShakerSort(vector<pair<int, int>>& arr) {
    bool swapped = true;
    int start = 0, end = arr.size() - 1;

    // Bi-directional bubble sort
    while (swapped) {
        swapped = false;
        for (int i = start; i < end; ++i) {
            if (arr[i].first > arr[i + 1].first) {
                swap(arr[i], arr[i + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
        swapped = false;
        --end;
        for (int i = end - 1; i >= start; --i) {
            if (arr[i].first > arr[i + 1].first) {
                swap(arr[i], arr[i + 1]);
                swapped = true;
            }
        }
        ++start;
    }
}

// ------------------------------
// Comb Sort (Not Stable)
// ------------------------------
inline void CombSort(vector<pair<int, int>>& arr) {
    int gap = arr.size();
    const double shrink = 1.3;
    bool sorted = false;

    // Reduce gap until it reaches 1
    while (gap > 1 || !sorted) {
        gap = max(1, int(gap / shrink));
        sorted = true;
        for (int i = 0; i + gap < arr.size(); ++i) {
            if (arr[i].first > arr[i + gap].first) {
                swap(arr[i], arr[i + gap]);
                sorted = false;
            }
        }
    }
}

// ------------------------------
// Tournament Sort (Stable)
// ------------------------------
inline void TournamentSort(vector<pair<int, int>>& arr) {
    int n = arr.size();
    if (n == 0) return;

    int m = 1;
    while (m < n) m *= 2; // Expand to next power of 2

    vector<pair<int, int>> tree(2 * m, {INT_MAX, INT_MAX});
    vector<bool> used(n, true); // Track unused elements

    // Fill leaves
    for (int i = 0; i < n; ++i)
        tree[m + i] = arr[i];

    // Build internal nodes
    for (int i = m - 1; i > 0; --i) {
        auto l = tree[2 * i];
        auto r = tree[2 * i + 1];
        tree[i] = (l.first < r.first || (l.first == r.first && l.second <= r.second)) ? l : r;
    }

    vector<pair<int, int>> result;
    for (int k = 0; k < n; ++k) {
        pair<int, int> winner = tree[1];
        result.push_back(winner);

        // Find index of winner in original array
        int idx = -1;
        for (int i = 0; i < n; ++i) {
            if (used[i] && arr[i] == winner) {
                idx = i;
                break;
            }
        }

        used[idx] = false;
        tree[m + idx] = {INT_MAX, INT_MAX};

        // Update tree
        for (int i = (m + idx) / 2; i > 0; i /= 2) {
            auto l = tree[2 * i];
            auto r = tree[2 * i + 1];
            tree[i] = (l.first < r.first || (l.first == r.first && l.second <= r.second)) ? l : r;
        }
    }

    arr = result;
}

// ------------------------------
// Introsort (Unstable)
// ------------------------------
inline void IntroSort(vector<pair<int, int>>& arr) {
    int depthLimit = 2 * log(arr.size());
    function<void(int, int, int)> introsortUtil;
    function<void(int, int)> heapSortIntro;
    function<int(int, int)> partition;

    // QuickSort partition
    partition = [&](int low, int high) {
        auto pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; ++j)
            if (arr[j].first <= pivot.first)
                swap(arr[++i], arr[j]);
        swap(arr[i + 1], arr[high]);
        return i + 1;
    };

    // HeapSort
    heapSortIntro = [&](int begin, int end) {
        function<void(int, int)> heapify = [&](int n, int i) {
            int largest = i, l = 2 * i + 1, r = 2 * i + 2;
            if (l < n && arr[begin + l].first > arr[begin + largest].first) largest = l;
            if (r < n && arr[begin + r].first > arr[begin + largest].first) largest = r;
            if (largest != i) {
                swap(arr[begin + i], arr[begin + largest]);
                heapify(n, largest);
            }
        };
        int n = end - begin + 1;
        for (int i = n / 2 - 1; i >= 0; --i) heapify(n, i);
        for (int i = n - 1; i > 0; --i) {
            swap(arr[begin], arr[begin + i]);
            heapify(i, 0);
        }
    };

    // Recursive Introsort
    introsortUtil = [&](int begin, int end, int depthLimit) {
        if (begin < end) {
            if (depthLimit == 0) {
                heapSortIntro(begin, end);
                return;
            }
            int pi = partition(begin, end);
            introsortUtil(begin, pi - 1, depthLimit - 1);
            introsortUtil(pi + 1, end, depthLimit - 1);
        }
    };

    introsortUtil(0, arr.size() - 1, depthLimit);
}
