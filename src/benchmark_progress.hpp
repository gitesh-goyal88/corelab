// BenchmarkRunner: measures empirical runtime of data structures vs Big-O complexity
// ProgressStore: flat-file storage ("CORELAB_PROGRESS_V1") persisting learner progress
#pragma once
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include "content.hpp"

using namespace std;

// ---------------------------------------------------------------------------
// 1. BenchmarkRunner (Microbenchmarks for Vector, Linked List, and BST)
// ---------------------------------------------------------------------------
struct BenchmarkResult {
    string operation;
    string theoreticalComplexity;
    int dataSize;
    double measuredMicroseconds;
    string note;
};

struct BenchmarkListNode {
    int val;
    BenchmarkListNode* nextPtr;
    BenchmarkListNode(int v, BenchmarkListNode* nxt) {
        val = v;
        nextPtr = nxt;
    }
};

struct BenchmarkBSTNode {
    int key;
    BenchmarkBSTNode* leftChild;
    BenchmarkBSTNode* rightChild;
    BenchmarkBSTNode(int k) {
        key = k;
        leftChild = nullptr;
        rightChild = nullptr;
    }
};

class BenchmarkRunner {
private:
    static double getElapsedMicroseconds(
        chrono::high_resolution_clock::time_point startTime,
        chrono::high_resolution_clock::time_point endTime
    ) {
        return chrono::duration<double, micro>(endTime - startTime).count();
    }

    static BenchmarkBSTNode* insertBst(BenchmarkBSTNode* node, int value) {
        if (node == nullptr) {
            return new BenchmarkBSTNode(value);
        }
        if (value < node->key) {
            node->leftChild = insertBst(node->leftChild, value);
        } else {
            node->rightChild = insertBst(node->rightChild, value);
        }
        return node;
    }

    static void freeBst(BenchmarkBSTNode* node) {
        if (node == nullptr) return;
        freeBst(node->leftChild);
        freeBst(node->rightChild);
        delete node;
    }

public:
    static string runBenchmark(const string& structure, int elementCount = 10000) {
        int N = max(1000, min(100000, elementCount));
        vector<BenchmarkResult> results;

        vector<int> randData(N);
        mt19937 rng(42);
        for (int i = 0; i < N; ++i) {
            randData[i] = (int)(rng() % (unsigned int)(N * 5));
        }

        if (structure == "vector" || structure == "array" || structure.empty()) {
            vector<int> arrVec;
            chrono::high_resolution_clock::time_point t0 = chrono::high_resolution_clock::now();
            for (int i = 0; i < N; ++i) {
                arrVec.push_back(randData[i]);
            }
            chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
            results.push_back({"vector::push_back", "O(1) amortised", N, getElapsedMicroseconds(t0, t1), "Fast contiguous memory allocation"});

            t0 = chrono::high_resolution_clock::now();
            volatile long long sum = 0;
            for (int i = 0; i < N; ++i) {
                sum += arrVec[i];
            }
            t1 = chrono::high_resolution_clock::now();
            (void)sum;
            results.push_back({"vector random access (v[i])", "O(1)", N, getElapsedMicroseconds(t0, t1), "CPU cache friendly, sequential indexing"});

            t0 = chrono::high_resolution_clock::now();
            sort(arrVec.begin(), arrVec.end());
            t1 = chrono::high_resolution_clock::now();
            results.push_back({"std::sort (introsort)", "O(N log N)", N, getElapsedMicroseconds(t0, t1), "Hybrid quicksort + heapsort + insertion sort"});

            t0 = chrono::high_resolution_clock::now();
            bool found = binary_search(arrVec.begin(), arrVec.end(), randData[N / 2]);
            t1 = chrono::high_resolution_clock::now();
            results.push_back({"std::binary_search", "O(log N)", N, getElapsedMicroseconds(t0, t1), found ? "Key found" : "Key not found"});
        }

        if (structure == "linked_list" || structure.empty()) {
            BenchmarkListNode* head = nullptr;
            chrono::high_resolution_clock::time_point t0 = chrono::high_resolution_clock::now();
            for (int i = 0; i < N; ++i) {
                head = new BenchmarkListNode(randData[i], head);
            }
            chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
            results.push_back({"linked_list insert at front", "O(1)", N, getElapsedMicroseconds(t0, t1), "Pointer swap without array shifting"});

            t0 = chrono::high_resolution_clock::now();
            BenchmarkListNode* current = head;
            int target = randData[N / 2];
            bool found = false;
            while (current != nullptr) {
                if (current->val == target) {
                    found = true;
                    break;
                }
                current = current->nextPtr;
            }
            t1 = chrono::high_resolution_clock::now();
            results.push_back({"linked_list linear search", "O(N)", N, getElapsedMicroseconds(t0, t1), found ? "Match found" : "Searched whole list"});

            while (head != nullptr) {
                BenchmarkListNode* temp = head;
                head = head->nextPtr;
                delete temp;
            }
        }

        if (structure == "bst" || structure.empty()) {
            BenchmarkBSTNode* root = nullptr;
            int subN = min(N, 20000); // prevent deep recursion on skewed trees
            chrono::high_resolution_clock::time_point t0 = chrono::high_resolution_clock::now();
            for (int i = 0; i < subN; ++i) {
                root = insertBst(root, randData[i]);
            }
            chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
            results.push_back({"BST insert", "O(log N) avg, O(N) worst", subN, getElapsedMicroseconds(t0, t1), "Average case logarithmic branching"});

            t0 = chrono::high_resolution_clock::now();
            int target = randData[subN / 2];
            BenchmarkBSTNode* current = root;
            bool found = false;
            while (current != nullptr) {
                if (current->key == target) {
                    found = true;
                    break;
                }
                if (target < current->key) {
                    current = current->leftChild;
                } else {
                    current = current->rightChild;
                }
            }
            t1 = chrono::high_resolution_clock::now();
            results.push_back({"BST search", "O(log N) avg", subN, getElapsedMicroseconds(t0, t1), found ? "Found in tree" : "Not found"});

            freeBst(root);
        }

        string json = "{\"benchmark\":\"runner\",\"structure\":" + q(structure) +
                      ",\"elements\":" + to_string(N) + ",\"results\":[";
        for (size_t i = 0; i < results.size(); ++i) {
            if (i > 0) json += ",";
            json += "{\"operation\":" + q(results[i].operation) +
                    ",\"complexity\":" + q(results[i].theoreticalComplexity) +
                    ",\"size\":" + to_string(results[i].dataSize) +
                    ",\"measured_us\":" + to_string(results[i].measuredMicroseconds) +
                    ",\"note\":" + q(results[i].note) + "}";
        }
        json += "]}";
        return json;
    }
};

// ---------------------------------------------------------------------------
// 2. ProgressStore (Flat-File Handling Using ifstream / ofstream)
// ---------------------------------------------------------------------------
struct ProgressRecord {
    string topic;
    long long timestamp;
    string status; // "completed", "in_progress", "started"
    int score;
};

class ProgressStore {
private:
    string filePath;

public:
    ProgressStore(const string& path = "progress.dat") {
        filePath = path;
    }

    vector<ProgressRecord> load() const {
        vector<ProgressRecord> records;
        ifstream inFile(filePath.c_str());
        if (!inFile.is_open()) {
            return records;
        }

        string header;
        if (!getline(inFile, header) || header != "CORELAB_PROGRESS_V1") {
            return records;
        }

        string line;
        while (getline(inFile, line)) {
            if (line.empty()) {
                continue;
            }
            stringstream ss(line);
            string topicName, tsStr, statusStr, scoreStr;
            if (getline(ss, topicName, '|') &&
                getline(ss, tsStr, '|') &&
                getline(ss, statusStr, '|') &&
                getline(ss, scoreStr, '|')) {
                ProgressRecord rec;
                rec.topic = topicName;
                rec.timestamp = atoll(tsStr.c_str());
                rec.status = statusStr;
                rec.score = atoi(scoreStr.c_str());
                records.push_back(rec);
            }
        }
        return records;
    }

    bool save(const string& topic, const string& status, int score) {
        vector<ProgressRecord> existing = load();
        bool updated = false;
        long long nowTs = chrono::duration_cast<chrono::seconds>(
            chrono::system_clock::now().time_since_epoch()).count();

        for (size_t i = 0; i < existing.size(); ++i) {
            if (existing[i].topic == topic) {
                existing[i].status = status;
                existing[i].score = score;
                existing[i].timestamp = nowTs;
                updated = true;
                break;
            }
        }
        if (!updated) {
            ProgressRecord newRec;
            newRec.topic = topic;
            newRec.timestamp = nowTs;
            newRec.status = status;
            newRec.score = score;
            existing.push_back(newRec);
        }

        ofstream outFile(filePath.c_str(), ios::trunc);
        if (!outFile.is_open()) {
            return false;
        }
        outFile << "CORELAB_PROGRESS_V1\n";
        for (size_t i = 0; i < existing.size(); ++i) {
            outFile << existing[i].topic << "|"
                    << existing[i].timestamp << "|"
                    << existing[i].status << "|"
                    << existing[i].score << "\n";
        }
        return true;
    }

    string toJson() const {
        vector<ProgressRecord> records = load();
        string json = "{\"format\":\"flat-file:CORELAB_PROGRESS_V1\",\"total_records\":" +
                      to_string(records.size()) + ",\"records\":[";
        for (size_t i = 0; i < records.size(); ++i) {
            if (i > 0) json += ",";
            json += "{\"topic\":" + q(records[i].topic) +
                    ",\"timestamp\":" + to_string(records[i].timestamp) +
                    ",\"status\":" + q(records[i].status) +
                    ",\"score\":" + to_string(records[i].score) + "}";
        }
        json += "]}";
        return json;
    }

    static ProgressStore& getInstance() {
        static ProgressStore store("progress.dat");
        return store;
    }
};
