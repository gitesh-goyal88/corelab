// Data Structures (Semester 3), part 4: Module 3 (multi list, trees, heaps).
#pragma once
#include <deque>
#include "ds3.hpp"

using namespace std;

class TreeDiagram {
public:
    vector<int> key;
    vector<vector<int>> ch;
    vector<string> lab;
    vector<string> cls;
    int root;

    TreeDiagram() {
        root = -1;
    }

    int addNode(int keyVal, vector<int> childList = {}) {
        key.push_back(keyVal);
        ch.push_back(childList);
        lab.push_back("");
        cls.push_back("");
        return (int)key.size() - 1;
    }
};

static void layoutTreeRecursive(const TreeDiagram& tree, bool isBinary, int nodeIdx, int depth,
                                double& rankCounter, vector<double>& xPos, vector<int>& depthArr) {
    if (depth > 40) return;
    depthArr[nodeIdx] = depth;
    if (isBinary) {
        int leftChild = tree.ch[nodeIdx].size() > 0 ? tree.ch[nodeIdx][0] : -1;
        int rightChild = tree.ch[nodeIdx].size() > 1 ? tree.ch[nodeIdx][1] : -1;
        if (leftChild >= 0) layoutTreeRecursive(tree, isBinary, leftChild, depth + 1, rankCounter, xPos, depthArr);
        xPos[nodeIdx] = rankCounter++;
        if (rightChild >= 0) layoutTreeRecursive(tree, isBinary, rightChild, depth + 1, rankCounter, xPos, depthArr);
    } else {
        bool isLeaf = true;
        double firstChildX = 0.0;
        double lastChildX = 0.0;
        bool hasFirst = false;
        for (size_t i = 0; i < tree.ch[nodeIdx].size(); ++i) {
            int childIdx = tree.ch[nodeIdx][i];
            if (childIdx >= 0) {
                layoutTreeRecursive(tree, isBinary, childIdx, depth + 1, rankCounter, xPos, depthArr);
                if (!hasFirst) {
                    firstChildX = xPos[childIdx];
                    hasFirst = true;
                }
                lastChildX = xPos[childIdx];
                isLeaf = false;
            }
        }
        xPos[nodeIdx] = isLeaf ? (rankCounter++) : ((firstChildX + lastChildX) / 2.0);
    }
}

static void layoutTree(const TreeDiagram& tree, bool isBinary, vector<double>& xPos, vector<int>& depthArr) {
    int count = (int)tree.key.size();
    xPos.assign(count, 0.0);
    depthArr.assign(count, 0);
    double rankCounter = 0.0;
    if (tree.root >= 0) {
        layoutTreeRecursive(tree, isBinary, tree.root, 0, rankCounter, xPos, depthArr);
    }
}

static void markReachableTreeNodes(const TreeDiagram& tree, int nodeIdx, int totalCount, vector<char>& visited) {
    if (nodeIdx < 0 || nodeIdx >= totalCount || visited[nodeIdx]) return;
    visited[nodeIdx] = 1;
    for (size_t i = 0; i < tree.ch[nodeIdx].size(); ++i) {
        markReachableTreeNodes(tree, tree.ch[nodeIdx][i], totalCount, visited);
    }
}

static string dsNodeDisplayLabel(const TreeDiagram& tree, int nodeIdx) {
    return tree.lab[nodeIdx].empty() ? S(tree.key[nodeIdx]) : tree.lab[nodeIdx];
}

static double dsNodeBoxWidth(const TreeDiagram& tree, int nodeIdx) {
    string label = dsNodeDisplayLabel(tree, nodeIdx);
    return label.size() > 3 ? (label.size() * 7.0 + 12.0) : 32.0;
}

static string dsDrawTree(const TreeDiagram& tree, bool isBinary, const string& caption,
                         const vector<pair<int, int>>& threads = {}, bool hideVirtualRoot = false,
                         const vector<pair<int, int>>& leafLinks = {}) {
    Sk canvas;
    vector<double> xPos;
    vector<int> depthArr;
    layoutTree(tree, isBinary, xPos, depthArr);

    int count = (int)tree.key.size();
    double maxX = 0.0;
    int maxDepth = hideVirtualRoot ? 1 : 0;
    vector<char> reachable(count, 0);
    if (tree.root >= 0) {
        markReachableTreeNodes(tree, tree.root, count, reachable);
    }

    int startNode = hideVirtualRoot ? 1 : 0;
    for (int i = startNode; i < count; ++i) {
        if (reachable[i]) {
            maxX = max(maxX, xPos[i]);
            maxDepth = max(maxDepth, depthArr[i]);
        }
    }

    double maxLabelW = 32.0;
    for (int i = startNode; i < count; ++i) {
        if (reachable[i]) {
            string label = dsNodeDisplayLabel(tree, i);
            if (label.size() > 3) {
                maxLabelW = max(maxLabelW, label.size() * 7.0 + 12.0);
            }
        }
    }

    double stepX = max(min(56.0, 500.0 / (maxX + 1.0)), maxLabelW + 10.0);
    double originX = 36.0 + (maxLabelW > 40.0 ? (maxLabelW - 32.0) / 2.0 : 0.0);
    double stepY = 54.0;
    double originY = hideVirtualRoot ? (28.0 - stepY) : 28.0;

    for (int u = 0; u < count; ++u) {
        if ((hideVirtualRoot && u == 0) || !reachable[u]) continue;
        double ux = originX + xPos[u] * stepX;
        double uy = originY + depthArr[u] * stepY;
        for (size_t i = 0; i < tree.ch[u].size(); ++i) {
            int c = tree.ch[u][i];
            if (c >= 0 && c < count) {
                string edgeId = "e" + S(c);
                canvas.W(edgeId.c_str(), {ux, uy, originX + xPos[c] * stepX, originY + depthArr[c] * stepY});
            }
        }
    }

    for (int i = startNode; i < count; ++i) {
        if (!reachable[i]) continue;
        string nodeId = "t" + S(i);
        string label = dsNodeDisplayLabel(tree, i);
        string style = tree.cls[i].empty() ? "steel2" : tree.cls[i];
        bool useLightText = (style == "dark" || style == "red" || style == "blue");
        double px = originX + xPos[i] * stepX;
        double py = originY + depthArr[i] * stepY;
        if (label.size() > 3) {
            double boxW = label.size() * 7.0 + 12.0;
            canvas.Rect(nodeId.c_str(), px - boxW / 2.0, py - 13.0, boxW, 26.0, style.c_str());
        } else {
            canvas.K(nodeId.c_str(), px, py, 16, style.c_str());
        }
        canvas.T(px, py, label, useLightText ? "mlight" : "middle");
    }

    for (size_t i = 0; i < leafLinks.size(); ++i) {
        int a = leafLinks[i].first;
        int b = leafLinks[i].second;
        if (a < count && b < count && reachable[a] && reachable[b]) {
            string nextId = "nx" + S(a);
            double ax = originX + xPos[a] * stepX + dsNodeBoxWidth(tree, a) / 2.0;
            double ay = originY + depthArr[a] * stepY;
            double bx = originX + xPos[b] * stepX - dsNodeBoxWidth(tree, b) / 2.0;
            double by = originY + depthArr[b] * stepY;
            canvas.Arr(ax, ay, bx, by, nextId.c_str());
        }
    }

    double extraBottom = 0.0;
    for (size_t i = 0; i < threads.size(); ++i) {
        int fromIdx = threads[i].first;
        int toIdx = threads[i].second;
        if (fromIdx != toIdx && fromIdx < count && toIdx < count) {
            double x1 = originX + xPos[fromIdx] * stepX + 8.0;
            double y1 = originY + depthArr[fromIdx] * stepY + 16.0;
            double x2 = originX + xPos[toIdx] * stepX;
            double y2 = originY + depthArr[toIdx] * stepY + 16.0;
            double yMid = max(y1, y2) + 24.0;
            string threadId = "th" + S(fromIdx);
            canvas.W(threadId.c_str(), {x1, y1, x1, yMid, x2, yMid});
            canvas.Arr(x2, yMid, x2, y2, threadId.c_str());
            extraBottom = 26.0;
        }
    }

    double width = max(300.0, originX + maxX * stepX + 40.0 + (maxLabelW > 40.0 ? maxLabelW / 2.0 : 0.0));
    double height = 28.0 + (maxDepth - (hideVirtualRoot ? 1 : 0)) * stepY + 50.0 + extraBottom;
    canvas.T(width / 2.0, height - 10.0, caption);
    return canvas.out((int)width, (int)height + 6, {});
}

static TreeDiagram dsBuildLevelTree(const Ints& values) {
    TreeDiagram tree;
    if (values.empty() || values[0] == -1) return tree;
    tree.root = tree.addNode(values[0], {-1, -1});
    deque<int> queueNodes;
    queueNodes.push_back(tree.root);
    size_t idx = 1;
    while (!queueNodes.empty() && idx < values.size()) {
        int parentIdx = queueNodes.front();
        queueNodes.pop_front();
        for (int side = 0; side < 2 && idx < values.size(); ++side, ++idx) {
            if (values[idx] != -1) {
                int childIdx = tree.addNode(values[idx], {-1, -1});
                tree.ch[parentIdx][side] = childIdx;
                queueNodes.push_back(childIdx);
            }
        }
    }
    return tree;
}

static TreeDiagram dsBuildHeapTree(const Ints& arr, int count) {
    TreeDiagram tree;
    for (int i = 0; i < count; ++i) {
        int leftChild = (2 * i + 1 < count) ? (2 * i + 1) : -1;
        int rightChild = (2 * i + 2 < count) ? (2 * i + 2) : -1;
        tree.addNode(arr[i], {leftChild, rightChild});
    }
    if (count > 0) tree.root = 0;
    return tree;
}

static const vector<string>& dsTreeRefs() {
    static const vector<string> refs = {
        "VisuAlgo: binary tree / BST|https://visualgo.net/en/bst",
        "GeeksforGeeks: Tree data structure|https://www.geeksforgeeks.org/introduction-to-tree-data-structure-and-algorithm-tutorials/",
        "cp-algorithms|https://cp-algorithms.com/"};
    return refs;
}

static const vector<string>& dsHeapRefs() {
    static const vector<string> refs = {
        "VisuAlgo: binary heap|https://visualgo.net/en/heap",
        "GeeksforGeeks: Heap data structure|https://www.geeksforgeeks.org/heap-data-structure/",
        "cppreference: std::priority_queue|https://en.cppreference.com/w/cpp/container/priority_queue"};
    return refs;
}

class HeapForest {
public:
    vector<int> key;
    vector<int> par;
    vector<int> deg;
    vector<vector<int>> ch;
    vector<char> mark;
    vector<char> dead;
    vector<int> roots;

    int addNode(int keyVal) {
        key.push_back(keyVal);
        par.push_back(-1);
        deg.push_back(0);
        ch.push_back({});
        mark.push_back(0);
        dead.push_back(0);
        return (int)key.size() - 1;
    }

    void linkTrees(int childIdx, int parentIdx) {
        par[childIdx] = parentIdx;
        ch[parentIdx].push_back(childIdx);
        deg[parentIdx]++;
        mark[childIdx] = 0;
    }
};

static string dsDrawForest(const HeapForest& forest, const string& caption, int minRootIdx = -1) {
    TreeDiagram tree;
    tree.addNode(0, {});
    tree.root = 0;
    for (size_t i = 0; i < forest.key.size(); ++i) {
        tree.addNode(forest.key[i], {});
    }
    for (size_t i = 0; i < forest.key.size(); ++i) {
        tree.ch[i + 1] = {};
        for (size_t cIdx = 0; cIdx < forest.ch[i].size(); ++cIdx) {
            tree.ch[i + 1].push_back(forest.ch[i][cIdx] + 1);
        }
        tree.cls[i + 1] = forest.mark[i] ? "red" : "";
        if ((int)i == minRootIdx) tree.cls[i + 1] = "weld";
        if (forest.dead[i]) tree.ch[i + 1] = {};
    }
    for (size_t i = 0; i < forest.roots.size(); ++i) {
        tree.ch[0].push_back(forest.roots[i] + 1);
    }
    return dsDrawTree(tree, false, caption, {}, true);
}

static string dsDrawMultiList(const Ints& subSizes, const vector<vector<char>>& subVisited,
                              const vector<char>& headVisited, const string& caption) {
    Sk canvas;
    int headCount = (int)subSizes.size();
    int maxSubSize = 0;
    for (int i = 0; i < headCount; ++i) {
        maxSubSize = max(maxSubSize, subSizes[i]);
    }
    for (int i = 0; i < headCount; ++i) {
        double x = 30.0 + i * 84.0;
        string headId = "h" + S(i);
        if (i + 1 < headCount) {
            string linkId = "hl" + S(i);
            canvas.Arr(x + 44, 38, x + 84, 38, linkId.c_str());
        }
        canvas.Rect(headId.c_str(), x, 24, 44, 28, headVisited[i] ? "weld" : "steel2");
        canvas.T(x + 22, 38, S(10 * (i + 1)));
        for (int j = 0; j < subSizes[i]; ++j) {
            double y = 80.0 + j * 46.0;
            string subId = "s" + S(i) + "_" + S(j);
            string arrowId = "d" + S(i) + "_" + S(j);
            canvas.Arr(x + 22, j == 0 ? 52 : y - 18, x + 22, y, arrowId.c_str());
            canvas.Rect(subId.c_str(), x, y, 44, 28, subVisited[i][j] ? "weld" : "steel");
            canvas.T(x + 22, y + 14, S(10 * (i + 1) + j + 1));
        }
        if (i == headCount - 1) {
            canvas.Arr(x + 44, 38, x + 62, 38, "");
            canvas.T(x + 66, 38, "NULL", "start");
        }
    }
    double width = max(300.0, 30.0 + headCount * 84.0 + 50.0);
    double height = 80.0 + maxSubSize * 46.0 + 30.0;
    canvas.T(width / 2.0, height - 10.0, caption);
    return canvas.out((int)width, (int)height, {});
}

static Step dsHeapStep(int lineNum, const string& noteText, Vars varsList, const Ints& heapArr,
                       int activeSize, int highlightIdx, vector<string> tags, vector<string> highlightIds,
                       const string& caption, int rangeLo = -1, int rangeHi = -1, const char* arrName = "heap") {
    Ints visible(heapArr.begin(), heapArr.begin() + activeSize);
    Step step = dsArrayStep(lineNum, noteText, varsList, visible, highlightIdx, tags, rangeLo, rangeHi, arrName);
    step.dg = dsDrawTree(dsBuildHeapTree(heapArr, activeSize), true, caption);
    step.hl = highlightIds;
    return step;
}

// ==================== Module 3 Simulations ====================

static Steps simMultiList(const Ints& input) {
    Ints sizes = dsTakeInts(input, 0, 5, {2, 0, 3, 1});
    for (size_t i = 0; i < sizes.size(); ++i) {
        sizes[i] = clamp(abs(sizes[i]), 0, 4);
    }
    int headCount = (int)sizes.size();
    vector<vector<char>> subVisited(headCount);
    for (int i = 0; i < headCount; ++i) {
        subVisited[i].assign(sizes[i], 0);
    }
    vector<char> headVisited(headCount, 0);
    Steps steps;
    string output;
    vector<string> callStack;

    steps.push_back(dsDiagramStep(0, "A multi list is a linked list whose nodes may also point DOWN to another list. Orange = already visited.",
                                  {{"heads", S(headCount)}},
                                  dsDrawMultiList(sizes, subVisited, headVisited, "each head may own a sub-list"), {}));
    for (int i = 0; i < headCount; ++i) {
        callStack.push_back("visit(list " + S(i + 1) + ")");
        headVisited[i] = 1;
        if (!output.empty()) output += ", ";
        output += S(10 * (i + 1));

        Step stepA = dsDiagramStep(2, "Visit head " + S(10 * (i + 1)) + ".", {{"output", output}},
                                   dsDrawMultiList(sizes, subVisited, headVisited, "visiting a head"), {"h" + S(i)});
        stepA.stack = callStack;
        steps.push_back(stepA);

        if (sizes[i] > 0) {
            Step stepB = dsDiagramStep(3, "This node has a down pointer: finish its whole sub-list before moving right.",
                                       {{"output", output}},
                                       dsDrawMultiList(sizes, subVisited, headVisited, "follow the down pointer"),
                                       {"d" + S(i) + "_0"});
            stepB.stack = callStack;
            steps.push_back(stepB);
        }
        for (int j = 0; j < sizes[i]; ++j) {
            subVisited[i][j] = 1;
            output += ", " + S(10 * (i + 1) + j + 1);
            Step stepC = dsDiagramStep(2, "Visit sub-list node " + S(10 * (i + 1) + j + 1) + ".",
                                       {{"output", output}},
                                       dsDrawMultiList(sizes, subVisited, headVisited, "inside the sub-list"),
                                       {"s" + S(i) + "_" + S(j)});
            stepC.stack = callStack;
            steps.push_back(stepC);
        }
        callStack.pop_back();
        if (i + 1 < headCount) {
            Step stepD = dsDiagramStep(1, "Sub-list done (back on the main list). Move right with next.",
                                       {{"output", output}},
                                       dsDrawMultiList(sizes, subVisited, headVisited, "p = p->next"),
                                       {"hl" + S(i)});
            stepD.stack = callStack;
            steps.push_back(stepD);
        }
    }
    steps.push_back(dsDiagramStep(1, "End of the main list. The output reads head, its sub-list, next head ... a depth-first order, i.e. the flattened list.",
                                  {{"output", output}},
                                  dsDrawMultiList(sizes, subVisited, headVisited, "flattened order"), {}));
    return steps;
}

static Steps simBinaryTreeBuild(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 12, {1, 2, 3, 4, 5, -1, 6});
    if (arr[0] == -1) arr[0] = 1;
    TreeDiagram tree;
    tree.root = tree.addNode(arr[0], {-1, -1});
    deque<int> q;
    q.push_back(tree.root);

    Steps steps;
    steps.push_back(dsDiagramStep(0, "A binary tree node has at most TWO children: left and right. The first value is the root.",
                                  {{"root", S(arr[0])}}, dsDrawTree(tree, true, "root"), {"t0"}));
    size_t idx = 1;
    while (!q.empty() && idx < arr.size()) {
        int u = q.front();
        q.pop_front();
        for (int side = 0; side < 2 && idx < arr.size(); ++side, ++idx) {
            int lineNum = side ? 4 : 3;
            if (arr[idx] == -1) {
                steps.push_back(dsDiagramStep(lineNum, string(side ? "Right" : "Left") + " child of " + S(tree.key[u]) + " is -1: no node here.",
                                              {{"parent", S(tree.key[u])}}, dsDrawTree(tree, true, "empty slot"), {"t" + S(u)}));
                continue;
            }
            int childIdx = tree.addNode(arr[idx], {-1, -1});
            tree.ch[u][side] = childIdx;
            q.push_back(childIdx);
            steps.push_back(dsDiagramStep(lineNum, S(arr[idx]) + " becomes the " + (side ? "right" : "left") + " child of " + S(tree.key[u]) + ".",
                                          {{"parent", S(tree.key[u])}, {"new node", S(arr[idx])}},
                                          dsDrawTree(tree, true, "filled level by level"), {"t" + S(childIdx), "e" + S(childIdx)}));
        }
    }
    steps.push_back(dsDiagramStep(0, "Tree built: " + S((int)tree.key.size()) + " nodes. Each level is filled from left to right.",
                                  {{"nodes", S((int)tree.key.size())}}, dsDrawTree(tree, true, "binary tree"), {}));
    return steps;
}

static int traceBinaryHeightRecursive(TreeDiagram& tree, int nodeIdx, Steps& steps, vector<string>& callStack,
                                      int& visitedCount, int& leafCount) {
    if (nodeIdx < 0) return 0;
    callStack.push_back("height(" + S(tree.key[nodeIdx]) + ")");
    int leftH = traceBinaryHeightRecursive(tree, tree.ch[nodeIdx][0], steps, callStack, visitedCount, leafCount);
    int rightH = traceBinaryHeightRecursive(tree, tree.ch[nodeIdx][1], steps, callStack, visitedCount, leafCount);
    int res = 1 + max(leftH, rightH);
    ++visitedCount;
    bool isLeaf = (tree.ch[nodeIdx][0] < 0 && tree.ch[nodeIdx][1] < 0);
    if (isLeaf) ++leafCount;
    tree.lab[nodeIdx] = S(tree.key[nodeIdx]) + ":" + S(res);
    tree.cls[nodeIdx] = "weld";
    Step step = dsDiagramStep(2, "Node " + S(tree.key[nodeIdx]) + ": left height " + S(leftH) + ", right height " + S(rightH) + " -> 1 + max = " + S(res) +
                              (isLeaf ? ". It is a leaf (no children)." : "."),
                              {{"height(" + S(tree.key[nodeIdx]) + ")", S(res)}, {"leaves so far", S(leafCount)}},
                              dsDrawTree(tree, true, "label = key:height"), {"t" + S(nodeIdx)});
    step.stack = callStack;
    steps.push_back(step);
    callStack.pop_back();
    return res;
}

static Steps simBinaryTreeHeight(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 12, {1, 2, 3, 4, 5, -1, 6});
    TreeDiagram tree = dsBuildLevelTree(arr);
    if (tree.root < 0) tree = dsBuildLevelTree({1, 2, 3});
    Steps steps;
    vector<string> callStack;
    int leafCount = 0;
    int visitedCount = 0;

    steps.push_back(dsDiagramStep(0, "Height = number of nodes on the longest root-to-leaf path. We compute it from the bottom up.",
                                  {}, dsDrawTree(tree, true, "height(u) = 1 + max(height(left), height(right))"), {}));
    int height = traceBinaryHeightRecursive(tree, tree.root, steps, callStack, visitedCount, leafCount);
    steps.push_back(dsDiagramStep(2, "Height of the tree = " + S(height) + ", nodes = " + S(visitedCount) + ", leaves = " + S(leafCount) +
                                  ". Every node is visited once: O(n) time, O(height) stack space.",
                                  {{"height", S(height)}, {"nodes", S(visitedCount)}, {"leaves", S(leafCount)}},
                                  dsDrawTree(tree, true, "done"), {}));
    return steps;
}

static void traceInvertTreeRecursive(TreeDiagram& tree, int nodeIdx, Steps& steps, vector<string>& callStack) {
    if (nodeIdx < 0) return;
    callStack.push_back("invert(" + S(tree.key[nodeIdx]) + ")");
    swap(tree.ch[nodeIdx][0], tree.ch[nodeIdx][1]);
    tree.cls[nodeIdx] = "weld";
    Step step = dsDiagramStep(2, "Swap the children of " + S(tree.key[nodeIdx]) + ".",
                              {{"node", S(tree.key[nodeIdx])}},
                              dsDrawTree(tree, true, "swapped (orange = done)"), {"t" + S(nodeIdx)});
    step.stack = callStack;
    steps.push_back(step);
    traceInvertTreeRecursive(tree, tree.ch[nodeIdx][0], steps, callStack);
    traceInvertTreeRecursive(tree, tree.ch[nodeIdx][1], steps, callStack);
    callStack.pop_back();
}

static Steps simBinaryTreeInvert(const Ints& input) {
    TreeDiagram tree = dsBuildLevelTree(dsTakeInts(input, 0, 12, {1, 2, 3, 4, 5, -1, 6}));
    if (tree.root < 0) tree = dsBuildLevelTree({1, 2, 3});
    Steps steps;
    vector<string> callStack;
    steps.push_back(dsDiagramStep(0, "Mirror the tree: every node swaps its left and right child.", {}, dsDrawTree(tree, true, "before"), {}));
    traceInvertTreeRecursive(tree, tree.root, steps, callStack);
    steps.push_back(dsDiagramStep(4, "Every node has been swapped: the tree is mirrored. O(n).", {}, dsDrawTree(tree, true, "after"), {}));
    return steps;
}

// ---------- K-ary Tree ----------

static TreeDiagram buildCompleteKaryTree(const Ints& input, int& arity, int& nodeCount) {
    arity = clamp(input.empty() ? 3 : input[0], 2, 4);
    nodeCount = clamp(input.size() > 1 ? input[1] : 10, 1, 13);
    TreeDiagram tree;
    for (int i = 0; i < nodeCount; ++i) {
        tree.addNode(i + 1, {});
    }
    for (int i = 1; i < nodeCount; ++i) {
        tree.ch[(i - 1) / arity].push_back(i);
    }
    tree.root = 0;
    return tree;
}

static Steps simKaryTreeBuild(const Ints& input) {
    int arity = 3;
    int nodeCount = 10;
    buildCompleteKaryTree(input, arity, nodeCount);
    TreeDiagram tree;
    tree.addNode(1, {});
    tree.root = 0;
    Steps steps;

    steps.push_back(dsDiagramStep(0, "A k-ary tree: every node has at most k = " + S(arity) + " children. Stored level by level in an array, node i (0-based) has children " +
                                  S(arity) + "i+1 .. " + S(arity) + "i+" + S(arity) + ".",
                                  {{"k", S(arity)}, {"n", S(nodeCount)}}, dsDrawTree(tree, false, "root = node 1"), {"t0"}));
    for (int i = 1; i < nodeCount; ++i) {
        int parentIdx = (i - 1) / arity;
        tree.addNode(i + 1, {});
        tree.ch[parentIdx].push_back(i);
        steps.push_back(dsDiagramStep(2, "Node " + S(i + 1) + " (array index " + S(i) + ") has parent index (" + S(i) + " - 1) / " + S(arity) + " = " + S(parentIdx) + ", which is node " + S(parentIdx + 1) + ".",
                                      {{"i", S(i)}, {"parent", S(parentIdx)}},
                                      dsDrawTree(tree, false, "each parent takes up to " + S(arity) + " children"),
                                      {"t" + S(i), "e" + S(i)}));
    }
    int approxHeight = (int)ceil(log((double)(nodeCount * (arity - 1) + 1)) / log((double)arity));
    steps.push_back(dsDiagramStep(0, "Complete " + S(arity) + "-ary tree with " + S(nodeCount) + " nodes. Height about log" + S(arity) + "(n): bigger k means a shorter tree.",
                                  {{"height", S(approxHeight)}}, dsDrawTree(tree, false, "complete k-ary tree"), {}));
    return steps;
}

static void traceKaryPreorder(TreeDiagram& tree, int nodeIdx, Steps& steps, vector<string>& callStack, string& output) {
    callStack.push_back("preorder(" + S(tree.key[nodeIdx]) + ")");
    if (!output.empty()) output += ", ";
    output += S(tree.key[nodeIdx]);
    tree.cls[nodeIdx] = "weld";
    Step step = dsDiagramStep(1, "Visit " + S(tree.key[nodeIdx]) + ".", {{"output", output}},
                              dsDrawTree(tree, false, "orange = visited"), {"t" + S(nodeIdx)});
    step.stack = callStack;
    steps.push_back(step);
    for (size_t i = 0; i < tree.ch[nodeIdx].size(); ++i) {
        traceKaryPreorder(tree, tree.ch[nodeIdx][i], steps, callStack, output);
    }
    callStack.pop_back();
}

static Steps simKaryTreePreorder(const Ints& input) {
    int arity = 3;
    int nodeCount = 10;
    TreeDiagram tree = buildCompleteKaryTree(input, arity, nodeCount);
    Steps steps;
    vector<string> callStack;
    string output;

    steps.push_back(dsDiagramStep(0, "Preorder: visit the node first, then each child subtree from left to right.", {},
                                  dsDrawTree(tree, false, "k-ary tree"), {}));
    traceKaryPreorder(tree, 0, steps, callStack, output);
    steps.push_back(dsDiagramStep(4, "Preorder result: " + output + ". O(n).", {{"output", output}},
                                  dsDrawTree(tree, false, "done"), {}));
    return steps;
}

static Ints queueKeysSnapshot(const deque<int>& queueNodes, const TreeDiagram& tree) {
    Ints keys;
    for (size_t i = 0; i < queueNodes.size(); ++i) {
        keys.push_back(tree.key[queueNodes[i]]);
    }
    return keys;
}

static Steps simKaryTreeLevelOrder(const Ints& input) {
    int arity = 3;
    int nodeCount = 10;
    TreeDiagram tree = buildCompleteKaryTree(input, arity, nodeCount);
    Steps steps;
    deque<int> q;
    q.push_back(0);
    string output;

    Step startStep = dsArrayStep(0, "Level order uses a QUEUE: visit a node, then add its children to the back.",
                                 {}, queueKeysSnapshot(q, tree), -1, {}, -1, -1, "queue");
    startStep.dg = dsDrawTree(tree, false, "k-ary tree");
    steps.push_back(startStep);

    while (!q.empty()) {
        int u = q.front();
        q.pop_front();
        if (!output.empty()) output += ", ";
        output += S(tree.key[u]);
        tree.cls[u] = "weld";
        for (size_t i = 0; i < tree.ch[u].size(); ++i) {
            q.push_back(tree.ch[u][i]);
        }
        Step step = dsArrayStep(2, "Visit " + S(tree.key[u]) + " and queue its " + S((int)tree.ch[u].size()) + " child(ren).",
                                {{"output", output}}, queueKeysSnapshot(q, tree), -1, {}, -1, -1, "queue");
        step.dg = dsDrawTree(tree, false, "orange = visited");
        step.hl = {"t" + S(u)};
        steps.push_back(step);
    }
    steps.push_back(dsDiagramStep(1, "Queue empty. Level order: " + output + ".", {{"output", output}},
                                  dsDrawTree(tree, false, "done"), {}));
    return steps;
}

// ---------- Tree Traversals ----------

static void recordTraversalVisit(TreeDiagram& tree, int nodeIdx, int lineNum, Steps& steps,
                                 const vector<string>& callStack, string& output) {
    if (!output.empty()) output += ", ";
    output += S(tree.key[nodeIdx]);
    tree.cls[nodeIdx] = "weld";
    Step step = dsDiagramStep(lineNum, "Visit " + S(tree.key[nodeIdx]) + " (print it).",
                              {{"output", output}}, dsDrawTree(tree, true, "visited: " + output), {"t" + S(nodeIdx)});
    step.stack = callStack;
    steps.push_back(step);
}

static void traceTreeTraversalRecursive(TreeDiagram& tree, int kind, int nodeIdx, Steps& steps,
                                        vector<string>& callStack, string& output) {
    if (nodeIdx < 0) return;
    const char* fnPrefix = (kind == 0) ? "inorder(" : (kind == 1 ? "preorder(" : "postorder(");
    callStack.push_back(string(fnPrefix) + S(tree.key[nodeIdx]) + ")");
    const char* enterAction = (kind == 0)
        ? ": go LEFT first."
        : (kind == 1 ? ": visit it now, then go left." : ": go LEFT, then RIGHT, visit last.");
    Step enterStep = dsDiagramStep(0, "Call on node " + S(tree.key[nodeIdx]) + enterAction,
                                   {{"output", output}}, dsDrawTree(tree, true, "recursion"), {"t" + S(nodeIdx)});
    enterStep.stack = callStack;
    steps.push_back(enterStep);

    if (kind == 1) recordTraversalVisit(tree, nodeIdx, 1, steps, callStack, output);
    traceTreeTraversalRecursive(tree, kind, tree.ch[nodeIdx][0], steps, callStack, output);
    if (kind == 0) recordTraversalVisit(tree, nodeIdx, 2, steps, callStack, output);
    traceTreeTraversalRecursive(tree, kind, tree.ch[nodeIdx][1], steps, callStack, output);
    if (kind == 2) recordTraversalVisit(tree, nodeIdx, 3, steps, callStack, output);
    callStack.pop_back();
}

static Steps runTreeTraversal(int kind, const Ints& input) {
    TreeDiagram tree = dsBuildLevelTree(dsTakeInts(input, 0, 12, {1, 2, 3, 4, 5, -1, 6, -1, -1, 7, 8}));
    if (tree.root < 0) tree = dsBuildLevelTree({1, 2, 3});
    Steps steps;
    vector<string> callStack;
    string output;
    const char* modeTitles[] = {
        "Inorder (left, node, right)",
        "Preorder (node, left, right)",
        "Postorder (left, right, node)",
        "Level order (queue)"};

    steps.push_back(dsDiagramStep(0, string(modeTitles[kind]) + ". Orange nodes are done.", {},
                                  dsDrawTree(tree, true, modeTitles[kind]), {}));
    if (kind == 3) {
        deque<int> q;
        q.push_back(tree.root);
        while (!q.empty()) {
            int u = q.front();
            q.pop_front();
            for (size_t i = 0; i < tree.ch[u].size(); ++i) {
                if (tree.ch[u][i] >= 0) q.push_back(tree.ch[u][i]);
            }
            if (!output.empty()) output += ", ";
            output += S(tree.key[u]);
            tree.cls[u] = "weld";
            Step step = dsArrayStep(2, "Dequeue and visit " + S(tree.key[u]) + "; add its children to the queue.",
                                    {{"output", output}}, queueKeysSnapshot(q, tree), -1, {}, -1, -1, "queue");
            step.dg = dsDrawTree(tree, true, "visited: " + output);
            step.hl = {"t" + S(u)};
            steps.push_back(step);
        }
        steps.push_back(dsDiagramStep(1, "Level order: " + output + ". Visits level by level using a queue, O(n).",
                                      {{"output", output}}, dsDrawTree(tree, true, "done"), {}));
        return steps;
    }

    traceTreeTraversalRecursive(tree, kind, tree.root, steps, callStack, output);
    steps.push_back(dsDiagramStep(4, string(modeTitles[kind]) + " result: " + output +
                                  ". Each node is visited once: O(n); stack depth = tree height.",
                                  {{"output", output}}, dsDrawTree(tree, true, "done"), {}));
    return steps;
}

static Steps simTraversalInorder(const Ints& input) { return runTreeTraversal(0, input); }
static Steps simTraversalPreorder(const Ints& input) { return runTreeTraversal(1, input); }
static Steps simTraversalPostorder(const Ints& input) { return runTreeTraversal(2, input); }
static Steps simTraversalLevelOrder(const Ints& input) { return runTreeTraversal(3, input); }

// ---------- Threaded Binary Tree ----------

static void collectInorderIndices(const TreeDiagram& tree, int nodeIdx, vector<int>& order) {
    if (nodeIdx < 0) return;
    collectInorderIndices(tree, tree.ch[nodeIdx][0], order);
    order.push_back(nodeIdx);
    collectInorderIndices(tree, tree.ch[nodeIdx][1], order);
}

static Steps simThreadedBuild(const Ints& input) {
    TreeDiagram tree = dsBuildLevelTree(dsTakeInts(input, 0, 12, {4, 2, 6, 1, 3, 5, 7}));
    if (tree.root < 0) tree = dsBuildLevelTree({4, 2, 6});
    vector<int> inorderOrder;
    collectInorderIndices(tree, tree.root, inorderOrder);

    Steps steps;
    vector<pair<int, int>> threads;
    string seqStr;
    for (size_t i = 0; i < inorderOrder.size(); ++i) {
        if (!seqStr.empty()) seqStr += ", ";
        seqStr += S(tree.key[inorderOrder[i]]);
    }

    steps.push_back(dsDiagramStep(0, "In an ordinary tree many right pointers are NULL. A threaded tree reuses them to point at the INORDER SUCCESSOR. Inorder order here: " + seqStr + ".",
                                  {{"inorder", seqStr}}, dsDrawTree(tree, true, "plain tree"), {}));
    for (size_t i = 0; i < inorderOrder.size(); ++i) {
        int u = inorderOrder[i];
        if (tree.ch[u][1] >= 0) {
            steps.push_back(dsDiagramStep(1, "Node " + S(tree.key[u]) + " already has a right child, so its right pointer is used. No thread.",
                                          {{"node", S(tree.key[u])}},
                                          dsDrawTree(tree, true, "thread arrows are drawn below", threads),
                                          {"t" + S(u)}));
            continue;
        }
        if (i + 1 < inorderOrder.size()) {
            threads.push_back({u, inorderOrder[i + 1]});
            steps.push_back(dsDiagramStep(2, "Node " + S(tree.key[u]) + " has no right child. Its right pointer becomes a THREAD to the inorder successor " +
                                          S(tree.key[inorderOrder[i + 1]]) + ".",
                                          {{"node", S(tree.key[u])}, {"successor", S(tree.key[inorderOrder[i + 1]])}},
                                          dsDrawTree(tree, true, "thread arrows are drawn below", threads),
                                          {"t" + S(u), "th" + S(u)}));
        } else {
            steps.push_back(dsDiagramStep(1, "Node " + S(tree.key[u]) + " is the LAST in inorder: its right pointer stays NULL.",
                                          {{"node", S(tree.key[u])}},
                                          dsDrawTree(tree, true, "thread arrows are drawn below", threads),
                                          {"t" + S(u)}));
        }
    }
    steps.push_back(dsDiagramStep(3, "All threads set. Now inorder traversal needs no recursion and no stack, and no NULL right pointers are wasted.",
                                  {}, dsDrawTree(tree, true, "threaded binary tree", threads), {}));
    return steps;
}

static int findLeftmostNode(const TreeDiagram& tree, int nodeIdx) {
    while (nodeIdx >= 0 && tree.ch[nodeIdx][0] >= 0) {
        nodeIdx = tree.ch[nodeIdx][0];
    }
    return nodeIdx;
}

static int findThreadTarget(const vector<pair<int, int>>& threads, int nodeIdx) {
    for (size_t i = 0; i < threads.size(); ++i) {
        if (threads[i].first == nodeIdx) return threads[i].second;
    }
    return -1;
}

static Steps simThreadedWalk(const Ints& input) {
    TreeDiagram tree = dsBuildLevelTree(dsTakeInts(input, 0, 12, {4, 2, 6, 1, 3, 5, 7}));
    if (tree.root < 0) tree = dsBuildLevelTree({4, 2, 6});
    vector<int> inorderOrder;
    collectInorderIndices(tree, tree.root, inorderOrder);
    vector<pair<int, int>> threads;
    for (size_t i = 0; i + 1 < inorderOrder.size(); ++i) {
        if (tree.ch[inorderOrder[i]][1] < 0) {
            threads.push_back({inorderOrder[i], inorderOrder[i + 1]});
        }
    }

    Steps steps;
    string output;
    int u = findLeftmostNode(tree, tree.root);
    steps.push_back(dsDiagramStep(0, "Start at the leftmost node: " + S(tree.key[u]) + ".", {},
                                  dsDrawTree(tree, true, "threaded tree", threads), {"t" + S(u)}));
    int guard = 0;
    while (u >= 0 && guard++ < 40) {
        if (!output.empty()) output += ", ";
        output += S(tree.key[u]);
        tree.cls[u] = "weld";
        steps.push_back(dsDiagramStep(2, "Visit " + S(tree.key[u]) + ".", {{"output", output}},
                                      dsDrawTree(tree, true, "orange = visited", threads), {"t" + S(u)}));
        int threadTarget = findThreadTarget(threads, u);
        if (threadTarget >= 0) {
            steps.push_back(dsDiagramStep(3, "Right pointer is a thread: jump straight to " + S(tree.key[threadTarget]) + " (the inorder successor).",
                                          {{"next", S(tree.key[threadTarget])}},
                                          dsDrawTree(tree, true, "follow the thread", threads), {"th" + S(u)}));
            u = threadTarget;
        } else if (tree.ch[u][1] >= 0) {
            int rightChild = tree.ch[u][1];
            int nextNode = findLeftmostNode(tree, rightChild);
            steps.push_back(dsDiagramStep(4, "Real right child " + S(tree.key[rightChild]) + ": go to the leftmost node of that subtree, " + S(tree.key[nextNode]) + ".",
                                          {{"next", S(tree.key[nextNode])}},
                                          dsDrawTree(tree, true, "go down-left", threads), {"t" + S(nextNode)}));
            u = nextNode;
        } else {
            u = -1;
        }
    }
    steps.push_back(dsDiagramStep(5, "Right pointer is NULL: done. Output " + output + ". O(n) time and O(1) extra space.",
                                  {{"output", output}}, dsDrawTree(tree, true, "done", threads), {}));
    return steps;
}

// ---------- Priority Queue using Binary Heap & Heap Sort ----------

static Steps simBinaryHeapInsert(const Ints& input) {
    Ints items = dsTakeInts(input, 0, 9, {5, 3, 8, 1, 9, 2, 7});
    Ints heapArr(items.size());
    int heapSize = 0;
    Steps steps;

    steps.push_back(dsHeapStep(0, "A min-heap is a complete binary tree stored in an array. Rule: every parent <= its children, so the SMALLEST item is always a[0]. Parent of i is (i-1)/2.",
                               {{"n", "0"}}, heapArr, 0, -1, {}, {}, "empty heap"));
    for (size_t idx = 0; idx < items.size(); ++idx) {
        int val = items[idx];
        heapArr[heapSize++] = val;
        int cur = heapSize - 1;
        steps.push_back(dsHeapStep(1, "Put " + S(val) + " in the first free cell, index " + S(cur) + ", at the bottom of the tree.",
                                   {{"x", S(val)}, {"n", S(heapSize)}}, heapArr, heapSize, cur,
                                   dsMakeTags(heapSize, {{cur, "new"}}), {"t" + S(cur)}, "append at the end"));
        while (cur > 0 && heapArr[(cur - 1) / 2] > heapArr[cur]) {
            int parentIdx = (cur - 1) / 2;
            steps.push_back(dsHeapStep(2, "Parent " + S(heapArr[parentIdx]) + " > " + S(heapArr[cur]) + ": the heap rule is broken. Swap them.",
                                       {{"i", S(cur)}, {"parent", S(parentIdx)}}, heapArr, heapSize, cur,
                                       dsMakeTags(heapSize, {{cur, "i"}, {parentIdx, "p"}}), {"t" + S(cur), "t" + S(parentIdx)}, "sift up"));
            swap(heapArr[cur], heapArr[parentIdx]);
            cur = parentIdx;
            steps.push_back(dsHeapStep(3, "Swapped. The new item moved up to index " + S(cur) + ".",
                                       {{"i", S(cur)}}, heapArr, heapSize, cur,
                                       dsMakeTags(heapSize, {{cur, "i"}}), {"t" + S(cur)}, "sift up"));
        }
        if (cur > 0) {
            steps.push_back(dsHeapStep(2, "Parent " + S(heapArr[(cur - 1) / 2]) + " <= " + S(heapArr[cur]) + ": the heap rule holds. Stop.",
                                       {{"i", S(cur)}}, heapArr, heapSize, cur,
                                       dsMakeTags(heapSize, {{cur, "i"}}), {"t" + S(cur)}, "heap rule ok"));
        } else if (heapSize > 1) {
            steps.push_back(dsHeapStep(2, "It reached the root: it is the new minimum.",
                                       {{"min", S(heapArr[0])}}, heapArr, heapSize, 0,
                                       dsMakeTags(heapSize, {{0, "min"}}), {"t0"}, "new root"));
        }
    }
    steps.push_back(dsHeapStep(0, "Insert costs at most the tree height, O(log n). Peeking at the minimum is O(1).",
                               {{"min", S(heapArr[0])}}, heapArr, heapSize, -1, {}, {}, "min-heap"));
    return steps;
}

static Steps simBinaryHeapExtract(const Ints& input) {
    Ints items = dsTakeInts(input, 0, 7, {5, 3, 8, 1, 9, 2, 7});
    Ints heapArr(items.size());
    int heapSize = 0;
    for (size_t k = 0; k < items.size(); ++k) {
        heapArr[heapSize++] = items[k];
        int cur = heapSize - 1;
        while (cur > 0 && heapArr[(cur - 1) / 2] > heapArr[cur]) {
            swap(heapArr[cur], heapArr[(cur - 1) / 2]);
            cur = (cur - 1) / 2;
        }
    }

    Steps steps;
    string extracted;
    steps.push_back(dsHeapStep(0, "This min-heap is already built. extractMin removes the root, always the smallest.",
                               {{"n", S(heapSize)}}, heapArr, heapSize, -1, {}, {}, "min-heap"));
    while (heapSize > 0) {
        int minVal = heapArr[0];
        if (!extracted.empty()) extracted += ", ";
        extracted += S(minVal);
        steps.push_back(dsHeapStep(1, "Remove the root " + S(minVal) + " (the minimum). Move the LAST item " + S(heapArr[heapSize - 1]) + " to the root to keep the tree complete.",
                                   {{"min", S(minVal)}, {"extracted", extracted}}, heapArr, heapSize, 0,
                                   dsMakeTags(heapSize, {{0, "root"}, {heapSize - 1, "last"}}), {"t0", "t" + S(heapSize - 1)}, "extract"));
        heapArr[0] = heapArr[heapSize - 1];
        --heapSize;
        if (heapSize == 0) {
            steps.push_back(dsHeapStep(1, "The heap is empty.", {{"extracted", extracted}}, heapArr, 0, -1, {}, {}, "empty"));
            break;
        }
        int cur = 0;
        steps.push_back(dsHeapStep(2, "The root " + S(heapArr[0]) + " may be too big. Sift it down.",
                                   {{"extracted", extracted}}, heapArr, heapSize, 0,
                                   dsMakeTags(heapSize, {{0, "i"}}), {"t0"}, "sift down"));
        while (true) {
            int leftIdx = 2 * cur + 1;
            int rightIdx = leftIdx + 1;
            int smallest = cur;
            if (leftIdx < heapSize && heapArr[leftIdx] < heapArr[smallest]) smallest = leftIdx;
            if (rightIdx < heapSize && heapArr[rightIdx] < heapArr[smallest]) smallest = rightIdx;
            if (smallest == cur) {
                steps.push_back(dsHeapStep(3, "Both children are >= " + S(heapArr[cur]) + " (or none): the heap rule holds.",
                                           {{"i", S(cur)}}, heapArr, heapSize, cur,
                                           dsMakeTags(heapSize, {{cur, "i"}}), {"t" + S(cur)}, "heap rule ok"));
                break;
            }
            steps.push_back(dsHeapStep(3, "Smaller child is " + S(heapArr[smallest]) + " < " + S(heapArr[cur]) + ": swap them.",
                                       {{"i", S(cur)}, {"child", S(smallest)}}, heapArr, heapSize, smallest,
                                       dsMakeTags(heapSize, {{cur, "i"}, {smallest, "c"}}), {"t" + S(cur), "t" + S(smallest)}, "sift down"));
            swap(heapArr[cur], heapArr[smallest]);
            cur = smallest;
        }
    }
    steps.push_back(dsHeapStep(4, "Items came out in sorted order: " + extracted + ". Each extract is O(log n), so n extracts sort the data in O(n log n) (this is heap sort).",
                               {{"extracted", extracted}}, heapArr, 0, -1, {}, {}, "sorted output"));
    return steps;
}

static void heapSortSiftDown(Ints& arr, int heapSize, int index, Steps& steps) {
    while (true) {
        int leftIdx = 2 * index + 1;
        int rightIdx = leftIdx + 1;
        int largest = index;
        if (leftIdx < heapSize && arr[leftIdx] > arr[largest]) largest = leftIdx;
        if (rightIdx < heapSize && arr[rightIdx] > arr[largest]) largest = rightIdx;
        if (largest == index) {
            steps.push_back(dsHeapStep(3, "Node " + S(arr[index]) + " is >= its children: this part is a heap.",
                                       {{"i", S(index)}}, arr, heapSize, index,
                                       dsMakeTags(heapSize, {{index, "i"}}), {"t" + S(index)}, "heap rule ok", 0, heapSize - 1));
            break;
        }
        steps.push_back(dsHeapStep(3, "Child " + S(arr[largest]) + " > " + S(arr[index]) + ": swap them.",
                                   {{"i", S(index)}, {"child", S(largest)}}, arr, heapSize, largest,
                                   dsMakeTags(heapSize, {{index, "i"}, {largest, "c"}}), {"t" + S(index), "t" + S(largest)}, "sift down", 0, heapSize - 1));
        swap(arr[index], arr[largest]);
        index = largest;
    }
}

static Steps simHeapSort(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 8, {4, 10, 3, 5, 1, 8, 7});
    int totalSize = (int)arr.size();
    Steps steps;

    steps.push_back(dsHeapStep(0, "Heap sort: first turn the array into a MAX-heap (parent >= children), then repeatedly move the biggest item to the end.",
                               {{"n", S(totalSize)}}, arr, totalSize, -1, {}, {}, "array viewed as a tree", 0, totalSize - 1));
    for (int i = totalSize / 2 - 1; i >= 0; --i) {
        steps.push_back(dsHeapStep(0, "Build phase: sift down node at index " + S(i) + ", value " + S(arr[i]) + ".",
                                   {{"i", S(i)}}, arr, totalSize, i,
                                   dsMakeTags(totalSize, {{i, "i"}}), {"t" + S(i)}, "build max-heap", 0, totalSize - 1));
        heapSortSiftDown(arr, totalSize, i, steps);
    }
    steps.push_back(dsHeapStep(0, "Max-heap built: the biggest value " + S(arr[0]) + " is at the root.",
                               {{"max", S(arr[0])}}, arr, totalSize, 0,
                               dsMakeTags(totalSize, {{0, "max"}}), {"t0"}, "max-heap", 0, totalSize - 1));
    for (int endIdx = totalSize - 1; endIdx > 0; --endIdx) {
        steps.push_back(dsHeapStep(2, "Swap the root " + S(arr[0]) + " with the last heap cell a[" + S(endIdx) + "] = " + S(arr[endIdx]) + ". The biggest value is now in its FINAL place.",
                                   {{"end", S(endIdx)}}, arr, endIdx + 1, endIdx,
                                   dsMakeTags(endIdx + 1, {{0, "root"}, {endIdx, "end"}}), {"t0", "t" + S(endIdx)}, "swap", 0, endIdx));
        swap(arr[0], arr[endIdx]);
        steps.push_back(dsHeapStep(2, "Swapped. Cells " + S(endIdx) + ".." + S(totalSize - 1) + " (dimmed) are sorted; the heap shrinks to " + S(endIdx) + " item(s).",
                                   {{"heap size", S(endIdx)}}, arr, endIdx, -1, {}, {}, "shrunk heap", 0, endIdx - 1));
        if (endIdx > 1) {
            heapSortSiftDown(arr, endIdx, 0, steps);
        }
    }
    steps.push_back(dsArrayStep(4, "Sorted: " + dsJoinInts(arr) + ". Heap sort is O(n log n) in every case and sorts in place with O(1) extra space (not stable).",
                                {{"sorted", dsJoinInts(arr)}}, arr, -1, {}, -1, -1, "a"));
    return steps;
}

// ---------- Binomial Heap ----------

static string binomialFormatRoots(const HeapForest& forest) {
    string text;
    for (size_t i = 0; i < forest.roots.size(); ++i) {
        if (!text.empty()) text += ", ";
        text += "B" + S(forest.deg[forest.roots[i]]);
    }
    return text.empty() ? string("(empty)") : text;
}

static void sortRootsByDegree(HeapForest& forest) {
    for (size_t i = 1; i < forest.roots.size(); ++i) {
        int currentRoot = forest.roots[i];
        int j = (int)i - 1;
        while (j >= 0 && forest.deg[forest.roots[j]] > forest.deg[currentRoot]) {
            forest.roots[j + 1] = forest.roots[j];
            --j;
        }
        forest.roots[j + 1] = currentRoot;
    }
}

static void binomialMergeEqualDegrees(HeapForest& forest, int currentRoot, Steps& steps, bool recordSteps, int lineNum) {
    if (find(forest.roots.begin(), forest.roots.end(), currentRoot) == forest.roots.end()) {
        forest.roots.push_back(currentRoot);
    }
    int guard = 0;
    while (guard++ < 20) {
        int matchingRoot = -1;
        for (size_t i = 0; i < forest.roots.size(); ++i) {
            int candidate = forest.roots[i];
            if (candidate != currentRoot && forest.deg[candidate] == forest.deg[currentRoot]) {
                matchingRoot = candidate;
                break;
            }
        }
        if (matchingRoot < 0) break;

        int treeDegree = forest.deg[currentRoot];
        int winner = (forest.key[currentRoot] <= forest.key[matchingRoot]) ? currentRoot : matchingRoot;
        int loser = (winner == currentRoot) ? matchingRoot : currentRoot;
        forest.roots.erase(find(forest.roots.begin(), forest.roots.end(), loser));
        forest.linkTrees(loser, winner);
        currentRoot = winner;

        if (recordSteps) {
            steps.push_back(dsDiagramStep(lineNum,
                                          "Two trees of degree " + S(treeDegree) + " (B" + S(treeDegree) + "). The root with the smaller key (" + S(forest.key[winner]) + ") stays on top; " +
                                          S(forest.key[loser]) + " becomes its child. Result: one B" + S(treeDegree + 1) + ".",
                                          {{"linked", S(forest.key[loser]) + " under " + S(forest.key[winner])}},
                                          dsDrawForest(forest, "roots: " + binomialFormatRoots(forest)),
                                          {"t" + S(winner + 1), "t" + S(loser + 1), "e" + S(loser + 1)}));
        }
    }
    sortRootsByDegree(forest);
}

static Steps simBinomialInsert(const Ints& input) {
    Ints keys = dsTakeInts(input, 0, 8, {10, 20, 5, 30, 15, 8, 25, 12});
    HeapForest forest;
    Steps steps;
    int nodeCount = 0;

    steps.push_back(dsDiagramStep(0, "A binomial heap is a set of binomial trees B0, B1, B2 ... at most one of each size, each obeying the min-heap rule. Bk has 2^k nodes. Inserting works exactly like adding 1 in binary.",
                                  {}, dsDrawForest(forest, "empty heap"), {}));
    for (size_t i = 0; i < keys.size(); ++i) {
        int keyVal = keys[i];
        int newRoot = forest.addNode(keyVal);
        forest.roots.push_back(newRoot);
        ++nodeCount;
        steps.push_back(dsDiagramStep(1, "Insert " + S(keyVal) + " as a one-node tree B0.",
                                      {{"n", S(nodeCount)}}, dsDrawForest(forest, "roots: " + binomialFormatRoots(forest)),
                                      {"t" + S(newRoot + 1)}));
        binomialMergeEqualDegrees(forest, newRoot, steps, true, 3);
        string binaryStr;
        for (int b = 5; b >= 0; --b) {
            binaryStr += ((nodeCount >> b) & 1) ? '1' : '0';
        }
        steps.push_back(dsDiagramStep(2, "n = " + S(nodeCount) + " is " + binaryStr + " in binary, and the heap has exactly one tree for each 1 bit: " + binomialFormatRoots(forest) + ".",
                                      {{"n", S(nodeCount)}, {"binary", binaryStr}},
                                      dsDrawForest(forest, "roots: " + binomialFormatRoots(forest)), {}));
    }
    steps.push_back(dsDiagramStep(0, "Insert is O(log n) worst case but O(1) amortised, just like a binary counter. The minimum is the smallest of the roots.",
                                  {}, dsDrawForest(forest, "roots: " + binomialFormatRoots(forest)), {}));
    return steps;
}

static Steps simBinomialExtract(const Ints& input) {
    int extractCount = clamp(input.empty() ? 1 : input[0], 1, 3);
    Ints keys = dsTakeInts(input, 1, 8, {10, 20, 5, 30, 15, 8, 25, 12});
    HeapForest forest;
    Steps dummySteps;
    for (size_t i = 0; i < keys.size(); ++i) {
        int newRoot = forest.addNode(keys[i]);
        forest.roots.push_back(newRoot);
        binomialMergeEqualDegrees(forest, newRoot, dummySteps, false, 0);
    }

    Steps steps;
    steps.push_back(dsDiagramStep(0, "This binomial heap was built from the keys. extractMin removes the smallest root, then repairs the structure.",
                                  {}, dsDrawForest(forest, "roots: " + binomialFormatRoots(forest)), {}));
    for (int e = 0; e < extractCount && !forest.roots.empty(); ++e) {
        int minRoot = forest.roots[0];
        for (size_t i = 0; i < forest.roots.size(); ++i) {
            if (forest.key[forest.roots[i]] < forest.key[minRoot]) minRoot = forest.roots[i];
        }
        steps.push_back(dsDiagramStep(1, "The minimum is the smallest ROOT: " + S(forest.key[minRoot]) + " (orange). Only the roots need checking: at most log n of them.",
                                      {{"min", S(forest.key[minRoot])}},
                                      dsDrawForest(forest, "roots: " + binomialFormatRoots(forest), minRoot),
                                      {"t" + S(minRoot + 1)}));
        forest.roots.erase(find(forest.roots.begin(), forest.roots.end(), minRoot));
        forest.dead[minRoot] = 1;
        vector<int> children = forest.ch[minRoot];
        forest.ch[minRoot].clear();
        for (size_t i = 0; i < children.size(); ++i) {
            int childIdx = children[i];
            forest.par[childIdx] = -1;
            forest.roots.push_back(childIdx);
        }
        sortRootsByDegree(forest);
        steps.push_back(dsDiagramStep(2, "Removed " + S(forest.key[minRoot]) + ". Its " + S((int)children.size()) + " child(ren) are binomial trees of degrees " +
                                      S(children.empty() ? 0 : (int)children.size() - 1) + " down to 0, so they simply join the root list.",
                                      {{"removed", S(forest.key[minRoot])}},
                                      dsDrawForest(forest, "roots: " + binomialFormatRoots(forest)), {}));
        vector<int> snapshotRoots = forest.roots;
        for (size_t i = 0; i < snapshotRoots.size(); ++i) {
            int r = snapshotRoots[i];
            if (find(forest.roots.begin(), forest.roots.end(), r) != forest.roots.end()) {
                binomialMergeEqualDegrees(forest, r, steps, true, 3);
            }
        }
        steps.push_back(dsDiagramStep(3, "After merging equal degrees, every degree appears at most once again: " + binomialFormatRoots(forest) + ".",
                                      {}, dsDrawForest(forest, "roots: " + binomialFormatRoots(forest)), {}));
    }
    steps.push_back(dsDiagramStep(0, "Extract-min is O(log n).",
                                  {}, dsDrawForest(forest, "roots: " + binomialFormatRoots(forest)), {}));
    return steps;
}

// ---------- Fibonacci Heap ----------

static int fibFindMinRoot(const HeapForest& forest) {
    int minIdx = -1;
    for (size_t i = 0; i < forest.roots.size(); ++i) {
        int r = forest.roots[i];
        if (minIdx < 0 || forest.key[r] < forest.key[minIdx]) minIdx = r;
    }
    return minIdx;
}

static string fibFormatRootList(const HeapForest& forest) {
    string text;
    for (size_t i = 0; i < forest.roots.size(); ++i) {
        if (!text.empty()) text += ", ";
        text += S(forest.key[forest.roots[i]]);
    }
    return text.empty() ? string("(empty)") : text;
}

static void fibRebuildRootsSnapshot(HeapForest& forest, const vector<int>& degreeTable,
                                    int currentRoot, const vector<int>& pending, size_t processedIdx) {
    forest.roots.clear();
    for (size_t i = 0; i < degreeTable.size(); ++i) {
        if (degreeTable[i] >= 0) forest.roots.push_back(degreeTable[i]);
    }
    if (currentRoot >= 0) {
        forest.roots.push_back(currentRoot);
    }
    for (size_t q = processedIdx + 1; q < pending.size(); ++q) {
        forest.roots.push_back(pending[q]);
    }
}

static void fibExtractMin(HeapForest& forest, Steps& steps, bool recordSteps) {
    int minRoot = fibFindMinRoot(forest);
    if (minRoot < 0) return;
    if (recordSteps) {
        steps.push_back(dsDiagramStep(1, "min pointer = " + S(forest.key[minRoot]) + " (orange). Remove it from the root list.",
                                      {{"min", S(forest.key[minRoot])}},
                                      dsDrawForest(forest, "root list: " + fibFormatRootList(forest), minRoot),
                                      {"t" + S(minRoot + 1)}));
    }
    forest.roots.erase(find(forest.roots.begin(), forest.roots.end(), minRoot));
    forest.dead[minRoot] = 1;
    vector<int> children = forest.ch[minRoot];
    forest.ch[minRoot].clear();
    for (size_t i = 0; i < children.size(); ++i) {
        int childIdx = children[i];
        forest.par[childIdx] = -1;
        forest.mark[childIdx] = 0;
        forest.roots.push_back(childIdx);
    }
    if (recordSteps) {
        steps.push_back(dsDiagramStep(2, "Its " + S((int)children.size()) + " child(ren) are promoted to the root list. Now the root list is long and untidy, so CONSOLIDATE: link roots of equal degree until all degrees differ.",
                                      {}, dsDrawForest(forest, "root list: " + fibFormatRootList(forest)), {}));
    }

    vector<int> pending = forest.roots;
    vector<int> degreeTable(32, -1);
    for (size_t i = 0; i < pending.size(); ++i) {
        int x = pending[i];
        int d = forest.deg[x];
        while (d < 31 && degreeTable[d] >= 0) {
            int y = degreeTable[d];
            degreeTable[d] = -1;
            if (forest.key[y] < forest.key[x]) swap(x, y);
            forest.linkTrees(y, x);
            d = forest.deg[x];
            fibRebuildRootsSnapshot(forest, degreeTable, x, pending, i);
            if (recordSteps) {
                steps.push_back(dsDiagramStep(3, "Two roots of degree " + S(d - 1) + ": link them. " + S(forest.key[y]) + " becomes a child of " + S(forest.key[x]) + " (smaller key stays on top).",
                                              {{"linked", S(forest.key[y]) + " under " + S(forest.key[x])}},
                                              dsDrawForest(forest, "root list: " + fibFormatRootList(forest)),
                                              {"t" + S(x + 1), "e" + S(y + 1)}));
            }
        }
        degreeTable[d] = x;
        fibRebuildRootsSnapshot(forest, degreeTable, -1, pending, i);
    }
    forest.roots.clear();
    for (size_t i = 0; i < degreeTable.size(); ++i) {
        if (degreeTable[i] >= 0) forest.roots.push_back(degreeTable[i]);
    }
    int newMin = fibFindMinRoot(forest);
    if (recordSteps) {
        steps.push_back(dsDiagramStep(3, "All root degrees are different now. New min = " + (newMin >= 0 ? S(forest.key[newMin]) : string("none")) + ".",
                                      {}, dsDrawForest(forest, "root list: " + fibFormatRootList(forest), newMin), {}));
    }
}

static Steps simFibonacciInsertAndExtract(const Ints& input) {
    Ints keys = dsTakeInts(input, 0, 8, {10, 20, 5, 30, 15, 8, 25, 12});
    HeapForest forest;
    Steps steps;
    steps.push_back(dsDiagramStep(0, "A Fibonacci heap is a collection of heap-ordered trees in a root list, plus a pointer to the minimum. It is LAZY: it postpones all tidying until extract-min.",
                                  {}, dsDrawForest(forest, "empty"), {}));
    for (size_t i = 0; i < keys.size(); ++i) {
        int keyVal = keys[i];
        int newRoot = forest.addNode(keyVal);
        forest.roots.push_back(newRoot);
        int minRoot = fibFindMinRoot(forest);
        steps.push_back(dsDiagramStep(0, "Insert " + S(keyVal) + ": just add a new tree to the root list. min pointer = " + S(forest.key[minRoot]) + ". No work done: O(1).",
                                      {{"min", S(forest.key[minRoot])}},
                                      dsDrawForest(forest, "root list: " + fibFormatRootList(forest), minRoot),
                                      {"t" + S(newRoot + 1)}));
    }
    steps.push_back(dsDiagramStep(1, "Now extractMin. This is where the postponed work happens.",
                                  {}, dsDrawForest(forest, "root list: " + fibFormatRootList(forest), fibFindMinRoot(forest)), {}));
    fibExtractMin(forest, steps, true);
    steps.push_back(dsDiagramStep(3, "Amortised cost: insert O(1), extract-min O(log n), decrease-key O(1). That is why Dijkstra and Prim reach O(E + V log V) with this heap.",
                                  {}, dsDrawForest(forest, "root list: " + fibFormatRootList(forest), fibFindMinRoot(forest)), {}));
    return steps;
}

static void markReachableForestNodes(const HeapForest& forest, int nodeIdx, vector<char>& reachable) {
    reachable[nodeIdx] = 1;
    for (size_t i = 0; i < forest.ch[nodeIdx].size(); ++i) {
        markReachableForestNodes(forest, forest.ch[nodeIdx][i], reachable);
    }
}

static void cutForestChild(HeapForest& forest, int childIdx) {
    int parentIdx = forest.par[childIdx];
    forest.ch[parentIdx].erase(find(forest.ch[parentIdx].begin(), forest.ch[parentIdx].end(), childIdx));
    forest.deg[parentIdx]--;
    forest.par[childIdx] = -1;
    forest.mark[childIdx] = 0;
    forest.roots.push_back(childIdx);
}

static Steps simFibonacciDecreaseKey(const Ints& input) {
    int oldKey = input.empty() ? 30 : input[0];
    int newKey = (input.size() > 1) ? input[1] : 2;
    Ints keys = dsTakeInts(input, 2, 8, {10, 20, 5, 30, 15, 8, 25, 12});

    HeapForest forest;
    Steps dummySteps;
    for (size_t i = 0; i < keys.size(); ++i) {
        int newRoot = forest.addNode(keys[i]);
        forest.roots.push_back(newRoot);
    }
    fibExtractMin(forest, dummySteps, false);

    vector<char> reachable(forest.key.size(), 0);
    for (size_t i = 0; i < forest.roots.size(); ++i) {
        markReachableForestNodes(forest, forest.roots[i], reachable);
    }

    int targetNode = -1;
    for (size_t i = 0; i < forest.key.size(); ++i) {
        if (reachable[i] && forest.key[i] == oldKey && forest.par[i] >= 0) {
            targetNode = (int)i;
            break;
        }
    }
    if (targetNode < 0) {
        for (size_t i = 0; i < forest.key.size(); ++i) {
            if (reachable[i] && forest.par[i] >= 0) {
                targetNode = (int)i;
                break;
            }
        }
    }
    if (targetNode < 0 && !forest.roots.empty()) {
        targetNode = forest.roots[0];
    }

    Steps steps;
    steps.push_back(dsDiagramStep(0, "This heap was built from your keys and consolidated once. Red nodes would be 'marked' (they already lost one child).",
                                  {}, dsDrawForest(forest, "root list: " + fibFormatRootList(forest), fibFindMinRoot(forest)), {}));
    if (targetNode < 0) return steps;

    oldKey = forest.key[targetNode];
    if (newKey >= oldKey) newKey = oldKey - 1;
    forest.key[targetNode] = newKey;
    steps.push_back(dsDiagramStep(1, "decreaseKey: " + S(oldKey) + " becomes " + S(newKey) +
                                  (forest.par[targetNode] >= 0 ? ". Compare with its parent " + S(forest.key[forest.par[targetNode]]) + "."
                                                               : ". It is a root, so nothing else is needed."),
                                  {{"new key", S(newKey)}},
                                  dsDrawForest(forest, "root list: " + fibFormatRootList(forest), fibFindMinRoot(forest)),
                                  {"t" + S(targetNode + 1)}));

    if (forest.par[targetNode] >= 0 && forest.key[targetNode] < forest.key[forest.par[targetNode]]) {
        int parentIdx = forest.par[targetNode];
        cutForestChild(forest, targetNode);
        steps.push_back(dsDiagramStep(3, "Heap rule broken (child < parent): CUT " + S(forest.key[targetNode]) + " out of its parent and make it a root. O(1).",
                                      {{"cut", S(forest.key[targetNode])}},
                                      dsDrawForest(forest, "root list: " + fibFormatRootList(forest), fibFindMinRoot(forest)),
                                      {"t" + S(targetNode + 1)}));
        int guard = 0;
        while (parentIdx >= 0 && guard++ < 10) {
            if (forest.par[parentIdx] < 0) {
                steps.push_back(dsDiagramStep(4, "Parent " + S(forest.key[parentIdx]) + " is a root: stop (roots are never marked).",
                                              {}, dsDrawForest(forest, "root list: " + fibFormatRootList(forest), fibFindMinRoot(forest)),
                                              {"t" + S(parentIdx + 1)}));
                break;
            }
            if (!forest.mark[parentIdx]) {
                forest.mark[parentIdx] = 1;
                steps.push_back(dsDiagramStep(4, "Parent " + S(forest.key[parentIdx]) + " was unmarked: MARK it (red). It has now lost one child.",
                                              {}, dsDrawForest(forest, "root list: " + fibFormatRootList(forest), fibFindMinRoot(forest)),
                                              {"t" + S(parentIdx + 1)}));
                break;
            }
            int grandParent = forest.par[parentIdx];
            cutForestChild(forest, parentIdx);
            steps.push_back(dsDiagramStep(4, "Parent " + S(forest.key[parentIdx]) + " was already marked, so it lost a second child: CUT it too (cascading cut).",
                                          {}, dsDrawForest(forest, "root list: " + fibFormatRootList(forest), fibFindMinRoot(forest)),
                                          {"t" + S(parentIdx + 1)}));
            parentIdx = grandParent;
        }
    }
    int minRoot = fibFindMinRoot(forest);
    steps.push_back(dsDiagramStep(6, "min pointer = " + S(forest.key[minRoot]) + ". Cutting instead of re-sifting keeps decrease-key at O(1) amortised.",
                                  {}, dsDrawForest(forest, "root list: " + fibFormatRootList(forest), minRoot), {}));
    return steps;
}

void addDS4b(map<string, Sim>& m);
void addDS5(map<string, Sim>& m);

void addDS4(map<string, Sim>& m) {
    addDS4b(m);
    addDS5(m);

    vector<string> refMultiList = {
        "GeeksforGeeks: Flatten a multilevel linked list|https://www.geeksforgeeks.org/flatten-a-multi-level-linked-list-set-2-depth-wise/",
        "GeeksforGeeks: Linked list|https://www.geeksforgeeks.org/data-structures/linked-list/",
        "VisuAlgo: linked list|https://visualgo.net/en/list"};
    dsRegisterIntSim(m, "Multi List", "Number of items in the sub-list under each head (up to 5 heads, 0 to 4 each)", "2, 0, 3, 1",
                     {"void visit(Node *h) {", "    for (p = h; p != NULL; p = p->next) {", "        print(p->data);", "        if (p->down != NULL)  visit(p->down);   // go into the sub-list", "    }", "}"},
                     refMultiList, {"LeetCode #430 (Flatten a Multilevel Doubly Linked List)", "LeetCode #114 (Flatten Binary Tree to Linked List)", "LeetCode #138 (Copy List with Random Pointer)"},
                     simMultiList);

    const char* levelLabel = "Level-order values, -1 = no node (up to 12)";
    dsRegisterIntSim(m, "Binary Tree", levelLabel, "1, 2, 3, 4, 5, -1, 6",
                     {"root = new Node(a[0]);   queue.push(root);", "for each next pair of values (left, right):", "    node = queue.pop();", "    if (left != -1)   node->left  = new Node(left),  queue.push(...);", "    if (right != -1)  node->right = new Node(right), queue.push(...);"},
                     dsTreeRefs(), {"LeetCode #104 (Maximum Depth of Binary Tree)", "LeetCode #102 (Binary Tree Level Order Traversal)", "LeetCode #226 (Invert Binary Tree)", "LeetCode #297 (Serialize and Deserialize Binary Tree)"},
                     simBinaryTreeBuild);
    dsRegisterIntSim(m, "Binary Tree#1", levelLabel, "1, 2, 3, 4, 5, -1, 6",
                     {"int height(Node *u) {", "    if (u == NULL)  return 0;", "    return 1 + max(height(u->left), height(u->right));", "}", "leaf: both children are NULL"},
                     dsTreeRefs(), {"LeetCode #104 (Maximum Depth of Binary Tree)", "LeetCode #111 (Minimum Depth of Binary Tree)", "LeetCode #110 (Balanced Binary Tree)", "LeetCode #543 (Diameter of Binary Tree)"},
                     simBinaryTreeHeight);
    dsRegisterIntSim(m, "Binary Tree#2", levelLabel, "1, 2, 3, 4, 5, -1, 6",
                     {"Node *invert(Node *u) {", "    if (u == NULL)  return NULL;", "    swap(u->left, u->right);", "    invert(u->left);  invert(u->right);", "    return u;", "}"},
                     dsTreeRefs(), {"LeetCode #226 (Invert Binary Tree)", "LeetCode #101 (Symmetric Tree)", "LeetCode #100 (Same Tree)"},
                     simBinaryTreeInvert);
    dsSetProgramNames("Binary Tree", {"Build from level order", "Height, leaves and size (recursion)", "Invert (mirror) the tree"});

    vector<string> refKary = {
        "GeeksforGeeks: N-ary tree|https://www.geeksforgeeks.org/generic-treesn-array-trees/",
        "VisuAlgo: tree|https://visualgo.net/en/bst",
        "GeeksforGeeks: Tree traversals|https://www.geeksforgeeks.org/tree-traversals-inorder-preorder-and-postorder/"};
    dsRegisterIntSim(m, "K-ary Tree", "k (2 to 4), then number of nodes (1 to 13)", "3, 10",
                     {"children of node i:   k*i + 1  ...  k*i + k", "parent of node i:     (i - 1) / k", "add node i:  attach it to parent (i - 1) / k"},
                     refKary, {"LeetCode #429 (N-ary Tree Level Order Traversal)", "LeetCode #559 (Maximum Depth of N-ary Tree)", "LeetCode #589 (N-ary Tree Preorder Traversal)"},
                     simKaryTreeBuild);
    dsRegisterIntSim(m, "K-ary Tree#1", "k (2 to 4), then number of nodes (1 to 13)", "3, 10",
                     {"void preorder(Node *u) {", "    visit(u);", "    for (child c of u, left to right)", "        preorder(c);", "}"},
                     refKary, {"LeetCode #589 (N-ary Tree Preorder Traversal)", "LeetCode #590 (N-ary Tree Postorder Traversal)", "LeetCode #559 (Maximum Depth of N-ary Tree)"},
                     simKaryTreePreorder);
    dsRegisterIntSim(m, "K-ary Tree#2", "k (2 to 4), then number of nodes (1 to 13)", "3, 10",
                     {"queue.push(root);", "while (queue not empty) {", "    u = queue.pop();   visit(u);", "    for (child c of u)  queue.push(c);", "}"},
                     refKary, {"LeetCode #429 (N-ary Tree Level Order Traversal)", "LeetCode #102 (Binary Tree Level Order Traversal)", "LeetCode #994 (Rotting Oranges)"},
                     simKaryTreeLevelOrder);
    dsSetProgramNames("K-ary Tree", {"Build with parent / child formulas", "Preorder (depth first)", "Level order (breadth first, queue)"});

    dsRegisterIntSim(m, "Tree Traversals", levelLabel, "1, 2, 3, 4, 5, -1, 6, -1, -1, 7, 8",
                     {"void inorder(Node *u) {", "    if (u == NULL)  return;", "    inorder(u->left);", "    visit(u);", "    inorder(u->right);", "}"},
                     dsTreeRefs(), {"LeetCode #94 (Binary Tree Inorder Traversal)", "LeetCode #98 (Validate Binary Search Tree)", "LeetCode #230 (Kth Smallest Element in a BST)"},
                     simTraversalInorder);
    dsRegisterIntSim(m, "Tree Traversals#1", levelLabel, "1, 2, 3, 4, 5, -1, 6, -1, -1, 7, 8",
                     {"void preorder(Node *u) {", "    visit(u);", "    preorder(u->left);", "    preorder(u->right);", "}"},
                     dsTreeRefs(), {"LeetCode #144 (Binary Tree Preorder Traversal)", "LeetCode #105 (Construct Binary Tree from Preorder and Inorder)", "LeetCode #572 (Subtree of Another Tree)"},
                     simTraversalPreorder);
    dsRegisterIntSim(m, "Tree Traversals#2", levelLabel, "1, 2, 3, 4, 5, -1, 6, -1, -1, 7, 8",
                     {"void postorder(Node *u) {", "    postorder(u->left);", "    postorder(u->right);", "    visit(u);", "}"},
                     dsTreeRefs(), {"LeetCode #145 (Binary Tree Postorder Traversal)", "LeetCode #124 (Binary Tree Maximum Path Sum)", "LeetCode #236 (Lowest Common Ancestor of a Binary Tree)"},
                     simTraversalPostorder);
    dsRegisterIntSim(m, "Tree Traversals#3", levelLabel, "1, 2, 3, 4, 5, -1, 6, -1, -1, 7, 8",
                     {"queue.push(root);", "while (queue not empty) {", "    u = queue.pop();   visit(u);", "    push left and right children;", "}"},
                     dsTreeRefs(), {"LeetCode #102 (Binary Tree Level Order Traversal)", "LeetCode #199 (Binary Tree Right Side View)", "LeetCode #103 (Zigzag Level Order Traversal)"},
                     simTraversalLevelOrder);
    dsSetProgramNames("Tree Traversals", {"Inorder (left, node, right)", "Preorder (node, left, right)", "Postorder (left, right, node)", "Level order (queue)"});

    vector<string> refThreaded = {
        "GeeksforGeeks: Threaded binary tree|https://www.geeksforgeeks.org/threaded-binary-tree/",
        "GeeksforGeeks: Inorder without recursion or stack|https://www.geeksforgeeks.org/inorder-tree-traversal-without-recursion-and-without-stack/",
        "VisuAlgo: BST|https://visualgo.net/en/bst"};
    dsRegisterIntSim(m, "Threaded Binary Tree", levelLabel, "4, 2, 6, 1, 3, 5, 7",
                     {"for each node u in inorder:", "    if (u->right == NULL)  {          // wasted NULL pointer", "        u->right = inorderSuccessor(u);  u->isThread = true;", "    }"},
                     refThreaded, {"LeetCode #99 (Recover Binary Search Tree: Morris traversal)", "LeetCode #94 (Binary Tree Inorder Traversal: Morris)", "LeetCode #173 (Binary Search Tree Iterator)"},
                     simThreadedBuild);
    dsRegisterIntSim(m, "Threaded Binary Tree#1", levelLabel, "4, 2, 6, 1, 3, 5, 7",
                     {"u = leftmost(root);", "while (u != NULL) {", "    visit(u);", "    if (u->isThread)  u = u->right;            // follow the thread", "    else  u = leftmost(u->right);              // real child: go down-left", "}"},
                     refThreaded, {"LeetCode #94 (Binary Tree Inorder Traversal)", "LeetCode #173 (Binary Search Tree Iterator)", "LeetCode #230 (Kth Smallest Element in a BST)"},
                     simThreadedWalk);
    dsSetProgramNames("Threaded Binary Tree", {"Build the threads", "Inorder walk using the threads (no stack)"});

    dsRegisterIntSim(m, "Priority Queue using Binary Heap", "Up to 9 numbers to insert one by one (min-heap)", "5, 3, 8, 1, 9, 2, 7",
                     {"insert(x):", "    a[n++] = x;", "    for (i = n - 1; i > 0 && a[(i - 1) / 2] > a[i]; i = (i - 1) / 2)", "        swap(a[i], a[(i - 1) / 2]);    // sift up"},
                     dsHeapRefs(), {"LeetCode #703 (Kth Largest Element in a Stream)", "LeetCode #1046 (Last Stone Weight)", "LeetCode #215 (Kth Largest Element in an Array)", "LeetCode #347 (Top K Frequent Elements)"},
                     simBinaryHeapInsert);
    dsRegisterIntSim(m, "Priority Queue using Binary Heap#1", "Up to 7 numbers (heap built first, then everything is extracted)", "5, 3, 8, 1, 9, 2, 7",
                     {"extractMin():", "    min = a[0];  a[0] = a[--n];", "    i = 0;", "    while (a smaller child c exists and a[c] < a[i]) { swap(a[i], a[c]);  i = c; }   // sift down", "    return min;"},
                     dsHeapRefs(), {"LeetCode #23 (Merge k Sorted Lists)", "LeetCode #1046 (Last Stone Weight)", "LeetCode #973 (K Closest Points to Origin)", "LeetCode #295 (Find Median from Data Stream)"},
                     simBinaryHeapExtract);
    dsSetProgramNames("Priority Queue using Binary Heap", {"Insert (sift up)", "Extract-min (sift down)"});

    dsRegisterIntSim(m, "Heap Sort", "Enter up to 8 numbers", "4, 10, 3, 5, 1, 8, 7",
                     {"for (i = n / 2 - 1; i >= 0; i--)  siftDown(a, n, i);    // build max-heap", "for (end = n - 1; end > 0; end--) {", "    swap(a[0], a[end]);                // largest goes to its final cell", "    siftDown(a, end, 0);               // repair the smaller heap", "}"},
                     dsHeapRefs(), {"LeetCode #912 (Sort an Array)", "LeetCode #215 (Kth Largest Element in an Array)", "LeetCode #1337 (The K Weakest Rows in a Matrix)"},
                     simHeapSort);
}

void addDS4b(map<string, Sim>& m) {
    vector<string> refBinomial = {
        "GeeksforGeeks: Binomial heap|https://www.geeksforgeeks.org/binomial-heap-2/",
        "VisuAlgo: heap|https://visualgo.net/en/heap",
        "Wikipedia: Binomial heap|https://en.wikipedia.org/wiki/Binomial_heap"};
    vector<string> lcBinomial = {
        "LeetCode #23 (Merge k Sorted Lists)",
        "LeetCode #703 (Kth Largest Element in a Stream)",
        "LeetCode #1046 (Last Stone Weight)"};

    dsRegisterIntSim(m, "Binomial Heap", "Up to 8 keys inserted one by one", "10, 20, 5, 30, 15, 8, 25, 12",
                     {"insert(x):", "    t = new tree B0 holding x;", "    while (the heap already has a tree of the same degree) {", "        link the two: smaller root on top, other becomes its child;", "    }"},
                     refBinomial, lcBinomial, simBinomialInsert);
    dsRegisterIntSim(m, "Binomial Heap#1", "How many extract-min (1 to 3), then up to 8 keys", "1, 10, 20, 5, 30, 15, 8, 25, 12",
                     {"extractMin():", "    find the root r with the smallest key;  remove r from the root list;", "    r's children (degrees k-1 ... 0) become a new binomial heap;", "    union the two heaps: link equal degrees like binary addition;"},
                     refBinomial, lcBinomial, simBinomialExtract);
    dsSetProgramNames("Binomial Heap", {"Insert (like binary addition)", "Extract-min (and merge)"});

    vector<string> refFib = {
        "GeeksforGeeks: Fibonacci heap|https://www.geeksforgeeks.org/fibonacci-heap-set-1-introduction/",
        "Wikipedia: Fibonacci heap|https://en.wikipedia.org/wiki/Fibonacci_heap",
        "VisuAlgo: heap|https://visualgo.net/en/heap"};
    vector<string> lcFib = {
        "LeetCode #743 (Network Delay Time: Dijkstra with a priority queue)",
        "LeetCode #787 (Cheapest Flights Within K Stops)",
        "LeetCode #1631 (Path With Minimum Effort)"};

    dsRegisterIntSim(m, "Fibonacci Heap", "Up to 8 keys to insert, then one extract-min", "10, 20, 5, 30, 15, 8, 25, 12",
                     {"insert(x):  add a one-node tree to the ROOT LIST;  update min;      // O(1), no linking", "extractMin():", "    remove the min root;  promote its children to the root list;", "    consolidate: link roots of equal degree until all degrees differ;"},
                     refFib, lcFib, simFibonacciInsertAndExtract);
    dsRegisterIntSim(m, "Fibonacci Heap#1", "Old key, new smaller key, then up to 8 keys", "30, 2, 10, 20, 5, 30, 15, 8, 25, 12",
                     {"decreaseKey(x, newKey):", "    x->key = newKey;", "    if (x has a parent p and x->key < p->key) {", "        cut x from p and move it to the root list;", "        while (p is marked)  cut p too (cascading cut);   mark the first unmarked parent;", "    }", "    update min;"},
                     refFib, lcFib, simFibonacciDecreaseKey);
    dsSetProgramNames("Fibonacci Heap", {"Insert (lazy) and extract-min (consolidate)", "Decrease-key (cut and cascading cut)"});
}
