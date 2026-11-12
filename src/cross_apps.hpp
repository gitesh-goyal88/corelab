// Cross-Cutting Applications (2nd-Year OOP + Data Structures):
// 1. CampusNavigator      (Weighted Graph + Dijkstra with Min-Heap)
// 2. CoursePrereqChecker  (DAG + 3-Color DFS Topological Order / Cycle Check)
// 3. AutocompleteTrie     (Trie + Frequency-Ranked Prefix Search)
// 4. TaskScheduler        (Priority Queue / Max-Priority Scheduling)
#pragma once
#include <algorithm>
#include <cctype>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "content.hpp"

using namespace std;

// ---------------------------------------------------------------------------
// 1. Campus Navigator (Weighted Undirected Graph + Dijkstra)
// ---------------------------------------------------------------------------
struct CampusNode {
    string id;
    string label;
    string category;
    int x;
    int y;
    string icon;
    string desc;
};

struct CampusEdge {
    string u;
    string v;
    int weight;
};

struct NeighborEdge {
    string destination;
    int weight;
};

struct RelaxLog {
    string targetNode;
    int edgeWeight;
    int oldDistance;
    int newDistance;
    bool wasUpdated;
};

struct DijkstraStepLog {
    int stepNumber;
    string currentNode;
    int currentDistance;
    vector<string> visitedNodes;
    vector<pair<string, int>> heapSnapshot;
    vector<RelaxLog> relaxations;
    string explanation;
};

class CampusNavigator {
public:
    static const vector<CampusNode>& getCampusNodes() {
        static const vector<CampusNode> nodes = {
            {"Main Gate", "Main Gate", "Entrance", 70, 260, "\xF0\x9F\x9A\xAA", "Campus security, entry plaza & shuttle stop"},
            {"Admin Block", "Admin Block", "Administrative", 220, 140, "\xF0\x9F\x8F\x9B\xEF\xB8\x8F", "Admissions, Dean's office & registrar"},
            {"Library", "Library", "Academic", 220, 390, "\xF0\x9F\x93\x96", "4-floor research library & silent study zones"},
            {"Academic Block A", "Academic Block A", "Academic", 420, 110, "\xF0\x9F\x8F\xAB", "Lecture halls, Physics & Applied Math depts"},
            {"Cafeteria", "Cafeteria", "Amenities", 380, 260, "\xE2\x98\x95", "Student food court, caf\xC3\xA9 & social courtyard"},
            {"Academic Block B", "Academic Block B", "Academic", 440, 400, "\xF0\x9F\x94\xAC", "Computer labs, software centers & maker space"},
            {"Sports Complex", "Sports Complex", "Amenities", 610, 90, "\xE2\x9A\xBD", "Indoor arena, gym, basketball & running track"},
            {"Computer Center", "Computer Center", "Academic", 590, 270, "\xF0\x9F\x92\xBB", "HPC server cluster & AI research computing lab"},
            {"Hostel 2", "Hostel 2", "Living", 750, 170, "\xF0\x9F\x8F\xA0", "Senior undergraduate residence block"},
            {"Hostel 1", "Hostel 1", "Living", 750, 380, "\xF0\x9F\x9B\x8F\xEF\xB8\x8F", "Junior residential halls & student lounge"}
        };
        return nodes;
    }

    static const vector<CampusEdge>& getCampusEdges() {
        static const vector<CampusEdge> edges = {
            {"Main Gate", "Admin Block", 120},
            {"Main Gate", "Library", 200},
            {"Admin Block", "Academic Block A", 150},
            {"Admin Block", "Cafeteria", 180},
            {"Library", "Academic Block B", 140},
            {"Library", "Hostel 1", 300},
            {"Academic Block A", "Academic Block B", 100},
            {"Academic Block A", "Sports Complex", 250},
            {"Academic Block B", "Computer Center", 80},
            {"Computer Center", "Cafeteria", 90},
            {"Computer Center", "Hostel 1", 160},
            {"Cafeteria", "Sports Complex", 130},
            {"Sports Complex", "Hostel 2", 210},
            {"Hostel 1", "Hostel 2", 110}
        };
        return edges;
    }

    static string findShortestPath(const string& fromPlace, const string& toPlace) {
        const vector<CampusNode>& nodes = getCampusNodes();
        const vector<CampusEdge>& edges = getCampusEdges();

        // Build undirected adjacency list
        map<string, vector<NeighborEdge>> adjList;
        for (size_t i = 0; i < nodes.size(); ++i) {
            adjList[nodes[i].id] = vector<NeighborEdge>();
        }
        for (size_t i = 0; i < edges.size(); ++i) {
            NeighborEdge e1;
            e1.destination = edges[i].v;
            e1.weight = edges[i].weight;
            adjList[edges[i].u].push_back(e1);

            NeighborEdge e2;
            e2.destination = edges[i].u;
            e2.weight = edges[i].weight;
            adjList[edges[i].v].push_back(e2);
        }

        string startPlace = fromPlace.empty() ? "Main Gate" : fromPlace;
        string destPlace = toPlace.empty() ? "Computer Center" : toPlace;

        if (adjList.find(startPlace) == adjList.end() || adjList.find(destPlace) == adjList.end()) {
            return "{\"error\":\"Unknown location. Known campus buildings available.\"}";
        }

        const int INF_DIST = 1000000;
        map<string, int> shortestDist;
        map<string, string> parentNode;
        map<string, bool> visitedMap;

        for (size_t i = 0; i < nodes.size(); ++i) {
            shortestDist[nodes[i].id] = INF_DIST;
            visitedMap[nodes[i].id] = false;
        }
        shortestDist[startPlace] = 0;

        typedef pair<int, string> HeapItem;
        priority_queue<HeapItem, vector<HeapItem>, greater<HeapItem>> minHeap;
        minHeap.push(make_pair(0, startPlace));

        vector<DijkstraStepLog> stepLogs;
        int stepCounter = 1;

        while (!minHeap.empty()) {
            HeapItem topItem = minHeap.top();
            minHeap.pop();
            int currentDist = topItem.first;
            string currentNode = topItem.second;

            if (visitedMap[currentNode]) {
                continue;
            }
            visitedMap[currentNode] = true;

            DijkstraStepLog logEntry;
            logEntry.stepNumber = stepCounter++;
            logEntry.currentNode = currentNode;
            logEntry.currentDistance = currentDist;

            for (map<string, bool>::iterator it = visitedMap.begin(); it != visitedMap.end(); ++it) {
                if (it->second) {
                    logEntry.visitedNodes.push_back(it->first);
                }
            }

            string explain = "Examining <b>" + currentNode + "</b> (tentative shortest dist: " + to_string(currentDist) + "m). ";

            if (currentNode == destPlace) {
                explain += "Target destination reached! Shortest path finalized.";
                logEntry.explanation = explain;
                stepLogs.push_back(logEntry);
                break;
            }

            int relaxedCount = 0;
            const vector<NeighborEdge>& neighbors = adjList[currentNode];
            for (size_t i = 0; i < neighbors.size(); ++i) {
                const NeighborEdge& edge = neighbors[i];
                if (visitedMap[edge.destination]) {
                    continue;
                }
                int oldDist = shortestDist[edge.destination];
                int newDist = currentDist + edge.weight;

                RelaxLog rlog;
                rlog.targetNode = edge.destination;
                rlog.edgeWeight = edge.weight;
                rlog.oldDistance = oldDist;
                rlog.newDistance = newDist;

                if (newDist < oldDist) {
                    shortestDist[edge.destination] = newDist;
                    parentNode[edge.destination] = currentNode;
                    minHeap.push(make_pair(newDist, edge.destination));
                    rlog.wasUpdated = true;
                    relaxedCount++;
                } else {
                    rlog.wasUpdated = false;
                }
                logEntry.relaxations.push_back(rlog);
            }

            if (relaxedCount > 0) {
                explain += "Relaxed " + to_string(relaxedCount) + " neighbor distance(s).";
            } else {
                explain += "No neighbor distances updated.";
            }
            logEntry.explanation = explain;

            // Snapshot heap state for step display
            priority_queue<HeapItem, vector<HeapItem>, greater<HeapItem>> heapCopy = minHeap;
            while (!heapCopy.empty()) {
                logEntry.heapSnapshot.push_back(make_pair(heapCopy.top().second, heapCopy.top().first));
                heapCopy.pop();
            }

            stepLogs.push_back(logEntry);
        }

        // Reconstruct shortest path from destination back to start
        vector<string> path;
        string walkNode = destPlace;
        while (walkNode != startPlace && parentNode.find(walkNode) != parentNode.end()) {
            path.push_back(walkNode);
            walkNode = parentNode[walkNode];
        }
        path.push_back(startPlace);
        reverse(path.begin(), path.end());

        int totalDist = shortestDist[destPlace];
        double walkTimeMin = (double)totalDist / 75.0; // ~75 meters/min normal walk
        int stepsEst = (int)(totalDist * 1.31);         // ~1.31 steps per meter
        int calEst = (int)(totalDist * 0.045);          // ~4.5 kcal per 100 meters

        ostringstream ss;
        ss << "{\"app\":\"campus_navigator\","
           << "\"from\":" << q(startPlace) << ","
           << "\"to\":" << q(destPlace) << ","
           << "\"total_distance_meters\":" << totalDist << ","
           << "\"est_walk_time_min\":" << (int)(walkTimeMin * 10) / 10.0 << ","
           << "\"est_steps\":" << stepsEst << ","
           << "\"est_calories\":" << calEst << ",";

        ss << "\"path\":[";
        for (size_t i = 0; i < path.size(); ++i) {
            if (i > 0) ss << ",";
            ss << q(path[i]);
        }
        ss << "],";

        ss << "\"path_edges\":[";
        for (size_t i = 0; i + 1 < path.size(); ++i) {
            if (i > 0) ss << ",";
            int edgeWeight = 0;
            for (size_t eIdx = 0; eIdx < edges.size(); ++eIdx) {
                if ((edges[eIdx].u == path[i] && edges[eIdx].v == path[i + 1]) ||
                    (edges[eIdx].v == path[i] && edges[eIdx].u == path[i + 1])) {
                    edgeWeight = edges[eIdx].weight;
                    break;
                }
            }
            ss << "{\"u\":" << q(path[i]) << ",\"v\":" << q(path[i + 1]) << ",\"weight\":" << edgeWeight << "}";
        }
        ss << "],";

        ss << "\"nodes\":[";
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (i > 0) ss << ",";
            const CampusNode& nd = nodes[i];
            ss << "{\"id\":" << q(nd.id) << ",\"label\":" << q(nd.label)
               << ",\"category\":" << q(nd.category)
               << ",\"x\":" << nd.x << ",\"y\":" << nd.y
               << ",\"icon\":" << q(nd.icon) << ",\"desc\":" << q(nd.desc) << "}";
        }
        ss << "],";

        ss << "\"edges\":[";
        for (size_t i = 0; i < edges.size(); ++i) {
            if (i > 0) ss << ",";
            const CampusEdge& eg = edges[i];
            ss << "{\"u\":" << q(eg.u) << ",\"v\":" << q(eg.v) << ",\"weight\":" << eg.weight << "}";
        }
        ss << "],";

        ss << "\"steps\":[";
        for (size_t i = 0; i < stepLogs.size(); ++i) {
            if (i > 0) ss << ",";
            const DijkstraStepLog& st = stepLogs[i];
            ss << "{\"step\":" << st.stepNumber << ",\"curr\":" << q(st.currentNode) << ",\"d\":" << st.currentDistance
               << ",\"explanation\":" << q(st.explanation) << ",\"visited\":[";
            for (size_t j = 0; j < st.visitedNodes.size(); ++j) {
                if (j > 0) ss << ",";
                ss << q(st.visitedNodes[j]);
            }
            ss << "],\"pq\":[";
            for (size_t j = 0; j < st.heapSnapshot.size(); ++j) {
                if (j > 0) ss << ",";
                ss << "{\"node\":" << q(st.heapSnapshot[j].first) << ",\"d\":" << st.heapSnapshot[j].second << "}";
            }
            ss << "],\"relaxations\":[";
            for (size_t j = 0; j < st.relaxations.size(); ++j) {
                if (j > 0) ss << ",";
                const RelaxLog& rl = st.relaxations[j];
                ss << "{\"to\":" << q(rl.targetNode) << ",\"weight\":" << rl.edgeWeight
                   << ",\"old_d\":" << (rl.oldDistance >= INF_DIST ? -1 : rl.oldDistance)
                   << ",\"new_d\":" << rl.newDistance
                   << ",\"updated\":" << (rl.wasUpdated ? "true" : "false") << "}";
            }
            ss << "]}";
        }
        ss << "],";

        ss << "\"places\":[";
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (i > 0) ss << ",";
            ss << q(nodes[i].id);
        }
        ss << "]}";

        return ss.str();
    }
};

// ---------------------------------------------------------------------------
// 2. Course Prerequisite Checker (DAG Topological Sort & Cycle Detection)
// ---------------------------------------------------------------------------
class CoursePrereqChecker {
private:
    static void dfsTopological(
        const string& courseCode,
        const map<string, vector<string>>& prereqGraph,
        map<string, int>& visitState,
        vector<string>& order,
        bool& hasCycle
    ) {
        visitState[courseCode] = 1; // 1 = currently visiting
        map<string, vector<string>>::const_iterator it = prereqGraph.find(courseCode);
        if (it != prereqGraph.end()) {
            const vector<string>& deps = it->second;
            for (size_t i = 0; i < deps.size(); ++i) {
                string pre = deps[i];
                if (visitState[pre] == 1) {
                    hasCycle = true;
                } else if (visitState[pre] == 0) {
                    dfsTopological(pre, prereqGraph, visitState, order, hasCycle);
                }
            }
        }
        visitState[courseCode] = 2; // 2 = finished
        order.push_back(courseCode);
    }

public:
    static string checkPrerequisites(const string& targetCourse) {
        static const map<string, vector<string>> prereqs = {
            {"CS101", vector<string>()},
            {"CS102", {"CS101"}},
            {"CS201", {"CS102"}},
            {"CS202", {"CS102"}},
            {"CS301", {"CS201"}},
            {"CS302", {"CS201", "CS202"}},
            {"CS401", {"CS301", "CS302"}},
            {"AI101", {"CS201"}},
            {"AI201", {"AI101", "CS302"}}
        };

        static const map<string, string> titles = {
            {"CS101", "Programming Foundations (C)"},
            {"CS102", "Software Development 2 (OOP & C++)"},
            {"CS201", "Data Structures"},
            {"CS202", "Computer Organization & Architecture"},
            {"CS301", "Algorithms & Complexity"},
            {"CS302", "Database Management Systems"},
            {"CS401", "Distributed Cloud Systems"},
            {"AI101", "Mathematical Foundations of AI"},
            {"AI201", "Deep Learning & Neural Networks"}
        };

        string target = targetCourse.empty() ? "CS401" : targetCourse;
        if (prereqs.find(target) == prereqs.end()) {
            return "{\"error\":\"Unknown course. Available: CS101, CS102, CS201, CS202, CS301, CS302, CS401, AI101, AI201\"}";
        }

        vector<string> order;
        map<string, int> visitState;
        bool hasCycle = false;

        dfsTopological(target, prereqs, visitState, order, hasCycle);

        string json = "{\"app\":\"course_prereq_checker\",\"target\":" + q(target) +
                      ",\"title\":" + q(titles.at(target)) +
                      ",\"has_cycle\":" + (hasCycle ? "true" : "false") + ",\"required_sequence\":[";
        for (size_t i = 0; i < order.size(); ++i) {
            if (i > 0) json += ",";
            string cname = titles.count(order[i]) ? titles.at(order[i]) : "";
            json += "{\"code\":" + q(order[i]) + ",\"name\":" + q(cname) + "}";
        }
        json += "],\"all_courses\":[";
        bool firstCourse = true;
        for (map<string, vector<string>>::const_iterator it = prereqs.begin(); it != prereqs.end(); ++it) {
            if (!firstCourse) json += ",";
            firstCourse = false;
            json += "{\"code\":" + q(it->first) + ",\"name\":" + q(titles.at(it->first)) + "}";
        }
        json += "]}";
        return json;
    }
};

// ---------------------------------------------------------------------------
// 3. Autocomplete (Trie + Frequency-Ranked Prefix Search)
// ---------------------------------------------------------------------------
class TrieNode {
public:
    map<char, TrieNode*> children;
    bool isWord;
    int frequency;

    TrieNode() {
        isWord = false;
        frequency = 0;
    }
};

class AutocompleteTrie {
private:
    TrieNode* root;

    static void dfsCollectWords(TrieNode* node, const string& currentWord, vector<pair<string, int>>& results) {
        if (node->isWord) {
            results.push_back(make_pair(currentWord, node->frequency));
        }
        for (map<char, TrieNode*>::iterator it = node->children.begin(); it != node->children.end(); ++it) {
            dfsCollectWords(it->second, currentWord + it->first, results);
        }
    }

    static bool compareByFrequency(const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second;
    }

    static void freeTrieNode(TrieNode* node) {
        if (node == nullptr) return;
        for (map<char, TrieNode*>::iterator it = node->children.begin(); it != node->children.end(); ++it) {
            freeTrieNode(it->second);
        }
        delete node;
    }

public:
    AutocompleteTrie() {
        root = new TrieNode();
    }

    ~AutocompleteTrie() {
        freeTrieNode(root);
    }

    void insert(const string& word, int freq) {
        TrieNode* current = root;
        for (size_t i = 0; i < word.size(); ++i) {
            char low = (char)tolower((unsigned char)word[i]);
            if (current->children.count(low) == 0) {
                current->children[low] = new TrieNode();
            }
            current = current->children[low];
        }
        current->isWord = true;
        current->frequency = freq;
    }

    vector<pair<string, int>> searchPrefix(const string& prefix, int maxLimit = 6) {
        TrieNode* current = root;
        string normalizedPrefix;
        for (size_t i = 0; i < prefix.size(); ++i) {
            char low = (char)tolower((unsigned char)prefix[i]);
            normalizedPrefix += low;
            if (current->children.count(low) == 0) {
                return vector<pair<string, int>>();
            }
            current = current->children[low];
        }

        vector<pair<string, int>> results;
        dfsCollectWords(current, normalizedPrefix, results);
        sort(results.begin(), results.end(), compareByFrequency);
        if ((int)results.size() > maxLimit) {
            results.resize(maxLimit);
        }
        return results;
    }

    static AutocompleteTrie& getSharedTrie() {
        static AutocompleteTrie trie;
        static bool isInitialized = false;
        if (!isInitialized) {
            isInitialized = true;
            vector<pair<string, int>> dictionary = {
                {"algorithm", 150}, {"array", 240}, {"abstraction", 120}, {"avl tree", 95}, {"adjacency list", 80},
                {"binary search", 300}, {"binary tree", 210}, {"breadth first search", 190}, {"backtracking", 140},
                {"classes and objects", 280}, {"constructor", 220}, {"composition", 160}, {"circular queue", 130},
                {"depth first search", 185}, {"dynamic programming", 290}, {"destructor", 170}, {"dijkstra", 145},
                {"encapsulation", 210}, {"exceptions", 195}, {"eigenvalues", 110},
                {"function overloading", 205}, {"friend function", 135}, {"fibonacci", 165},
                {"graph traversal", 175}, {"gradient descent", 225}, {"generics", 120},
                {"hash table", 260}, {"heap sort", 160}, {"huffman coding", 90},
                {"inheritance", 270}, {"interface", 190}, {"iteration protocols", 115},
                {"linked list", 310}, {"linear regression", 180},
                {"merge sort", 230}, {"multiple inheritance", 140}, {"method overriding", 195},
                {"n-queens problem", 150}, {"normal distribution", 130},
                {"operator overloading", 185}, {"object oriented", 320},
                {"polymorphism", 275}, {"priority queue", 200}, {"pure virtual", 165},
                {"quick sort", 240}, {"queue", 280},
                {"recursion", 300}, {"red black tree", 140}, {"rat in a maze", 135},
                {"stack", 315}, {"standard template library", 290}, {"segment tree", 110},
                {"templates", 230}, {"tower of hanoi", 160}, {"tries", 180},
                {"virtual functions", 250}, {"vector", 270}
            };
            for (size_t i = 0; i < dictionary.size(); ++i) {
                trie.insert(dictionary[i].first, dictionary[i].second);
            }
        }
        return trie;
    }

    static string queryJson(const string& query) {
        string qstr = query.empty() ? "al" : query;
        vector<pair<string, int>> matches = getSharedTrie().searchPrefix(qstr);

        string json = "{\"app\":\"autocomplete\",\"query\":" + q(qstr) + ",\"suggestions\":[";
        for (size_t i = 0; i < matches.size(); ++i) {
            if (i > 0) json += ",";
            json += "{\"word\":" + q(matches[i].first) + ",\"frequency\":" + to_string(matches[i].second) + "}";
        }
        json += "]}";
        return json;
    }
};

// ---------------------------------------------------------------------------
// 4. Task Scheduler (Max-Priority Queue Simulation)
// ---------------------------------------------------------------------------
struct TaskItem {
    int id;
    string title;
    int priority;    // 1 (low) to 10 (critical)
    int burstTimeMs;
};

class TaskScheduler {
private:
    static bool compareTaskPriority(const TaskItem& a, const TaskItem& b) {
        if (a.priority != b.priority) {
            return a.priority > b.priority;
        }
        return a.burstTimeMs < b.burstTimeMs;
    }

public:
    static string runAction(const string& action, int prio = 5, const string& name = "") {
        static vector<TaskItem> taskList = {
            {1, "Render UI Frames (60fps)", 9, 16},
            {2, "Sync Analytics in Background", 2, 120},
            {3, "Process Critical DB Transaction", 10, 45},
            {4, "Garbage Collection Sweep", 4, 85},
            {5, "Download Course Video Stream", 6, 250},
            {6, "Handle WebSocket Ping", 8, 5}
        };
        static int nextTaskId = 7;

        if (action == "add" && !name.empty()) {
            int clampedPrio = max(1, min(10, prio));
            TaskItem item;
            item.id = nextTaskId++;
            item.title = name;
            item.priority = clampedPrio;
            item.burstTimeMs = 50;
            taskList.push_back(item);
        }

        vector<TaskItem> scheduled = taskList;
        sort(scheduled.begin(), scheduled.end(), compareTaskPriority);

        TaskItem nextTask;
        if (scheduled.empty()) {
            nextTask.id = 0;
            nextTask.title = "None";
            nextTask.priority = 0;
            nextTask.burstTimeMs = 0;
        } else {
            nextTask = scheduled.front();
        }

        if (action == "execute" && !taskList.empty()) {
            for (vector<TaskItem>::iterator it = taskList.begin(); it != taskList.end(); ++it) {
                if (it->id == nextTask.id) {
                    taskList.erase(it);
                    break;
                }
            }
        }

        string json = "{\"app\":\"task_scheduler\",\"action\":" + q(action) +
                      ",\"next_to_execute\":{\"id\":" + to_string(nextTask.id) +
                      ",\"title\":" + q(nextTask.title) +
                      ",\"priority\":" + to_string(nextTask.priority) +
                      ",\"burst_ms\":" + to_string(nextTask.burstTimeMs) + "},\"queue\":[";
        for (size_t i = 0; i < scheduled.size(); ++i) {
            if (i > 0) json += ",";
            json += "{\"id\":" + to_string(scheduled[i].id) +
                    ",\"title\":" + q(scheduled[i].title) +
                    ",\"priority\":" + to_string(scheduled[i].priority) +
                    ",\"burst_ms\":" + to_string(scheduled[i].burstTimeMs) + "}";
        }
        json += "]}";
        return json;
    }
};
