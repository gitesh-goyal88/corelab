// Standalone verification test for the Red-Black Tree engine in src/ds5.hpp.
// Exercises 3,000 random inserts and deletes against reference set<int> across all 4 RB invariants.
#include <climits>
#include <cmath>
#include <iostream>
#include <random>
#include <set>
#include "../src/ds8.hpp"

using namespace std;

static int rbCheckSubtree(const BinaryTreePool& pool, int nodeIdx, long long minKey, long long maxKey,
                          bool& valid, string* reason) {
    if (nodeIdx < 0) return 1;
    if (!valid) return 0;

    long long nodeKey = pool.tree.key[nodeIdx];
    if (nodeKey <= minKey || nodeKey >= maxKey) {
        if (valid && reason) *reason = "BST order broken at " + S(pool.tree.key[nodeIdx]);
        valid = false;
        return 0;
    }
    for (int side = 0; side < 2; ++side) {
        int childIdx = pool.tree.ch[nodeIdx][side];
        if (childIdx >= 0 && pool.par[childIdx] != nodeIdx) {
            if (valid && reason) *reason = "wrong parent link at " + S(pool.tree.key[childIdx]);
            valid = false;
        }
    }
    if (pool.isRed[nodeIdx] && (rbIsRed(pool, pool.leftChild(nodeIdx)) || rbIsRed(pool, pool.rightChild(nodeIdx)))) {
        if (valid && reason) *reason = "red node " + S(pool.tree.key[nodeIdx]) + " has a red child";
        valid = false;
    }
    int leftBH = rbCheckSubtree(pool, pool.leftChild(nodeIdx), minKey, nodeKey, valid, reason);
    int rightBH = rbCheckSubtree(pool, pool.rightChild(nodeIdx), nodeKey, maxKey, valid, reason);
    if (leftBH != rightBH) {
        if (valid && reason) *reason = "black heights differ below " + S(pool.tree.key[nodeIdx]);
        valid = false;
    }
    return leftBH + (pool.isRed[nodeIdx] ? 0 : 1);
}

static bool rbCheck(const BinaryTreePool& pool, string* reason = nullptr) {
    if (pool.root < 0) return true;
    bool valid = true;
    if (pool.isRed[pool.root]) {
        if (reason) *reason = "the root is red";
        valid = false;
    }
    if (pool.par[pool.root] != -1) {
        if (valid && reason) *reason = "the root has a parent";
        valid = false;
    }
    rbCheckSubtree(pool, pool.root, LLONG_MIN, LLONG_MAX, valid, reason);
    return valid;
}

static void collectInorderKeys(const BinaryTreePool& pool, int nodeIdx, vector<int>& keysOut) {
    if (nodeIdx < 0) return;
    collectInorderKeys(pool, pool.leftChild(nodeIdx), keysOut);
    keysOut.push_back(pool.tree.key[nodeIdx]);
    collectInorderKeys(pool, pool.rightChild(nodeIdx), keysOut);
}

int main() {
    mt19937 rng(42);
    int insertCount = 0;
    int deleteCount = 0;

    for (int trial = 0; trial < 3000; ++trial) {
        BinaryTreePool pool;
        set<int> expectedSet;
        bool record = (trial < 10);
        int opCount = 30 + (int)(rng() % 50);

        for (int op = 0; op < opCount; ++op) {
            int keyVal = (int)(rng() % 40);
            Steps steps;
            if (rng() % 100 < 55) {
                int insertedIdx = rbInsert(pool, keyVal, &steps, record);
                bool wasNew = expectedSet.insert(keyVal).second;
                if ((insertedIdx >= 0) != wasNew) {
                    cout << "insert result mismatch\n";
                    return 1;
                }
                ++insertCount;
            } else {
                bool deleted = rbDelete(pool, keyVal, &steps, record);
                bool existed = expectedSet.erase(keyVal) > 0;
                if (deleted != existed) {
                    cout << "delete result mismatch key " << keyVal << "\n";
                    return 1;
                }
                ++deleteCount;
            }

            string reason;
            if (!rbCheck(pool, &reason)) {
                cout << "RB invariant broken trial " << trial << " step " << op << ": " << reason << "\n";
                return 1;
            }
            if (rbCountNodes(pool, pool.root) != (int)expectedSet.size()) {
                cout << "size mismatch\n";
                return 1;
            }
            if (!expectedSet.empty() && rbHeight(pool, pool.root) > 2 * log2((double)expectedSet.size() + 1) + 1e-9) {
                cout << "height bound exceeded\n";
                return 1;
            }

            vector<int> actualKeys;
            collectInorderKeys(pool, pool.root, actualKeys);
            vector<int> expectedKeys(expectedSet.begin(), expectedSet.end());
            if (actualKeys != expectedKeys) {
                cout << "inorder keys mismatch\n";
                return 1;
            }
        }
    }

    cout << "rb_stress: 3000 rounds passed (" << insertCount << " inserts, " << deleteCount << " deletes)\n";
    return 0;
}
