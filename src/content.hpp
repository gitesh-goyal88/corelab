// Content layer: curriculum data lives in C++ (single source of truth).
#pragma once
#include <cstdio>
#include <string>
#include <vector>
#include "depth_tier.hpp"

using namespace std;

struct Topic {
    string name;
    string group;                // section heading shown in the topic window
    vector<string> items;        // optional sub-points (e.g. malloc, calloc...)
};

struct Subject {
    string name;
    vector<Topic> topics;        // empty = syllabus not added yet
};

struct Semester {
    int id;
    string name;
    string tag;
    vector<Subject> subjects;
};

static const vector<Semester>& curriculum() {
    static const vector<Semester> data = {
        {1, "Semester 1", "First year - foundations",
         {{"Mathematics 1",
           {
               {"Chain Rule", "Partial Differentiation", {}},
               {"Change of Variables (Partial Derivatives)", "Partial Differentiation", {}},
               {"Taylor Series", "Partial Differentiation", {}},
               {"Maxima and Minima", "Partial Differentiation", {}},
               {"Jacobians", "Partial Differentiation", {}},
               {"Change of Order of Integration", "Double Integrals", {}},
               {"Change of Variables (Double Integrals)", "Double Integrals", {}},
               {"Areas and Volumes", "Double Integrals", {}},
               {"Plotting Curves and Surfaces", "Double Integrals", {}},
               {"Gradient", "Vector Differentiation", {}},
               {"Divergence", "Vector Differentiation", {}},
               {"Curl", "Vector Differentiation", {}},
               {"Normal and Tangent to a Surface", "Vector Differentiation", {}},
               {"Line Integrals", "Vector Integration", {}},
               {"Green's Theorem", "Vector Integration", {}},
               {"Surface Integrals", "Vector Integration", {}},
               {"Gauss's Theorem", "Vector Integration", {}},
               {"Stokes' Theorem", "Vector Integration", {}},
               {"Constant-Coefficient Equations", "Differential Equations", {}},
               {"Cauchy–Euler Equations", "Differential Equations", {}},
               {"Practical Applications: Oscillations", "Differential Equations", {}},
               {"Practical Applications: Cooling and Growth", "Differential Equations", {}},
               {"Laplace Transform", "Laplace Transform", {}},
               {"Inverse Laplace Transform", "Laplace Transform", {}},
               {"Dirac Delta Function", "Laplace Transform", {}},
               {"Unit Step Function", "Laplace Transform", {}},
               {"Solving Initial-Value Problems", "Laplace Transform", {}},
               {"Row Echelon Form", "Matrices", {}},
               {"Rank of a Matrix", "Matrices", {}},
               {"Gauss Elimination", "Matrices", {}},
               {"Eigenvalues and Eigenvectors", "Matrices", {}},
               {"Diagonalization", "Matrices", {}},
               {"Quadratic Forms", "Matrices", {}},
           }},
          {"Physics 1",
           {
               {"Interference", "Physical Optics", {}},
               {"Intensity Distribution", "Physical Optics", {}},
               {"Fresnel's Biprism", "Physical Optics", {}},
               {"Newton's Rings", "Physical Optics", {}},
               {"Michelson Interferometer", "Physical Optics", {}},
               {"Fraunhofer Diffraction", "Physical Optics", {}},
               {"Single Slit", "Physical Optics", {}},
               {"Double Slit", "Physical Optics", {}},
               {"Diffraction Grating", "Physical Optics", {}},
               {"Polarization", "Physical Optics", {}},
               {"Birefringence", "Physical Optics", {}},
               {"Polarizers", "Physical Optics", {}},
               {"Compensators", "Physical Optics", {}},
               {"Wave Plates", "Physical Optics", {}},
               {"Retardation Plate", "Physical Optics", {}},
               {"Optical Activity", "Physical Optics", {}},
               {"Polarimeter", "Physical Optics", {}},
               {"Resolving Power of Microscope", "Physical Optics", {}},
               {"Frame of Reference", "Relativity", {}},
               {"Galilean Transformations", "Relativity", {}},
               {"Michelson–Morley Experiment", "Relativity", {}},
               {"Lorentz Transformations", "Relativity", {}},
               {"Addition of Velocities", "Relativity", {}},
               {"Mass Variation with Velocity", "Relativity", {}},
               {"Mass–Energy Relation", "Relativity", {}},
               {"Spectral Lines", "Atomic Structure", {}},
               {"Spin and Orbital Angular Momentum", "Atomic Structure", {}},
               {"Quantum Numbers", "Atomic Structure", {}},
               {"Designation of States", "Atomic Structure", {}},
               {"Atoms in Magnetic Field", "Atomic Structure", {}},
               {"Zeeman Effect", "Atomic Structure", {}},
               {"Black Body Radiation", "Radiation", {}},
               {"Wien's Law", "Radiation", {}},
               {"Rayleigh–Jeans Law", "Radiation", {}},
               {"Bose–Einstein Statistics", "Radiation", {}},
               {"Planck's Law", "Radiation", {}},
               {"Wien's Displacement Law", "Radiation", {}},
               {"Wave–Particle Duality", "Quantum Mechanics", {}},
               {"Compton Scattering", "Quantum Mechanics", {}},
               {"Matter Waves", "Quantum Mechanics", {}},
               {"Heisenberg's Uncertainty Principle", "Quantum Mechanics", {}},
               {"Schrödinger Wave Equation", "Quantum Mechanics", {}},
               {"Particle in a Box (1D and 3D)", "Quantum Mechanics", {}},
               {"Potential Barrier", "Quantum Mechanics", {}},
               {"Tunnel Diode", "Quantum Mechanics", {}},
           }},
          {"Electronics",
           {
               {"Resistor Combinations (Series / Parallel / Mixed)", "Basic Circuit Analysis", {}},
               {"Kirchhoff's Laws", "Basic Circuit Analysis", {}},
               {"Voltage Divider", "Basic Circuit Analysis", {}},
               {"Current Divider", "Basic Circuit Analysis", {}},
               {"Nodal Analysis", "Basic Circuit Analysis", {}},
               {"Mesh Analysis", "Basic Circuit Analysis", {}},
               {"Superposition", "Basic Circuit Analysis", {}},
               {"Thevenin's Theorem", "Basic Circuit Analysis", {}},
               {"Norton's Theorem", "Basic Circuit Analysis", {}},
               {"PN Junction", "PN Junction Diode", {}},
               {"Diode Biasing (Forward and Reverse)", "PN Junction Diode", {}},
               {"Diode I-V Characteristics", "PN Junction Diode", {}},
               {"Half-Wave Rectifier", "PN Junction Diode", {}},
               {"Full-Wave Rectifier", "PN Junction Diode", {}},
               {"Clipper", "PN Junction Diode", {}},
               {"Clamper", "PN Junction Diode", {}},
               {"Zener Diode Applications (Shunt Regulator)", "Zener Diode", {}},
               {"Line Regulation", "Zener Diode", {}},
               {"Load Regulation", "Zener Diode", {}},
               {"BJT Operation", "BJT", {}},
               {"BJT Characteristics", "BJT", {}},
               {"BJT Biasing", "BJT", {}},
               {"BJT Stability", "BJT", {}},
               {"MOSFET Operation", "MOSFET", {}},
               {"MOSFET Characteristics", "MOSFET", {}},
               {"MOSFET Biasing", "MOSFET", {}},
               {"Op-Amp Parameters", "Op-Amp", {}},
               {"Ideal Op-Amp", "Op-Amp", {}},
               {"Op-Amp Equivalent Circuit", "Op-Amp", {}},
               {"Inverting Amplifier", "Op-Amp", {}},
               {"Non-Inverting Amplifier", "Op-Amp", {}},
               {"Voltage Follower", "Op-Amp", {}},
               {"Summing Amplifier (Summer)", "Op-Amp", {}},
               {"Comparator", "Op-Amp", {}},
               {"Difference Amplifier", "Op-Amp", {}},
               {"Integrator", "Op-Amp", {}},
               {"Differentiator", "Op-Amp", {}},
           }},
          {"Software Development 1",
           {
               {"Software Development Life Cycle (SDLC)", "Foundations", {}},
               {"Algorithms", "Foundations", {}},
               {"Flowcharts and Pseudocode", "Foundations", {}},
               {"Variables, Constants and Data Types", "Foundations", {}},
               {"Input/Output Functions", "Basics and Control Flow", {}},
               {"Operators and Operator Precedence", "Basics and Control Flow", {}},
               {"Conditional Statements", "Basics and Control Flow", {"if", "if-else", "switch"}},
               {"Loops", "Basics and Control Flow", {"for", "while", "do-while"}},
               {"Arrays", "Arrays and Strings", {}},
               {"Multidimensional Arrays (2D and 3D)", "Arrays and Strings", {}},
               {"Strings and String Library Functions", "Arrays and Strings", {}},
               {"Functions and Parameter Passing", "Functions and Pointers", {}},
               {"Recursion", "Functions and Pointers", {}},
               {"Pointers and Pointer Arithmetic", "Functions and Pointers", {}},
               {"Pointers and Arrays", "Functions and Pointers", {}},
               {"Pointers and Strings", "Functions and Pointers", {}},
               {"Dynamic Memory Allocation", "Memory", {"malloc()", "calloc()", "realloc()", "free()"}},
               {"Structures", "User-defined Types", {}},
               {"Unions", "User-defined Types", {}},
               {"Arrays of Structures", "User-defined Types", {}},
               {"Pointers to Structures", "User-defined Types", {}},
               {"Enumerations (enum)", "User-defined Types", {}},
               {"File Handling", "Files and Program Arguments", {}},
               {"Command-Line Arguments (argc, argv)", "Files and Program Arguments", {}},
           }}}},
        {2, "Semester 2", "First year - building up",
         {{"Software Development 2",
           {
               {"Procedural Programming vs Object-Oriented Programming", "Module 1: Introduction to OOP", {}},
               {"Characteristics of Object-Oriented Languages", "Module 1: Introduction to OOP", {}},
               {"Separation of Behaviour and Implementation", "Module 1: Introduction to OOP", {}},
               {"Classes and Objects", "Module 2: OOP Concepts Using C++", {}},
               {"Internal Representation of Objects", "Module 2: OOP Concepts Using C++", {}},
               {"Constructors", "Module 2: OOP Concepts Using C++", {}},
               {"Destructors", "Module 2: OOP Concepts Using C++", {}},
               {"Function Overloading", "Module 2: OOP Concepts Using C++", {}},
               {"Operator Overloading", "Module 2: OOP Concepts Using C++", {}},
               {"Static Functions", "Module 2: OOP Concepts Using C++", {}},
               {"Friend Functions", "Module 2: OOP Concepts Using C++", {}},
               {"Base Class and Derived Class", "Module 3: Inheritance Using C++", {}},
               {"Method Overriding", "Module 3: Inheritance Using C++", {}},
               {"Private Inheritance", "Module 3: Inheritance Using C++", {}},
               {"Public Inheritance", "Module 3: Inheritance Using C++", {}},
               {"Multiple Inheritance", "Module 3: Inheritance Using C++", {}},
               {"Virtual Functions", "Module 4: Polymorphism Using C++", {}},
               {"Pure Virtual Functions", "Module 4: Polymorphism Using C++", {}},
               {"Abstract Classes", "Module 4: Polymorphism Using C++", {}},
               {"Dynamic Dispatch", "Module 4: Polymorphism Using C++", {}},
               {"Internal Representation of Method Tables", "Module 4: Polymorphism Using C++", {}},
               {"RTTI", "Module 4: Polymorphism Using C++", {}},
               {"Models, Views and Model Elements", "Module 5: UML and Relationships", {}},
               {"Class Diagrams", "Module 5: UML and Relationships", {}},
               {"Association", "Module 5: UML and Relationships", {}},
               {"Aggregation", "Module 5: UML and Relationships", {}},
               {"Composition", "Module 5: UML and Relationships", {}},
               {"Inheritance and Its Implementation", "Module 5: UML and Relationships", {}},
               {"Exceptions", "Module 6: Exceptions, Templates and STL", {}},
               {"Try, Catch and Throw", "Module 6: Exceptions, Templates and STL", {}},
               {"Re-throwing Exceptions", "Module 6: Exceptions, Templates and STL", {}},
               {"Exception and Inheritance", "Module 6: Exceptions, Templates and STL", {}},
               {"Function Templates", "Module 6: Exceptions, Templates and STL", {}},
               {"Function Template Overloading", "Module 6: Exceptions, Templates and STL", {}},
               {"Class Templates", "Module 6: Exceptions, Templates and STL", {}},
               {"Collection Classes", "Module 6: Exceptions, Templates and STL", {}},
               {"Iteration Protocols", "Module 6: Exceptions, Templates and STL", {}},
               {"Standard Template Library (STL)", "Module 6: Exceptions, Templates and STL", {}},
               {"Fundamentals of Data Structures", "Module 7: Introduction to Data Structures", {}},
               {"Dynamic Memory Allocation (C++)", "Module 7: Introduction to Data Structures", {}},
               {"Linear and Non-Linear Data Structures", "Module 7: Introduction to Data Structures", {}},
               {"Limitations of Arrays", "Module 7: Introduction to Data Structures", {}},
               {"Need for Linked Lists", "Module 7: Introduction to Data Structures", {}},
               {"Types of Linked Lists", "Module 7: Introduction to Data Structures", {}},
               {"Stack (SDF2)", "Module 7: Introduction to Data Structures", {}},
               {"Queue (SDF2)", "Module 7: Introduction to Data Structures", {}},
               {"Stack and Queue Operations Using STL", "Module 7: Introduction to Data Structures", {}},
               {"Recursion (SDF2)", "Module 7: Introduction to Data Structures", {}},
               {"Recursion Removal Using Stack", "Module 7: Introduction to Data Structures", {}},
               {"Tower of Hanoi", "Module 7: Introduction to Data Structures", {}},
               {"N-Queens Problem", "Module 7: Introduction to Data Structures", {}},
               {"Rat in a Maze", "Module 7: Introduction to Data Structures", {}},
           }}, {"WorkShop",
           {
               {"T-Joint", "Unit 1: Carpentry Shop", {}},
               {"Dovetail or Cross-Lap Joint", "Unit 1: Carpentry Shop", {}},
               {"Gas and Arc Welding Equipment", "Unit 2: Welding Shop", {}},
               {"Welding Safety Measures", "Unit 2: Welding Shop", {}},
               {"Butt Joint (Welding)", "Unit 2: Welding Shop", {}},
               {"Lap Joint (Welding)", "Unit 2: Welding Shop", {}},
               {"Square Tray (GI Sheet)", "Unit 3: Sheet Metal Shop", {}},
               {"Funnel (GI Sheet)", "Unit 3: Sheet Metal Shop", {}},
               {"V-Groove Fit", "Unit 4: Fitting Shop", {}},
               {"Square Fit", "Unit 4: Fitting Shop", {}},
               {"Turning on a Lathe", "Unit 5: Machine Shop", {}},
               {"Facing on a Lathe", "Unit 5: Machine Shop", {}},
               {"Grooving on a Lathe", "Unit 5: Machine Shop", {}},
               {"Slotting on a Shaper", "Unit 5: Machine Shop", {}},
               {"Face Milling", "Unit 5: Machine Shop", {}},
               {"G-Codes and M-Codes (CNC)", "Unit 5: Machine Shop", {}},
           }}}},
        {3, "Semester 3", "Second year - core CS",
         {{"Data Structures",
           {
               {"Overview of Linear Data Structures", "Module 1: Linear Data Structures", {}},
               {"Array", "Module 1: Linear Data Structures", {}},
               {"Linked List", "Module 1: Linear Data Structures", {}},
               {"Stack", "Module 1: Linear Data Structures", {}},
               {"Queue", "Module 1: Linear Data Structures", {}},
               {"Interpolation Search", "Module 2: Searching, Sorting and Hashing", {}},
               {"Median Search", "Module 2: Searching, Sorting and Hashing", {}},
               {"Hashing", "Module 2: Searching, Sorting and Hashing", {}},
               {"Hash Table", "Module 2: Searching, Sorting and Hashing", {}},
               {"Chaining", "Module 2: Searching, Sorting and Hashing", {}},
               {"Probing", "Module 2: Searching, Sorting and Hashing", {}},
               {"Merge Sort", "Module 2: Searching, Sorting and Hashing", {}},
               {"Quick Sort", "Module 2: Searching, Sorting and Hashing", {}},
               {"Radix Sort", "Module 2: Searching, Sorting and Hashing", {}},
               {"Bucket Sort", "Module 2: Searching, Sorting and Hashing", {}},
               {"Counting Sort", "Module 2: Searching, Sorting and Hashing", {}},
               {"Time Complexity Analysis", "Module 2: Searching, Sorting and Hashing", {}},
               {"Space Complexity Analysis", "Module 2: Searching, Sorting and Hashing", {}},
               {"Multi List", "Module 3: Non-Linear Data Structures - Multi List and Tree", {}},
               {"Binary Tree", "Module 3: Non-Linear Data Structures - Multi List and Tree", {}},
               {"K-ary Tree", "Module 3: Non-Linear Data Structures - Multi List and Tree", {}},
               {"Tree Traversals", "Module 3: Non-Linear Data Structures - Multi List and Tree", {}},
               {"Threaded Binary Tree", "Module 3: Non-Linear Data Structures - Multi List and Tree", {}},
               {"Priority Queue using Binary Heap", "Module 3: Non-Linear Data Structures - Multi List and Tree", {}},
               {"Binomial Heap", "Module 3: Non-Linear Data Structures - Multi List and Tree", {}},
               {"Fibonacci Heap", "Module 3: Non-Linear Data Structures - Multi List and Tree", {}},
               {"Heap Sort", "Module 3: Non-Linear Data Structures - Multi List and Tree", {}},
               {"Binary Search Tree (BST)", "Module 4: Non-Linear Data Structures - Binary and Multiway Trees", {}},
               {"Balanced BST", "Module 4: Non-Linear Data Structures - Binary and Multiway Trees", {}},
               {"AVL Tree", "Module 4: Non-Linear Data Structures - Binary and Multiway Trees", {}},
               {"Red-Black (RB) Tree", "Module 4: Non-Linear Data Structures - Binary and Multiway Trees", {}},
               {"B-Tree", "Module 4: Non-Linear Data Structures - Binary and Multiway Trees", {}},
               {"B+ Tree", "Module 4: Non-Linear Data Structures - Binary and Multiway Trees", {}},
               {"Fundamentals of Graph", "Module 5: Non-Linear Data Structures - Graph", {}},
               {"Adjacency Matrix", "Module 5: Non-Linear Data Structures - Graph", {}},
               {"Adjacency List", "Module 5: Non-Linear Data Structures - Graph", {}},
               {"Graph Traversal", "Module 5: Non-Linear Data Structures - Graph", {}},
               {"Depth First Search (DFS)", "Module 5: Non-Linear Data Structures - Graph", {}},
               {"Breadth First Search (BFS)", "Module 5: Non-Linear Data Structures - Graph", {}},
               {"Shortest Path Algorithms", "Module 5: Non-Linear Data Structures - Graph", {}},
               {"Minimum Spanning Tree (MST)", "Module 5: Non-Linear Data Structures - Graph", {}},
           }}, {"Mathematical Foundations of AI and DS",
           {
               {"Vector Space", "Module 1: Linear Algebra", {}},
               {"Subspace", "Module 1: Linear Algebra", {}},
               {"Basis and Dimension", "Module 1: Linear Algebra", {}},
               {"Linear Transformations", "Module 1: Linear Algebra", {}},
               {"Matrix Representation of Linear Transformations", "Module 1: Linear Algebra", {}},
               {"Rank–Nullity Theorem", "Module 1: Linear Algebra", {}},
               {"Norm of Vectors and Matrices", "Module 1: Linear Algebra", {}},
               {"Inner Product Space", "Module 1: Linear Algebra", {}},
               {"Gram–Schmidt Orthogonalization", "Module 1: Linear Algebra", {}},
               {"Singular Value Decomposition (SVD)", "Module 1: Linear Algebra", {}},
               {"Probability Theory", "Module 2: Probability", {}},
               {"One-Dimensional Random Variables", "Module 2: Probability", {}},
               {"Two-Dimensional Random Variables", "Module 2: Probability", {}},
               {"Expectation", "Module 2: Probability", {}},
               {"Variance", "Module 2: Probability", {}},
               {"Covariance", "Module 2: Probability", {}},
               {"Marginal Distributions", "Module 2: Probability", {}},
               {"Conditional Distributions", "Module 2: Probability", {}},
               {"Bernoulli Distribution", "Module 2: Probability", {}},
               {"Binomial Distribution", "Module 2: Probability", {}},
               {"Poisson Distribution", "Module 2: Probability", {}},
               {"Normal Distribution", "Module 2: Probability", {}},
               {"Central Limit Theorem (CLT)", "Module 3: Statistics", {}},
               {"Correlation and Regression", "Module 3: Statistics", {}},
               {"Least Square Method", "Module 3: Statistics", {}},
               {"Naïve Bayes Classifier", "Module 3: Statistics", {}},
               {"Convex and Non-Convex Functions", "Module 4: Optimization Techniques", {}},
               {"Linear Programming: Graphical Method", "Module 4: Optimization Techniques", {}},
               {"Linear Programming: Simplex Method", "Module 4: Optimization Techniques", {}},
               {"Duality", "Module 4: Optimization Techniques", {}},
               {"Nonlinear Programming Problems", "Module 4: Optimization Techniques", {}},
               {"Lagrange Multipliers", "Module 4: Optimization Techniques", {}},
               {"Karush–Kuhn–Tucker (KKT) Conditions", "Module 4: Optimization Techniques", {}},
               {"Gradient Descent Method", "Module 4: Optimization Techniques", {}},
               {"Optimal Separating Hyperplane", "Module 4: Optimization Techniques", {}},
           }}}}};
    return data;
}


static string jsonEscape(const string& inputStr) {
    // valid JSON for any input: escape control characters and replace malformed UTF-8 with U+FFFD
    string out;
    out.reserve(inputStr.size() + 8);
    size_t idx = 0;
    size_t len = inputStr.size();
    while (idx < len) {
        unsigned char ch = (unsigned char)inputStr[idx];
        if (ch == '"' || ch == '\\') {
            out += '\\';
            out += (char)ch;
            ++idx;
        } else if (ch < 0x20) {
            char buf[8];
            snprintf(buf, sizeof buf, "\\u%04x", ch);
            out += buf;
            ++idx;
        } else if (ch < 0x80) {
            out += (char)ch;
            ++idx;
        } else {
            size_t utfLen = (ch >= 0xC2 && ch <= 0xDF) ? 2 : (ch >= 0xE0 && ch <= 0xEF) ? 3 : (ch >= 0xF0 && ch <= 0xF4) ? 4 : 0;
            bool validUtf = (utfLen > 0 && idx + utfLen <= len);
            for (size_t k = 1; validUtf && k < utfLen; ++k) {
                if (((unsigned char)inputStr[idx + k] & 0xC0) != 0x80) {
                    validUtf = false;
                }
            }
            if (validUtf && utfLen == 3) {
                unsigned char secondByte = (unsigned char)inputStr[idx + 1];
                if ((ch == 0xE0 && secondByte < 0xA0) || (ch == 0xED && secondByte > 0x9F)) {
                    validUtf = false;
                }
            }
            if (validUtf && utfLen == 4) {
                unsigned char secondByte = (unsigned char)inputStr[idx + 1];
                if ((ch == 0xF0 && secondByte < 0x90) || (ch == 0xF4 && secondByte > 0x8F)) {
                    validUtf = false;
                }
            }
            if (validUtf) {
                out.append(inputStr, idx, utfLen);
                idx += utfLen;
            } else {
                out += "\xEF\xBF\xBD";
                ++idx;
            }
        }
    }
    return out;
}

static string q(const string& inputStr) {
    return "\"" + jsonEscape(inputStr) + "\"";
}

static string curriculumJson() {
    string json = "[";
    const vector<Semester>& semesters = curriculum();
    for (size_t i = 0; i < semesters.size(); ++i) {
        if (i > 0) json += ",";
        json += "{\"id\":" + to_string(semesters[i].id) + ",\"name\":" + q(semesters[i].name) +
                ",\"tag\":" + q(semesters[i].tag) + ",\"subjects\":[";
        for (size_t k = 0; k < semesters[i].subjects.size(); ++k) {
            const Subject& subj = semesters[i].subjects[k];
            if (k > 0) json += ",";
            json += "{\"name\":" + q(subj.name) + ",\"topics\":[";
            for (size_t t = 0; t < subj.topics.size(); ++t) {
                if (t > 0) json += ",";
                json += "{\"name\":" + q(subj.topics[t].name) + ",\"tier\":" + q(tierToString(getTopicTier(subj.topics[t].name))) +
                        ",\"group\":" + q(subj.topics[t].group) + ",\"items\":[";
                for (size_t m = 0; m < subj.topics[t].items.size(); ++m) {
                    if (m > 0) json += ",";
                    json += q(subj.topics[t].items[m]);
                }
                json += "]}";
            }
            json += "]}";
        }
        json += "]}";
    }
    return json + "]";
}
