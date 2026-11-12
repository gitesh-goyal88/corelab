// Data Structures (Semester 3), part 2: Module 2 sorting, median search, complexity.
#pragma once
#include "ds1.hpp"

using namespace std;

static string dsJoinInts(const Ints& arr, size_t startIndex = 0) {
    string result;
    for (size_t i = startIndex; i < arr.size(); ++i) {
        if (!result.empty()) result += ", ";
        result += S(arr[i]);
    }
    return result.empty() ? "(none)" : result;
}

static const vector<string>& dsSortRefs() {
    static const vector<string> refs = {
        "VisuAlgo: sorting animations|https://visualgo.net/en/sorting",
        "GeeksforGeeks: Sorting algorithms|https://www.geeksforgeeks.org/sorting-algorithms/",
        "cppreference: std::sort|https://en.cppreference.com/w/cpp/algorithm/sort"};
    return refs;
}

static void pushWithCallStack(Steps& steps, const vector<string>& callStack, Step step) {
    step.stack = callStack;
    steps.push_back(step);
}

// ---------- Merge Sort ----------

static void mergeSortRecursive(Ints& arr, int lo, int hi, int totalSize, Steps& steps, vector<string>& callStack) {
    callStack.push_back("mergeSort(" + S(lo) + ", " + S(hi) + ")");
    string enterMsg = (lo >= hi)
        ? "Only one element (index " + S(lo) + "): already sorted. Return."
        : "mergeSort(" + S(lo) + ", " + S(hi) + "): " + S(hi - lo + 1) + " elements, so split them.";
    pushWithCallStack(steps, callStack, dsArrayStep(1, enterMsg, {{"lo", S(lo)}, {"hi", S(hi)}}, arr, -1,
                                                    dsMakeTags(totalSize, {{lo, "lo"}, {hi, "hi"}}), lo, hi));
    if (lo < hi) {
        int mid = (lo + hi) / 2;
        pushWithCallStack(steps, callStack,
                          dsArrayStep(2, "mid = (" + S(lo) + " + " + S(hi) + ") / 2 = " + S(mid) + ". Left half " + S(lo) + ".." + S(mid) + ", right half " + S(mid + 1) + ".." + S(hi) + ".",
                                      {{"mid", S(mid)}}, arr, mid,
                                      dsMakeTags(totalSize, {{lo, "lo"}, {mid, "mid"}, {hi, "hi"}}), lo, hi));
        mergeSortRecursive(arr, lo, mid, totalSize, steps, callStack);
        mergeSortRecursive(arr, mid + 1, hi, totalSize, steps, callStack);

        Ints leftHalf(arr.begin() + lo, arr.begin() + mid + 1);
        Ints rightHalf(arr.begin() + mid + 1, arr.begin() + hi + 1);
        size_t i = 0;
        size_t j = 0;
        int writeIdx = lo;
        pushWithCallStack(steps, callStack,
                          dsArrayStep(4, "merge: left half [" + dsJoinInts(leftHalf) + "] and right half [" + dsJoinInts(rightHalf) + "] are both sorted. Repeatedly take the smaller front item.",
                                      {{"L", dsJoinInts(leftHalf)}, {"R", dsJoinInts(rightHalf)}}, arr, -1,
                                      dsMakeTags(totalSize, {{lo, "lo"}, {hi, "hi"}}), lo, hi));
        while (i < leftHalf.size() || j < rightHalf.size()) {
            bool takeLeft = (j >= rightHalf.size()) || (i < leftHalf.size() && leftHalf[i] <= rightHalf[j]);
            string reason;
            if (i >= leftHalf.size()) {
                reason = "Left half is used up: copy " + S(rightHalf[j]) + " from the right.";
            } else if (j >= rightHalf.size()) {
                reason = "Right half is used up: copy " + S(leftHalf[i]) + " from the left.";
            } else if (takeLeft) {
                reason = "Compare " + S(leftHalf[i]) + " (left) with " + S(rightHalf[j]) + " (right): " + S(leftHalf[i]) + " <= " + S(rightHalf[j]) + ", take the left.";
            } else {
                reason = "Compare " + S(leftHalf[i]) + " (left) with " + S(rightHalf[j]) + " (right): " + S(leftHalf[i]) + " > " + S(rightHalf[j]) + ", take the right.";
            }
            arr[writeIdx] = takeLeft ? leftHalf[i++] : rightHalf[j++];
            pushWithCallStack(steps, callStack,
                              dsArrayStep(4, reason + " a[" + S(writeIdx) + "] = " + S(arr[writeIdx]) + ".",
                                          {{"L left", dsJoinInts(leftHalf, i)}, {"R left", dsJoinInts(rightHalf, j)}},
                                          arr, writeIdx, dsMakeTags(totalSize, {{writeIdx, "k"}}), lo, hi));
            ++writeIdx;
        }
    }
    callStack.pop_back();
}

static Steps simMergeSort(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 8, {38, 27, 43, 3, 9, 82, 10, 5});
    int totalSize = (int)arr.size();
    Steps steps;
    vector<string> callStack;
    steps.push_back(dsArrayStep(0, "Divide and conquer: split in halves until one element is left, then merge sorted halves. Time O(n log n) always, extra space O(n).",
                                {{"n", S(totalSize)}}, arr, -1));
    mergeSortRecursive(arr, 0, totalSize - 1, totalSize, steps, callStack);
    steps.push_back(dsArrayStep(5, "Sorted. Every level of the recursion does O(n) merging work and there are about log2(n) levels: O(n log n).",
                                {{"sorted", dsJoinInts(arr)}}, arr, -1));
    return steps;
}

// ---------- Quick Sort ----------

static vector<string> makeQuickSortTags(int totalSize, const vector<int>& fixedIndices,
                                        vector<pair<int, string>> extraTags) {
    for (size_t i = 0; i < fixedIndices.size(); ++i) {
        extraTags.push_back({fixedIndices[i], "ok"});
    }
    return dsMakeTags(totalSize, extraTags);
}

static void quickSortRecursive(Ints& arr, int lo, int hi, int totalSize, Steps& steps,
                               vector<string>& callStack, vector<int>& fixedIndices) {
    callStack.push_back("quickSort(" + S(lo) + ", " + S(hi) + ")");
    if (lo > hi) {
        callStack.pop_back();
        return;
    }
    if (lo == hi) {
        fixedIndices.push_back(lo);
        pushWithCallStack(steps, callStack,
                          dsArrayStep(0, "One element (index " + S(lo) + ") is always in its final place.",
                                      {{"lo", S(lo)}}, arr, lo, makeQuickSortTags(totalSize, fixedIndices, {}), lo, hi));
        callStack.pop_back();
        return;
    }
    int pivot = arr[hi];
    int smallEnd = lo - 1;
    pushWithCallStack(steps, callStack,
                      dsArrayStep(1, "Pivot = a[" + S(hi) + "] = " + S(pivot) + ". i = " + S(smallEnd) + " marks the end of the 'smaller or equal' zone.",
                                  {{"pivot", S(pivot)}, {"i", S(smallEnd)}}, arr, hi,
                                  makeQuickSortTags(totalSize, fixedIndices, {{hi, "P"}}), lo, hi));
    for (int j = lo; j < hi; ++j) {
        int val = arr[j];
        bool isSmallOrEqual = (val <= pivot);
        string msg;
        if (isSmallOrEqual) {
            ++smallEnd;
            swap(arr[smallEnd], arr[j]);
            if (smallEnd == j) {
                msg = "a[" + S(j) + "] = " + S(val) + " <= " + S(pivot) + ": already in the small zone, i = " + S(smallEnd) + ".";
            } else {
                msg = "a[" + S(j) + "] = " + S(val) + " <= " + S(pivot) + ": swap it into the small zone, i = " + S(smallEnd) + ".";
            }
        } else {
            msg = "a[" + S(j) + "] = " + S(val) + " > " + S(pivot) + ": leave it in the large zone.";
        }
        pushWithCallStack(steps, callStack,
                          dsArrayStep(3, msg, {{"i", S(smallEnd)}, {"j", S(j)}}, arr, j,
                                      makeQuickSortTags(totalSize, fixedIndices, {{smallEnd >= lo ? smallEnd : -1, "i"}, {j, "j"}, {hi, "P"}}),
                                      lo, hi));
    }
    swap(arr[smallEnd + 1], arr[hi]);
    int pivotPos = smallEnd + 1;
    fixedIndices.push_back(pivotPos);
    pushWithCallStack(steps, callStack,
                      dsArrayStep(4, "Swap the pivot into index " + S(pivotPos) + ". Everything left of it is <= " + S(pivot) + ", everything right is larger. The pivot is now in its FINAL place.",
                                  {{"p", S(pivotPos)}}, arr, pivotPos, makeQuickSortTags(totalSize, fixedIndices, {}), lo, hi));
    quickSortRecursive(arr, lo, pivotPos - 1, totalSize, steps, callStack, fixedIndices);
    quickSortRecursive(arr, pivotPos + 1, hi, totalSize, steps, callStack, fixedIndices);
    callStack.pop_back();
}

static Steps simQuickSort(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 8, {10, 80, 30, 90, 40, 50, 70});
    int totalSize = (int)arr.size();
    Steps steps;
    vector<string> callStack;
    vector<int> fixedIndices;
    steps.push_back(dsArrayStep(0, "Pick a pivot, put smaller items on its left and larger on its right, then sort each side the same way. Average O(n log n), worst case O(n^2).",
                                {{"n", S(totalSize)}}, arr, -1));
    quickSortRecursive(arr, 0, totalSize - 1, totalSize, steps, callStack, fixedIndices);
    steps.push_back(dsArrayStep(5, "Sorted. Already-sorted input makes the pivot the largest every time (n levels): the O(n^2) worst case.",
                                {{"sorted", dsJoinInts(arr)}}, arr, -1, makeQuickSortTags(totalSize, fixedIndices, {})));
    return steps;
}

// ---------- Radix Sort ----------

static Steps simRadixSort(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 8, {170, 45, 75, 90, 802, 24, 2, 66});
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] = (int)(abs((long long)arr[i]) % 1000);
    }
    int count = (int)arr.size();
    int maxVal = *max_element(arr.begin(), arr.end());
    Steps steps;
    steps.push_back(dsArrayStep(0, "Radix sort never compares two numbers. It sorts by the ones digit, then tens, then hundreds. Each pass must be STABLE.",
                                {{"max", S(maxVal)}}, arr, -1));

    int divisor = 1;
    for (int pass = 0; divisor <= max(maxVal, 1) && pass < 3; divisor *= 10, ++pass) {
        vector<Ints> buckets(10);
        const char* passName = (pass == 0) ? "ones" : (pass == 1 ? "tens" : "hundreds");
        for (int i = 0; i < count; ++i) {
            int digit = (arr[i] / divisor) % 10;
            buckets[digit].push_back(arr[i]);
            Vars vars;
            for (int d = 0; d < 10; ++d) {
                if (!buckets[d].empty()) {
                    vars.push_back({"bucket " + S(d), dsJoinInts(buckets[d])});
                }
            }
            steps.push_back(dsArrayStep(1, "Pass on the " + string(passName) + " digit: " + S(arr[i]) + " has digit " + S(digit) + ", so it goes to bucket " + S(digit) + ".",
                                        vars, arr, i, dsMakeTags(count, {{i, "d=" + S(digit)}})));
        }
        Ints collected;
        for (int d = 0; d < 10; ++d) {
            for (size_t k = 0; k < buckets[d].size(); ++k) {
                collected.push_back(buckets[d][k]);
            }
        }
        arr = collected;
        steps.push_back(dsArrayStep(2, "Read the buckets in order 0 to 9. Order after the " + string(passName) + " pass: " + dsJoinInts(arr) + ".",
                                    {{"after pass", dsJoinInts(arr)}}, arr, -1));
    }
    steps.push_back(dsArrayStep(3, "No more digits. Sorted. Time O(d * (n + 10)) for d digits: linear in n when d is small.",
                                {{"sorted", dsJoinInts(arr)}}, arr, -1));
    return steps;
}

// ---------- Bucket Sort ----------

static Vars buildBucketVars(const vector<Ints>& buckets) {
    Vars vars;
    for (int b = 0; b < 5; ++b) {
        vars.push_back({"bucket " + S(b) + " (" + S(b * 20) + "-" + S(b * 20 + 19) + ")", dsJoinInts(buckets[b])});
    }
    return vars;
}

static Steps simBucketSort(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 9, {29, 25, 3, 49, 9, 37, 21, 43, 4});
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] = (int)(abs((long long)arr[i]) % 100);
    }
    int count = (int)arr.size();
    Steps steps;
    vector<Ints> buckets(5);

    steps.push_back(dsArrayStep(0, "Spread the numbers into 5 buckets by range (0-19, 20-39, 40-59, 60-79, 80-99), sort each small bucket, then join. Great when data is evenly spread.",
                                {{"buckets", "5"}}, arr, -1));
    for (int i = 0; i < count; ++i) {
        int bIdx = arr[i] / 20;
        buckets[bIdx].push_back(arr[i]);
        steps.push_back(dsArrayStep(0, S(arr[i]) + " / 20 = " + S(bIdx) + ", so it goes to bucket " + S(bIdx) + ".",
                                    buildBucketVars(buckets), arr, i, dsMakeTags(count, {{i, "b" + S(bIdx)}})));
    }
    for (int b = 0; b < 5; ++b) {
        if (buckets[b].size() > 1) {
            sort(buckets[b].begin(), buckets[b].end());
            steps.push_back(dsArrayStep(1, "Insertion-sort bucket " + S(b) + ": it now reads [" + dsJoinInts(buckets[b]) + "]. Buckets are tiny, so this is cheap.",
                                        buildBucketVars(buckets), arr, -1));
        }
    }
    Ints sorted;
    for (int b = 0; b < 5; ++b) {
        for (size_t k = 0; k < buckets[b].size(); ++k) {
            sorted.push_back(buckets[b][k]);
        }
    }
    steps.push_back(dsArrayStep(2, "Join the buckets in order. Sorted. Average O(n + k) when data is evenly spread; worst case O(n^2) if everything lands in one bucket.",
                                {{"sorted", dsJoinInts(sorted)}}, sorted, -1));
    return steps;
}

// ---------- Counting Sort ----------

static Step makeCountingStep(int lineNum, const string& noteText, Vars varsList, const Ints& arr,
                             const Ints& counts, int highlightCell, vector<string> tags) {
    Step step = dsArrayStep(lineNum, noteText, varsList, arr, -1, tags);
    vector<string> matrix;
    for (int q = 0; q < 8; ++q) matrix.push_back(S(q));
    for (int q = 0; q < 8; ++q) matrix.push_back(S(counts[q]));
    step.mat = matrix;
    step.mc = 8;
    step.hi = highlightCell;
    return step;
}

static Steps simCountingSort(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 10, {4, 2, 2, 8, 3, 3, 1});
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] = (int)(abs((long long)arr[i]) % 8);
    }
    int count = (int)arr.size();
    Steps steps;
    Ints counts(8, 0);

    steps.push_back(makeCountingStep(0, "Values are small whole numbers 0 to 7, so we can simply COUNT each value. Top row of the table = value, bottom row = count.",
                                     {{"n", S(count)}}, arr, counts, -1, {}));
    for (int i = 0; i < count; ++i) {
        ++counts[arr[i]];
        steps.push_back(makeCountingStep(0, "Read " + S(arr[i]) + ": count[" + S(arr[i]) + "] becomes " + S(counts[arr[i]]) + ".",
                                         {{"x", S(arr[i])}}, arr, counts, 8 + arr[i], dsMakeTags(count, {{i, "x"}})));
    }
    for (int val = 1; val < 8; ++val) {
        counts[val] += counts[val - 1];
        steps.push_back(makeCountingStep(1, "Prefix sum: count[" + S(val) + "] += count[" + S(val - 1) + "] gives " + S(counts[val]) +
                                         ". It is now the number of items <= " + S(val) + ", i.e. one past the LAST slot for value " + S(val) + ".",
                                         {{"v", S(val)}}, arr, counts, 8 + val, {}));
    }
    Ints output(count, INT_MIN);
    for (int i = count - 1; i >= 0; --i) {
        int val = arr[i];
        int outPos = --counts[val];
        output[outPos] = arr[i];
        steps.push_back(makeCountingStep(3, "Take a[" + S(i) + "] = " + S(val) + " (going right to left keeps equal values in their original order). count[" + S(val) + "] drops to " + S(counts[val]) + ", so it goes to out[" + S(outPos) + "].",
                                         {{"x", S(val)}, {"pos", S(outPos)}}, output, counts, 8 + val,
                                         dsMakeTags(count, {{outPos, "<-" + S(val)}})));
    }
    steps.push_back(makeCountingStep(4, "Sorted in O(n + K) time with O(K) extra counts. No comparisons were made.",
                                     {{"sorted", dsJoinInts(output)}}, output, counts, -1, {}));
    return steps;
}

// ---------- Median Search ----------

static Steps simQuickSelect(const Ints& input) {
    Ints arr = dsTakeInts(input, 1, 9, {7, 10, 4, 3, 20, 15, 9, 1});
    int count = (int)arr.size();
    int kOneBased = clamp(input.empty() ? (count + 1) / 2 : input[0], 1, count);
    int targetIdx = kOneBased - 1;
    int lo = 0;
    int hi = count - 1;
    Steps steps;

    steps.push_back(dsArrayStep(0, "Find the " + S(kOneBased) + (kOneBased == (count + 1) / 2 ? "th smallest, which is the MEDIAN" : "th smallest") +
                                " WITHOUT sorting everything. Partition once, then keep only the side that contains index " + S(targetIdx) + ".",
                                {{"k (0-based)", S(targetIdx)}, {"n", S(count)}}, arr, -1));
    while (lo <= hi) {
        int pivot = arr[hi];
        int smallEnd = lo - 1;
        for (int j = lo; j < hi; ++j) {
            if (arr[j] <= pivot) {
                ++smallEnd;
                swap(arr[smallEnd], arr[j]);
            }
        }
        swap(arr[smallEnd + 1], arr[hi]);
        int pivotPos = smallEnd + 1;
        steps.push_back(dsArrayStep(1, "Partition " + S(lo) + ".." + S(hi) + " around pivot " + S(pivot) + ". It lands at index " + S(pivotPos) + ": " + S(pivotPos - lo) + " smaller-or-equal item(s) on its left.",
                                    {{"pivot", S(pivot)}, {"p", S(pivotPos)}}, arr, pivotPos, dsMakeTags(count, {{pivotPos, "P"}}), lo, hi));
        if (pivotPos == targetIdx) {
            steps.push_back(dsArrayStep(2, "p == k: a[" + S(pivotPos) + "] = " + S(arr[pivotPos]) + " is the answer. Only part of the array was ever touched.",
                                        {{"answer", S(arr[pivotPos])}}, arr, pivotPos, dsMakeTags(count, {{pivotPos, "answer"}}), lo, hi));
            return steps;
        }
        if (targetIdx < pivotPos) {
            steps.push_back(dsArrayStep(3, "k = " + S(targetIdx) + " < p = " + S(pivotPos) + ": the answer is on the LEFT. Throw away the right side.",
                                        {{"hi", S(pivotPos - 1)}}, arr, -1, dsMakeTags(count, {{pivotPos, "P"}}), lo, pivotPos - 1));
            hi = pivotPos - 1;
        } else {
            steps.push_back(dsArrayStep(4, "k = " + S(targetIdx) + " > p = " + S(pivotPos) + ": the answer is on the RIGHT. Throw away the left side.",
                                        {{"lo", S(pivotPos + 1)}}, arr, -1, dsMakeTags(count, {{pivotPos, "P"}}), pivotPos + 1, hi));
            lo = pivotPos + 1;
        }
    }
    return steps;
}

static Steps simMedianOfMedians(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 10, {7, 10, 4, 3, 20, 15, 9, 1, 12, 6});
    int count = (int)arr.size();
    Steps steps;
    Ints medians;

    steps.push_back(dsArrayStep(0, "Median of medians finds a GOOD pivot in linear time, so quickselect never degrades to O(n^2). First cut the array into groups of 5.",
                                {{"n", S(count)}}, arr, -1));
    for (int g = 0; g * 5 < count; ++g) {
        int lo = g * 5;
        int hi = min(count, lo + 5) - 1;
        sort(arr.begin() + lo, arr.begin() + hi + 1);
        int midIdx = lo + (hi - lo) / 2;
        medians.push_back(arr[midIdx]);
        steps.push_back(dsArrayStep(1, "Group " + S(g + 1) + " (indices " + S(lo) + ".." + S(hi) + ") sorted. Its median is " + S(arr[midIdx]) + ".",
                                    {{"medians so far", dsJoinInts(medians)}}, arr, midIdx, dsMakeTags(count, {{midIdx, "med"}}), lo, hi));
    }
    Ints sortedMedians = medians;
    sort(sortedMedians.begin(), sortedMedians.end());
    int pivot = sortedMedians[(sortedMedians.size() - 1) / 2];
    steps.push_back(dsArrayStep(2, "The medians are [" + dsJoinInts(medians) + "]. Their median is " + S(pivot) + ": this is the pivot.",
                                {{"pivot", S(pivot)}}, arr, -1));
    int smallerCount = 0;
    int equalCount = 0;
    for (size_t i = 0; i < arr.size(); ++i) {
        if (arr[i] < pivot) ++smallerCount;
        else if (arr[i] == pivot) ++equalCount;
    }
    int largerCount = count - smallerCount - equalCount;
    steps.push_back(dsArrayStep(3, "Partition around " + S(pivot) + ": " + S(smallerCount) + " item(s) are smaller, " + S(equalCount) + " equal, " + S(largerCount) + " larger. Half the groups have their median <= pivot, so at least about 30% of the items are on each side: the recursion shrinks fast. Total O(n).",
                                {{"smaller", S(smallerCount)}, {"larger", S(largerCount)}}, arr, -1));
    return steps;
}

// ---------- Time and Space Complexity ----------

static Step attachComplexityTable(Step step) {
    step.mat = {"n =", "4", "8", "16", "32",
                "O(log n)", "2", "3", "4", "5",
                "O(n)", "4", "8", "16", "32",
                "O(n log n)", "8", "24", "64", "160",
                "O(n^2)", "16", "64", "256", "1024"};
    step.mc = 5;
    return step;
}

static Steps simTimeLinear(const Ints& input) {
    int n = clamp(input.empty() ? 6 : input[0], 1, 12);
    Steps steps;
    int ops = 0;
    steps.push_back(mk(0, "Count how many times the basic step runs as n grows. n = " + S(n) + ".", {{"n", S(n)}, {"ops", "0"}}));
    for (int i = 0; i < n; ++i) {
        ++ops;
        steps.push_back(mk(2, "Iteration i = " + S(i) + ": ops++ gives " + S(ops) + ".", {{"i", S(i)}, {"ops", S(ops)}}));
    }
    steps.push_back(attachComplexityTable(mk(2, "ops = n = " + S(n) + ". Doubling n doubles the work: O(n), linear. Table: how the work grows for n = 4, 8, 16, 32.", {{"ops", S(ops)}})));
    return steps;
}

static Steps simTimeQuadratic(const Ints& input) {
    int n = clamp(input.empty() ? 4 : input[0], 1, 6);
    Steps steps;
    int ops = 0;
    steps.push_back(mk(0, "Two nested loops. n = " + S(n) + ".", {{"n", S(n)}, {"ops", "0"}}));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            ++ops;
            steps.push_back(mk(3, "i = " + S(i) + ", j = " + S(j) + ": ops = " + S(ops) + ".", {{"i", S(i)}, {"j", S(j)}, {"ops", S(ops)}}));
        }
    }
    steps.push_back(attachComplexityTable(mk(3, "ops = n * n = " + S(ops) + ". Doubling n makes the work 4 times bigger: O(n^2), quadratic.", {{"ops", S(ops)}})));
    return steps;
}

static Steps simTimeLogarithmic(const Ints& input) {
    int n = clamp(input.empty() ? 64 : input[0], 1, 4096);
    int startN = n;
    Steps steps;
    int ops = 0;
    steps.push_back(mk(0, "The loop halves n every time. n = " + S(n) + ".", {{"n", S(n)}, {"ops", "0"}}));
    while (n > 1) {
        steps.push_back(mk(1, "n = " + S(n) + " > 1, so continue.", {{"n", S(n)}, {"ops", S(ops)}}));
        n /= 2;
        ++ops;
        steps.push_back(mk(3, "n = n / 2 = " + S(n) + ", ops = " + S(ops) + ".", {{"n", S(n)}, {"ops", S(ops)}}));
    }
    steps.push_back(attachComplexityTable(mk(4, "ops = " + S(ops) + " for n = " + S(startN) + ". Squaring n only adds one more step: O(log n). This is why binary search is so fast.", {{"ops", S(ops)}})));
    return steps;
}

static Steps simSpaceConstant(const Ints& input) {
    int n = clamp(input.empty() ? 5 : input[0], 1, 8);
    int sum = 0;
    Steps steps;
    steps.push_back(mk(0, "Memory used = 3 variables (n, i, sum), however large n is.", {{"n", S(n)}, {"sum", "0"}, {"extra memory", "3 ints"}}));
    for (int i = 1; i <= n; ++i) {
        sum += i;
        steps.push_back(mk(2, "sum += " + S(i) + " gives " + S(sum) + ". Still the same 3 variables.", {{"i", S(i)}, {"sum", S(sum)}, {"extra memory", "3 ints"}}));
    }
    steps.push_back(mk(3, "Returns " + S(sum) + ". Extra space O(1): constant, does not grow with n.", {{"extra memory", "3 ints = O(1)"}}));
    return steps;
}

static int traceRecursiveSum(int k, Steps& steps, vector<string>& callStack) {
    callStack.push_back("sum(" + S(k) + ")");
    pushWithCallStack(steps, callStack, mk(0, "Call sum(" + S(k) + "). Each pending call keeps its own frame in memory.", {{"stack depth", S((int)callStack.size())}}));
    int result = 0;
    if (k == 0) {
        pushWithCallStack(steps, callStack, mk(1, "n == 0: return 0. The stack is at its deepest: " + S((int)callStack.size()) + " frames.", {{"stack depth", S((int)callStack.size())}}));
        result = 0;
    } else {
        result = k + traceRecursiveSum(k - 1, steps, callStack);
        pushWithCallStack(steps, callStack, mk(2, "Back in sum(" + S(k) + "): return " + S(k) + " + the result = " + S(result) + ". This frame is freed.",
                                              {{"result", S(result)}, {"stack depth", S((int)callStack.size())}}));
    }
    callStack.pop_back();
    return result;
}

static Steps simSpaceRecursion(const Ints& input) {
    int n = clamp(input.empty() ? 5 : input[0], 1, 8);
    Steps steps;
    vector<string> callStack;
    int result = traceRecursiveSum(n, steps, callStack);
    steps.push_back(mk(3, "sum(" + S(n) + ") = " + S(result) + ". The stack held n + 1 frames at once: extra space O(n), even though no array was created.",
                       {{"max depth", S(n + 1)}}));
    return steps;
}

static Steps simSpaceArrayCopy(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 8, {3, 1, 4, 1, 5});
    int count = (int)arr.size();
    Ints copyArr(count, INT_MIN);
    Steps steps;

    steps.push_back(dsArrayStep(0, "A new array b with n = " + S(count) + " cells is created next to a. That extra memory depends on n.",
                                {{"n", S(count)}, {"extra memory", S(count) + " ints"}}, copyArr, -1, {}, -1, -1, "b"));
    for (int i = 0; i < count; ++i) {
        copyArr[i] = arr[i];
        steps.push_back(dsArrayStep(2, "b[" + S(i) + "] = a[" + S(i) + "] = " + S(arr[i]) + ".",
                                    {{"i", S(i)}, {"extra memory", S(count) + " ints"}}, copyArr, i, {}, -1, -1, "b"));
    }
    steps.push_back(dsArrayStep(3, "Extra space O(n): it doubles when the input doubles. In-place algorithms (like reversing with two pointers) need only O(1).",
                                {{"extra memory", S(count) + " ints = O(n)"}}, copyArr, -1, {}, -1, -1, "b"));
    return steps;
}

void addDS3(map<string, Sim>& m);

void addDS2(map<string, Sim>& m) {
    addDS3(m);

    dsRegisterIntSim(m, "Merge Sort", "Enter up to 8 numbers", "38, 27, 43, 3, 9, 82, 10, 5",
                     {"void mergeSort(a, lo, hi) {", "    if (lo >= hi)  return;", "    mid = (lo + hi) / 2;", "    mergeSort(a, lo, mid);  mergeSort(a, mid + 1, hi);", "    merge(a, lo, mid, hi);     // two sorted halves -> one", "}"},
                     dsSortRefs(), {"LeetCode #912 (Sort an Array)", "LeetCode #88 (Merge Sorted Array)", "LeetCode #21 (Merge Two Sorted Lists)", "LeetCode #148 (Sort List)"},
                     simMergeSort);

    dsRegisterIntSim(m, "Quick Sort", "Enter up to 8 numbers", "10, 80, 30, 90, 40, 50, 70",
                     {"quickSort(a, lo, hi):", "    pivot = a[hi];  i = lo - 1;", "    for (j = lo; j < hi; j++)", "        if (a[j] <= pivot)  swap(a[++i], a[j]);", "    swap(a[i + 1], a[hi]);   p = i + 1;", "    quickSort(a, lo, p - 1);  quickSort(a, p + 1, hi);"},
                     dsSortRefs(), {"LeetCode #912 (Sort an Array)", "LeetCode #75 (Sort Colors)", "LeetCode #215 (Kth Largest Element in an Array)", "LeetCode #169 (Majority Element)"},
                     simQuickSort);

    dsRegisterIntSim(m, "Radix Sort", "Enter up to 8 numbers (0 to 999)", "170, 45, 75, 90, 802, 24, 2, 66",
                     {"for (exp = 1; max / exp > 0; exp *= 10) {", "    for each x in a:  bucket[(x / exp) % 10].push_back(x);", "    a = buckets 0..9 joined in order;   // stable", "}"},
                     dsSortRefs(), {"LeetCode #164 (Maximum Gap)", "LeetCode #912 (Sort an Array)", "LeetCode #561 (Array Partition)"},
                     simRadixSort);

    dsRegisterIntSim(m, "Bucket Sort", "Enter up to 9 numbers (0 to 99)", "29, 25, 3, 49, 9, 37, 21, 43, 4",
                     {"for each x in a:   bucket[x / 20].push_back(x);   // 5 buckets", "for each bucket:   insertion-sort it;", "a = buckets 0..4 joined in order;"},
                     dsSortRefs(), {"LeetCode #347 (Top K Frequent Elements)", "LeetCode #451 (Sort Characters By Frequency)", "LeetCode #164 (Maximum Gap)"},
                     simBucketSort);

    dsRegisterIntSim(m, "Counting Sort", "Enter up to 10 numbers (0 to 7)", "4, 2, 2, 7, 3, 3, 1",
                     {"for (i = 0; i < n; i++)   count[a[i]]++;", "for (v = 1; v < K; v++)   count[v] += count[v - 1];   // prefix sums", "for (i = n - 1; i >= 0; i--) {              // right to left = stable", "    out[--count[a[i]]] = a[i];", "}"},
                     dsSortRefs(), {"LeetCode #75 (Sort Colors)", "LeetCode #1122 (Relative Sort Array)", "LeetCode #1051 (Height Checker)", "LeetCode #451 (Sort Characters By Frequency)"},
                     simCountingSort);

    vector<string> refMedian = {
        "GeeksforGeeks: QuickSelect|https://www.geeksforgeeks.org/quickselect-algorithm/",
        "GeeksforGeeks: Median of medians|https://www.geeksforgeeks.org/kth-smallest-largest-element-in-unsorted-array-set-3-worst-case-linear-time/",
        "VisuAlgo: sorting|https://visualgo.net/en/sorting"};
    dsRegisterIntSim(m, "Median Search", "Rank k first (1 = smallest), then up to 9 numbers", "5, 7, 10, 4, 3, 20, 15, 9, 1",
                     {"quickSelect(a, lo, hi, k):", "    p = partition(a, lo, hi);        // pivot lands at index p", "    if (p == k)  return a[p];", "    if (k < p)   return quickSelect(a, lo, p - 1, k);", "    return quickSelect(a, p + 1, hi, k);"},
                     refMedian, {"LeetCode #215 (Kth Largest Element in an Array)", "LeetCode #462 (Minimum Moves to Equal Array Elements II)", "LeetCode #295 (Find Median from Data Stream)", "LeetCode #4 (Median of Two Sorted Arrays)"},
                     simQuickSelect);
    dsRegisterIntSim(m, "Median Search#1", "Enter up to 10 numbers", "7, 10, 4, 3, 20, 15, 9, 1, 12, 6",
                     {"split a into groups of 5;", "sort each group;  take its median;", "pivot = median of those medians;", "partition around pivot  (at least 30% of items land on each side)"},
                     refMedian, {"LeetCode #215 (Kth Largest Element in an Array)", "LeetCode #169 (Majority Element)"},
                     simMedianOfMedians);
    dsSetProgramNames("Median Search", {"QuickSelect: k-th smallest / median", "Median of medians: a guaranteed good pivot"});

    vector<string> refComplexity = {
        "GeeksforGeeks: Analysis of algorithms|https://www.geeksforgeeks.org/analysis-of-algorithms/",
        "Big-O Cheat Sheet|https://www.bigocheatsheet.com/",
        "VisuAlgo: sorting and complexity|https://visualgo.net/en/sorting"};
    dsRegisterIntSim(m, "Time Complexity Analysis", "Enter n", "6",
                     {"int ops = 0;", "for (int i = 0; i < n; i++)", "    ops++;          // one unit of work"},
                     refComplexity, {"LeetCode #1 (Two Sum: O(n^2) brute force vs O(n) hash map)", "LeetCode #26 (Remove Duplicates from Sorted Array)", "LeetCode #70 (Climbing Stairs)"},
                     simTimeLinear);
    dsRegisterIntSim(m, "Time Complexity Analysis#1", "Enter n (1 to 6)", "4",
                     {"int ops = 0;", "for (int i = 0; i < n; i++)", "    for (int j = 0; j < n; j++)", "        ops++;"},
                     refComplexity, {"LeetCode #1 (Two Sum: brute force is O(n^2))", "LeetCode #217 (Contains Duplicate)", "LeetCode #283 (Move Zeroes)"},
                     simTimeQuadratic);
    dsRegisterIntSim(m, "Time Complexity Analysis#2", "Enter n (1 to 4096)", "64",
                     {"int ops = 0;", "while (n > 1) {", "    n = n / 2;", "    ops++;", "}"},
                     refComplexity, {"LeetCode #704 (Binary Search)", "LeetCode #69 (Sqrt(x))", "LeetCode #50 (Pow(x, n))"},
                     simTimeLogarithmic);
    dsSetProgramNames("Time Complexity Analysis", {"One loop: O(n)", "Nested loops: O(n^2)", "Halving loop: O(log n)"});

    vector<string> refSpace = {
        "GeeksforGeeks: Space complexity|https://www.geeksforgeeks.org/g-fact-86-what-is-space-complexity/",
        "Big-O Cheat Sheet|https://www.bigocheatsheet.com/",
        "VisuAlgo: recursion tree|https://visualgo.net/en/recursion"};
    dsRegisterIntSim(m, "Space Complexity Analysis", "Enter n (1 to 8)", "5",
                     {"int sum = 0;", "for (int i = 1; i <= n; i++)", "    sum += i;", "return sum;"},
                     refSpace, {"LeetCode #283 (Move Zeroes: in place, O(1) space)", "LeetCode #344 (Reverse String: O(1) space)", "LeetCode #26 (Remove Duplicates from Sorted Array)"},
                     simSpaceConstant);
    dsRegisterIntSim(m, "Space Complexity Analysis#1", "Enter n (1 to 8)", "5",
                     {"int sum(int n) {", "    if (n == 0)  return 0;", "    return n + sum(n - 1);", "}"},
                     refSpace, {"LeetCode #509 (Fibonacci Number)", "LeetCode #70 (Climbing Stairs)", "LeetCode #206 (Reverse Linked List: recursive is O(n) space)"},
                     simSpaceRecursion);
    dsRegisterIntSim(m, "Space Complexity Analysis#2", "Enter up to 8 numbers", "3, 1, 4, 1, 5",
                     {"int *b = new int[n];     // extra array", "for (int i = 0; i < n; i++)", "    b[i] = a[i];", "return b;"},
                     refSpace, {"LeetCode #88 (Merge Sorted Array)", "LeetCode #189 (Rotate Array: O(n) extra vs O(1) in place)", "LeetCode #238 (Product of Array Except Self)"},
                     simSpaceArrayCopy);
    dsSetProgramNames("Space Complexity Analysis", {"Loop with a few variables: O(1)", "Recursion: stack frames O(n)", "Copying an array: O(n)"});
}
