// Depth Tiers: Flagship, Standard, Light
#pragma once
#include <string>
#include <vector>

using namespace std;

enum class DepthTier {
    Light,
    Standard,
    Flagship
};

static const char* tierToString(DepthTier tier) {
    switch (tier) {
        case DepthTier::Flagship: return "Flagship";
        case DepthTier::Standard: return "Standard";
        case DepthTier::Light:    return "Light";
    }
    return "Standard";
}

static DepthTier getTopicTier(const string& topicName) {
    // Flagship topics: core fundamental architectures & multi-step algorithms
    static const vector<string> flagshipTopics = {
        "Procedural Programming vs Object-Oriented Programming",
        "Classes and Objects",
        "Constructors",
        "Destructors",
        "Method Overriding",
        "Virtual Functions",
        "Dynamic Dispatch",
        "Internal Representation of Method Tables",
        "Class Diagrams",
        "Composition",
        "Exceptions",
        "Function Templates",
        "Class Templates",
        "Standard Template Library (STL)",
        "Dynamic Memory Allocation (C++)",
        "Need for Linked Lists",
        "Types of Linked Lists",
        "Stack (SDF2)",
        "Queue (SDF2)",
        "Recursion (SDF2)",
        "Tower of Hanoi",
        "N-Queens Problem",
        "Rat in a Maze",
        "Array", "Linked List", "Stack", "Queue",
        "Merge Sort", "Quick Sort", "Binary Search Tree (BST)", "AVL Tree",
        "Breadth First Search (BFS)", "Depth First Search (DFS)",
        "Shortest Path Algorithms", "Minimum Spanning Tree (MST)", "Tries"
    };

    // Light topics: syntax, quick definitions, overview items
    static const vector<string> lightTopics = {
        "Separation of Behaviour and Implementation",
        "Internal Representation of Objects",
        "Private Inheritance",
        "RTTI",
        "Models, Views and Model Elements",
        "Association",
        "Aggregation",
        "Inheritance and Its Implementation",
        "Re-throwing Exceptions",
        "Exception and Inheritance",
        "Function Template Overloading",
        "Collection Classes",
        "Iteration Protocols",
        "Linear and Non-Linear Data Structures",
        "Limitations of Arrays",
        "Recursion Removal Using Stack",
        "Stack and Queue Operations Using STL",
        "Overview of Linear Data Structures"
    };

    for (size_t i = 0; i < flagshipTopics.size(); ++i) {
        if (flagshipTopics[i] == topicName) {
            return DepthTier::Flagship;
        }
    }
    for (size_t i = 0; i < lightTopics.size(); ++i) {
        if (lightTopics[i] == topicName) {
            return DepthTier::Light;
        }
    }
    return DepthTier::Standard;
}
