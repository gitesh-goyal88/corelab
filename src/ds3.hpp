// Data Structures (Semester 3), part 3: hashing.
#pragma once
#include <cmath>
#include <cstdio>
#include "ds2.hpp"

using namespace std;

// Hash table with chaining: buckets down the left, each chain to the right.
static string dsDrawHashChains(int bucketCount, const vector<Ints>& chains, const string& caption) {
    Sk canvas;
    size_t maxLen = 1;
    for (size_t i = 0; i < chains.size(); ++i) {
        maxLen = max(maxLen, chains[i].size());
    }
    for (int i = 0; i < bucketCount; ++i) {
        double y = 12.0 + i * 36.0;
        string bucketId = "b" + S(i);
        canvas.Rect(bucketId.c_str(), 16, y, 36, 28, "steel");
        canvas.T(34, y + 14, S(i));
        if (i < (int)chains.size() && !chains[i].empty()) {
            for (size_t j = 0; j < chains[i].size(); ++j) {
                double x = 76.0 + j * 58.0;
                string nodeId = "c" + S(i) + "_" + S((int)j);
                string arrowId = "a" + S(i) + "_" + S((int)j);
                canvas.Arr(j == 0 ? 52 : x - 14, y + 14, x, y + 14, arrowId.c_str());
                canvas.Rect(nodeId.c_str(), x, y, 44, 28, "weld");
                canvas.T(x + 22, y + 14, S(chains[i][j]));
            }
        } else {
            canvas.T(68, y + 14, "NULL", "start");
        }
    }
    double width = max(300.0, 76.0 + (double)maxLen * 58.0 + 20.0);
    canvas.T(width / 2, 12 + bucketCount * 36 + 10, caption);
    return canvas.out((int)width, 12 + bucketCount * 36 + 26, {});
}

static const vector<string>& dsHashRefs() {
    static const vector<string> refs = {
        "VisuAlgo: hash table|https://visualgo.net/en/hashtable",
        "GeeksforGeeks: Hashing|https://www.geeksforgeeks.org/hashing-data-structure/",
        "cppreference: std::unordered_map|https://en.cppreference.com/w/cpp/container/unordered_map"};
    return refs;
}

static Ints dsTakeHashKeys(const Ints& input, size_t startIndex, size_t maxCount, Ints defaultKeys) {
    Ints result = dsTakeInts(input, startIndex, maxCount, defaultKeys);
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = (int)(abs((long long)result[i]) % 10000);
    }
    return result;
}

// ---------- Hashing: hash functions ----------

static int computeHashValue(int methodKind, int tableSize, int key, string& explanation) {
    if (methodKind == 0) {
        explanation = S(key) + " mod " + S(tableSize) + " = " + S(key % tableSize);
        return key % tableSize;
    }
    if (methodKind == 1) {
        double frac = key * 0.6180339887;
        frac -= floor(frac);
        int slot = clamp((int)(tableSize * frac), 0, tableSize - 1);
        char buf[64];
        snprintf(buf, sizeof(buf), "%.4f", frac);
        explanation = "frac(" + S(key) + " x 0.618) = " + buf + ", floor(" + S(tableSize) + " x " + buf + ") = " + S(slot);
        return slot;
    }
    int sum = 0;
    int temp = key;
    string parts;
    while (temp > 0) {
        sum += temp % 100;
        parts = S(temp % 100) + (parts.empty() ? "" : " + ") + parts;
        temp /= 100;
    }
    if (parts.empty()) parts = "0";
    explanation = "fold " + S(key) + " into pairs: " + parts + " = " + S(sum) + ", then " + S(sum) + " mod " + S(tableSize) + " = " + S(sum % tableSize);
    return sum % tableSize;
}

static Steps runHashMethod(int methodKind, const Ints& input) {
    int tableSize = clamp(input.empty() ? 7 : input[0], 2, 11);
    Ints keys = dsTakeHashKeys(input, 1, 8, {50, 700, 76, 85, 92, 73, 101});
    Ints table(tableSize, INT_MIN);
    Steps steps;
    int collisions = 0;

    steps.push_back(dsArrayStep(0, "A hash function turns a key into a slot number between 0 and " + S(tableSize - 1) + ". The table has " + S(tableSize) + " slots.",
                                {{"m", S(tableSize)}}, table, -1, {}, -1, -1, "table"));
    for (size_t i = 0; i < keys.size(); ++i) {
        int key = keys[i];
        string how;
        int slot = computeHashValue(methodKind, tableSize, key, how);
        bool isSlotEmpty = (table[slot] == INT_MIN);
        if (isSlotEmpty) table[slot] = key;
        else ++collisions;

        int codeLine = (methodKind == 2) ? 2 : (methodKind == 1 ? 1 : 0);
        string statusMsg = isSlotEmpty
            ? ". Slot " + S(slot) + " was empty: store " + S(key) + "."
            : ". COLLISION: slot " + S(slot) + " already holds " + S(table[slot]) + ". (Chaining and probing, the next topics, solve this.)";
        steps.push_back(dsArrayStep(codeLine, "h(" + S(key) + "): " + how + statusMsg,
                                    {{"key", S(key)}, {"h(key)", S(slot)}, {"collisions", S(collisions)}},
                                    table, slot, dsMakeTags(tableSize, {{slot, "h"}}), -1, -1, "table"));
    }
    int usedSlots = 0;
    for (size_t i = 0; i < table.size(); ++i) {
        if (table[i] != INT_MIN) ++usedSlots;
    }
    steps.push_back(dsArrayStep(0, S((int)keys.size()) + " keys, " + S(usedSlots) + " slots used, " + S(collisions) + " collision(s). A good hash function spreads keys evenly and is fast to compute.",
                                {{"load factor", to_string((double)keys.size() / tableSize).substr(0, 4)}}, table, -1, {}, -1, -1, "table"));
    return steps;
}

static Steps simHashingDivision(const Ints& input) {
    return runHashMethod(0, input);
}

static Steps simHashingMultiplication(const Ints& input) {
    return runHashMethod(1, input);
}

static Steps simHashingFolding(const Ints& input) {
    return runHashMethod(2, input);
}

// ---------- Hash Table: linear probing & rehashing ----------

static Steps simHashTableProbing(const Ints& input) {
    Ints commands = dsTakeInts(input, 0, 12, {15, 22, 8, -22, 29, -36, 36, 43});
    const int M = 7;
    Ints table(M, INT_MIN);
    Steps steps;

    steps.push_back(dsArrayStep(0, "A hash table stores a key at slot k mod " + S(M) + ". If that slot is taken we try the next one (linear probing). Positive number = insert, negative = search.",
                                {{"m", S(M)}}, table, -1, {}, -1, -1, "table"));
    for (size_t cIdx = 0; cIdx < commands.size(); ++cIdx) {
        int cmd = commands[cIdx];
        if (cmd == 0) continue;
        int key = (int)(abs((long long)cmd) % 10000);
        int startSlot = key % M;
        bool isInsert = (cmd > 0);
        steps.push_back(dsArrayStep(isInsert ? 0 : 3,
                                    string(isInsert ? "insert(" : "search(") + S(key) + "): h = " + S(key) + " mod " + S(M) + " = " + S(startSlot) + ". Start probing at slot " + S(startSlot) + ".",
                                    {{"key", S(key)}, {"h", S(startSlot)}}, table, startSlot, dsMakeTags(M, {{startSlot, "h"}}), -1, -1, "table"));
        bool finished = false;
        for (int probe = 0; probe < M && !finished; ++probe) {
            int pos = (startSlot + probe) % M;
            bool isFull = (table[pos] != INT_MIN);
            if (isInsert) {
                if (!isFull) {
                    table[pos] = key;
                    steps.push_back(dsArrayStep(2, "Slot " + S(pos) + " is empty: store " + S(key) + (probe ? " (after " + S(probe) + " collision(s))." : "."),
                                                {{"probes", S(probe + 1)}}, table, pos, dsMakeTags(M, {{startSlot, "h"}, {pos, "new"}}), -1, -1, "table"));
                    finished = true;
                } else if (table[pos] == key) {
                    steps.push_back(dsArrayStep(1, "Slot " + S(pos) + " already holds " + S(key) + ": nothing to insert.",
                                                {{"probes", S(probe + 1)}}, table, pos, dsMakeTags(M, {{pos, "same"}}), -1, -1, "table"));
                    finished = true;
                } else {
                    steps.push_back(dsArrayStep(1, "Slot " + S(pos) + " holds " + S(table[pos]) + ": collision. Try slot " + S((pos + 1) % M) + ".",
                                                {{"probes", S(probe + 1)}}, table, pos, dsMakeTags(M, {{startSlot, "h"}, {pos, "full"}}), -1, -1, "table"));
                }
            } else {
                if (!isFull) {
                    steps.push_back(dsArrayStep(4, "Slot " + S(pos) + " is empty: the probe sequence ends, so " + S(key) + " is NOT in the table.",
                                                {{"probes", S(probe + 1)}}, table, pos, dsMakeTags(M, {{pos, "empty"}}), -1, -1, "table"));
                    finished = true;
                } else if (table[pos] == key) {
                    steps.push_back(dsArrayStep(5, "Slot " + S(pos) + " holds " + S(key) + ": FOUND after " + S(probe + 1) + " probe(s).",
                                                {{"probes", S(probe + 1)}}, table, pos, dsMakeTags(M, {{pos, "found"}}), -1, -1, "table"));
                    finished = true;
                } else {
                    steps.push_back(dsArrayStep(4, "Slot " + S(pos) + " holds " + S(table[pos]) + ", not " + S(key) + ". Keep probing.",
                                                {{"probes", S(probe + 1)}}, table, pos, dsMakeTags(M, {{startSlot, "h"}, {pos, "full"}}), -1, -1, "table"));
                }
            }
        }
        if (!finished) {
            steps.push_back(dsArrayStep(isInsert ? 1 : 4,
                                        string(isInsert ? "The table is FULL: " : "All slots checked: ") + S(key) +
                                        (isInsert ? " cannot be inserted. Real tables resize before this happens." : " is not in the table."),
                                        {{"probes", S(M)}}, table, -1, {}, -1, -1, "table"));
        }
    }
    return steps;
}

static int placeKeyLinearProbing(Ints& table, int key) {
    int tableSize = (int)table.size();
    int startSlot = key % tableSize;
    for (int i = 0; i < tableSize; ++i) {
        int pos = (startSlot + i) % tableSize;
        if (table[pos] == INT_MIN || table[pos] == key) {
            table[pos] = key;
            return pos;
        }
    }
    return -1;
}

static Steps simHashTableRehash(const Ints& input) {
    Ints keys = dsTakeHashKeys(input, 0, 8, {12, 7, 25, 31, 9, 40, 18, 63});
    int tableSize = 5;
    Ints table(tableSize, INT_MIN);
    Steps steps;
    int itemCount = 0;

    steps.push_back(dsArrayStep(0, "Small table: 5 slots. As it fills up, probing gets slow, so the table grows once the load factor n/m passes 0.7.",
                                {{"m", S(tableSize)}, {"n", "0"}}, table, -1, {}, -1, -1, "table"));
    for (size_t i = 0; i < keys.size(); ++i) {
        int key = keys[i];
        int placedSlot = placeKeyLinearProbing(table, key);
        if (placedSlot < 0) break;
        ++itemCount;
        double loadFactor = (double)itemCount / tableSize;
        steps.push_back(dsArrayStep(0, "Insert " + S(key) + " at slot " + S(placedSlot) + ". Load factor = " + S(itemCount) + " / " + S(tableSize) + " = " + to_string(loadFactor).substr(0, 4) + ".",
                                    {{"n", S(itemCount)}, {"m", S(tableSize)}}, table, placedSlot,
                                    dsMakeTags(tableSize, {{placedSlot, "new"}}), -1, -1, "table"));
        if (loadFactor > 0.7) {
            Ints oldTable = table;
            int newSize = 2 * tableSize + 1;
            Ints newTable(newSize, INT_MIN);
            for (size_t k = 0; k < oldTable.size(); ++k) {
                if (oldTable[k] != INT_MIN) {
                    placeKeyLinearProbing(newTable, oldTable[k]);
                }
            }
            table = newTable;
            tableSize = newSize;
            steps.push_back(dsArrayStep(2, "Load factor above 0.7: grow to m = " + S(tableSize) + " and REHASH every key (positions change because k mod m changes). One resize costs O(n), but it is rare, so insert stays O(1) on average.",
                                        {{"m", S(tableSize)}, {"n", S(itemCount)}}, table, -1, {}, -1, -1, "table"));
        }
    }
    return steps;
}

// ---------- Chaining ----------

static Steps simChainingInsert(const Ints& input) {
    Ints keys = dsTakeHashKeys(input, 0, 8, {15, 11, 27, 8, 20, 35, 40, 6});
    const int M = 5;
    vector<Ints> chains(M);
    Steps steps;

    steps.push_back(dsDiagramStep(0, "Chaining: each slot is the head of a linked list, so many keys can share a slot. m = " + S(M) + " buckets.",
                                  {{"m", S(M)}}, dsDrawHashChains(M, chains, "separate chaining"), {}));
    int insertedCount = 0;
    for (size_t idx = 0; idx < keys.size(); ++idx) {
        int key = keys[idx];
        int bucket = key % M;
        string collisionMsg = chains[bucket].empty()
            ? ". Bucket " + S(bucket) + " is empty."
            : ". Bucket " + S(bucket) + " already has " + S((int)chains[bucket].size()) + " key(s): a collision, handled by the chain.";
        steps.push_back(dsDiagramStep(1, "h(" + S(key) + ") = " + S(key) + " mod " + S(M) + " = " + S(bucket) + collisionMsg,
                                      {{"key", S(key)}, {"i", S(bucket)}}, dsDrawHashChains(M, chains, "bucket " + S(bucket)),
                                      {"b" + S(bucket)}));
        chains[bucket].insert(chains[bucket].begin(), key);
        ++insertedCount;
        steps.push_back(dsDiagramStep(3, "New node " + S(key) + " becomes the first node of chain " + S(bucket) + ". Chain length is now " + S((int)chains[bucket].size()) + ". Load factor n/m = " + to_string((double)insertedCount / M).substr(0, 3) + ".",
                                      {{"n", S(insertedCount)}}, dsDrawHashChains(M, chains, "inserted at the front"),
                                      {"c" + S(bucket) + "_0", "a" + S(bucket) + "_0", "b" + S(bucket)}));
    }
    size_t longestChain = 0;
    for (size_t i = 0; i < chains.size(); ++i) {
        longestChain = max(longestChain, chains[i].size());
    }
    steps.push_back(dsDiagramStep(0, "Done. Longest chain = " + S((int)longestChain) + ". Average search cost is about 1 + n/m, so keep n/m small. Worst case O(n) if every key lands in one bucket.",
                                  {{"longest chain", S((int)longestChain)}},
                                  dsDrawHashChains(M, chains, "chains grow only where collisions happen"), {}));
    return steps;
}

static Steps simChainingSearch(const Ints& input) {
    int key = (int)(abs((long long)(input.empty() ? 35 : input[0])) % 10000);
    Ints keys = dsTakeHashKeys(input, 1, 8, {15, 11, 27, 8, 20, 35, 40, 6});
    const int M = 5;
    vector<Ints> chains(M);
    for (size_t i = 0; i < keys.size(); ++i) {
        chains[keys[i] % M].insert(chains[keys[i] % M].begin(), keys[i]);
    }
    Steps steps;
    int bucket = key % M;
    steps.push_back(dsDiagramStep(1, "The table is already built. Search for " + S(key) + ": h = " + S(key) + " mod " + S(M) + " = " + S(bucket) + ". Only chain " + S(bucket) + " needs to be looked at, not the whole table.",
                                  {{"key", S(key)}, {"i", S(bucket)}},
                                  dsDrawHashChains(M, chains, "search chain " + S(bucket)), {"b" + S(bucket)}));
    for (size_t j = 0; j < chains[bucket].size(); ++j) {
        bool matched = (chains[bucket][j] == key);
        steps.push_back(dsDiagramStep(3, "Node " + S((int)j) + " holds " + S(chains[bucket][j]) + (matched ? " = key: FOUND." : " != " + S(key) + ", go to the next node."),
                                      {{"node", S(chains[bucket][j])}},
                                      dsDrawHashChains(M, chains, matched ? "found" : "keep walking"),
                                      {"c" + S(bucket) + "_" + S((int)j)}));
        if (matched) return steps;
    }
    steps.push_back(dsDiagramStep(4, "End of chain " + S(bucket) + ": " + S(key) + " is NOT in the table. Cost = length of one chain.",
                                  {{"result", "not found"}}, dsDrawHashChains(M, chains, "not found"), {"b" + S(bucket)}));
    return steps;
}

// ---------- Probing Strategies ----------

static Steps runOpenAddressingProbe(int probeKind, const Ints& input) {
    const int M = 7;
    Ints keys = dsTakeHashKeys(input, 0, 6, {50, 700, 76, 85, 92, 73});
    Ints table(M, INT_MIN);
    Steps steps;

    const char* intro = (probeKind == 0)
        ? "Linear probing: try h, h+1, h+2 ..."
        : (probeKind == 1 ? "Quadratic probing: try h, h+1, h+4, h+9 ..."
                          : "Double hashing: step size comes from a second hash h2(k) = 1 + k mod 6");
    steps.push_back(dsArrayStep(0, string(intro) + " (all mod " + S(M) + "). Table size 7.",
                                {{"m", S(M)}}, table, -1, {}, -1, -1, "table"));
    for (size_t idx = 0; idx < keys.size(); ++idx) {
        int key = keys[idx];
        int h1 = key % M;
        int h2 = 1 + key % (M - 1);
        bool placed = false;
        for (int stepNum = 0; stepNum < M; ++stepNum) {
            int offset = (probeKind == 0) ? stepNum : (probeKind == 1 ? stepNum * stepNum : stepNum * h2);
            int pos = (int)(((long long)h1 + offset) % M);
            string formula = (probeKind == 0)
                ? "(" + S(h1) + " + " + S(stepNum) + ")"
                : (probeKind == 1 ? "(" + S(h1) + " + " + S(stepNum) + "^2)"
                                  : "(" + S(h1) + " + " + S(stepNum) + " x " + S(h2) + ")");
            if (table[pos] == INT_MIN) {
                table[pos] = key;
                placed = true;
                steps.push_back(dsArrayStep(2, "key " + S(key) + ", probe " + S(stepNum) + ": " + formula + " mod " + S(M) + " = " + S(pos) + ". Empty: store " + S(key) + ".",
                                            {{"key", S(key)}, {"i", S(stepNum)}, {"slot", S(pos)}}, table, pos,
                                            dsMakeTags(M, {{pos, "new"}}), -1, -1, "table"));
                break;
            }
            steps.push_back(dsArrayStep(1, "key " + S(key) + ", probe " + S(stepNum) + ": " + formula + " mod " + S(M) + " = " + S(pos) + ". Slot holds " + S(table[pos]) + ": collision, try the next probe.",
                                        {{"key", S(key)}, {"i", S(stepNum)}, {"slot", S(pos)}}, table, pos,
                                        dsMakeTags(M, {{pos, "full"}}), -1, -1, "table"));
        }
        if (!placed) {
            steps.push_back(dsArrayStep(1, "key " + S(key) + ": all " + S(M) + " probes hit full slots. " +
                                        string(probeKind == 1 ? "Quadratic probing only reaches about half the slots, so it can fail even when the table is not full." : "No free slot found.") + " Key not stored.",
                                        {{"key", S(key)}}, table, -1, {}, -1, -1, "table"));
        }
    }
    const char* summary = (probeKind == 0)
        ? "Linear probing is simple and cache-friendly but forms long runs of full slots (primary clustering)."
        : (probeKind == 1 ? "Quadratic probing spreads keys out and avoids primary clustering, but keys with the same h still follow the same path (secondary clustering)."
                          : "Double hashing gives each key its own step size, so clustering is the lowest. Choose m prime so every slot can be reached.");
    steps.push_back(dsArrayStep(0, string(summary), {}, table, -1, {}, -1, -1, "table"));
    return steps;
}

static Steps simLinearProbing(const Ints& input) {
    return runOpenAddressingProbe(0, input);
}

static Steps simQuadraticProbing(const Ints& input) {
    return runOpenAddressingProbe(1, input);
}

static Steps simDoubleHashing(const Ints& input) {
    return runOpenAddressingProbe(2, input);
}

void addDS4(map<string, Sim>& m);

void addDS3(map<string, Sim>& m) {
    addDS4(m);
    vector<string> lcHash = {
        "LeetCode #705 (Design HashSet)",
        "LeetCode #706 (Design HashMap)",
        "LeetCode #1 (Two Sum)",
        "LeetCode #217 (Contains Duplicate)"};

    dsRegisterIntSim(m, "Hashing", "Table size m first, then up to 8 keys", "7, 50, 700, 76, 85, 92, 73, 101",
                     {"int h(int key) {", "    return key % m;", "}"},
                     dsHashRefs(), lcHash, simHashingDivision);
    dsRegisterIntSim(m, "Hashing#1", "Table size m first, then up to 8 keys", "7, 50, 700, 76, 85, 92, 73, 101",
                     {"double A = 0.6180339887;", "int h(int key) {", "    return (int)(m * fmod(key * A, 1.0));", "}"},
                     dsHashRefs(), lcHash, simHashingMultiplication);
    dsRegisterIntSim(m, "Hashing#2", "Table size m first, then up to 8 keys", "7, 1234, 5678, 4321, 98, 7654, 2020",
                     {"int h(int key) {", "    int sum = 0;", "    while (key > 0) { sum += key % 100; key /= 100; }", "    return sum % m;", "}"},
                     dsHashRefs(), lcHash, simHashingFolding);
    dsSetProgramNames("Hashing", {"Division method: k mod m", "Multiplication method", "Folding method"});

    dsRegisterIntSim(m, "Hash Table", "Commands: positive = insert, negative = search (m = 7)", "15, 22, 8, -22, 29, -36, 36, 43",
                     {"insert(k):  i = k % m;", "    while (t[i] is full)  i = (i + 1) % m;", "    t[i] = k;", "search(k):  i = k % m;", "    while (t[i] is full && t[i] != k)  i = (i + 1) % m;", "    return t[i] == k;"},
                     dsHashRefs(), lcHash, simHashTableProbing);
    dsRegisterIntSim(m, "Hash Table#1", "Up to 8 keys (table starts with 5 slots)", "12, 7, 25, 31, 9, 40, 18, 63",
                     {"insert(k):  place k with linear probing;  n++;", "if (n / m > 0.7) {            // load factor limit", "    m = 2 * m + 1;", "    reinsert every key with the new m;", "}"},
                     dsHashRefs(), lcHash, simHashTableRehash);
    dsSetProgramNames("Hash Table", {"Insert and search (linear probing)", "Load factor and rehashing"});

    dsRegisterIntSim(m, "Chaining", "Up to 8 keys (5 buckets)", "15, 11, 27, 8, 20, 35, 40, 6",
                     {"insert(k):", "    i = k % m;", "    new node holding k;  node->next = bucket[i];", "    bucket[i] = node;        // add at the front, O(1)"},
                     dsHashRefs(), lcHash, simChainingInsert);
    dsRegisterIntSim(m, "Chaining#1", "Key to search, then up to 8 keys (5 buckets)", "35, 15, 11, 27, 8, 20, 35, 40, 6",
                     {"search(k):", "    i = k % m;", "    for (node = bucket[i]; node != NULL; node = node->next)", "        if (node->data == k)  return true;", "    return false;"},
                     dsHashRefs(), lcHash, simChainingSearch);
    dsSetProgramNames("Chaining", {"Insert keys (collisions form chains)", "Search a key (walk one chain)"});

    dsRegisterIntSim(m, "Probing", "Up to 6 keys (m = 7)", "50, 700, 76, 85, 92, 73",
                     {"for (i = 0; i < m; i++) {", "    pos = (h(k) + i) % m;", "    if (t[pos] is empty)  { t[pos] = k;  break; }", "}"},
                     dsHashRefs(), lcHash, simLinearProbing);
    dsRegisterIntSim(m, "Probing#1", "Up to 6 keys (m = 7)", "50, 700, 76, 85, 92, 73",
                     {"for (i = 0; i < m; i++) {", "    pos = (h(k) + i * i) % m;", "    if (t[pos] is empty)  { t[pos] = k;  break; }", "}"},
                     dsHashRefs(), lcHash, simQuadraticProbing);
    dsRegisterIntSim(m, "Probing#2", "Up to 6 keys (m = 7)", "50, 700, 76, 85, 92, 73",
                     {"h2 = 1 + k % (m - 1);", "for (i = 0; i < m; i++) {", "    pos = (h(k) + i * h2) % m;", "    if (t[pos] is empty)  { t[pos] = k;  break; }", "}"},
                     dsHashRefs(), lcHash, simDoubleHashing);
    dsSetProgramNames("Probing", {"Linear probing", "Quadratic probing", "Double hashing"});
}
