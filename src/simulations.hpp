// Simulation engine: each topic turns a user input into a list of steps (line, note, variables...).
#pragma once
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <map>
#include <sstream>
#include "content.hpp"

using namespace std;

typedef vector<pair<string, string>> Vars;

struct Step {
    int line = 0;
    string note;
    Vars vars;
    vector<int> arr;
    int hi = -1;
    int cols = 0;
    int node = -1;  // flowchart box to highlight
    bool chr = false;
    string nm = "a";
    vector<string> hl;     // parts of the diagram to highlight
    vector<string> mat;    // matrix shown as a table (row by row)
    int mc = 0;            // number of columns of mat
    vector<string> stack;
    vector<string> tags;   // small label under each array cell (e.g. L, M, R)
    int rl = -1, rh = -1;  // active range of the array; cells outside are dimmed
    string dg;             // per-step drawing JSON, optional
};

typedef vector<Step> Steps;
typedef vector<int> Ints;

struct Sim {
    string label, def;
    vector<string> code, refs, practice;
    function<Steps(const Ints&)> run;
    function<Steps(const string&)> runS;  // text-input topics
    string flow;                          // flowchart layout (JSON), optional
    vector<string> presets;               // quick-pick input values
    function<string(const string&)> diag; // circuit diagram (JSON), optional
    function<vector<string>(const string&)> codeFn;  // input-dependent code lines, optional
};

static map<string, vector<string>>& programNames() {
    static map<string, vector<string>> programMap;
    return programMap;
}

static string S(int value) {
    return to_string(value);
}

static Step mk(int lineNum, string noteText, Vars varsList = {}) {
    Step step;
    step.line = lineNum;
    step.note = noteText;
    step.vars = varsList;
    return step;
}

static Step mkArr(int lineNum, string noteText, Vars varsList, const Ints& arrayData, int highlightIdx) {
    Step step = mk(lineNum, noteText, varsList);
    step.arr = arrayData;
    step.hi = highlightIdx;
    return step;
}

static Ints parseInts(const string& inputText) {
    string cleaned = inputText;
    for (size_t i = 0; i < cleaned.size(); ++i) {
        if (!isdigit((unsigned char)cleaned[i]) && cleaned[i] != '-') {
            cleaned[i] = ' ';
        }
    }
    istringstream stream(cleaned);
    Ints numbers;
    int number = 0;
    while (stream >> number) {
        numbers.push_back(number);
    }
    return numbers;
}

static string urlDecode(const string& encoded) {
    string output;
    for (size_t i = 0; i < encoded.size(); ++i) {
        if (encoded[i] == '+') {
            output += ' ';
        } else if (encoded[i] == '%' && i + 2 < encoded.size() &&
                   isxdigit((unsigned char)encoded[i + 1]) &&
                   isxdigit((unsigned char)encoded[i + 2])) {
            output += (char)strtol(encoded.substr(i + 1, 2).c_str(), nullptr, 16);
            i += 2;
        } else {
            output += encoded[i];
        }
    }
    return output;
}

static string queryParam(const string& queryString, const string& key) {
    istringstream stream(queryString);
    string keyValue;
    while (getline(stream, keyValue, '&')) {
        size_t eqPos = keyValue.find('=');
        if (eqPos != string::npos && keyValue.substr(0, eqPos) == key) {
            return urlDecode(keyValue.substr(eqPos + 1));
        }
    }
    return "";
}

static string jsonStringArray(const vector<string>& items) {
    string result = "[";
    for (size_t i = 0; i < items.size(); ++i) {
        if (i > 0) result += ",";
        result += q(items[i]);
    }
    return result + "]";
}

static Steps simConditionalStatements(const Ints& input) {
    int n = input.empty() ? 0 : input[0];
    Steps steps;
    Vars vars = {{"n", S(n)}};
    steps.push_back(mk(0, "n is set to " + S(n) + ".", vars));
    steps.push_back(mk(1, "Is n > 0 ? " + string(n > 0 ? "Yes." : "No."), vars));
    if (n > 0) {
        steps.push_back(mk(2, "Condition true: prints Positive. All else-branches are skipped.", vars));
        return steps;
    }
    steps.push_back(mk(3, "Is n < 0 ? " + string(n < 0 ? "Yes." : "No."), vars));
    if (n < 0) {
        steps.push_back(mk(4, "Condition true: prints Negative.", vars));
    } else {
        steps.push_back(mk(5, "No condition matched, so else runs.", vars));
        steps.push_back(mk(6, "Prints Zero.", vars));
    }
    return steps;
}

static Steps simLoops(const Ints& input) {
    int n = clamp(input.empty() ? 5 : input[0], 1, 10);
    int sum = 0;
    Steps steps;
    steps.push_back(mk(0, "sum starts at 0.", {{"n", S(n)}, {"sum", "0"}}));
    for (int i = 1; i <= n; ++i) {
        steps.push_back(mk(1, "Check i <= n: " + S(i) + " <= " + S(n) + " is true, so enter the body.", {{"i", S(i)}, {"n", S(n)}, {"sum", S(sum)}}));
        steps.push_back(mk(2, "sum = " + S(sum) + " + " + S(i) + " = " + S(sum + i) + ". Then i++ runs.", {{"i", S(i)}, {"sum", S(sum + i)}}));
        sum += i;
    }
    steps.push_back(mk(1, "i is now " + S(n + 1) + ". " + S(n + 1) + " <= " + S(n) + " is false, so the loop ends.", {{"i", S(n + 1)}, {"sum", S(sum)}}));
    steps.push_back(mk(4, "Prints " + S(sum) + ".", {{"sum", S(sum)}}));
    return steps;
}

static Steps simArrays(const Ints& input) {
    Ints arr = input;
    if (arr.empty()) arr = {4, 9, 2, 7, 5};
    if (arr.size() > 10) arr.resize(10);
    int n = (int)arr.size();
    int maxVal = arr[0];
    Steps steps;
    steps.push_back(mkArr(0, "max takes the first element, a[0] = " + S(maxVal) + ".", {{"max", S(maxVal)}}, arr, 0));
    for (int i = 1; i < n; ++i) {
        steps.push_back(mkArr(1, "i = " + S(i) + ": check i < n, so continue.", {{"i", S(i)}, {"max", S(maxVal)}}, arr, i));
        bool isGreater = arr[i] > maxVal;
        steps.push_back(mkArr(2, "Is a[" + S(i) + "] > max ? " + S(arr[i]) + " > " + S(maxVal) + " is " + (isGreater ? "true." : "false."),
                              {{"i", S(i)}, {"a[i]", S(arr[i])}, {"max", S(maxVal)}}, arr, i));
        if (isGreater) {
            maxVal = arr[i];
            steps.push_back(mkArr(3, "max becomes " + S(maxVal) + ".", {{"i", S(i)}, {"max", S(maxVal)}}, arr, i));
        }
    }
    steps.push_back(mkArr(5, "Loop finished. Prints the largest value: " + S(maxVal) + ".", {{"max", S(maxVal)}}, arr, -1));
    return steps;
}

static void pushStackStep(Steps& steps, const vector<string>& callStack, Step step) {
    step.stack = callStack;
    steps.push_back(step);
}

static Steps simRecursion(const Ints& input) {
    int n = clamp(input.empty() ? 4 : input[0], 0, 8);
    Steps steps;
    vector<string> callStack;
    for (int k = n; ; --k) {
        callStack.push_back("fact(" + S(k) + ")");
        pushStackStep(steps, callStack, mk(1, "Inside fact(" + S(k) + "): is n <= 1 ? " + (k <= 1 ? "Yes, base case." : "No."), {{"n", S(k)}}));
        if (k <= 1) break;
        pushStackStep(steps, callStack, mk(2, "Calls fact(" + S(k - 1) + ") and waits for its result.", {{"n", S(k)}}));
    }
    int result = 1;
    pushStackStep(steps, callStack, mk(1, "Base case returns 1.", {{"return", "1"}}));
    callStack.pop_back();
    for (int j = 2; j <= n; ++j) {
        result *= j;
        pushStackStep(steps, callStack, mk(2, "fact(" + S(j) + ") returns " + S(j) + " * " + S(result / j) + " = " + S(result) + ".",
                                           {{"n", S(j)}, {"return", S(result)}}));
        callStack.pop_back();
    }
    pushStackStep(steps, callStack, mk(3, n <= 1 ? "Result: 1." : "Stack is empty. " + S(n) + "! = " + S(result) + ".", {{"result", S(result)}}));
    return steps;
}

static Steps simPointerArithmetic(const Ints& input) {
    Ints arr = input;
    if (arr.empty()) arr = {10, 20, 30, 40};
    if (arr.size() > 8) arr.resize(8);
    int n = (int)arr.size();
    Steps steps;
    steps.push_back(mkArr(0, "p stores the address of a[0] (the array name decays to a pointer).", {{"p", "&a[0]"}}, arr, 0));
    for (int i = 0; i < n; ++i) {
        steps.push_back(mkArr(1, "i = " + S(i) + ": check i < n, so continue.", {{"i", S(i)}, {"p", "&a[0]"}}, arr, i));
        steps.push_back(mkArr(2, "p + " + S(i) + " moves " + S(i) + " ints ahead (" + S(i * 4) + " bytes) to &a[" + S(i) + "]. Dereferencing gives " + S(arr[i]) + ".",
                              {{"i", S(i)}, {"p + i", "&a[" + S(i) + "]"}, {"*(p + i)", S(arr[i])}}, arr, i));
    }
    steps.push_back(mkArr(3, "Loop ends. p itself never changed; only p + i was computed.", {{"p", "&a[0]"}}, arr, -1));
    return steps;
}

void addMore(map<string, Sim>& m);
void addVariants(map<string, Sim>& m);
void addElectronics(map<string, Sim>& m);
void addCircuits(map<string, Sim>& m);
void addMatrixSteps(map<string, Sim>& m);
void addMatrixSteps2(map<string, Sim>& m);
void addWorkshop(map<string, Sim>& m);
void addWorkshop2(map<string, Sim>& m);
void addDS1(map<string, Sim>& m);
void addSDF2_1(map<string, Sim>& m);

static const map<string, Sim>& sims() {
    static const vector<string> REFS = {
        "Learn-C.org (interactive tutorial)|https://www.learn-c.org/",
        "GeeksforGeeks: C Programming|https://www.geeksforgeeks.org/c-programming-language/",
        "cppreference: C language|https://en.cppreference.com/w/c"};
    static map<string, Sim> m;
    if (!m.empty()) return m;

    m["Conditional Statements"] = {
        "Enter an integer", "-4",
        {"int n = INPUT;", "if (n > 0)", "    printf(\"Positive\");", "else if (n < 0)", "    printf(\"Negative\");", "else", "    printf(\"Zero\");"},
        REFS,
        {"LeetCode #412 (Fizz Buzz)", "LeetCode #1822 (Sign of the Product of an Array)"},
        simConditionalStatements, nullptr, "", {}};

    m["Loops"] = {
        "Enter n (1 to 10) to sum 1..n", "5",
        {"int sum = 0;", "for (int i = 1; i <= n; i++) {", "    sum += i;", "}", "printf(\"%d\", sum);"},
        REFS,
        {"LeetCode #1480 (Running Sum of 1d Array)", "LeetCode #202 (Happy Number)", "LeetCode #412 (Fizz Buzz)"},
        simLoops, nullptr, "", {}};

    m["Arrays"] = {
        "Enter up to 10 numbers (comma separated)", "4, 9, 2, 7, 5",
        {"int max = a[0];", "for (int i = 1; i < n; i++) {", "    if (a[i] > max)", "        max = a[i];", "}", "printf(\"%d\", max);"},
        REFS,
        {"LeetCode #1 (Two Sum)", "LeetCode #485 (Max Consecutive Ones)", "LeetCode #1295 (Find Numbers with Even Number of Digits)"},
        simArrays, nullptr, "", {}};

    m["Recursion"] = {
        "Enter n (0 to 8) to compute n!", "4",
        {"int fact(int n) {", "    if (n <= 1) return 1;", "    return n * fact(n - 1);", "}"},
        REFS,
        {"LeetCode #509 (Fibonacci Number)", "LeetCode #344 (Reverse String)", "LeetCode #231 (Power of Two)"},
        simRecursion, nullptr, "", {}};

    m["Pointers and Pointer Arithmetic"] = {
        "Enter up to 8 numbers (comma separated)", "10, 20, 30, 40",
        {"int *p = a;", "for (int i = 0; i < n; i++) {", "    printf(\"%d\", *(p + i));", "}"},
        REFS,
        {"LeetCode #283 (Move Zeroes)", "LeetCode #26 (Remove Duplicates from Sorted Array)", "LeetCode #344 (Reverse String)"},
        simPointerArithmetic, nullptr, "", {}};

    addMore(m);
    addVariants(m);
    addElectronics(m);
    addCircuits(m);
    addMatrixSteps(m);
    addMatrixSteps2(m);
    addWorkshop(m);
    addWorkshop2(m);
    addDS1(m);
    addSDF2_1(m);
    return m;
}

static string simulateJson(const string& topic, const string& input, int opt) {
    string lookupKey = opt > 0 ? topic + "#" + S(opt) : topic;
    map<string, Sim>::const_iterator it = sims().find(lookupKey);
    if (it == sims().end()) return "{\"available\":false}";
    const Sim& sim = it->second;
    const string effectiveInput = input.empty() ? sim.def : input;
    Steps steps = sim.runS ? sim.runS(effectiveInput) : sim.run(parseInts(effectiveInput));

    vector<string> progNames;
    if (programNames().count(topic)) {
        progNames = programNames()[topic];
    }

    string json = "{\"available\":true,\"label\":" + q(sim.label) +
                  ",\"default\":" + q(sim.def) +
                  ",\"code\":" + jsonStringArray(sim.codeFn ? sim.codeFn(effectiveInput) : sim.code) +
                  ",\"programs\":" + jsonStringArray(progNames) +
                  ",\"presets\":" + jsonStringArray(sim.presets) +
                  ",\"flow\":" + (sim.flow.empty() ? string("null") : sim.flow) +
                  ",\"diagram\":" + (sim.diag ? sim.diag(effectiveInput) : string("null")) +
                  ",\"refs\":" + jsonStringArray(sim.refs) +
                  ",\"practice\":" + jsonStringArray(sim.practice) +
                  ",\"steps\":[";
    for (size_t i = 0; i < steps.size(); ++i) {
        const Step& s = steps[i];
        if (i > 0) json += ",";
        json += "{\"cols\":" + S(s.cols) +
                ",\"chr\":" + (s.chr ? "true" : "false") +
                ",\"nm\":" + q(s.nm) +
                ",\"hl\":" + jsonStringArray(s.hl) +
                ",\"mc\":" + S(s.mc) +
                ",\"mat\":" + jsonStringArray(s.mat) +
                ",\"node\":" + S(s.node) +
                ",\"line\":" + S(s.line) +
                ",\"note\":" + q(s.note) +
                ",\"hi\":" + S(s.hi) +
                ",\"vars\":[";
        for (size_t k = 0; k < s.vars.size(); ++k) {
            if (k > 0) json += ",";
            json += "[" + q(s.vars[k].first) + "," + q(s.vars[k].second) + "]";
        }
        json += "],\"arr\":[";
        for (size_t k = 0; k < s.arr.size(); ++k) {
            if (k > 0) json += ",";
            json += S(s.arr[k]);
        }
        json += "],\"stack\":" + jsonStringArray(s.stack) +
                ",\"tags\":" + jsonStringArray(s.tags) +
                ",\"rl\":" + S(s.rl) +
                ",\"rh\":" + S(s.rh) +
                ",\"dg\":" + (s.dg.empty() ? string("null") : s.dg) + "}";
    }
    return json + "]}";
}
