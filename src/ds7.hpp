// Data Structures (Semester 3), part 7: Module 6 (advanced data structures).
#pragma once
#include "ds6.hpp"

using namespace std;

static Ints dsCharCodes(const string& text) {
    Ints codes;
    for (size_t i = 0; i < text.size(); ++i) {
        codes.push_back((unsigned char)text[i]);
    }
    return codes;
}

static Step dsStringStep(int lineNum, const string& noteText, Vars varsList, const string& text, int highlightIdx,
                         vector<string> tags = {}, int rangeLo = -1, int rangeHi = -1, const char* arrayName = "s") {
    Step step = dsArrayStep(lineNum, noteText, varsList, dsCharCodes(text), highlightIdx, tags, rangeLo, rangeHi, arrayName);
    step.chr = true;
    return step;
}

// Segment tree over arr[0..n-1] for range sums.
class SegmentTreePool {
public:
    TreeDiagram tree;
    vector<int> segLo;
    vector<int> segHi;
    vector<int> segSum;
    Ints arr;

    SegmentTreePool() {
        tree.root = 0;
    }

    int buildRange(int leftIdx, int rightIdx) {
        int nodeIdx = tree.addNode(0, {-1, -1});
        segLo.push_back(leftIdx);
        segHi.push_back(rightIdx);
        segSum.push_back(0);
        if (leftIdx < rightIdx) {
            int mid = (leftIdx + rightIdx) / 2;
            int leftChildIdx = buildRange(leftIdx, mid);
            int rightChildIdx = buildRange(mid + 1, rightIdx);
            tree.ch[nodeIdx] = {leftChildIdx, rightChildIdx};
            segSum[nodeIdx] = segSum[leftChildIdx] + segSum[rightChildIdx];
        } else {
            segSum[nodeIdx] = arr[leftIdx];
        }
        tree.key[nodeIdx] = segSum[nodeIdx];
        return nodeIdx;
    }

    void relabelNodes() {
        for (size_t i = 0; i < tree.key.size(); ++i) {
            tree.key[i] = segSum[i];
            tree.lab[i] = (segLo[i] == segHi[i]) ? S(segSum[i]) : (S(segLo[i]) + "-" + S(segHi[i]) + ":" + S(segSum[i]));
        }
    }
};

class IntervalTreePool {
public:
    TreeDiagram tree;
    vector<int> lowVal;
    vector<int> highVal;
    vector<int> maxHigh;
    vector<int> parentIdx;
    int root;

    IntervalTreePool() {
        root = -1;
    }

    int addIntervalNode(int low, int high) {
        int idx = tree.addNode(low, {-1, -1});
        lowVal.push_back(low);
        highVal.push_back(high);
        maxHigh.push_back(high);
        parentIdx.push_back(-1);
        return idx;
    }

    void relabelNodes() {
        for (size_t i = 0; i < tree.key.size(); ++i) {
            tree.lab[i] = S(lowVal[i]) + "-" + S(highVal[i]) + " (" + S(maxHigh[i]) + ")";
        }
    }
};

class PrefixTriePool {
public:
    vector<array<int, 26>> nextChild;
    vector<char> isWordEnd;
    vector<char> letterChar;
    vector<int> parentIdx;

    PrefixTriePool() {
        createNode('^', -1);
    }

    int createNode(char letter, int parent) {
        array<int, 26> childRow;
        childRow.fill(-1);
        nextChild.push_back(childRow);
        isWordEnd.push_back(0);
        letterChar.push_back(letter);
        parentIdx.push_back(parent);
        return (int)nextChild.size() - 1;
    }

    TreeDiagram toTreeDiagram() const {
        TreeDiagram diagram;
        for (size_t i = 0; i < nextChild.size(); ++i) {
            diagram.addNode((int)i, {});
            diagram.lab[i] = (i == 0) ? "root" : string(1, letterChar[i]);
            diagram.cls[i] = isWordEnd[i] ? "weld" : "";
            for (int c = 0; c < 26; ++c) {
                if (nextChild[i][c] >= 0) diagram.ch[i].push_back(nextChild[i][c]);
            }
        }
        diagram.root = 0;
        return diagram;
    }
};

class SuffixTreePool {
public:
    vector<string> edgeLabel;
    vector<int> suffixStart;
    vector<vector<int>> children;

    SuffixTreePool() {
        edgeLabel.push_back("root");
        suffixStart.push_back(-1);
        children.push_back({});
    }

    int createNode(const string& label, int sufStart) {
        edgeLabel.push_back(label);
        suffixStart.push_back(sufStart);
        children.push_back({});
        return (int)edgeLabel.size() - 1;
    }

    void sortChildren(int nodeIdx) {
        vector<int>& childList = children[nodeIdx];
        for (size_t i = 1; i < childList.size(); ++i) {
            int keyNode = childList[i];
            int j = (int)i - 1;
            while (j >= 0 && edgeLabel[childList[j]] > edgeLabel[keyNode]) {
                childList[j + 1] = childList[j];
                --j;
            }
            childList[j + 1] = keyNode;
        }
    }

    TreeDiagram toTreeDiagram() const {
        TreeDiagram diagram;
        for (size_t i = 0; i < edgeLabel.size(); ++i) {
            diagram.addNode((int)i, children[i]);
            diagram.lab[i] = edgeLabel[i] + (suffixStart[i] >= 0 ? " [" + S(suffixStart[i]) + "]" : "");
            diagram.cls[i] = (suffixStart[i] >= 0) ? "weld" : "";
        }
        diagram.root = 0;
        return diagram;
    }

    // Returns (new leaf index, split node index or -1).
    pair<int, int> insertSuffix(const string& suffixText, int startPos) {
        int cur = 0;
        size_t pos = 0;
        while (true) {
            int found = -1;
            for (size_t i = 0; i < children[cur].size(); ++i) {
                int childIdx = children[cur][i];
                if (edgeLabel[childIdx][0] == suffixText[pos]) {
                    found = childIdx;
                    break;
                }
            }
            if (found < 0) {
                int leafIdx = createNode(suffixText.substr(pos), startPos);
                children[cur].push_back(leafIdx);
                sortChildren(cur);
                return make_pair(leafIdx, -1);
            }
            size_t matchLen = 0;
            while (matchLen < edgeLabel[found].size() && pos + matchLen < suffixText.size() &&
                   edgeLabel[found][matchLen] == suffixText[pos + matchLen]) {
                ++matchLen;
            }
            if (matchLen == edgeLabel[found].size()) {
                cur = found;
                pos += matchLen;
                continue;
            }
            int midIdx = createNode(edgeLabel[found].substr(0, matchLen), -1);
            string restLabel = edgeLabel[found].substr(matchLen);
            edgeLabel[found] = restLabel;
            vector<int>& curChildren = children[cur];
            for (size_t i = 0; i < curChildren.size(); ++i) {
                if (curChildren[i] == found) {
                    curChildren[i] = midIdx;
                    break;
                }
            }
            children[midIdx].push_back(found);
            int leafIdx = createNode(suffixText.substr(pos + matchLen), startPos);
            children[midIdx].push_back(leafIdx);
            sortChildren(midIdx);
            sortChildren(cur);
            return make_pair(leafIdx, midIdx);
        }
    }
};

static const vector<string>& dsSegmentTreeRefs() {
    static const vector<string> refs = {
        "VisuAlgo: segment tree|https://visualgo.net/en/segmenttree",
        "cp-algorithms: segment tree|https://cp-algorithms.com/data_structures/segment_tree.html",
        "GeeksforGeeks: Segment tree|https://www.geeksforgeeks.org/segment-tree-data-structure/"};
    return refs;
}

static int dsInsertInterval(IntervalTreePool& pool, int low, int high) {
    if (pool.root < 0) {
        pool.root = pool.addIntervalNode(low, high);
        pool.tree.root = pool.root;
        return pool.root;
    }
    int cur = pool.root;
    while (true) {
        int side = (low < pool.lowVal[cur]) ? 0 : 1;
        int childIdx = pool.tree.ch[cur][side];
        if (childIdx < 0) {
            int newNodeIdx = pool.addIntervalNode(low, high);
            pool.tree.ch[cur][side] = newNodeIdx;
            pool.parentIdx[newNodeIdx] = cur;
            return newNodeIdx;
        }
        cur = childIdx;
    }
}

static vector<pair<int, int>> dsParseIntervals(const Ints& input, size_t startIndex, Ints defaultValues) {
    Ints rawValues = dsTakeInts(input, startIndex, 12, defaultValues);
    vector<pair<int, int>> intervals;
    for (size_t i = 0; i + 1 < rawValues.size() && intervals.size() < 6; i += 2) {
        int a = abs(rawValues[i]) % 100;
        int b = abs(rawValues[i + 1]) % 100;
        if (a > b) swap(a, b);
        intervals.push_back(make_pair(a, b));
    }
    return intervals;
}

static Steps simIntervalTreeInsert(const Ints& input) {
    vector<pair<int, int>> intervals = dsParseIntervals(input, 0, {15, 20, 10, 30, 17, 19, 5, 20, 12, 15, 30, 40});
    IntervalTreePool pool;
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "An interval tree stores intervals [low, high]. Each node shows  low-high (max): max is the biggest high end anywhere below it. That extra number lets us skip whole subtrees when searching for overlaps.",
        {}, dsDrawTree(pool.tree, true, "empty"), {}));
    for (size_t i = 0; i < intervals.size(); ++i) {
        int low = intervals[i].first;
        int high = intervals[i].second;
        int newNodeIdx = dsInsertInterval(pool, low, high);
        pool.relabelNodes();
        steps.push_back(dsDiagramStep(2,
            "Insert [" + S(low) + ", " + S(high) + "] by its LOW end " + S(low) + " like a normal BST insert.",
            {{"interval", "[" + S(low) + "," + S(high) + "]"}},
            dsDrawTree(pool.tree, true, "ordered by low end"), {"t" + S(newNodeIdx), "e" + S(newNodeIdx)}));
        string updatedList;
        for (int u = pool.parentIdx[newNodeIdx]; u >= 0; u = pool.parentIdx[u]) {
            if (high > pool.maxHigh[u]) {
                pool.maxHigh[u] = high;
                if (!updatedList.empty()) updatedList += ", ";
                updatedList += S(pool.lowVal[u]) + "-" + S(pool.highVal[u]);
            }
        }
        pool.relabelNodes();
        steps.push_back(dsDiagramStep(3,
            updatedList.empty() ? "No ancestor's max had to change."
                                : "Walk up: the max of node(s) " + updatedList + " grows to " + S(high) + ".",
            {}, dsDrawTree(pool.tree, true, "max updated on the way up"), {"t" + S(newNodeIdx)}));
    }
    steps.push_back(dsDiagramStep(1,
        "Built. Insert and delete cost O(log n) if the tree is kept balanced (e.g. as an AVL tree). Next: use the max values to find overlaps quickly.",
        {}, dsDrawTree(pool.tree, true, "interval tree"), {}));
    return steps;
}

static Steps simIntervalTreeSearch(const Ints& input) {
    int queryLow = input.empty() ? 14 : abs(input[0]) % 100;
    int queryHigh = input.size() > 1 ? abs(input[1]) % 100 : 16;
    if (queryLow > queryHigh) swap(queryLow, queryHigh);
    vector<pair<int, int>> intervals = dsParseIntervals(input, 2, {15, 20, 10, 30, 17, 19, 5, 20, 12, 15, 30, 40});
    IntervalTreePool pool;
    for (size_t i = 0; i < intervals.size(); ++i) {
        int newNodeIdx = dsInsertInterval(pool, intervals[i].first, intervals[i].second);
        for (int u = pool.parentIdx[newNodeIdx]; u >= 0; u = pool.parentIdx[u]) {
            pool.maxHigh[u] = max(pool.maxHigh[u], intervals[i].second);
        }
    }
    pool.relabelNodes();
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "Find an interval that OVERLAPS the query [" + S(queryLow) + ", " + S(queryHigh) +
            "]. Two intervals overlap when each starts before the other ends.",
        {{"query", "[" + S(queryLow) + "," + S(queryHigh) + "]"}},
        dsDrawTree(pool.tree, true, "start at the root"), {"t" + S(pool.root)}));
    int cur = pool.root;
    int guard = 0;
    while (cur >= 0 && guard++ < 30) {
        bool overlaps = (pool.lowVal[cur] <= queryHigh && queryLow <= pool.highVal[cur]);
        if (overlaps) {
            pool.tree.cls[cur] = "weld";
            steps.push_back(dsDiagramStep(2,
                "[" + S(pool.lowVal[cur]) + ", " + S(pool.highVal[cur]) + "] overlaps [" + S(queryLow) + ", " + S(queryHigh) +
                    "] (" + S(pool.lowVal[cur]) + " <= " + S(queryHigh) + " and " + S(queryLow) + " <= " + S(pool.highVal[cur]) + "). FOUND.",
                {{"answer", "[" + S(pool.lowVal[cur]) + "," + S(pool.highVal[cur]) + "]"}},
                dsDrawTree(pool.tree, true, "overlap found"), {"t" + S(cur)}));
            return steps;
        }
        int leftChildIdx = pool.tree.ch[cur][0];
        bool goLeft = (leftChildIdx >= 0 && pool.maxHigh[leftChildIdx] >= queryLow);
        steps.push_back(dsDiagramStep(3,
            "[" + S(pool.lowVal[cur]) + ", " + S(pool.highVal[cur]) + "] does not overlap. " +
                (leftChildIdx < 0
                     ? "No left child: go right."
                     : "Left subtree max = " + S(pool.maxHigh[leftChildIdx]) +
                           (goLeft ? " >= " + S(queryLow) + ": an overlap COULD be on the left, go left."
                                   : " < " + S(queryLow) + ": nothing there reaches " + S(queryLow) + ", skip the whole left subtree and go right.")),
            {{"node", S(pool.lowVal[cur]) + "-" + S(pool.highVal[cur])}},
            dsDrawTree(pool.tree, true, "use the max value to prune"), {"t" + S(cur)}));
        cur = goLeft ? leftChildIdx : pool.tree.ch[cur][1];
    }
    steps.push_back(dsDiagramStep(4,
        "No stored interval overlaps [" + S(queryLow) + ", " + S(queryHigh) + "]. The search followed ONE path: O(log n).",
        {}, dsDrawTree(pool.tree, true, "no overlap"), {}));
    return steps;
}

static void dsSegBuildWalk(SegmentTreePool& segTree, int nodeIdx, Steps& steps) {
    for (size_t i = 0; i < segTree.tree.ch[nodeIdx].size(); ++i) {
        int childIdx = segTree.tree.ch[nodeIdx][i];
        if (childIdx >= 0) dsSegBuildWalk(segTree, childIdx, steps);
    }
    string rangeText = "[" + S(segTree.segLo[nodeIdx]) + ".." + S(segTree.segHi[nodeIdx]) + "]";
    segTree.tree.cls[nodeIdx] = "weld";
    bool isLeaf = (segTree.segLo[nodeIdx] == segTree.segHi[nodeIdx]);
    steps.push_back(dsDiagramStep(isLeaf ? 1 : 3,
        isLeaf ? "Leaf " + rangeText + " = a[" + S(segTree.segLo[nodeIdx]) + "] = " + S(segTree.segSum[nodeIdx]) + "."
               : "Node " + rangeText + ": sum of its two children = " + S(segTree.segSum[segTree.tree.ch[nodeIdx][0]]) +
                     " + " + S(segTree.segSum[segTree.tree.ch[nodeIdx][1]]) + " = " + S(segTree.segSum[nodeIdx]) + ".",
        {{"segment", rangeText}, {"sum", S(segTree.segSum[nodeIdx])}},
        dsDrawTree(segTree.tree, true, "label = range:sum"), {"t" + S(nodeIdx)}));
}

static Steps simSegmentTreeBuild(const Ints& input) {
    SegmentTreePool segTree;
    segTree.arr = dsTakeInts(input, 0, 8, {2, 5, 1, 4, 9, 3, 7, 6});
    for (size_t i = 0; i < segTree.arr.size(); ++i) segTree.arr[i] = clamp(segTree.arr[i], -99, 99);
    int n = (int)segTree.arr.size();
    segTree.tree.root = 0;
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "A segment tree answers RANGE questions (sum, min, max...) quickly. Each node stores the answer for a segment: the root covers all of [0.." +
            S(n - 1) + "], its children cover the two halves, the leaves are single items.",
        {{"n", S(n)}}, "{\"w\":300,\"h\":60,\"el\":[],\"map\":[]}", {}));
    segTree.buildRange(0, n - 1);
    segTree.relabelNodes();
    dsSegBuildWalk(segTree, 0, steps);
    steps.push_back(dsDiagramStep(3,
        "Built with " + S(segTree.tree.key.size()) + " nodes (about 2n). A range query or a single update now costs only O(log n), instead of O(n) for scanning the array.",
        {{"total", S(segTree.segSum[0])}}, dsDrawTree(segTree.tree, true, "segment tree (sum)"), {}));
    return steps;
}

static void dsSegQueryWalk(SegmentTreePool& segTree, int nodeIdx, int queryLeft, int queryRight, int& totalSum, Steps& steps) {
    int l = segTree.segLo[nodeIdx];
    int r = segTree.segHi[nodeIdx];
    string rangeText = "[" + S(l) + ".." + S(r) + "]";
    if (r < queryLeft || l > queryRight) {
        steps.push_back(dsDiagramStep(1,
            "Segment " + rangeText + " is completely OUTSIDE the query: contributes 0, do not look inside.",
            {{"running sum", S(totalSum)}}, dsDrawTree(segTree.tree, true, "outside: skip"), {"t" + S(nodeIdx)}));
        return;
    }
    if (queryLeft <= l && r <= queryRight) {
        totalSum += segTree.segSum[nodeIdx];
        segTree.tree.cls[nodeIdx] = "weld";
        steps.push_back(dsDiagramStep(2,
            "Segment " + rangeText + " is completely INSIDE the query: take its stored sum " + S(segTree.segSum[nodeIdx]) +
                ". Running sum = " + S(totalSum) + ".",
            {{"running sum", S(totalSum)}}, dsDrawTree(segTree.tree, true, "inside: use the stored sum"), {"t" + S(nodeIdx)}));
        return;
    }
    segTree.tree.cls[nodeIdx] = "blue";
    steps.push_back(dsDiagramStep(3,
        "Segment " + rangeText + " overlaps the query only PARTLY: ask both children.",
        {{"running sum", S(totalSum)}}, dsDrawTree(segTree.tree, true, "partial overlap"), {"t" + S(nodeIdx)}));
    dsSegQueryWalk(segTree, segTree.tree.ch[nodeIdx][0], queryLeft, queryRight, totalSum, steps);
    dsSegQueryWalk(segTree, segTree.tree.ch[nodeIdx][1], queryLeft, queryRight, totalSum, steps);
    segTree.tree.cls[nodeIdx] = "";
}

static Steps simSegmentTreeQuery(const Ints& input) {
    SegmentTreePool segTree;
    segTree.arr = dsTakeInts(input, 2, 8, {2, 5, 1, 4, 9, 3, 7, 6});
    for (size_t i = 0; i < segTree.arr.size(); ++i) segTree.arr[i] = clamp(segTree.arr[i], -99, 99);
    int n = (int)segTree.arr.size();
    segTree.buildRange(0, n - 1);
    segTree.tree.root = 0;
    segTree.relabelNodes();
    int queryLeft = clamp(input.empty() ? 2 : abs(input[0]), 0, n - 1);
    int queryRight = clamp(input.size() > 1 ? abs(input[1]) : 6, 0, n - 1);
    if (queryLeft > queryRight) swap(queryLeft, queryRight);
    Steps steps;
    int totalSum = 0;
    steps.push_back(dsDiagramStep(0,
        "Sum of a[" + S(queryLeft) + ".." + S(queryRight) +
            "]. Each node covers a segment; we use ready-made node sums wherever a segment fits completely inside the query.",
        {{"query", "[" + S(queryLeft) + ".." + S(queryRight) + "]"}},
        dsDrawTree(segTree.tree, true, "labels: range:sum"), {"t0"}));
    dsSegQueryWalk(segTree, 0, queryLeft, queryRight, totalSum, steps);
    int checkSum = 0;
    for (int i = queryLeft; i <= queryRight; ++i) checkSum += segTree.arr[i];
    steps.push_back(dsDiagramStep(3,
        "Answer = " + S(totalSum) + " (check by adding the items one by one: " + S(checkSum) +
            "). At most about 4 log n nodes are visited.",
        {{"answer", S(totalSum)}}, dsDrawTree(segTree.tree, true, "orange = nodes used"), {}));
    return steps;
}

static Steps simSegmentTreeUpdate(const Ints& input) {
    SegmentTreePool segTree;
    segTree.arr = dsTakeInts(input, 2, 8, {2, 5, 1, 4, 9, 3, 7, 6});
    for (size_t i = 0; i < segTree.arr.size(); ++i) segTree.arr[i] = clamp(segTree.arr[i], -99, 99);
    int n = (int)segTree.arr.size();
    segTree.buildRange(0, n - 1);
    segTree.tree.root = 0;
    int updateIdx = clamp(input.empty() ? 3 : abs(input[0]), 0, n - 1);
    int newVal = clamp(input.size() > 1 ? input[1] : 10, -99, 99);
    segTree.relabelNodes();
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "Change a[" + S(updateIdx) + "] from " + S(segTree.arr[updateIdx]) + " to " + S(newVal) +
            ". Only the nodes on the path from that leaf to the root contain it.",
        {{"index", S(updateIdx)}, {"new value", S(newVal)}}, dsDrawTree(segTree.tree, true, "before the update"), {}));
    vector<int> path;
    int u = 0;
    while (true) {
        path.push_back(u);
        if (segTree.segLo[u] == segTree.segHi[u]) break;
        int mid = (segTree.segLo[u] + segTree.segHi[u]) / 2;
        u = (updateIdx <= mid) ? segTree.tree.ch[u][0] : segTree.tree.ch[u][1];
    }
    string pathLabels;
    for (size_t i = 0; i < path.size(); ++i) {
        int nodeIdx = path[i];
        if (!pathLabels.empty()) pathLabels += " > ";
        pathLabels += "[" + S(segTree.segLo[nodeIdx]) + ".." + S(segTree.segHi[nodeIdx]) + "]";
        segTree.tree.cls[nodeIdx] = "blue";
    }
    steps.push_back(dsDiagramStep(2, "Go down to the leaf: " + pathLabels + ".",
        {}, dsDrawTree(segTree.tree, true, "path to the leaf (blue)"), {"t" + S(path.back())}));
    segTree.segSum[path.back()] = newVal;
    segTree.relabelNodes();
    steps.push_back(dsDiagramStep(1, "Leaf set to " + S(newVal) + ".",
        {{"leaf", S(newVal)}}, dsDrawTree(segTree.tree, true, "leaf updated"), {"t" + S(path.back())}));
    for (int i = (int)path.size() - 2; i >= 0; --i) {
        int p = path[i];
        int leftSum = segTree.segSum[segTree.tree.ch[p][0]];
        int rightSum = segTree.segSum[segTree.tree.ch[p][1]];
        segTree.segSum[p] = leftSum + rightSum;
        segTree.relabelNodes();
        steps.push_back(dsDiagramStep(3,
            "Recompute [" + S(segTree.segLo[p]) + ".." + S(segTree.segHi[p]) + "] = " + S(leftSum) + " + " + S(rightSum) + " = " +
                S(segTree.segSum[p]) + ".",
            {{"new sum", S(segTree.segSum[p])}}, dsDrawTree(segTree.tree, true, "recompute upwards"), {"t" + S(p)}));
    }
    for (size_t i = 0; i < segTree.tree.cls.size(); ++i) segTree.tree.cls[i].clear();
    steps.push_back(dsDiagramStep(3,
        "Update finished after touching only " + S(path.size()) +
            " nodes: O(log n). The array version would also be O(1) to update but O(n) to query: the segment tree balances both.",
        {{"total", S(segTree.segSum[0])}}, dsDrawTree(segTree.tree, true, "updated tree"), {}));
    return steps;
}

static string dsLowerAlnum(const string& input, size_t maxLen) {
    string result;
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
        if (((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) && result.size() < maxLen) result += c;
    }
    return result;
}

static pair<string, string> dsSplitPipe(const string& input) {
    size_t pos = input.find('|');
    if (pos == string::npos) return make_pair(input, "");
    return make_pair(input.substr(0, pos), input.substr(pos + 1));
}

static vector<string> dsExtractWords(const string& input, size_t maxWords, size_t maxWordLen) {
    vector<string> words;
    string current;
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
        if (c >= 'a' && c <= 'z') {
            if (current.size() < maxWordLen) current += c;
        } else {
            if (!current.empty() && words.size() < maxWords) words.push_back(current);
            current.clear();
        }
    }
    if (!current.empty() && words.size() < maxWords) words.push_back(current);
    return words;
}

static const vector<string>& dsStringRefs() {
    static const vector<string> refs = {
        "VisuAlgo: suffix tree / suffix array|https://visualgo.net/en/suffixtree",
        "GeeksforGeeks: Pattern searching|https://www.geeksforgeeks.org/pattern-searching/",
        "cp-algorithms: string algorithms|https://cp-algorithms.com/string/prefix-function.html"};
    return refs;
}

static Steps simStringNaiveSearch(const string& input) {
    pair<string, string> parts = dsSplitPipe(input);
    string text = dsLowerAlnum(parts.first, 14);
    string pattern = dsLowerAlnum(parts.second, 5);
    if (text.empty()) text = "abxabcabcaby";
    if (pattern.empty()) pattern = "abcaby";
    if (pattern.size() > text.size()) pattern = pattern.substr(0, text.size());
    int n = (int)text.size();
    int m = (int)pattern.size();
    Steps steps;
    string foundPositions;
    int comparisons = 0;
    steps.push_back(dsStringStep(0,
        "A string is an array of characters. Naive pattern search slides the pattern \"" + pattern + "\" along the text \"" + text +
            "\" and compares character by character. The row under the cells shows the pattern's letters.",
        {{"n", S(n)}, {"m", S(m)}}, text, -1));
    for (int i = 0; i + m <= n; ++i) {
        int j = 0;
        vector<string> tags(n);
        for (; j < m; ++j) {
            ++comparisons;
            for (int k = 0; k < m; ++k) tags[i + k] = string(1, pattern[k]);
            bool matched = (text[i + j] == pattern[j]);
            if (!matched) tags[i + j] += "x";
            steps.push_back(dsStringStep(1,
                "Window starts at " + S(i) + ": compare text[" + S(i + j) + "] = '" + string(1, text[i + j]) +
                    "' with pattern[" + S(j) + "] = '" + string(1, pattern[j]) + "': " +
                    (matched ? "match." : "MISMATCH, slide the pattern one place right."),
                {{"i", S(i)}, {"j", S(j)}, {"comparisons", S(comparisons)}}, text, i + j, tags, i, i + m - 1));
            if (!matched) break;
        }
        if (j == m) {
            if (!foundPositions.empty()) foundPositions += ", ";
            foundPositions += S(i);
            steps.push_back(dsStringStep(2,
                "All " + S(m) + " characters match: the pattern occurs at index " + S(i) + ".",
                {{"found at", foundPositions}}, text, i, tags, i, i + m - 1));
        }
    }
    steps.push_back(dsStringStep(3,
        foundPositions.empty()
            ? "No occurrence. " + S(comparisons) + " comparisons. Worst case O(n x m)."
            : "Occurrence(s) at index: " + foundPositions + ". " + S(comparisons) +
                  " comparisons. Worst case O(n x m); KMP, suffix trees and suffix arrays (next topics) do better.",
        {{"comparisons", S(comparisons)}}, text, -1));
    return steps;
}

static vector<string> dsBuildLpsTags(const Ints& lps, int m, int curIdx, int prefixLen) {
    vector<string> tags(m);
    for (int k = 0; k < curIdx; ++k) tags[k] = "=" + S(lps[k]);
    if (curIdx < m) tags[curIdx] += (tags[curIdx].empty() ? "" : " ") + string("i");
    if (prefixLen >= 0 && prefixLen < m) tags[prefixLen] += (tags[prefixLen].empty() ? "" : " ") + string("len");
    return tags;
}

static Steps simStringKmpLps(const string& input) {
    string pattern = dsLowerAlnum(input, 10);
    if (pattern.size() < 2) pattern = "abcabcd";
    int m = (int)pattern.size();
    Ints lps(m, 0);
    Steps steps;
    int len = 0;
    int i = 1;
    steps.push_back(dsStringStep(0,
        "The prefix function lps[k] = length of the longest proper prefix of p[0..k] that is also a SUFFIX of it. KMP uses it to avoid re-reading text. Tags: =x is a finished lps value.",
        {{"pattern", pattern}}, pattern, 0, dsBuildLpsTags(lps, m, 1, 0)));
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            lps[i] = ++len;
            steps.push_back(dsStringStep(2,
                "p[" + S(i) + "] = '" + string(1, pattern[i]) + "' equals p[" + S(len - 1) + "] = '" +
                    string(1, pattern[len - 1]) + "': the border grows. lps[" + S(i) + "] = " + S(len) + ".",
                {{"i", S(i)}, {"len", S(len)}, {"lps", dsJoinInts(lps)}},
                pattern, i, dsBuildLpsTags(lps, m, i + 1, len)));
            ++i;
        } else if (len > 0) {
            steps.push_back(dsStringStep(3,
                "p[" + S(i) + "] = '" + string(1, pattern[i]) + "' differs from p[" + S(len) + "] = '" +
                    string(1, pattern[len]) + "'. Fall back: len = lps[" + S(len - 1) + "] = " + S(lps[len - 1]) +
                    " (try a shorter border); i stays.",
                {{"i", S(i)}, {"len", S(lps[len - 1])}},
                pattern, i, dsBuildLpsTags(lps, m, i, lps[len - 1])));
            len = lps[len - 1];
        } else {
            lps[i] = 0;
            steps.push_back(dsStringStep(4,
                "p[" + S(i) + "] = '" + string(1, pattern[i]) + "' matches nothing at the start: lps[" + S(i) + "] = 0.",
                {{"i", S(i)}, {"lps", dsJoinInts(lps)}},
                pattern, i, dsBuildLpsTags(lps, m, i + 1, 0)));
            ++i;
        }
    }
    steps.push_back(dsStringStep(5,
        "lps = [" + dsJoinInts(lps) +
            "]. During a search, when a mismatch happens after j matched characters, the pattern jumps to lps[j - 1] instead of restarting: total O(n + m).",
        {{"lps", dsJoinInts(lps)}}, pattern, -1, dsBuildLpsTags(lps, m, m, -1)));
    return steps;
}

static Steps simStringTwoPointers(const string& input) {
    string word = dsLowerAlnum(input, 10);
    if (word.size() < 2) word = "racecar";
    string original = word;
    int n = (int)word.size();
    int i = 0;
    int j = n - 1;
    Steps steps;
    bool isPalindrome = true;
    steps.push_back(dsStringStep(0,
        "Two pointers: i from the left, j from the right. Strings are arrays, so s[i] and s[j] can be swapped in place: O(n) time, O(1) space.",
        {{"word", word}}, word, -1, dsMakeTags(n, {{0, "i"}, {n - 1, "j"}})));
    while (i < j) {
        if (word[i] != word[j]) isPalindrome = false;
        steps.push_back(dsStringStep(2,
            "Swap s[" + S(i) + "] = '" + string(1, word[i]) + "' and s[" + S(j) + "] = '" + string(1, word[j]) + "'." +
                (word[i] == word[j] ? " (They are equal, so this pair is symmetric.)" : " (They differ, so the word is not a palindrome.)"),
            {{"i", S(i)}, {"j", S(j)}}, word, -1, dsMakeTags(n, {{i, "i"}, {j, "j"}})));
        swap(word[i], word[j]);
        steps.push_back(dsStringStep(3, "Swapped. Move the pointers inward.",
            {{"word", word}}, word, -1, dsMakeTags(n, {{i, "i"}, {j, "j"}})));
        ++i;
        --j;
    }
    steps.push_back(dsStringStep(4,
        "Pointers met. Reversed: \"" + word + "\". \"" + original + "\" is " +
            (isPalindrome ? "a PALINDROME (equal to its reverse)." : "NOT a palindrome."),
        {{"reversed", word}, {"palindrome", isPalindrome ? "yes" : "no"}}, word, -1));
    return steps;
}

static string dsDrawPrefixTrie(const PrefixTriePool& trie, const string& caption) {
    return dsDrawTree(trie.toTreeDiagram(), false, caption);
}

static Steps simTrieInsert(const string& input) {
    vector<string> words = dsExtractWords(input, 6, 6);
    if (words.empty()) words = {"apple", "app", "apply", "apt", "bat", "bar"};
    PrefixTriePool trie;
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "A trie (prefix tree) stores words letter by letter. Words that share a prefix share the same path. An orange node marks the end of a stored word.",
        {}, dsDrawPrefixTrie(trie, "empty trie"), {"t0"}));
    for (size_t wi = 0; wi < words.size(); ++wi) {
        const string& w = words[wi];
        int cur = 0;
        string prefix;
        for (size_t ci = 0; ci < w.size(); ++ci) {
            char c = w[ci];
            prefix += c;
            int childIdx = trie.nextChild[cur][c - 'a'];
            bool created = (childIdx < 0);
            if (created) {
                childIdx = trie.createNode(c, cur);
                trie.nextChild[cur][c - 'a'] = childIdx;
            }
            steps.push_back(dsDiagramStep(created ? 3 : 4,
                "Word \"" + w + "\", letter '" + string(1, c) + "': " +
                    (created ? "no child '" + string(1, c) + "' here, so create a new node."
                             : "the child '" + string(1, c) + "' already exists (shared prefix \"" + prefix + "\"): just follow it."),
                {{"word", w}, {"prefix", prefix}},
                dsDrawPrefixTrie(trie, created ? "new node" : "shared path"),
                {"t" + S(childIdx), "e" + S(childIdx)}));
            cur = childIdx;
        }
        trie.isWordEnd[cur] = 1;
        steps.push_back(dsDiagramStep(6, "End of \"" + w + "\": mark this node as a word end (orange).",
            {{"word", w}}, dsDrawPrefixTrie(trie, "word end marked"), {"t" + S(cur)}));
    }
    steps.push_back(dsDiagramStep(0,
        "Inserting or searching a word of length L costs O(L), independent of how many words are stored. Memory is shared by common prefixes.",
        {{"nodes", S(trie.nextChild.size())}}, dsDrawPrefixTrie(trie, "trie with " + S(words.size()) + " word(s)"), {}));
    return steps;
}

static Steps simTrieSearch(const string& input) {
    pair<string, string> parts = dsSplitPipe(input);
    vector<string> words = dsExtractWords(parts.first, 6, 6);
    vector<string> queries = dsExtractWords(parts.second, 3, 6);
    if (words.empty()) words = {"apple", "app", "apply", "apt", "bat", "bar"};
    if (queries.empty()) queries = {"app", "appl", "bay"};
    PrefixTriePool trie;
    for (size_t wi = 0; wi < words.size(); ++wi) {
        int cur = 0;
        for (size_t ci = 0; ci < words[wi].size(); ++ci) {
            char c = words[wi][ci];
            if (trie.nextChild[cur][c - 'a'] < 0) {
                int id = trie.createNode(c, cur);
                trie.nextChild[cur][c - 'a'] = id;
            }
            cur = trie.nextChild[cur][c - 'a'];
        }
        trie.isWordEnd[cur] = 1;
    }
    Steps steps;
    steps.push_back(dsDiagramStep(0, "A trie built from the words. Now look up each query: walk down one letter at a time.",
        {}, dsDrawPrefixTrie(trie, "start at the root"), {"t0"}));
    for (size_t qi = 0; qi < queries.size(); ++qi) {
        const string& q = queries[qi];
        int cur = 0;
        bool ok = true;
        string prefix;
        for (size_t ci = 0; ci < q.size(); ++ci) {
            char c = q[ci];
            prefix += c;
            int childIdx = trie.nextChild[cur][c - 'a'];
            if (childIdx < 0) {
                steps.push_back(dsDiagramStep(2,
                    "Query \"" + q + "\": no child '" + string(1, c) + "' after \"" + prefix.substr(0, prefix.size() - 1) +
                        "\". NOT FOUND, not even as a prefix.",
                    {{"query", q}}, dsDrawPrefixTrie(trie, "missing letter"), {"t" + S(cur)}));
                ok = false;
                break;
            }
            steps.push_back(dsDiagramStep(1, "Query \"" + q + "\": follow '" + string(1, c) + "'.",
                {{"query", q}, {"walked", prefix}}, dsDrawPrefixTrie(trie, "walking down"), {"t" + S(childIdx), "e" + S(childIdx)}));
            cur = childIdx;
        }
        if (ok) {
            bool isWord = (trie.isWordEnd[cur] != 0);
            steps.push_back(dsDiagramStep(3,
                isWord ? "\"" + q + "\" is a stored WORD (end mark is on)."
                       : "\"" + q + "\" is only a PREFIX of longer words (no end mark): search says no, startsWith says yes.",
                {{"query", q}, {"is word", isWord ? "yes" : "no"}},
                dsDrawPrefixTrie(trie, isWord ? "word found" : "prefix only"), {"t" + S(cur)}));
        }
    }
    steps.push_back(dsDiagramStep(4, "Each lookup cost O(length of the query), no matter how many words are stored.",
        {}, dsDrawPrefixTrie(trie, "done"), {}));
    return steps;
}

static void dsTrieDfsAutocomplete(const PrefixTriePool& trie, int nodeIdx, string currentWord, string& suggestionsText, Steps& steps) {
    if (trie.isWordEnd[nodeIdx]) {
        if (!suggestionsText.empty()) suggestionsText += ", ";
        suggestionsText += currentWord;
        steps.push_back(dsDiagramStep(3, "End-of-word node: output \"" + currentWord + "\".",
            {{"suggestions", suggestionsText}}, dsDrawPrefixTrie(trie, "word found"), {"t" + S(nodeIdx)}));
    }
    for (int c = 0; c < 26; ++c) {
        if (trie.nextChild[nodeIdx][c] >= 0) {
            dsTrieDfsAutocomplete(trie, trie.nextChild[nodeIdx][c], currentWord + (char)('a' + c), suggestionsText, steps);
        }
    }
}

static Steps simTrieAutocomplete(const string& input) {
    pair<string, string> parts = dsSplitPipe(input);
    vector<string> words = dsExtractWords(parts.first, 6, 6);
    vector<string> queries = dsExtractWords(parts.second, 1, 6);
    if (words.empty()) words = {"apple", "app", "apply", "apt", "bat", "bar"};
    string prefix = queries.empty() ? "ap" : queries[0];
    PrefixTriePool trie;
    for (size_t wi = 0; wi < words.size(); ++wi) {
        int cur = 0;
        for (size_t ci = 0; ci < words[wi].size(); ++ci) {
            char c = words[wi][ci];
            if (trie.nextChild[cur][c - 'a'] < 0) {
                int id = trie.createNode(c, cur);
                trie.nextChild[cur][c - 'a'] = id;
            }
            cur = trie.nextChild[cur][c - 'a'];
        }
        trie.isWordEnd[cur] = 1;
    }
    Steps steps;
    steps.push_back(dsDiagramStep(0, "Autocomplete: suggest all stored words that start with \"" + prefix + "\".",
        {{"prefix", prefix}}, dsDrawPrefixTrie(trie, "start at the root"), {"t0"}));
    int cur = 0;
    for (size_t i = 0; i < prefix.size(); ++i) {
        char c = prefix[i];
        int childIdx = trie.nextChild[cur][c - 'a'];
        if (childIdx < 0) {
            steps.push_back(dsDiagramStep(1, "No child '" + string(1, c) + "': no stored word starts with \"" + prefix + "\".",
                {}, dsDrawPrefixTrie(trie, "no suggestions"), {"t" + S(cur)}));
            return steps;
        }
        steps.push_back(dsDiagramStep(1, "Follow '" + string(1, c) + "'.",
            {}, dsDrawPrefixTrie(trie, "walking the prefix"), {"t" + S(childIdx), "e" + S(childIdx)}));
        cur = childIdx;
    }
    steps.push_back(dsDiagramStep(2,
        "Prefix node reached. Every word below this node starts with \"" + prefix + "\". Depth-first search from here.",
        {}, dsDrawPrefixTrie(trie, "subtree = all matches"), {"t" + S(cur)}));
    string suggestionsText;
    dsTrieDfsAutocomplete(trie, cur, prefix, suggestionsText, steps);
    steps.push_back(dsDiagramStep(3,
        "Suggestions (in alphabetical order for free): " + (suggestionsText.empty() ? string("none") : suggestionsText) + ".",
        {{"suggestions", suggestionsText}}, dsDrawPrefixTrie(trie, "done"), {}));
    return steps;
}

static Steps simSuffixArrayBuild(const string& input) {
    string text = dsLowerAlnum(input, 8);
    if (text.size() < 2) text = "banana";
    int n = (int)text.size();
    Steps steps;
    string allSuffixes;
    for (int i = 0; i < n; ++i) {
        if (!allSuffixes.empty()) allSuffixes += "  ";
        allSuffixes += S(i) + ":" + text.substr(i);
    }
    steps.push_back(dsStringStep(0,
        "Every suffix starts at a different position. A suffix array stores the START POSITIONS of the suffixes in sorted (dictionary) order: far smaller than a suffix tree.",
        {{"suffixes", allSuffixes}}, text, -1));
    Ints sa;
    for (int i = 0; i < n; ++i) {
        int pos = 0;
        while (pos < (int)sa.size() && text.substr(sa[pos]) < text.substr(i)) ++pos;
        sa.insert(sa.begin() + pos, i);
        string sortedText;
        for (size_t k = 0; k < sa.size(); ++k) {
            if (!sortedText.empty()) sortedText += " < ";
            sortedText += text.substr(sa[k]);
        }
        steps.push_back(dsArrayStep(1,
            "Insert suffix " + S(i) + " \"" + text.substr(i) + "\" into the sorted list: it goes at position " + S(pos) + ".",
            {{"sorted suffixes", sortedText}}, sa, pos, {}, -1, -1, "SA"));
    }
    Ints lcp(n, 0);
    for (int k = 1; k < n; ++k) {
        string a = text.substr(sa[k - 1]);
        string b = text.substr(sa[k]);
        int common = 0;
        while (common < (int)a.size() && common < (int)b.size() && a[common] == b[common]) ++common;
        lcp[k] = common;
    }
    string finalText;
    for (int k = 0; k < n; ++k) {
        if (!finalText.empty()) finalText += "  ";
        finalText += S(sa[k]) + ":" + text.substr(sa[k]);
    }
    int maxLcp = *max_element(lcp.begin(), lcp.end());
    steps.push_back(dsArrayStep(3,
        "Sorted. SA = [" + dsJoinInts(sa) + "]. LCP (shared prefix with the previous suffix) = [" + dsJoinInts(lcp) +
            "]. The sum of LCP tells how many repeated substrings exist; the largest LCP is the longest repeated substring (" +
            S(maxLcp) + " letters here).",
        {{"suffixes in order", finalText}, {"LCP", dsJoinInts(lcp)}}, sa, -1, {}, -1, -1, "SA"));
    return steps;
}

static Steps simSuffixArraySearch(const string& input) {
    pair<string, string> parts = dsSplitPipe(input);
    string text = dsLowerAlnum(parts.first, 8);
    string pattern = dsLowerAlnum(parts.second, 4);
    if (text.size() < 2) text = "banana";
    if (pattern.empty()) pattern = "ana";
    int n = (int)text.size();
    Ints sa(n);
    for (int i = 0; i < n; ++i) sa[i] = i;
    for (int i = 1; i < n; ++i) {
        int key = sa[i];
        int j = i - 1;
        while (j >= 0 && text.substr(sa[j]) > text.substr(key)) {
            sa[j + 1] = sa[j];
            --j;
        }
        sa[j + 1] = key;
    }
    Steps steps;
    int lo = 0;
    int hi = n;
    int iter = 0;
    steps.push_back(dsArrayStep(0,
        "Text \"" + text + "\", sorted suffix start positions SA. Search for \"" + pattern +
            "\": all suffixes starting with it sit NEXT TO EACH OTHER, so binary search finds them.",
        {{"pattern", pattern}}, sa, -1, dsMakeTags(n, {{0, "L"}, {n - 1, "R"}}), 0, n - 1, "SA"));
    while (lo < hi && iter++ < 12) {
        int mid = (lo + hi) / 2;
        string suffix = text.substr(sa[mid]);
        bool isSmaller = (suffix.substr(0, pattern.size()) < pattern);
        steps.push_back(dsArrayStep(2,
            "mid = " + S(mid) + ": SA[mid] = " + S(sa[mid]) + ", suffix \"" + suffix + "\". Compare its first " +
                S(pattern.size()) + " letters \"" + suffix.substr(0, pattern.size()) + "\" with \"" + pattern + "\": " +
                (isSmaller ? "smaller, go right." : "not smaller, go left (or stay)."),
            {{"lo", S(lo)}, {"mid", S(mid)}, {"hi", S(hi)}},
            sa, mid, dsMakeTags(n, {{lo, "L"}, {mid, "M"}, {hi - 1, "R"}}), lo, hi - 1, "SA"));
        if (isSmaller) lo = mid + 1;
        else hi = mid;
    }
    string occurrences;
    int k = lo;
    while (k < n && text.substr(sa[k]).compare(0, pattern.size(), pattern) == 0) {
        if (!occurrences.empty()) occurrences += ", ";
        occurrences += S(sa[k]);
        ++k;
    }
    steps.push_back(dsArrayStep(5,
        occurrences.empty()
            ? "First candidate at position " + S(lo) + " does not start with \"" + pattern + "\": the pattern is NOT in the text."
            : "Suffixes starting with \"" + pattern + "\" begin at SA[" + S(lo) + "]. Occurrences (start positions): " +
                  occurrences + ". Cost O(m log n) per search.",
        {{"occurrences", occurrences.empty() ? "none" : occurrences}},
        sa, lo < n ? lo : -1, {}, occurrences.empty() ? -1 : lo, occurrences.empty() ? -1 : k - 1, "SA"));
    return steps;
}

static void dsCollectSuffixLeaves(const SuffixTreePool& st, int nodeIdx, vector<int>& leafStarts) {
    if (st.suffixStart[nodeIdx] >= 0) leafStarts.push_back(st.suffixStart[nodeIdx]);
    for (size_t i = 0; i < st.children[nodeIdx].size(); ++i) {
        dsCollectSuffixLeaves(st, st.children[nodeIdx][i], leafStarts);
    }
}

static Steps simSuffixTreeBuild(const string& input) {
    string text = dsLowerAlnum(input, 7);
    if (text.size() < 2) text = "banana";
    text += "$";
    int n = (int)text.size();
    SuffixTreePool st;
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "A suffix tree is a compressed trie of ALL suffixes of the text. Each node shows the letters on the edge coming into it; orange leaves show where the suffix starts. The '$' marks the end so no suffix is a prefix of another. Text: \"" +
            text + "\".",
        {{"suffixes", S(n)}}, dsDrawTree(st.toTreeDiagram(), false, "empty"), {"t0"}));
    for (int i = 0; i < n; ++i) {
        pair<int, int> res = st.insertSuffix(text.substr(i), i);
        vector<string> hl = {"t" + S(res.first), "e" + S(res.first)};
        if (res.second >= 0) hl.push_back("t" + S(res.second));
        steps.push_back(dsDiagramStep(res.second >= 0 ? 2 : 3,
            "Insert suffix " + S(i) + " \"" + text.substr(i) + "\": " +
                (res.second >= 0 ? "it shares a prefix with an existing edge, so that edge is SPLIT at the shared part and the rest becomes a new leaf."
                                 : "no existing edge starts with its first letter, so it becomes a new leaf directly below the node it reached."),
            {{"suffix", text.substr(i)}},
            dsDrawTree(st.toTreeDiagram(), false, res.second >= 0 ? "edge split" : "new leaf"), hl));
    }
    int leafCount = 0;
    for (size_t i = 0; i < st.suffixStart.size(); ++i) {
        if (st.suffixStart[i] >= 0) ++leafCount;
    }
    steps.push_back(dsDiagramStep(1,
        "Done: " + S(leafCount) +
            " leaves, one per suffix. Any substring of the text is a path from the root. Built here the simple way in O(n^2); Ukkonen's algorithm builds it in O(n). Space is O(n) nodes (but the suffix ARRAY is much smaller in practice).",
        {{"leaves", S(leafCount)}}, dsDrawTree(st.toTreeDiagram(), false, "suffix tree"), {}));
    return steps;
}

static Steps simSuffixTreeSearch(const string& input) {
    pair<string, string> parts = dsSplitPipe(input);
    string text = dsLowerAlnum(parts.first, 7);
    string pattern = dsLowerAlnum(parts.second, 5);
    if (text.size() < 2) text = "banana";
    if (pattern.empty()) pattern = "ana";
    text += "$";
    SuffixTreePool st;
    for (int i = 0; i < (int)text.size(); ++i) st.insertSuffix(text.substr(i), i);
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "Is \"" + pattern + "\" a substring of \"" + text.substr(0, text.size() - 1) +
            "\"? Walk the pattern down from the root: O(length of pattern), no matter how long the text is.",
        {{"pattern", pattern}}, dsDrawTree(st.toTreeDiagram(), false, "start at the root"), {"t0"}));
    int cur = 0;
    size_t pos = 0;
    while (pos < pattern.size()) {
        int found = -1;
        for (size_t i = 0; i < st.children[cur].size(); ++i) {
            int c = st.children[cur][i];
            if (st.edgeLabel[c][0] == pattern[pos]) {
                found = c;
                break;
            }
        }
        if (found < 0) {
            steps.push_back(dsDiagramStep(2,
                "No edge starts with '" + string(1, pattern[pos]) + "': the pattern is NOT in the text.",
                {}, dsDrawTree(st.toTreeDiagram(), false, "not found"), {"t" + S(cur)}));
            return steps;
        }
        size_t matchLen = 0;
        while (matchLen < st.edgeLabel[found].size() && pos + matchLen < pattern.size() &&
               st.edgeLabel[found][matchLen] == pattern[pos + matchLen]) {
            ++matchLen;
        }
        if (pos + matchLen < pattern.size() && matchLen < st.edgeLabel[found].size()) {
            steps.push_back(dsDiagramStep(3,
                "Edge \"" + st.edgeLabel[found] + "\" differs from the pattern after " + S(matchLen) + " letter(s): NOT FOUND.",
                {}, dsDrawTree(st.toTreeDiagram(), false, "mismatch"), {"t" + S(found)}));
            return steps;
        }
        steps.push_back(dsDiagramStep(2,
            "Follow the edge \"" + st.edgeLabel[found] + "\": " +
                (pos + matchLen >= pattern.size() ? "the whole pattern is matched (possibly inside this edge)."
                                                  : "matched, continue below."),
            {{"matched", pattern.substr(0, min(pattern.size(), pos + matchLen))}},
            dsDrawTree(st.toTreeDiagram(), false, "following edges"), {"t" + S(found), "e" + S(found)}));
        pos += matchLen;
        cur = found;
    }
    vector<int> leafStarts;
    dsCollectSuffixLeaves(st, cur, leafStarts);
    sort(leafStarts.begin(), leafStarts.end());
    string occText = dsJoinInts(leafStarts);
    steps.push_back(dsDiagramStep(4,
        "Pattern found. Every LEAF below this point is one occurrence; their suffix numbers are the start positions: " +
            occText + ". The number of occurrences = the number of leaves below.",
        {{"occurrences", occText}}, dsDrawTree(st.toTreeDiagram(), false, "occurrences = leaves below"), {"t" + S(cur)}));
    return steps;
}

void addDS7b(map<string, Sim>& m);

void addDS7(map<string, Sim>& m) {
    addDS7b(m);

    vector<string> intervalRefs = {
        "GeeksforGeeks: Interval tree|https://www.geeksforgeeks.org/interval-tree/",
        "Wikipedia: Interval tree|https://en.wikipedia.org/wiki/Interval_tree",
        "VisuAlgo: BST|https://visualgo.net/en/bst"};
    vector<string> intervalPractice = {
        "LeetCode #253 (Meeting Rooms II)", "LeetCode #56 (Merge Intervals)",
        "LeetCode #729 (My Calendar I)", "LeetCode #57 (Insert Interval)"};
    dsRegisterIntSim(m, "Interval Tree", "Intervals as pairs: low high low high ... (up to 6)",
                     "15, 20, 10, 30, 17, 19, 5, 20, 12, 15, 30, 40",
                     {"a BST ordered by the interval's LOW end;",
                      "every node also stores  max = largest HIGH end in its subtree;",
                      "insert(low, high):  ordinary BST insert by low;",
                      "    then walk back up:  node->max = max(node->max, high);"},
                     intervalRefs, intervalPractice, simIntervalTreeInsert);
    dsRegisterIntSim(m, "Interval Tree#1", "Query low, query high, then intervals",
                     "14, 16, 15, 20, 10, 30, 17, 19, 5, 20, 12, 15, 30, 40",
                     {"overlaps(a, b) :  a.low <= b.high  &&  b.low <= a.high;",
                      "search(node, q):", "    if node overlaps q:  return node;",
                      "    if (node->left != NULL && node->left->max >= q.low)  go left;",
                      "    else  go right;"},
                     intervalRefs, intervalPractice, simIntervalTreeSearch);
    dsSetProgramNames("Interval Tree", {"Insert (BST by low end, track max)", "Find an overlapping interval"});

    dsRegisterIntSim(m, "Segment Tree", "Up to 8 numbers", "2, 5, 1, 4, 9, 3, 7, 6",
                     {"build(node, l, r):", "    if (l == r)  tree[node] = a[l];",
                      "    else {  build(left, l, mid);  build(right, mid + 1, r);",
                      "            tree[node] = tree[left] + tree[right];  }      // combine"},
                     dsSegmentTreeRefs(),
                     {"LeetCode #307 (Range Sum Query - Mutable)", "LeetCode #218 (The Skyline Problem)",
                      "LeetCode #315 (Count of Smaller Numbers After Self)", "LeetCode #327 (Count of Range Sum)"},
                     simSegmentTreeBuild);
    dsRegisterIntSim(m, "Segment Tree#1", "Range low, range high, then up to 8 numbers", "2, 6, 2, 5, 1, 4, 9, 3, 7, 6",
                     {"query(node, l, r, ql, qr):", "    if [l, r] is outside [ql, qr]:      return 0;",
                      "    if [l, r] is inside  [ql, qr]:      return tree[node];",
                      "    return query(left, ...) + query(right, ...);       // partial overlap: ask both children"},
                     dsSegmentTreeRefs(),
                     {"LeetCode #307 (Range Sum Query - Mutable)", "LeetCode #303 (Range Sum Query - Immutable)",
                      "LeetCode #1157 (Online Majority Element In Subarray)"},
                     simSegmentTreeQuery);
    dsRegisterIntSim(m, "Segment Tree#2", "Index, new value, then up to 8 numbers", "3, 10, 2, 5, 1, 4, 9, 3, 7, 6",
                     {"update(node, l, r, idx, val):", "    if (l == r)  { tree[node] = val;  return; }",
                      "    update the child that contains idx;",
                      "    tree[node] = tree[left] + tree[right];      // recompute on the way back up"},
                     dsSegmentTreeRefs(),
                     {"LeetCode #307 (Range Sum Query - Mutable)", "LeetCode #308 (Range Sum Query 2D - Mutable)",
                      "LeetCode #2286 (Booking Concert Tickets in Groups)"},
                     simSegmentTreeUpdate);
    dsSetProgramNames("Segment Tree", {"Build the tree (sums)", "Range sum query", "Point update"});
}

void addDS7b(map<string, Sim>& m) {
    vector<string> strPractice = {
        "LeetCode #28 (Find the Index of the First Occurrence in a String)",
        "LeetCode #459 (Repeated Substring Pattern)",
        "LeetCode #214 (Shortest Palindrome)",
        "LeetCode #125 (Valid Palindrome)"};
    dsRegisterTextSim(m, "String Data Structures", "text|pattern (up to 14 and 5 letters or digits)", "abxabcabcaby|abcaby",
                      {"for (i = 0; i + m <= n; i++) {            // try every starting position",
                       "    for (j = 0; j < m && text[i + j] == pat[j]; j++) ;",
                       "    if (j == m)  report i;", "}"},
                      dsStringRefs(), strPractice, simStringNaiveSearch);
    dsRegisterTextSim(m, "String Data Structures#1", "A pattern (up to 10 letters)", "abcabcd",
                      {"lps[0] = 0;  len = 0;  i = 1;", "while (i < m) {",
                       "    if (p[i] == p[len])  lps[i++] = ++len;",
                       "    else if (len > 0)    len = lps[len - 1];       // fall back, i does not move",
                       "    else                 lps[i++] = 0;", "}"},
                      dsStringRefs(),
                      {"LeetCode #28 (Find the Index of the First Occurrence in a String)",
                       "LeetCode #1392 (Longest Happy Prefix)", "LeetCode #459 (Repeated Substring Pattern)"},
                      simStringKmpLps);
    dsRegisterTextSim(m, "String Data Structures#2", "A word (up to 10 letters)", "racecar",
                      {"i = 0;  j = n - 1;", "while (i < j) {", "    swap(s[i], s[j]);", "    i++;  j--;", "}",
                       "palindrome check: compare s[i] with s[j] instead of swapping"},
                      dsStringRefs(),
                      {"LeetCode #344 (Reverse String)", "LeetCode #125 (Valid Palindrome)",
                       "LeetCode #5 (Longest Palindromic Substring)", "LeetCode #647 (Palindromic Substrings)"},
                      simStringTwoPointers);
    dsSetProgramNames("String Data Structures", {"Naive pattern search", "KMP prefix function (lps)", "Two pointers: reverse and palindrome"});

    vector<string> trieRefs = {
        "VisuAlgo / USFCA: trie|https://www.cs.usfca.edu/~galles/visualization/Trie.html",
        "GeeksforGeeks: Trie|https://www.geeksforgeeks.org/trie-insert-and-search/",
        "Wikipedia: Trie|https://en.wikipedia.org/wiki/Trie"};
    vector<string> triePractice = {
        "LeetCode #208 (Implement Trie - Prefix Tree)", "LeetCode #211 (Design Add and Search Words)",
        "LeetCode #212 (Word Search II)", "LeetCode #648 (Replace Words)"};
    dsRegisterTextSim(m, "Tries", "Words separated by spaces (up to 6 words, 6 letters)", "apple app apply apt bat bar",
                      {"insert(word):", "    node = root;", "    for (each letter c of word) {",
                       "        if (node has no child c)  create it;", "        node = child c;", "    }",
                       "    mark node as END OF WORD;"},
                      trieRefs, triePractice, simTrieInsert);
    dsRegisterTextSim(m, "Tries#1", "words | queries   e.g.  apple app apply bat | app appl bay",
                      "apple app apply apt bat bar | app appl bay",
                      {"search(word):", "    walk down the letters of word;",
                       "    if a letter is missing:  return not found;",
                       "    return node is END OF WORD ? \"word\" : \"only a prefix\";",
                       "startsWith(prefix):  same walk, but no need for the end mark"},
                      trieRefs, triePractice, simTrieSearch);
    dsRegisterTextSim(m, "Tries#2", "words | prefix   e.g.  apple app apply apt bat | ap",
                      "apple app apply apt bat bar | ap",
                      {"autocomplete(prefix):", "    walk down the prefix;  if missing: return nothing;",
                       "    DFS from that node:",
                       "        whenever a node is END OF WORD, output the letters on the path"},
                      trieRefs,
                      {"LeetCode #1268 (Search Suggestions System)", "LeetCode #642 (Design Search Autocomplete System)",
                       "LeetCode #208 (Implement Trie)"},
                      simTrieAutocomplete);
    dsSetProgramNames("Tries", {"Insert words (shared prefixes)", "Search a word / check a prefix", "Autocomplete"});

    vector<string> suffixArrRefs = {
        "cp-algorithms: suffix array|https://cp-algorithms.com/string/suffix-array.html",
        "GeeksforGeeks: Suffix array|https://www.geeksforgeeks.org/suffix-array-set-1-introduction/",
        "Wikipedia: Suffix array|https://en.wikipedia.org/wiki/Suffix_array"};
    vector<string> suffixArrPractice = {
        "LeetCode #1044 (Longest Duplicate Substring)", "LeetCode #1062 (Longest Repeating Substring)",
        "LeetCode #214 (Shortest Palindrome)", "LeetCode #28 (Find the Index of the First Occurrence)"};
    dsRegisterTextSim(m, "Suffix Array", "A text (up to 8 letters)", "banana",
                      {"list all suffixes of the text;", "sort them alphabetically;",
                       "SA[k] = start position of the k-th smallest suffix;",
                       "LCP[k] = length of the common prefix of the suffixes SA[k-1] and SA[k]"},
                      suffixArrRefs, suffixArrPractice, simSuffixArrayBuild);
    dsRegisterTextSim(m, "Suffix Array#1", "text | pattern   e.g.  banana|ana", "banana|ana",
                      {"lo = 0;  hi = n;", "while (lo < hi) {", "    mid = (lo + hi) / 2;",
                       "    if (suffix(SA[mid]) < pattern)  lo = mid + 1;   else  hi = mid;", "}",
                       "occurrences = consecutive suffixes from lo that start with the pattern"},
                      suffixArrRefs, suffixArrPractice, simSuffixArraySearch);
    dsSetProgramNames("Suffix Array", {"Build SA and LCP", "Pattern search by binary search"});

    vector<string> suffixTreeRefs = {
        "VisuAlgo: suffix tree|https://visualgo.net/en/suffixtree",
        "GeeksforGeeks: Suffix tree|https://www.geeksforgeeks.org/suffix-tree-application-1-substring-check/",
        "Wikipedia: Suffix tree|https://en.wikipedia.org/wiki/Suffix_tree"};
    dsRegisterTextSim(m, "Suffix Tree", "A text (up to 7 letters; a $ end mark is added)", "banana",
                      {"for each suffix of text + '$', longest first:",
                       "    walk down from the root following matching letters;",
                       "    if the path ends in the MIDDLE of an edge:  split the edge;",
                       "    attach the rest of the suffix as a new leaf;"},
                      suffixTreeRefs,
                      {"LeetCode #1044 (Longest Duplicate Substring)", "LeetCode #28 (Find the Index of the First Occurrence)",
                       "LeetCode #1923 (Longest Common Subpath)"},
                      simSuffixTreeBuild);
    dsRegisterTextSim(m, "Suffix Tree#1", "text | pattern   e.g.  banana|ana", "banana|ana",
                      {"node = root;", "for each letter of the pattern:",
                       "    follow the edge that starts with it;  compare along the edge;",
                       "    if a letter differs:  return not found;",
                       "occurrences = suffix numbers of all LEAVES below where we stopped"},
                      suffixTreeRefs,
                      {"LeetCode #28 (Find the Index of the First Occurrence)",
                       "LeetCode #1044 (Longest Duplicate Substring)", "LeetCode #686 (Repeated String Match)"},
                      simSuffixTreeSearch);
    dsSetProgramNames("Suffix Tree", {"Build by inserting every suffix", "Search a pattern (walk the edges)"});
}
