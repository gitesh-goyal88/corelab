// Data Structures (Semester 3), part 5: Module 4 (BST, AVL, red-black, multiway trees, B-tree, B+ tree).
#pragma once
#include "ds4.hpp"

using namespace std;

class BinaryTreePool {
public:
    TreeDiagram tree;
    vector<int> par;
    vector<int> heightArr;
    vector<char> isRed;
    int root;

    BinaryTreePool() {
        root = -1;
    }

    int createNode(int keyVal) {
        int idx = tree.addNode(keyVal, {-1, -1});
        par.push_back(-1);
        heightArr.push_back(1);
        isRed.push_back(0);
        return idx;
    }

    int leftChild(int idx) const {
        return idx < 0 ? -1 : tree.ch[idx][0];
    }

    int rightChild(int idx) const {
        return idx < 0 ? -1 : tree.ch[idx][1];
    }

    int heightOf(int idx) const {
        return idx < 0 ? 0 : heightArr[idx];
    }

    void updateHeight(int idx) {
        heightArr[idx] = 1 + max(heightOf(leftChild(idx)), heightOf(rightChild(idx)));
    }

    int balanceFactor(int idx) const {
        return heightOf(leftChild(idx)) - heightOf(rightChild(idx));
    }

    void setChild(int parentIdx, int side, int childIdx) {
        if (parentIdx < 0) {
            root = childIdx;
            tree.root = childIdx;
        } else {
            tree.ch[parentIdx][side] = childIdx;
        }
        if (childIdx >= 0) {
            par[childIdx] = parentIdx;
        }
    }

    int childSide(int parentIdx, int childIdx) const {
        return (parentIdx >= 0 && tree.ch[parentIdx][0] == childIdx) ? 0 : 1;
    }

    void rotateRight(int pivotY) {
        int pivotX = leftChild(pivotY);
        int parentIdx = par[pivotY];
        int side = childSide(parentIdx, pivotY);
        int movedSubtree = rightChild(pivotX);

        tree.ch[pivotY][0] = movedSubtree;
        if (movedSubtree >= 0) par[movedSubtree] = pivotY;
        tree.ch[pivotX][1] = pivotY;
        par[pivotY] = pivotX;
        setChild(parentIdx, side, pivotX);
        updateHeight(pivotY);
        updateHeight(pivotX);
    }

    void rotateLeft(int pivotX) {
        int pivotY = rightChild(pivotX);
        int parentIdx = par[pivotX];
        int side = childSide(parentIdx, pivotX);
        int movedSubtree = leftChild(pivotY);

        tree.ch[pivotX][1] = movedSubtree;
        if (movedSubtree >= 0) par[movedSubtree] = pivotX;
        tree.ch[pivotY][0] = pivotX;
        par[pivotX] = pivotY;
        setChild(parentIdx, side, pivotY);
        updateHeight(pivotX);
        updateHeight(pivotY);
    }

    int insertPlain(int keyVal, vector<int>* searchPath = nullptr) {
        if (root < 0) {
            int newNode = createNode(keyVal);
            root = newNode;
            tree.root = newNode;
            return newNode;
        }
        int cur = root;
        while (true) {
            if (tree.key[cur] == keyVal) return -1;
            if (searchPath) searchPath->push_back(cur);
            int side = (keyVal < tree.key[cur]) ? 0 : 1;
            int childIdx = tree.ch[cur][side];
            if (childIdx < 0) {
                int newNode = createNode(keyVal);
                tree.ch[cur][side] = newNode;
                par[newNode] = cur;
                return newNode;
            }
            cur = childIdx;
        }
    }

    int minNode(int startIdx) const {
        int cur = startIdx;
        while (leftChild(cur) >= 0) {
            cur = leftChild(cur);
        }
        return cur;
    }

    // Physically removes nodeIdx (which has at most one child); returns the parent where rebalancing starts.
    int cutNode(int nodeIdx) {
        int replacement = (leftChild(nodeIdx) >= 0) ? leftChild(nodeIdx) : rightChild(nodeIdx);
        int parentIdx = par[nodeIdx];
        int side = childSide(parentIdx, nodeIdx);
        setChild(parentIdx, side, replacement);
        par[nodeIdx] = -1;
        return parentIdx;
    }
};

static void applyAvlLabels(BinaryTreePool& pool) {
    for (size_t i = 0; i < pool.tree.key.size(); ++i) {
        int bf = pool.balanceFactor((int)i);
        string signPad = (bf > 0) ? " +" : " ";
        pool.tree.lab[i] = S(pool.tree.key[i]) + signPad + (bf != 0 ? S(bf) : "0");
    }
}

static void clearNodeLabels(BinaryTreePool& pool) {
    for (size_t i = 0; i < pool.tree.lab.size(); ++i) {
        pool.tree.lab[i].clear();
    }
}

static void applyRedBlackColors(BinaryTreePool& pool) {
    for (size_t i = 0; i < pool.tree.key.size(); ++i) {
        pool.tree.cls[i] = pool.isRed[i] ? "red" : "dark";
    }
}

static int getBstHeight(const BinaryTreePool& pool) {
    return pool.heightOf(pool.root);
}

static const vector<string>& dsBstRefs() {
    static const vector<string> refs = {
        "VisuAlgo: BST and AVL|https://visualgo.net/en/bst",
        "GeeksforGeeks: Binary search tree|https://www.geeksforgeeks.org/binary-search-tree-data-structure/",
        "USFCA: BST / AVL visualization|https://www.cs.usfca.edu/~galles/visualization/BST.html"};
    return refs;
}

class MultiwayNode {
public:
    vector<int> keys;
    vector<int> ch;
    int par;
    bool leaf;
    int nextLeaf;

    MultiwayNode() {
        par = -1;
        leaf = true;
        nextLeaf = -1;
    }
};

static TreeDiagram buildMultiwayDiagram(const vector<MultiwayNode>& nodes, int rootIdx) {
    TreeDiagram tree;
    for (size_t i = 0; i < nodes.size(); ++i) {
        tree.addNode((int)i, {});
        string label;
        for (size_t k = 0; k < nodes[i].keys.size(); ++k) {
            if (!label.empty()) label += "|";
            label += S(nodes[i].keys[k]);
        }
        tree.lab[i] = label.empty() ? "-" : label;
        tree.ch[i] = nodes[i].ch;
    }
    tree.root = rootIdx;
    return tree;
}

static string formatMultiwayKeys(const vector<int>& keys) {
    string joined;
    for (size_t i = 0; i < keys.size(); ++i) {
        if (!joined.empty()) joined += " | ";
        joined += S(keys[i]);
    }
    return joined;
}

class MultiwayTree {
public:
    vector<MultiwayNode> nodes;
    int root;
    int maxKeys;
    bool isBPlus;
    bool recordSteps;
    Steps* stepsOut;

    MultiwayTree() {
        root = -1;
        maxKeys = 3;
        isBPlus = false;
        recordSteps = false;
        stepsOut = nullptr;
    }

    vector<pair<int, int>> getLeafLinks() const {
        vector<pair<int, int>> result;
        if (!isBPlus || root < 0) return result;
        int cur = root;
        while (!nodes[cur].leaf) {
            cur = nodes[cur].ch[0];
        }
        while (cur >= 0 && nodes[cur].nextLeaf >= 0) {
            result.push_back({cur, nodes[cur].nextLeaf});
            cur = nodes[cur].nextLeaf;
        }
        return result;
    }

    void recordStep(int lineNum, const string& noteText, vector<string> highlightIds,
                    const string& caption, Vars varsList = {}) {
        if (recordSteps && stepsOut != nullptr) {
            stepsOut->push_back(dsDiagramStep(lineNum, noteText, varsList,
                                              dsDrawTree(buildMultiwayDiagram(nodes, root), false, caption, {}, false, getLeafLinks()),
                                              highlightIds));
        }
    }

    static int countKeysBefore(const vector<int>& keys, int target, bool inclusive) {
        int count = 0;
        for (size_t i = 0; i < keys.size(); ++i) {
            if (inclusive ? (keys[i] <= target) : (keys[i] < target)) {
                ++count;
            }
        }
        return count;
    }

    static bool containsKey(const vector<int>& keys, int target) {
        return find(keys.begin(), keys.end(), target) != keys.end();
    }

    void insertKey(int keyVal) {
        if (root < 0) {
            MultiwayNode rootNode;
            rootNode.keys = {keyVal};
            nodes.push_back(rootNode);
            root = 0;
            recordStep(1, "The tree is empty: " + S(keyVal) + " becomes the first key of the root node.", {"t0"}, "first node");
            return;
        }
        int cur = root;
        int guard = 0;
        while (!nodes[cur].leaf && guard++ < 30) {
            int childSlot = countKeysBefore(nodes[cur].keys, keyVal, isBPlus);
            if (!isBPlus && containsKey(nodes[cur].keys, keyVal)) {
                recordStep(1, S(keyVal) + " is already in an internal node: skipped.", {"t" + S(cur)}, "duplicate");
                return;
            }
            recordStep(1, "Node [" + formatMultiwayKeys(nodes[cur].keys) + "]: " + S(keyVal) + " fits child number " + S(childSlot) + " (between the keys), so go down there.",
                       {"t" + S(cur), "e" + S(nodes[cur].ch[childSlot])}, "descend");
            cur = nodes[cur].ch[childSlot];
        }
        if (containsKey(nodes[cur].keys, keyVal)) {
            recordStep(1, S(keyVal) + " is already in the tree: skipped.", {"t" + S(cur)}, "duplicate");
            return;
        }
        int insertPos = countKeysBefore(nodes[cur].keys, keyVal, false);
        nodes[cur].keys.insert(nodes[cur].keys.begin() + insertPos, keyVal);
        recordStep(2, "Insert " + S(keyVal) + " into the leaf in sorted position: [" + formatMultiwayKeys(nodes[cur].keys) + "].",
                   {"t" + S(cur)}, "insert into the leaf");

        guard = 0;
        while ((int)nodes[cur].keys.size() > maxKeys && guard++ < 30) {
            recordStep(3, "The node now holds " + S((int)nodes[cur].keys.size()) + " keys but the limit is " + S(maxKeys) + ": it OVERFLOWS and must split.",
                       {"t" + S(cur)}, "overflow");
            bool isLeafNode = nodes[cur].leaf;
            int totalKeys = (int)nodes[cur].keys.size();
            int midIdx = (isBPlus && isLeafNode) ? ((totalKeys + 1) / 2) : (totalKeys / 2);
            int promotedKey = nodes[cur].keys[midIdx];

            MultiwayNode rightNode;
            rightNode.leaf = isLeafNode;
            int rightStart = (isBPlus && isLeafNode) ? midIdx : (midIdx + 1);
            rightNode.keys.assign(nodes[cur].keys.begin() + rightStart, nodes[cur].keys.end());
            if (!isLeafNode) {
                rightNode.ch.assign(nodes[cur].ch.begin() + midIdx + 1, nodes[cur].ch.end());
            }
            nodes[cur].keys.resize(midIdx);
            if (!isLeafNode) {
                nodes[cur].ch.resize(midIdx + 1);
            }

            int rightIdx = (int)nodes.size();
            rightNode.par = nodes[cur].par;
            if (isBPlus && isLeafNode) {
                rightNode.nextLeaf = nodes[cur].nextLeaf;
                nodes[cur].nextLeaf = rightIdx;
            }
            nodes.push_back(rightNode);
            for (size_t i = 0; i < nodes[rightIdx].ch.size(); ++i) {
                nodes[nodes[rightIdx].ch[i]].par = rightIdx;
            }

            string splitDesc = (isBPlus && isLeafNode)
                ? "leaf split: the separator " + S(promotedKey) + " is COPIED up (it also stays in the right leaf, so every key remains in a leaf)."
                : "the middle key " + S(promotedKey) + " MOVES up to the parent; the rest is divided into two nodes.";
            if (nodes[cur].par < 0) {
                MultiwayNode newRoot;
                newRoot.leaf = false;
                newRoot.keys = {promotedKey};
                newRoot.ch = {cur, rightIdx};
                int newRootIdx = (int)nodes.size();
                nodes.push_back(newRoot);
                nodes[cur].par = newRootIdx;
                nodes[rightIdx].par = newRootIdx;
                root = newRootIdx;
                recordStep(3, "Split: " + splitDesc + " There was no parent, so a NEW ROOT is created: the tree grows one level taller.",
                           {"t" + S(newRootIdx), "t" + S(cur), "t" + S(rightIdx)}, "root split");
                cur = newRootIdx;
            } else {
                int parentIdx = nodes[cur].par;
                int slot = (int)(find(nodes[parentIdx].ch.begin(), nodes[parentIdx].ch.end(), cur) - nodes[parentIdx].ch.begin());
                nodes[parentIdx].keys.insert(nodes[parentIdx].keys.begin() + slot, promotedKey);
                nodes[parentIdx].ch.insert(nodes[parentIdx].ch.begin() + slot + 1, rightIdx);
                nodes[rightIdx].par = parentIdx;
                recordStep(3, "Split: " + splitDesc + " The parent now has [" + formatMultiwayKeys(nodes[parentIdx].keys) + "].",
                           {"t" + S(parentIdx), "t" + S(cur), "t" + S(rightIdx)}, "node split");
                cur = parentIdx;
            }
        }
    }
};

// ==================== Red-Black Tree Engine ====================

static void rbSnapStep(BinaryTreePool& pool, Steps* stepsOut, bool recordSteps, int lineNum,
                       const string& noteText, vector<string> highlightIds, const string& caption,
                       Vars varsList = {}, int doubleBlackNode = -1) {
    if (!recordSteps || stepsOut == nullptr) return;
    applyRedBlackColors(pool);
    if (doubleBlackNode >= 0) pool.tree.cls[doubleBlackNode] = "blue";
    stepsOut->push_back(dsDiagramStep(lineNum, noteText, varsList, dsDrawTree(pool.tree, true, caption), highlightIds));
}

static string rbColorName(const BinaryTreePool& pool, int nodeIdx) {
    if (nodeIdx < 0) return "black NULL";
    return pool.isRed[nodeIdx] ? "red" : "black";
}

static int rbHeight(const BinaryTreePool& pool, int nodeIdx) {
    if (nodeIdx < 0) return 0;
    return 1 + max(rbHeight(pool, pool.leftChild(nodeIdx)), rbHeight(pool, pool.rightChild(nodeIdx)));
}

static int rbCountNodes(const BinaryTreePool& pool, int nodeIdx) {
    if (nodeIdx < 0) return 0;
    return 1 + rbCountNodes(pool, pool.leftChild(nodeIdx)) + rbCountNodes(pool, pool.rightChild(nodeIdx));
}

static int rbBlackHeight(const BinaryTreePool& pool) {
    int blackCount = 0;
    for (int u = pool.root; u >= 0; u = pool.leftChild(u)) {
        if (!pool.isRed[u]) ++blackCount;
    }
    return blackCount;
}

static bool rbIsRed(const BinaryTreePool& pool, int nodeIdx) {
    return nodeIdx >= 0 && pool.isRed[nodeIdx];
}

static int rbInsert(BinaryTreePool& pool, int keyVal, Steps* stepsOut, bool recordSteps) {
    vector<int> path;
    bool duplicate = false;
    for (int u = pool.root; u >= 0;) {
        if (pool.tree.key[u] == keyVal) {
            duplicate = true;
            break;
        }
        path.push_back(u);
        u = (keyVal < pool.tree.key[u]) ? pool.leftChild(u) : pool.rightChild(u);
    }
    if (duplicate) {
        rbSnapStep(pool, stepsOut, recordSteps, 0, S(keyVal) + " already present: skipped.", {}, "duplicate");
        return -1;
    }
    if (recordSteps) {
        for (size_t i = 0; i < path.size(); ++i) {
            int u = path[i];
            bool goLeft = (keyVal < pool.tree.key[u]);
            rbSnapStep(pool, stepsOut, true, 0,
                       "Insert " + S(keyVal) + ": compare with " + S(pool.tree.key[u]) + " (" + rbColorName(pool, u) + "). " +
                       S(keyVal) + (goLeft ? " < " : " > ") + S(pool.tree.key[u]) + ", so go " + (goLeft ? "LEFT." : "RIGHT."),
                       {"t" + S(u)}, "searching for the empty spot", {{"key", S(keyVal)}, {"node", S(pool.tree.key[u])}});
        }
    }

    int newNode = pool.insertPlain(keyVal);
    pool.isRed[newNode] = 1;
    vector<string> newHighlight = (pool.par[newNode] >= 0) ? vector<string>{"t" + S(newNode), "e" + S(newNode)} : vector<string>{"t" + S(newNode)};
    rbSnapStep(pool, stepsOut, recordSteps, 0,
               "Insert " + S(keyVal) + " as a RED leaf (a new red node never breaks rule 4).",
               newHighlight, "new red leaf", {{"inserted", S(keyVal)}});

    int guard = 0;
    while (guard++ < 40) {
        int parentIdx = pool.par[newNode];
        if (parentIdx < 0) break;
        if (!pool.isRed[parentIdx]) {
            rbSnapStep(pool, stepsOut, recordSteps, 1,
                       "Parent " + S(pool.tree.key[parentIdx]) + " is BLACK, so there are no two reds in a row: the tree is already valid.",
                       {"t" + S(parentIdx), "t" + S(newNode)}, "parent is black");
            break;
        }
        int grandParent = pool.par[parentIdx];
        if (grandParent < 0) break;
        bool parentIsLeft = (pool.leftChild(grandParent) == parentIdx);
        int uncleIdx = parentIsLeft ? pool.rightChild(grandParent) : pool.leftChild(grandParent);
        string uncleDesc = (uncleIdx >= 0) ? (S(pool.tree.key[uncleIdx]) + ", " + rbColorName(pool, uncleIdx)) : string("NULL = black");
        rbSnapStep(pool, stepsOut, recordSteps, 1,
                   "Parent " + S(pool.tree.key[parentIdx]) + " and " + S(pool.tree.key[newNode]) + " are both RED: two reds in a row break rule 3. Look at the uncle (" + uncleDesc + ").",
                   {"t" + S(newNode), "t" + S(parentIdx)}, "red-red conflict");

        if (uncleIdx >= 0 && pool.isRed[uncleIdx]) {
            pool.isRed[parentIdx] = 0;
            pool.isRed[uncleIdx] = 0;
            pool.isRed[grandParent] = 1;
            rbSnapStep(pool, stepsOut, recordSteps, 2,
                       "Parent " + S(pool.tree.key[parentIdx]) + " and uncle " + S(pool.tree.key[uncleIdx]) + " are both RED: recolor them black and the grandparent " + S(pool.tree.key[grandParent]) + " red. Then check the grandparent.",
                       {"t" + S(parentIdx), "t" + S(uncleIdx), "t" + S(grandParent)}, "recolor");
            newNode = grandParent;
            continue;
        }
        if (parentIsLeft) {
            if (pool.rightChild(parentIdx) == newNode) {
                rbSnapStep(pool, stepsOut, recordSteps, 3,
                           "Uncle is black and " + S(pool.tree.key[newNode]) + " is the RIGHT child of a LEFT child (a 'triangle'): rotate the parent left first.",
                           {"t" + S(newNode), "t" + S(parentIdx)}, "triangle case");
                pool.rotateLeft(parentIdx);
                newNode = parentIdx;
                parentIdx = pool.par[newNode];
            }
            pool.isRed[parentIdx] = 0;
            pool.isRed[grandParent] = 1;
            rbSnapStep(pool, stepsOut, recordSteps, 3,
                       "Uncle is black and the three nodes form a straight line: recolor, then rotate the grandparent " + S(pool.tree.key[grandParent]) + " right.",
                       {"t" + S(parentIdx), "t" + S(grandParent)}, "line case: recolor");
            pool.rotateRight(grandParent);
        } else {
            if (pool.leftChild(parentIdx) == newNode) {
                rbSnapStep(pool, stepsOut, recordSteps, 3,
                           "Uncle is black and " + S(pool.tree.key[newNode]) + " is the LEFT child of a RIGHT child (a 'triangle'): rotate the parent right first.",
                           {"t" + S(newNode), "t" + S(parentIdx)}, "triangle case");
                pool.rotateRight(parentIdx);
                newNode = parentIdx;
                parentIdx = pool.par[newNode];
            }
            pool.isRed[parentIdx] = 0;
            pool.isRed[grandParent] = 1;
            rbSnapStep(pool, stepsOut, recordSteps, 3,
                       "Uncle is black and the three nodes form a straight line: recolor, then rotate the grandparent " + S(pool.tree.key[grandParent]) + " left.",
                       {"t" + S(parentIdx), "t" + S(grandParent)}, "line case: recolor");
            pool.rotateLeft(grandParent);
        }
        rbSnapStep(pool, stepsOut, recordSteps, 3,
                   "Rotation done: the red-red conflict is gone and no more fixing is needed.",
                   {"t" + S(parentIdx)}, "balanced");
        break;
    }

    if (pool.isRed[pool.root]) {
        pool.isRed[pool.root] = 0;
        rbSnapStep(pool, stepsOut, recordSteps, 5, "The root must be black: recolor it.", {"t" + S(pool.root)}, "root black");
    }
    return newNode;
}

static bool rbDelete(BinaryTreePool& pool, int keyVal, Steps* stepsOut, bool recordSteps) {
    int targetIdx = pool.root;
    while (targetIdx >= 0 && pool.tree.key[targetIdx] != keyVal) {
        bool goLeft = (keyVal < pool.tree.key[targetIdx]);
        rbSnapStep(pool, stepsOut, recordSteps, 0,
                   S(keyVal) + (goLeft ? " < " : " > ") + S(pool.tree.key[targetIdx]) + ": go " + (goLeft ? "left." : "right."),
                   {"t" + S(targetIdx)}, "searching", {{"key", S(keyVal)}});
        targetIdx = goLeft ? pool.leftChild(targetIdx) : pool.rightChild(targetIdx);
    }
    if (targetIdx < 0) {
        rbSnapStep(pool, stepsOut, recordSteps, 0, S(keyVal) + " is not in the tree: nothing to delete.", {}, "not found");
        return false;
    }
    rbSnapStep(pool, stepsOut, recordSteps, 0,
               "Found " + S(keyVal) + " (" + rbColorName(pool, targetIdx) + "). Delete it as in an ordinary BST, then repair the colours.",
               {"t" + S(targetIdx)}, "node found", {{"key", S(keyVal)}});

    int removeNode = targetIdx;
    if (pool.leftChild(targetIdx) >= 0 && pool.rightChild(targetIdx) >= 0) {
        removeNode = pool.minNode(pool.rightChild(targetIdx));
        rbSnapStep(pool, stepsOut, recordSteps, 1,
                   "TWO children: the inorder successor (smallest key in the right subtree) is " + S(pool.tree.key[removeNode]) + ". Copy it into this node (the node keeps its own colour), then remove the successor node instead.",
                   {"t" + S(targetIdx), "t" + S(removeNode)}, "successor", {{"successor", S(pool.tree.key[removeNode])}});
        pool.tree.key[targetIdx] = pool.tree.key[removeNode];
        rbSnapStep(pool, stepsOut, recordSteps, 1,
                   "Key copied. The node that is really removed now is the old successor node: it has no left child, so it has at most one child.",
                   {"t" + S(targetIdx), "t" + S(removeNode)}, "key copied");
    }

    bool removedWasRed = pool.isRed[removeNode];
    int replacementIdx = (pool.leftChild(removeNode) >= 0) ? pool.leftChild(removeNode) : pool.rightChild(removeNode);
    int parentIdx = pool.par[removeNode];
    int replacementSide = pool.childSide(parentIdx, removeNode);
    int removedKey = pool.tree.key[removeNode];

    string childDesc = (replacementIdx >= 0)
        ? (" and its only child is " + S(pool.tree.key[replacementIdx]) + " (" + rbColorName(pool, replacementIdx) + ").")
        : " and has no real children (its children are black NULLs).";
    vector<string> rmHl = (replacementIdx >= 0) ? vector<string>{"t" + S(removeNode), "t" + S(replacementIdx)} : vector<string>{"t" + S(removeNode)};
    rbSnapStep(pool, stepsOut, recordSteps, 2,
               "The node to remove (" + S(removedKey) + ") is " + rbColorName(pool, removeNode) + childDesc,
               rmHl, "node to remove");

    pool.cutNode(removeNode);
    if (pool.root < 0) {
        rbSnapStep(pool, stepsOut, recordSteps, 2, "That was the last node: the tree is now empty.", {}, "empty tree");
        return true;
    }
    vector<string> parentHl = (parentIdx >= 0) ? vector<string>{"t" + S(parentIdx)} : vector<string>{};
    if (removedWasRed) {
        rbSnapStep(pool, stepsOut, recordSteps, 3,
                   "The removed node was RED: no black node disappeared and no two reds can meet, so nothing else changes. Done.",
                   parentHl, "red node removed");
        return true;
    }
    if (replacementIdx >= 0 && pool.isRed[replacementIdx]) {
        pool.isRed[replacementIdx] = 0;
        rbSnapStep(pool, stepsOut, recordSteps, 4,
                   "The removed node was BLACK but its child " + S(pool.tree.key[replacementIdx]) + " is RED: color that child BLACK. The path keeps the same number of black nodes. Done.",
                   {"t" + S(replacementIdx)}, "child recolored black");
        return true;
    }

    string spotDesc = (replacementIdx >= 0)
        ? "drawn blue"
        : ("the empty " + string(replacementSide == 0 ? "left" : "right") + " child slot of " + S(pool.tree.key[parentIdx]));
    rbSnapStep(pool, stepsOut, recordSteps, 5,
               "The removed node was BLACK and its replacement is black (or NULL): every path through this spot now has ONE black node too few. This spot is 'double black' (" + spotDesc + "). The loop below moves the problem up or fixes it with a rotation.",
               parentHl, "double black", {}, replacementIdx);

    bool fixedByRotation = false;
    int guard = 0;
    while (replacementIdx != pool.root && !rbIsRed(pool, replacementIdx) && guard++ < 40) {
        bool isLeftSpot = (replacementSide == 0);
        int siblingIdx = isLeftSpot ? pool.rightChild(parentIdx) : pool.leftChild(parentIdx);
        if (siblingIdx < 0) break;
        string rotateDir = isLeftSpot ? "left" : "right";

        if (rbIsRed(pool, siblingIdx)) {
            rbSnapStep(pool, stepsOut, recordSteps, 7,
                       "Case 1: the sibling " + S(pool.tree.key[siblingIdx]) + " is RED. Make the sibling BLACK and the parent " + S(pool.tree.key[parentIdx]) + " RED, then rotate the parent " + rotateDir + " (toward the double-black spot). The spot gets a BLACK sibling and one of cases 2-4 applies.",
                       {"t" + S(parentIdx), "t" + S(siblingIdx)}, "case 1: red sibling", {}, replacementIdx);
            pool.isRed[siblingIdx] = 0;
            pool.isRed[parentIdx] = 1;
            if (isLeftSpot) pool.rotateLeft(parentIdx);
            else pool.rotateRight(parentIdx);
            siblingIdx = isLeftSpot ? pool.rightChild(parentIdx) : pool.leftChild(parentIdx);
            rbSnapStep(pool, stepsOut, recordSteps, 7,
                       "After the rotation the new sibling of the spot is " + (siblingIdx >= 0 ? S(pool.tree.key[siblingIdx]) : string("NULL")) + " (black).",
                       siblingIdx >= 0 ? vector<string>{"t" + S(siblingIdx)} : vector<string>{}, "after case 1", {}, replacementIdx);
            if (siblingIdx < 0) break;
        }

        int sibLeft = pool.leftChild(siblingIdx);
        int sibRight = pool.rightChild(siblingIdx);
        int nearChild = isLeftSpot ? sibLeft : sibRight;
        int farChild = isLeftSpot ? sibRight : sibLeft;

        if (!rbIsRed(pool, sibLeft) && !rbIsRed(pool, sibRight)) {
            pool.isRed[siblingIdx] = 1;
            rbSnapStep(pool, stepsOut, recordSteps, 8,
                       "Case 2: the sibling " + S(pool.tree.key[siblingIdx]) + " is black and both its children are black. Color the sibling RED (its side loses one black, so both sides match again) and move the double-black problem UP to the parent " +
                       S(pool.tree.key[parentIdx]) + (pool.isRed[parentIdx] ? " (it is red, so it will simply turn black next)." : " (drawn blue)."),
                       {"t" + S(siblingIdx), "t" + S(parentIdx)}, "case 2: sibling red, move up", {}, pool.isRed[parentIdx] ? -1 : parentIdx);
            replacementIdx = parentIdx;
            parentIdx = pool.par[replacementIdx];
            if (parentIdx >= 0) replacementSide = pool.childSide(parentIdx, replacementIdx);
            continue;
        }

        if (!rbIsRed(pool, farChild)) {
            int nearIdx = nearChild;
            rbSnapStep(pool, stepsOut, recordSteps, 9,
                       "Case 3: the sibling " + S(pool.tree.key[siblingIdx]) + " is black, its far child is black but its near child " + S(pool.tree.key[nearIdx]) + " is RED. Swap their colours and rotate the sibling " +
                       (isLeftSpot ? "right" : "left") + " (away from the spot) so the far child becomes red.",
                       {"t" + S(siblingIdx), "t" + S(nearIdx)}, "case 3: near child red", {}, replacementIdx);
            pool.isRed[nearIdx] = 0;
            pool.isRed[siblingIdx] = 1;
            if (isLeftSpot) pool.rotateRight(siblingIdx);
            else pool.rotateLeft(siblingIdx);
            siblingIdx = isLeftSpot ? pool.rightChild(parentIdx) : pool.leftChild(parentIdx);
            farChild = isLeftSpot ? pool.rightChild(siblingIdx) : pool.leftChild(siblingIdx);
            rbSnapStep(pool, stepsOut, recordSteps, 9,
                       "After the rotation the sibling is " + S(pool.tree.key[siblingIdx]) + " (black) and its far child " + S(pool.tree.key[farChild]) + " is RED: now case 4 applies.",
                       {"t" + S(siblingIdx), "t" + S(farChild)}, "after case 3", {}, replacementIdx);
        }

        rbSnapStep(pool, stepsOut, recordSteps, 10,
                   "Case 4: the sibling " + S(pool.tree.key[siblingIdx]) + " is black and its far child " + S(pool.tree.key[farChild]) + " is RED. The sibling takes the parent's colour, the parent " +
                   S(pool.tree.key[parentIdx]) + " becomes black, the far child becomes black, then rotate the parent " + rotateDir + ". The missing black is restored.",
                   {"t" + S(parentIdx), "t" + S(siblingIdx), "t" + S(farChild)}, "case 4: rotate and recolor", {}, replacementIdx);
        pool.isRed[siblingIdx] = pool.isRed[parentIdx];
        pool.isRed[parentIdx] = 0;
        pool.isRed[farChild] = 0;
        if (isLeftSpot) pool.rotateLeft(parentIdx);
        else pool.rotateRight(parentIdx);
        replacementIdx = pool.root;
        fixedByRotation = true;
        rbSnapStep(pool, stepsOut, recordSteps, 10,
                   "Rotation done: every path has its black count back and there are no two reds in a row. Done.",
                   {"t" + S(siblingIdx)}, "balanced");
        break;
    }

    if (!fixedByRotation) {
        if (replacementIdx >= 0 && pool.isRed[replacementIdx]) {
            pool.isRed[replacementIdx] = 0;
            rbSnapStep(pool, stepsOut, recordSteps, 6,
                       "The spot reached a RED node (" + S(pool.tree.key[replacementIdx]) + "): color it BLACK. That supplies the missing black. Done.",
                       {"t" + S(replacementIdx)}, "red node becomes black");
        } else {
            rbSnapStep(pool, stepsOut, recordSteps, 6,
                       "The double-black spot reached the ROOT: every path lost one black node, so black counts are equal again and nothing else is needed. Done.",
                       {"t" + S(pool.root)}, "double black reached the root");
        }
    }
    if (pool.root >= 0 && pool.isRed[pool.root]) {
        pool.isRed[pool.root] = 0;
        rbSnapStep(pool, stepsOut, recordSteps, 11, "The root must be black: recolor it.", {"t" + S(pool.root)}, "root black");
    }
    return true;
}

// ==================== Module 4 Simulations ====================

static string drawBstDiagram(BinaryTreePool& pool, const string& caption, bool useAvlLabels = false) {
    if (useAvlLabels) applyAvlLabels(pool);
    else clearNodeLabels(pool);
    return dsDrawTree(pool.tree, true, caption);
}

static Steps simBstInsert(const Ints& input) {
    Ints keys = dsTakeInts(input, 0, 9, {50, 30, 70, 20, 40, 60, 80, 35});
    BinaryTreePool pool;
    Steps steps;
    steps.push_back(dsDiagramStep(0, "BST rule: for every node, ALL keys in the left subtree are smaller and ALL keys in the right subtree are larger. Inorder traversal gives sorted order.",
                                  {}, drawBstDiagram(pool, "empty tree"), {}));
    for (size_t kIdx = 0; kIdx < keys.size(); ++kIdx) {
        int keyVal = keys[kIdx];
        vector<int> path;
        int newNode = pool.insertPlain(keyVal, &path);
        if (newNode < 0) {
            steps.push_back(dsDiagramStep(1, S(keyVal) + " is already in the tree: BSTs here keep distinct keys, so nothing is inserted.",
                                          {{"key", S(keyVal)}}, drawBstDiagram(pool, "duplicate ignored"), {}));
            continue;
        }
        BinaryTreePool previewPool = pool;
        for (size_t i = 0; i < path.size(); ++i) {
            int u = path[i];
            int parentOfNew = previewPool.par[newNode];
            int newSide = (previewPool.tree.ch[parentOfNew][0] == newNode) ? 0 : 1;
            previewPool.tree.ch[parentOfNew][newSide] = -1;
            steps.push_back(dsDiagramStep(2, "Insert " + S(keyVal) + ": compare with " + S(pool.tree.key[u]) + ". " +
                                          S(keyVal) + (keyVal < pool.tree.key[u] ? " < " : " > ") + S(pool.tree.key[u]) + ", so go " + (keyVal < pool.tree.key[u] ? "LEFT." : "RIGHT."),
                                          {{"key", S(keyVal)}, {"node", S(pool.tree.key[u])}},
                                          dsDrawTree(previewPool.tree, true, "searching for the empty spot"), {"t" + S(u)}));
        }
        steps.push_back(dsDiagramStep(1, "Found an empty spot: " + S(keyVal) + " becomes a new leaf. Height is now " + S(getBstHeight(pool)) + ".",
                                      {{"inserted", S(keyVal)}, {"height", S(getBstHeight(pool))}},
                                      drawBstDiagram(pool, "new leaf"), {"t" + S(newNode), "e" + S(newNode)}));
    }
    steps.push_back(dsDiagramStep(4, "Average insert, search and delete: O(log n). Worst case O(n) when keys arrive already sorted (next topic: balanced trees).",
                                  {{"height", S(getBstHeight(pool))}},
                                  drawBstDiagram(pool, "BST with " + S((int)pool.tree.key.size()) + " keys"), {}));
    return steps;
}

static Steps simBstSearch(const Ints& input) {
    int keyVal = input.empty() ? 60 : input[0];
    Ints keys = dsTakeInts(input, 1, 9, {50, 30, 70, 20, 40, 60, 80, 35});
    BinaryTreePool pool;
    for (size_t i = 0; i < keys.size(); ++i) pool.insertPlain(keys[i]);
    Steps steps;
    int comparisons = 0;

    steps.push_back(dsDiagramStep(0, "Search for " + S(keyVal) + ". Every comparison throws away one whole subtree, like binary search.",
                                  {{"key", S(keyVal)}}, drawBstDiagram(pool, "start at the root"), {"t" + S(pool.root)}));
    int cur = pool.root;
    while (cur >= 0) {
        ++comparisons;
        if (pool.tree.key[cur] == keyVal) {
            pool.tree.cls[cur] = "weld";
            steps.push_back(dsDiagramStep(2, S(keyVal) + " == " + S(pool.tree.key[cur]) + ": FOUND after " + S(comparisons) + " comparison(s).",
                                          {{"comparisons", S(comparisons)}}, drawBstDiagram(pool, "found"), {"t" + S(cur)}));
            return steps;
        }
        bool goLeft = (keyVal < pool.tree.key[cur]);
        steps.push_back(dsDiagramStep(3, S(keyVal) + (goLeft ? " < " : " > ") + S(pool.tree.key[cur]) + ": go " + (goLeft ? "left" : "right") + ".",
                                      {{"node", S(pool.tree.key[cur])}, {"comparisons", S(comparisons)}},
                                      drawBstDiagram(pool, goLeft ? "go left" : "go right"), {"t" + S(cur)}));
        cur = goLeft ? pool.leftChild(cur) : pool.rightChild(cur);
    }
    steps.push_back(dsDiagramStep(5, "Reached NULL: " + S(keyVal) + " is NOT in the tree. Cost = the height of the tree.",
                                  {{"comparisons", S(comparisons)}}, drawBstDiagram(pool, "not found"), {}));
    return steps;
}

static Steps simBstDelete(const Ints& input) {
    int keyVal = input.empty() ? 50 : input[0];
    Ints keys = dsTakeInts(input, 1, 9, {50, 30, 70, 20, 40, 60, 80, 35});
    BinaryTreePool pool;
    for (size_t i = 0; i < keys.size(); ++i) pool.insertPlain(keys[i]);

    Steps steps;
    steps.push_back(dsDiagramStep(0, "Delete " + S(keyVal) + ". First find it, then handle one of three cases.",
                                  {{"key", S(keyVal)}}, drawBstDiagram(pool, "before"), {}));
    int targetIdx = pool.root;
    while (targetIdx >= 0 && pool.tree.key[targetIdx] != keyVal) {
        bool goLeft = (keyVal < pool.tree.key[targetIdx]);
        steps.push_back(dsDiagramStep(1, S(keyVal) + (goLeft ? " < " : " > ") + S(pool.tree.key[targetIdx]) + ": go " + (goLeft ? "left." : "right."),
                                      {}, drawBstDiagram(pool, "searching"), {"t" + S(targetIdx)}));
        targetIdx = goLeft ? pool.leftChild(targetIdx) : pool.rightChild(targetIdx);
    }
    if (targetIdx < 0) {
        steps.push_back(dsDiagramStep(1, S(keyVal) + " is not in the tree: nothing to delete.", {}, drawBstDiagram(pool, "not found"), {}));
        return steps;
    }

    pool.tree.cls[targetIdx] = "red";
    int leftIdx = pool.leftChild(targetIdx);
    int rightIdx = pool.rightChild(targetIdx);
    if (leftIdx < 0 && rightIdx < 0) {
        steps.push_back(dsDiagramStep(2, "Case 1: " + S(keyVal) + " is a LEAF. Just cut it off.", {}, drawBstDiagram(pool, "leaf"), {"t" + S(targetIdx)}));
        pool.cutNode(targetIdx);
    } else if (leftIdx < 0 || rightIdx < 0) {
        int onlyChild = (leftIdx >= 0) ? leftIdx : rightIdx;
        steps.push_back(dsDiagramStep(3, "Case 2: ONE child. The child takes the place of " + S(keyVal) + ".",
                                      {}, drawBstDiagram(pool, "one child"), {"t" + S(targetIdx), "t" + S(onlyChild)}));
        pool.cutNode(targetIdx);
    } else {
        int successorIdx = pool.minNode(rightIdx);
        steps.push_back(dsDiagramStep(5, "Case 3: TWO children. Find the inorder SUCCESSOR: the smallest key in the right subtree = " + S(pool.tree.key[successorIdx]) + ".",
                                      {{"successor", S(pool.tree.key[successorIdx])}}, drawBstDiagram(pool, "successor"), {"t" + S(successorIdx)}));
        pool.tree.key[targetIdx] = pool.tree.key[successorIdx];
        pool.tree.cls[targetIdx] = "";
        steps.push_back(dsDiagramStep(5, "Copy " + S(pool.tree.key[successorIdx]) + " into the node being deleted.",
                                      {}, drawBstDiagram(pool, "key copied"), {"t" + S(targetIdx)}));
        pool.tree.cls[successorIdx] = "red";
        pool.cutNode(successorIdx);
        steps.push_back(dsDiagramStep(6, "Now delete the successor node (it has at most one child). The BST rule still holds.",
                                      {}, drawBstDiagram(pool, "successor removed"), {"t" + S(targetIdx)}));
    }
    for (size_t i = 0; i < pool.tree.cls.size(); ++i) {
        if (pool.tree.cls[i] == "red") pool.tree.cls[i] = "";
    }
    steps.push_back(dsDiagramStep(7, "Done. Deletion costs O(height).", {}, drawBstDiagram(pool, "after"), {}));
    return steps;
}

static Steps simBalancedBstSkewed(const Ints& input) {
    Ints keys = dsTakeInts(input, 0, 8, {10, 20, 30, 40, 50, 60, 70});
    sort(keys.begin(), keys.end());
    BinaryTreePool pool;
    Steps steps;

    steps.push_back(dsDiagramStep(0, "What goes wrong without balancing: keys that arrive in sorted order build a CHAIN.",
                                  {}, drawBstDiagram(pool, "empty"), {}));
    for (size_t i = 0; i < keys.size(); ++i) {
        int newNode = pool.insertPlain(keys[i]);
        if (newNode < 0) continue;
        steps.push_back(dsDiagramStep(1, S(keys[i]) + " is larger than everything, so it goes to the far right. Height = " + S(getBstHeight(pool)) + ".",
                                      {{"inserted", S(keys[i])}, {"height", S(getBstHeight(pool))}},
                                      drawBstDiagram(pool, "height " + S(getBstHeight(pool))), {"t" + S(newNode)}));
    }
    int count = (int)pool.tree.key.size();
    int idealHeight = (int)ceil(log2((double)count + 1));
    steps.push_back(dsDiagramStep(2, "The tree is a linked list: height " + S(getBstHeight(pool)) + " for " + S(count) + " keys. A balanced tree would have height about " + S(idealHeight) + ". A balanced BST keeps height O(log n) always: AVL and red-black trees do this with rotations.",
                                  {{"height", S(getBstHeight(pool))}, {"balanced height", S(idealHeight)}},
                                  drawBstDiagram(pool, "skewed tree"), {}));
    return steps;
}

static int buildBalancedFromSorted(const Ints& arr, int lo, int hi, int parentIdx,
                                   BinaryTreePool& pool, Steps& steps, vector<string>& callStack) {
    if (lo > hi) return -1;
    int mid = (lo + hi) / 2;
    callStack.push_back("build(" + S(lo) + ", " + S(hi) + ")");
    int nodeIdx = pool.createNode(arr[mid]);
    if (parentIdx < 0) {
        pool.root = nodeIdx;
        pool.tree.root = nodeIdx;
    } else {
        int side = (arr[mid] < pool.tree.key[parentIdx]) ? 0 : 1;
        pool.tree.ch[parentIdx][side] = nodeIdx;
        pool.par[nodeIdx] = parentIdx;
    }
    Step step = dsArrayStep(3, "build(" + S(lo) + ".." + S(hi) + "): mid = " + S(mid) + ", so " + S(arr[mid]) + " becomes the root of this part.",
                            {{"lo", S(lo)}, {"mid", S(mid)}, {"hi", S(hi)}}, arr, mid,
                            dsMakeTags(arr.size(), {{lo, "lo"}, {mid, "mid"}, {hi, "hi"}}), lo, hi);
    step.dg = drawBstDiagram(pool, "balanced by construction");
    step.hl = {"t" + S(nodeIdx), "e" + S(nodeIdx)};
    step.stack = callStack;
    steps.push_back(step);

    buildBalancedFromSorted(arr, lo, mid - 1, nodeIdx, pool, steps, callStack);
    buildBalancedFromSorted(arr, mid + 1, hi, nodeIdx, pool, steps, callStack);
    callStack.pop_back();
    return nodeIdx;
}

static Steps simBalancedBstBuild(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 9, {10, 20, 30, 40, 50, 60, 70});
    sort(arr.begin(), arr.end());
    int count = (int)arr.size();
    BinaryTreePool pool;
    Steps steps;
    vector<string> callStack;

    Step startStep = dsArrayStep(0, "Pick the MIDDLE element as the root so both sides have (almost) the same size, then repeat for each half.",
                                 {{"n", S(count)}}, arr, -1, {}, 0, count - 1);
    startStep.dg = drawBstDiagram(pool, "empty");
    steps.push_back(startStep);

    buildBalancedFromSorted(arr, 0, count - 1, -1, pool, steps, callStack);
    steps.push_back(dsDiagramStep(6, "Built. Height = " + S(getBstHeight(pool)) + " for " + S(count) + " keys (the minimum possible). O(n) time.",
                                  {{"height", S(getBstHeight(pool))}}, drawBstDiagram(pool, "height-balanced BST"), {}));
    return steps;
}

// ---------- AVL Tree ----------

static void recordAvlSnap(BinaryTreePool& pool, Steps* stepsOut, bool recordSteps, int lineNum,
                          const string& noteText, vector<string> highlightIds, const string& caption) {
    if (recordSteps && stepsOut != nullptr) {
        applyAvlLabels(pool);
        stepsOut->push_back(dsDiagramStep(lineNum, noteText, {}, dsDrawTree(pool.tree, true, caption), highlightIds));
    }
}

static void avlRebalanceUpward(BinaryTreePool& pool, int startNode, bool stopAfterFirst, Steps* stepsOut, bool recordSteps) {
    int cur = startNode;
    int guard = 0;
    while (cur >= 0 && guard++ < 60) {
        pool.updateHeight(cur);
        int bf = pool.balanceFactor(cur);
        if (abs(bf) <= 1) {
            recordAvlSnap(pool, stepsOut, recordSteps, 1,
                          "Node " + S(pool.tree.key[cur]) + ": height " + S(pool.heightArr[cur]) + ", balance factor (left height - right height) = " + S(bf) + ". Within -1..+1: fine.",
                          {"t" + S(cur)}, "labels: key and balance factor");
            cur = pool.par[cur];
            continue;
        }
        int z = cur;
        string zKey = S(pool.tree.key[z]);
        if (bf > 1) {
            int l = pool.leftChild(z);
            if (pool.balanceFactor(l) >= 0) {
                recordAvlSnap(pool, stepsOut, recordSteps, 2,
                              "Node " + zKey + " has balance factor +" + S(bf) + " (left-heavy) and its left child leans left: LEFT-LEFT case. One RIGHT rotation fixes it.",
                              {"t" + S(z), "t" + S(l)}, "imbalance found");
                pool.rotateRight(z);
            } else {
                recordAvlSnap(pool, stepsOut, recordSteps, 2,
                              "Node " + zKey + " is left-heavy (+" + S(bf) + ") but its left child leans RIGHT: LEFT-RIGHT case. Two rotations: left on the child, then right on " + zKey + ".",
                              {"t" + S(z), "t" + S(l)}, "imbalance found");
                pool.rotateLeft(l);
                recordAvlSnap(pool, stepsOut, recordSteps, 3,
                              "First rotation (left rotate at " + S(pool.tree.key[l]) + ") done: now it looks like the left-left case.",
                              {"t" + S(l)}, "after 1st rotation");
                pool.rotateRight(z);
            }
        } else {
            int r = pool.rightChild(z);
            if (pool.balanceFactor(r) <= 0) {
                recordAvlSnap(pool, stepsOut, recordSteps, 2,
                              "Node " + zKey + " has balance factor " + S(bf) + " (right-heavy) and its right child leans right: RIGHT-RIGHT case. One LEFT rotation fixes it.",
                              {"t" + S(z), "t" + S(r)}, "imbalance found");
                pool.rotateLeft(z);
            } else {
                recordAvlSnap(pool, stepsOut, recordSteps, 2,
                              "Node " + zKey + " is right-heavy (" + S(bf) + ") but its right child leans LEFT: RIGHT-LEFT case. Two rotations: right on the child, then left on " + zKey + ".",
                              {"t" + S(z), "t" + S(r)}, "imbalance found");
                pool.rotateRight(r);
                recordAvlSnap(pool, stepsOut, recordSteps, 3,
                              "First rotation (right rotate at " + S(pool.tree.key[r]) + ") done: now it looks like the right-right case.",
                              {"t" + S(r)}, "after 1st rotation");
                pool.rotateLeft(z);
            }
        }
        int newTop = pool.par[z];
        recordAvlSnap(pool, stepsOut, recordSteps, 3,
                      "Rotation done. The subtree is balanced again and its height is back to what it was before.",
                      {"t" + S(newTop)}, "balanced");
        if (stopAfterFirst) return;
        cur = pool.par[newTop];
    }
}

static Steps simAvlInsert(const Ints& input) {
    Ints keys = dsTakeInts(input, 0, 9, {10, 20, 30, 40, 50, 25});
    BinaryTreePool pool;
    Steps steps;
    steps.push_back(dsDiagramStep(0, "AVL tree: a BST where for EVERY node the left and right heights differ by at most 1. After each insert we rebalance with rotations, so the height stays O(log n).",
                                  {}, dsDrawTree(pool.tree, true, "empty"), {}));
    for (size_t i = 0; i < keys.size(); ++i) {
        int keyVal = keys[i];
        int newNode = pool.insertPlain(keyVal);
        if (newNode < 0) {
            steps.push_back(dsDiagramStep(0, S(keyVal) + " is already present: skipped.", {}, dsDrawTree(pool.tree, true, "duplicate"), {}));
            continue;
        }
        applyAvlLabels(pool);
        steps.push_back(dsDiagramStep(0, "Insert " + S(keyVal) + " like a normal BST.",
                                      {{"inserted", S(keyVal)}}, dsDrawTree(pool.tree, true, "plain BST insert"), {"t" + S(newNode), "e" + S(newNode)}));
        avlRebalanceUpward(pool, pool.par[newNode], true, &steps, true);
    }
    applyAvlLabels(pool);
    steps.push_back(dsDiagramStep(1, "Height " + S(getBstHeight(pool)) + " for " + S((int)pool.tree.key.size()) + " keys. Search, insert, delete are all O(log n) guaranteed. At most 1 rotation (2 for double) per insert.",
                                  {{"height", S(getBstHeight(pool))}}, dsDrawTree(pool.tree, true, "AVL tree"), {}));
    return steps;
}

static Steps simAvlDelete(const Ints& input) {
    int keyVal = input.empty() ? 40 : input[0];
    Ints keys = dsTakeInts(input, 1, 9, {20, 10, 30, 25, 35, 50, 45, 40});
    BinaryTreePool pool;
    Steps dummySteps;
    for (size_t i = 0; i < keys.size(); ++i) {
        int newNode = pool.insertPlain(keys[i]);
        if (newNode >= 0) avlRebalanceUpward(pool, pool.par[newNode], true, &dummySteps, false);
    }

    Steps steps;
    applyAvlLabels(pool);
    steps.push_back(dsDiagramStep(0, "An AVL tree built from your keys. Delete " + S(keyVal) + ".",
                                  {{"key", S(keyVal)}}, dsDrawTree(pool.tree, true, "before"), {}));
    int targetIdx = pool.root;
    while (targetIdx >= 0 && pool.tree.key[targetIdx] != keyVal) {
        targetIdx = (keyVal < pool.tree.key[targetIdx]) ? pool.leftChild(targetIdx) : pool.rightChild(targetIdx);
    }
    if (targetIdx < 0) {
        steps.push_back(dsDiagramStep(0, S(keyVal) + " is not in the tree.", {}, dsDrawTree(pool.tree, true, "not found"), {}));
        return steps;
    }

    int startNode = -1;
    if (pool.leftChild(targetIdx) >= 0 && pool.rightChild(targetIdx) >= 0) {
        int successorIdx = pool.minNode(pool.rightChild(targetIdx));
        pool.tree.key[targetIdx] = pool.tree.key[successorIdx];
        steps.push_back(dsDiagramStep(0, "Two children: copy the inorder successor " + S(pool.tree.key[successorIdx]) + " into the node, then remove the successor.",
                                      {}, dsDrawTree(pool.tree, true, "successor copied"), {"t" + S(targetIdx)}));
        startNode = pool.cutNode(successorIdx);
        pool.tree.cls[successorIdx] = "";
    } else {
        startNode = pool.cutNode(targetIdx);
    }
    applyAvlLabels(pool);
    steps.push_back(dsDiagramStep(0, "Removed like in a BST. Now walk UP from the parent and repair balance.",
                                  {}, dsDrawTree(pool.tree, true, "after plain delete"),
                                  startNode >= 0 ? vector<string>{"t" + S(startNode)} : vector<string>{}));
    avlRebalanceUpward(pool, startNode, false, &steps, true);
    applyAvlLabels(pool);
    steps.push_back(dsDiagramStep(3, "Delete finished. Height " + S(getBstHeight(pool)) + ". Delete may rotate at several levels but still costs O(log n).",
                                  {{"height", S(getBstHeight(pool))}}, dsDrawTree(pool.tree, true, "AVL tree"), {}));
    return steps;
}

// ---------- Red-Black Tree Simulations ----------

static void appendRbSummary(BinaryTreePool& pool, Steps& steps, int lineNum, const string& extraText, const string& caption) {
    int count = rbCountNodes(pool, pool.root);
    int height = rbHeight(pool, pool.root);
    int blackH = rbBlackHeight(pool);
    rbSnapStep(pool, &steps, true, lineNum,
               "Height " + S(height) + " for " + S(count) + " keys, black height " + S(blackH) + ". " + extraText,
               {}, caption, {{"height", S(height)}, {"keys", S(count)}, {"black height", S(blackH)}});
}

static Steps simRbInsert(const Ints& input) {
    Ints keys = dsTakeInts(input, 0, 9, {10, 20, 30, 15, 25, 5, 1});
    BinaryTreePool pool;
    Steps steps;
    rbSnapStep(pool, &steps, true, 0,
               "Red-black rules: 1) every node is red or black, 2) root is black, 3) a red node has no red child, 4) every path from a node to its leaves has the same number of black nodes. Together they keep height <= 2 log2(n+1).",
               {}, "empty");
    for (size_t i = 0; i < keys.size(); ++i) {
        rbInsert(pool, keys[i], &steps, true);
    }
    appendRbSummary(pool, steps, 5, "Red-black trees rotate less often than AVL trees (at most 2 rotations per insert), so they are used in std::map, std::set and Java TreeMap.", "red-black tree");
    return steps;
}

static Steps simRbSearch(const Ints& input) {
    int keyVal = input.empty() ? 25 : input[0];
    Ints keys = dsTakeInts(input, 1, 9, {10, 20, 30, 15, 25, 5, 1});
    BinaryTreePool pool;
    Steps dummySteps;
    for (size_t i = 0; i < keys.size(); ++i) rbInsert(pool, keys[i], &dummySteps, false);

    Steps steps;
    int comparisons = 0;
    int count = rbCountNodes(pool, pool.root);
    int maxRbHeight = (int)(2 * log2((double)count + 1));
    rbSnapStep(pool, &steps, true, 0,
               "A red-black tree built from your keys (red nodes are red, black nodes are dark). Search for " + S(keyVal) +
               ". Searching works exactly like in a BST and ignores the colours; the colours only guarantee that the height is at most 2 log2(n+1) = " + S(maxRbHeight) + ", so the search is fast.",
               pool.root >= 0 ? vector<string>{"t" + S(pool.root)} : vector<string>{}, "start at the root", {{"key", S(keyVal)}});

    int u = pool.root;
    while (u >= 0) {
        ++comparisons;
        if (pool.tree.key[u] == keyVal) {
            rbSnapStep(pool, &steps, true, 2,
                       S(keyVal) + " == " + S(pool.tree.key[u]) + " (" + rbColorName(pool, u) + " node): FOUND after " + S(comparisons) + " comparison(s).",
                       {"t" + S(u)}, "found", {{"comparisons", S(comparisons)}});
            return steps;
        }
        bool goLeft = (keyVal < pool.tree.key[u]);
        rbSnapStep(pool, &steps, true, 3,
                   S(keyVal) + (goLeft ? " < " : " > ") + S(pool.tree.key[u]) + " (" + rbColorName(pool, u) + "): go " + (goLeft ? "left." : "right."),
                   {"t" + S(u)}, goLeft ? "go left" : "go right", {{"node", S(pool.tree.key[u])}, {"comparisons", S(comparisons)}});
        u = goLeft ? pool.leftChild(u) : pool.rightChild(u);
    }
    rbSnapStep(pool, &steps, true, 5,
               "Reached NULL: " + S(keyVal) + " is NOT in the tree. The search cost " + S(comparisons) + " comparison(s), at most the height " + S(rbHeight(pool, pool.root)) + ", which red-black rules keep O(log n).",
               {}, "not found", {{"comparisons", S(comparisons)}});
    return steps;
}

static Steps simRbDelete(const Ints& input) {
    int keyVal = input.empty() ? 5 : input[0];
    Ints keys = dsTakeInts(input, 1, 9, {5, 30, 15, 55, 25, 60, 20, 35});
    BinaryTreePool pool;
    Steps dummySteps;
    for (size_t i = 0; i < keys.size(); ++i) rbInsert(pool, keys[i], &dummySteps, false);

    Steps steps;
    rbSnapStep(pool, &steps, true, 0,
               "A red-black tree built from your keys. Delete " + S(keyVal) + ". A BST delete removes a node with at most one child; if that node was BLACK, one black node disappears from some paths, so rule 4 breaks and must be repaired with recoloring and at most 3 rotations.",
               {}, "before", {{"key", S(keyVal)}});
    bool removed = rbDelete(pool, keyVal, &steps, true);
    if (removed) {
        appendRbSummary(pool, steps, 11,
                        pool.root < 0 ? "The tree is empty again." : "Delete finished: all four red-black rules hold again. Cost O(log n), with at most 3 rotations.",
                        "red-black tree after delete");
    }
    return steps;
}

// ---------- B-Tree and B+ Tree Simulations ----------

static string drawMultiway(const vector<MultiwayNode>& nodes, int rootIdx, const string& caption) {
    return dsDrawTree(buildMultiwayDiagram(nodes, rootIdx), false, caption);
}

static Steps simBTreeInsert(const Ints& input) {
    int order = clamp(input.empty() ? 4 : input[0], 3, 5);
    Ints keys = dsTakeInts(input, 1, 12, {10, 20, 5, 6, 12, 30, 7, 17, 3});
    Steps steps;
    MultiwayTree tree;
    tree.maxKeys = order - 1;
    tree.recordSteps = true;
    tree.stepsOut = &steps;

    steps.push_back(dsDiagramStep(0, "B-tree of order " + S(order) + ": every node holds at most " + S(order - 1) + " keys. It stays perfectly balanced because it grows by SPLITTING upward, never by getting deeper at one leaf.",
                                  {{"order m", S(order)}, {"max keys", S(order - 1)}}, drawMultiway(tree.nodes, tree.root, "empty"), {}));
    for (size_t i = 0; i < keys.size(); ++i) {
        tree.insertKey(keys[i]);
    }
    steps.push_back(dsDiagramStep(4, "Done. All leaves are on the same level, and every node (except the root) is at least half full. Search, insert and delete cost O(log n) NODE reads: ideal for disks and databases, where one node = one disk block.",
                                  {}, drawMultiway(tree.nodes, tree.root, "B-tree of order " + S(order)), {}));
    return steps;
}

static Steps simBTreeSearch(const Ints& input) {
    int keyVal = input.empty() ? 17 : input[0];
    Ints keys = dsTakeInts(input, 1, 12, {10, 20, 5, 6, 12, 30, 7, 17, 3});
    Steps dummySteps;
    Steps steps;
    MultiwayTree tree;
    tree.maxKeys = 3;
    tree.stepsOut = &dummySteps;
    for (size_t i = 0; i < keys.size(); ++i) tree.insertKey(keys[i]);

    steps.push_back(dsDiagramStep(0, "A B-tree of order 4 built from your keys. Search for " + S(keyVal) + ".",
                                  {{"key", S(keyVal)}}, drawMultiway(tree.nodes, tree.root, "start at the root"), {"t" + S(tree.root)}));
    int cur = tree.root;
    int nodeReads = 0;
    while (cur >= 0 && nodeReads < 40) {
        ++nodeReads;
        const MultiwayNode& nd = tree.nodes[cur];
        if (MultiwayTree::containsKey(nd.keys, keyVal)) {
            steps.push_back(dsDiagramStep(2, "Node [" + formatMultiwayKeys(nd.keys) + "] contains " + S(keyVal) + ": FOUND after reading " + S(nodeReads) + " node(s).",
                                          {{"node reads", S(nodeReads)}}, drawMultiway(tree.nodes, tree.root, "found"), {"t" + S(cur)}));
            return steps;
        }
        if (nd.leaf) {
            steps.push_back(dsDiagramStep(3, "Leaf [" + formatMultiwayKeys(nd.keys) + "] does not contain " + S(keyVal) + ": NOT FOUND after " + S(nodeReads) + " node read(s).",
                                          {{"node reads", S(nodeReads)}}, drawMultiway(tree.nodes, tree.root, "not found"), {"t" + S(cur)}));
            return steps;
        }
        int childSlot = MultiwayTree::countKeysBefore(nd.keys, keyVal, false);
        steps.push_back(dsDiagramStep(4, "Node [" + formatMultiwayKeys(nd.keys) + "]: " + S(keyVal) + " lies between the surrounding keys, so follow child " + S(childSlot) + ".",
                                      {{"child", S(childSlot)}}, drawMultiway(tree.nodes, tree.root, "descend"),
                                      {"t" + S(cur), "e" + S(nd.ch[childSlot])}));
        cur = nd.ch[childSlot];
    }
    return steps;
}

static Steps simBPlusInsert(const Ints& input) {
    int order = clamp(input.empty() ? 4 : input[0], 3, 4);
    Ints keys = dsTakeInts(input, 1, 12, {10, 20, 5, 6, 12, 30, 7, 17, 3});
    Steps steps;
    MultiwayTree tree;
    tree.maxKeys = order - 1;
    tree.isBPlus = true;
    tree.recordSteps = true;
    tree.stepsOut = &steps;

    steps.push_back(dsDiagramStep(0, "B+ tree of order " + S(order) + ": up to " + S(order - 1) + " keys per node. Unlike a B-tree, EVERY key is stored in a leaf, and the leaves are chained by arrows.",
                                  {{"order m", S(order)}}, drawMultiway(tree.nodes, tree.root, "empty"), {}));
    for (size_t i = 0; i < keys.size(); ++i) {
        tree.insertKey(keys[i]);
    }
    steps.push_back(dsDiagramStep(4, "Done. Internal nodes are only a guide (copies); the leaf level holds all the data in sorted order, so a full scan or a range query just follows the leaf arrows.",
                                  {}, dsDrawTree(buildMultiwayDiagram(tree.nodes, tree.root), false, "B+ tree of order " + S(order), {}, false, tree.getLeafLinks()), {}));
    return steps;
}

static string drawBPlusWithLinks(const MultiwayTree& tree, const string& caption) {
    return dsDrawTree(buildMultiwayDiagram(tree.nodes, tree.root), false, caption, {}, false, tree.getLeafLinks());
}

static Steps simBPlusRangeQuery(const Ints& input) {
    int rangeLo = input.empty() ? 6 : input[0];
    int rangeHi = (input.size() > 1) ? input[1] : 17;
    if (rangeHi < rangeLo) swap(rangeLo, rangeHi);
    Ints keys = dsTakeInts(input, 2, 12, {10, 20, 5, 6, 12, 30, 7, 17, 3});

    Steps dummySteps;
    Steps steps;
    MultiwayTree tree;
    tree.maxKeys = 3;
    tree.isBPlus = true;
    tree.stepsOut = &dummySteps;
    for (size_t i = 0; i < keys.size(); ++i) tree.insertKey(keys[i]);

    steps.push_back(dsDiagramStep(0, "Range query: all keys from " + S(rangeLo) + " to " + S(rangeHi) + ". First descend like a normal search to the leaf where " + S(rangeLo) + " belongs.",
                                  {{"low", S(rangeLo)}, {"high", S(rangeHi)}},
                                  drawBPlusWithLinks(tree, "start at the root"), {"t" + S(tree.root)}));
    int cur = tree.root;
    int guard = 0;
    while (!tree.nodes[cur].leaf && guard++ < 30) {
        int childSlot = MultiwayTree::countKeysBefore(tree.nodes[cur].keys, rangeLo, true);
        steps.push_back(dsDiagramStep(0, "Node [" + formatMultiwayKeys(tree.nodes[cur].keys) + "]: go to child " + S(childSlot) + ".",
                                      {}, drawBPlusWithLinks(tree, "descend"), {"t" + S(cur)}));
        cur = tree.nodes[cur].ch[childSlot];
    }

    string output;
    int leafVisits = 0;
    while (cur >= 0 && leafVisits++ < 30) {
        bool reachedUpperLimit = false;
        string leafMatches;
        for (size_t i = 0; i < tree.nodes[cur].keys.size(); ++i) {
            int k = tree.nodes[cur].keys[i];
            if (k > rangeHi) {
                reachedUpperLimit = true;
                break;
            }
            if (k >= rangeLo) {
                if (!leafMatches.empty()) leafMatches += ", ";
                leafMatches += S(k);
                if (!output.empty()) output += ", ";
                output += S(k);
            }
        }
        steps.push_back(dsDiagramStep(2, "Leaf [" + formatMultiwayKeys(tree.nodes[cur].keys) + "]: report " +
                                      (leafMatches.empty() ? string("nothing") : leafMatches) +
                                      (reachedUpperLimit ? ". A key above " + S(rangeHi) + " was reached: STOP." : ". Follow the next arrow."),
                                      {{"result", output.empty() ? "(none)" : output}},
                                      drawBPlusWithLinks(tree, "walk the leaf chain"), {"t" + S(cur), "nx" + S(cur)}));
        if (reachedUpperLimit) break;
        cur = tree.nodes[cur].nextLeaf;
    }
    steps.push_back(dsDiagramStep(3, "Result: " + (output.empty() ? string("no keys in range") : output) +
                                  ". Only one descent plus a walk along the leaves: this is why databases use B+ trees for BETWEEN and ORDER BY.",
                                  {{"result", output}}, drawBPlusWithLinks(tree, "done"), {}));
    return steps;
}

void addDS5b(map<string, Sim>& m);
void addDS6(map<string, Sim>& m);

void addDS5(map<string, Sim>& m) {
    addDS5b(m);
    addDS6(m);

    dsRegisterIntSim(m, "Binary Search Tree (BST)", "Up to 9 keys inserted in this order", "50, 30, 70, 20, 40, 60, 80, 35",
                     {"insert(root, key):", "    if (root == NULL)  return new Node(key);", "    if (key < root->data)  root->left  = insert(root->left,  key);", "    else if (key > root->data)  root->right = insert(root->right, key);", "    return root;"},
                     dsBstRefs(), {"LeetCode #701 (Insert into a Binary Search Tree)", "LeetCode #98 (Validate Binary Search Tree)", "LeetCode #230 (Kth Smallest Element in a BST)", "LeetCode #235 (Lowest Common Ancestor of a BST)"},
                     simBstInsert);
    dsRegisterIntSim(m, "Binary Search Tree (BST)#1", "Key to find, then up to 9 keys", "60, 50, 30, 70, 20, 40, 60, 80, 35",
                     {"search(root, key):", "    while (root != NULL) {", "        if (key == root->data)  return root;", "        root = (key < root->data) ? root->left : root->right;", "    }", "    return NULL;"},
                     dsBstRefs(), {"LeetCode #700 (Search in a Binary Search Tree)", "LeetCode #270 (Closest Binary Search Tree Value)", "LeetCode #653 (Two Sum IV - Input is a BST)"},
                     simBstSearch);
    dsRegisterIntSim(m, "Binary Search Tree (BST)#2", "Key to delete, then up to 9 keys", "50, 50, 30, 70, 20, 40, 60, 80, 35",
                     {"delete(root, key):", "    find the node z holding key;", "    if z is a leaf:           remove it;", "    else if z has one child:  replace z by that child;", "    else {                    // two children", "        s = smallest node in z's RIGHT subtree;   z->data = s->data;", "        delete s (it has at most one child);", "    }"},
                     dsBstRefs(), {"LeetCode #450 (Delete Node in a BST)", "LeetCode #669 (Trim a Binary Search Tree)", "LeetCode #538 (Convert BST to Greater Tree)"},
                     simBstDelete);
    dsSetProgramNames("Binary Search Tree (BST)", {"Insert", "Search", "Delete (3 cases)"});

    dsRegisterIntSim(m, "Balanced BST", "Up to 8 keys (inserted in SORTED order)", "10, 20, 30, 40, 50, 60, 70",
                     {"for (key in sorted order)", "    insert(root, key);          // plain BST, no balancing", "height = n   ->   search is O(n)"},
                     dsBstRefs(), {"LeetCode #1382 (Balance a Binary Search Tree)", "LeetCode #110 (Balanced Binary Tree)", "LeetCode #108 (Convert Sorted Array to Binary Search Tree)"},
                     simBalancedBstSkewed);
    dsRegisterIntSim(m, "Balanced BST#1", "Up to 9 keys (sorted for you)", "10, 20, 30, 40, 50, 60, 70",
                     {"Node *build(a, lo, hi) {", "    if (lo > hi)  return NULL;", "    mid = (lo + hi) / 2;", "    node = new Node(a[mid]);", "    node->left  = build(a, lo, mid - 1);", "    node->right = build(a, mid + 1, hi);", "    return node;"},
                     dsBstRefs(), {"LeetCode #108 (Convert Sorted Array to Binary Search Tree)", "LeetCode #109 (Convert Sorted List to Binary Search Tree)", "LeetCode #1382 (Balance a Binary Search Tree)"},
                     simBalancedBstBuild);
    dsSetProgramNames("Balanced BST", {"Why balance? Sorted input gives a chain", "Build a balanced BST from a sorted array"});

    vector<string> refAvl = {
        "VisuAlgo: AVL tree|https://visualgo.net/en/bst",
        "GeeksforGeeks: AVL tree|https://www.geeksforgeeks.org/introduction-to-avl-tree/",
        "USFCA: AVL visualization|https://www.cs.usfca.edu/~galles/visualization/AVLtree.html"};
    dsRegisterIntSim(m, "AVL Tree", "Up to 9 keys inserted in this order", "10, 20, 30, 40, 50, 25",
                     {"insert key as in an ordinary BST;", "walk up from the new leaf:  update height, compute balance factor bf = h(left) - h(right);", "if  bf > 1  or  bf < -1  at node z:", "    LL: rotateRight(z)     RR: rotateLeft(z)", "    LR: rotateLeft(z->left), rotateRight(z)     RL: rotateRight(z->right), rotateLeft(z)"},
                     refAvl, {"LeetCode #110 (Balanced Binary Tree)", "LeetCode #1382 (Balance a Binary Search Tree)", "LeetCode #108 (Convert Sorted Array to Binary Search Tree)", "LeetCode #729 (My Calendar I: ordered set)"},
                     simAvlInsert);
    dsRegisterIntSim(m, "AVL Tree#1", "Key to delete, then up to 9 keys", "40, 20, 10, 30, 25, 35, 50, 45",
                     {"delete key as in an ordinary BST (leaf / one child / inorder successor);", "walk up from the node where something was removed:", "    update height, check the balance factor, rotate if needed;", "    keep going up: a delete may need a rotation at several levels"},
                     refAvl, {"LeetCode #450 (Delete Node in a BST)", "LeetCode #110 (Balanced Binary Tree)", "LeetCode #1382 (Balance a Binary Search Tree)"},
                     simAvlDelete);
    dsSetProgramNames("AVL Tree", {"Insert with rotations (LL, RR, LR, RL)", "Delete with rebalancing"});

    vector<string> refRb = {
        "VisuAlgo: red-black tree|https://visualgo.net/en/bst",
        "GeeksforGeeks: Red-black tree|https://www.geeksforgeeks.org/introduction-to-red-black-tree/",
        "USFCA: red-black visualization|https://www.cs.usfca.edu/~galles/visualization/RedBlack.html"};
    dsRegisterIntSim(m, "Red-Black (RB) Tree", "Up to 9 keys inserted in this order", "10, 20, 30, 15, 25, 5, 1",
                     {"insert key as a RED leaf (ordinary BST insert);", "while (parent is RED) {", "    if (uncle is RED)   { recolor parent + uncle BLACK, grandparent RED;  move up; }", "    else {  rotate (first the parent if it is a 'triangle', then the grandparent);  recolor;  stop; }", "}", "root = BLACK"},
                     refRb, {"LeetCode #729 (My Calendar I: std::map is a red-black tree)", "LeetCode #220 (Contains Duplicate III)", "LeetCode #1382 (Balance a Binary Search Tree)"},
                     simRbInsert);
    dsRegisterIntSim(m, "Red-Black (RB) Tree#1", "Key to search, then up to 9 keys", "25, 10, 20, 30, 15, 25, 5, 1",
                     {"search(root, key):", "    while (root != NULL) {", "        if (key == root->data)  return root;", "        root = (key < root->data) ? root->left : root->right;", "    }", "    return NULL;   // colours are never looked at"},
                     refRb, {"LeetCode #700 (Search in a Binary Search Tree)", "LeetCode #729 (My Calendar I: std::map is a red-black tree)", "LeetCode #220 (Contains Duplicate III)"},
                     simRbSearch);
    dsRegisterIntSim(m, "Red-Black (RB) Tree#2", "Key to delete, then up to 9 keys", "5, 5, 30, 15, 55, 25, 60, 20, 35",
                     {"find the node z holding key  (ordinary BST search);", "if z has two children:  copy the inorder successor's key into z, then remove the successor node instead;", "v = the node that is really removed (at most one child);   x = its child (may be NULL);", "if v is RED:  just remove it;                  // black counts do not change", "else if x is RED:  remove v and color x BLACK;", "else {  remove v;  x is 'double black' (its paths have one black too few):", "    while (x is not the root and x is black) {", "        sibling RED:                       recolor, rotate the parent toward x     // case 1", "        sibling BLACK, both children BLACK:  sibling = RED;  x = parent      // case 2", "        sibling BLACK, far child BLACK, near child RED:  swap colours, rotate sibling away   // case 3", "        sibling BLACK, far child RED:      recolor, rotate the parent toward x;  stop   // case 4", "    }   color x BLACK;   root = BLACK;  }"},
                     refRb, {"LeetCode #450 (Delete Node in a BST)", "LeetCode #729 (My Calendar I: std::map is a red-black tree)", "LeetCode #1382 (Balance a Binary Search Tree)"},
                     simRbDelete);
    dsSetProgramNames("Red-Black (RB) Tree", {"Insert (recolor and rotate)", "Search", "Delete (double-black repair)"});
}

void addDS5b(map<string, Sim>& m) {
    vector<string> refBTree = {
        "VisuAlgo / USFCA: B-tree|https://www.cs.usfca.edu/~galles/visualization/BTree.html",
        "GeeksforGeeks: B-tree insertion|https://www.geeksforgeeks.org/insert-operation-in-b-tree/",
        "Wikipedia: B-tree|https://en.wikipedia.org/wiki/B-tree"};
    vector<string> lcBTree = {
        "DBMS idea: database indexes use B / B+ trees (fewer disk reads)",
        "LeetCode #729 (My Calendar I: ordered map)",
        "LeetCode #220 (Contains Duplicate III)"};
    dsRegisterIntSim(m, "B-Tree", "Order m (3 to 5), then up to 12 keys", "4, 10, 20, 5, 6, 12, 30, 7, 17, 3",
                     {"a node holds at most m - 1 keys (and m children), all leaves at the same depth;", "insert(k):  descend to the correct leaf;  add k in sorted order;", "while (node has more than m - 1 keys) {", "    split: the middle key moves UP to the parent, the rest form two nodes;", "}   // if the root splits, a new root is created: the tree grows UPWARDS"},
                     refBTree, lcBTree, simBTreeInsert);
    dsRegisterIntSim(m, "B-Tree#1", "Key to search, then up to 12 keys (order 4)", "17, 10, 20, 5, 6, 12, 30, 7, 17, 3",
                     {"search(node, k):", "    scan the node's sorted keys;", "    if k is found:  return it;", "    if the node is a leaf:  return not found;", "    go to the child between the two keys that surround k;"},
                     refBTree, lcBTree, simBTreeSearch);
    dsSetProgramNames("B-Tree", {"Insert with node splitting", "Search"});

    vector<string> refBPlus = {
        "USFCA: B+ tree visualization|https://www.cs.usfca.edu/~galles/visualization/BPlusTree.html",
        "GeeksforGeeks: B+ tree|https://www.geeksforgeeks.org/introduction-of-b-tree/",
        "Wikipedia: B+ tree|https://en.wikipedia.org/wiki/B%2B_tree"};
    vector<string> lcBPlus = {
        "DBMS idea: range queries (BETWEEN) walk the linked leaves",
        "LeetCode #729 (My Calendar I)",
        "LeetCode #352 (Data Stream as Disjoint Intervals)"};
    dsRegisterIntSim(m, "B+ Tree", "Order m (3 or 4), then up to 12 keys", "4, 10, 20, 5, 6, 12, 30, 7, 17, 3",
                     {"internal nodes store only SEPARATOR keys; ALL real keys live in the leaves;", "leaves are linked left to right (next pointer);", "insert(k): add to the right leaf;  while (a node overflows) {", "    leaf split:     copy the first key of the right half UP (it stays in the leaf);", "    internal split: the middle key moves up;  }"},
                     refBPlus, lcBPlus, simBPlusInsert);
    dsRegisterIntSim(m, "B+ Tree#1", "Range low, range high, then up to 12 keys (order 4)", "6, 17, 10, 20, 5, 6, 12, 30, 7, 17, 3",
                     {"find the leaf where  low  would be;       // normal descent, O(log n)", "walk the leaf chain with next pointers:", "    print every key <= high;", "stop at the first key > high  (no need to go back up the tree)"},
                     refBPlus, lcBPlus, simBPlusRangeQuery);
    dsSetProgramNames("B+ Tree", {"Insert (leaf split copies the key up)", "Range query along the linked leaves"});
}
