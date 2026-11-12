// Program choices per topic (stored as "Topic#k"), the SDLC models and real flowchart drawings.
#pragma once
#include "simulations2.hpp"

inline Step nd(int l, int node, std::string n, Vars v = {}) { Step s = mk(l, std::move(n), std::move(v)); s.node = node; return s; }

inline void addVariants(std::map<std::string, Sim>& m) {
    const std::vector<std::string> R = {
        "Learn-C.org (interactive tutorial)|https://www.learn-c.org/",
        "GeeksforGeeks: C Programming|https://www.geeksforgeeks.org/c-programming-language/",
        "cppreference: C language|https://en.cppreference.com/w/c"};
    auto I = [&](const std::string& key, const char* lab, const char* def, std::vector<std::string> code, std::vector<std::string> pr, std::function<Steps(const Ints&)> f) {
        m[key] = Sim{lab, def, code, R, pr, f, nullptr, "", {}}; };
    auto T = [&](const std::string& key, const char* lab, const char* def, std::vector<std::string> code, std::vector<std::string> pr, std::function<Steps(const std::string&)> f) {
        Sim s{lab, def, code, R, pr, nullptr, f, "", {}}; m[key] = s; };
    auto names = [&](const char* topic, std::vector<std::string> n) { programNames()[topic] = n; };

    /* ---------- SDLC: choose the life cycle model; pick a project from the list ---------- */
    struct Model { const char* name; std::vector<std::string> code; std::vector<std::pair<int, std::string>> seq; };
    std::vector<Model> models = {
        {"Waterfall model", {"1. Planning", "2. Requirement Analysis", "3. Design", "4. Implementation (Coding)", "5. Testing", "6. Deployment", "7. Maintenance"},
         {{0, "Decide goals, budget, team and schedule for %P."}, {1, "Collect every requirement for %P and freeze it in an SRS document."}, {2, "Design the architecture, database and screens of %P."},
          {3, "Developers write the code of %P."}, {4, "Testers test the complete %P and report bugs."}, {5, "Release %P to users."}, {6, "Fix issues in %P and add small updates. Waterfall never goes back: each phase finishes before the next one starts. Best when requirements are fixed."}}},
        {"Agile (Scrum) model", {"Product Backlog (list of features)", "Sprint Planning", "Develop + Test (1 to 4 weeks)", "Sprint Review (demo to the client)", "Retrospective (what to improve)", "Release a working increment"},
         {{0, "The team writes a backlog of features wanted in %P."}, {1, "Sprint 1: pick the top features for this short cycle."}, {2, "Sprint 1: build and test them together."}, {3, "Sprint 1: demo to the client and collect feedback."}, {4, "Sprint 1: discuss what to improve."}, {5, "Sprint 1 done: a working piece of %P is released."},
          {1, "Sprint 2: pick the next features, changing priorities if the client asks."}, {2, "Sprint 2: build and test."}, {3, "Sprint 2: demo and feedback."}, {5, "Sprint 2 done: %P has more features."},
          {5, "After several sprints %P is complete. Agile welcomes changing requirements and delivers working software often."}}},
        {"Spiral model", {"1. Planning (set objectives)", "2. Risk Analysis", "3. Engineering (build a prototype)", "4. Evaluation (customer review)"},
         {{0, "Loop 1: set the objectives for the first prototype of %P."}, {1, "Loop 1: find the biggest risks (cost, technology) and plan to reduce them."}, {2, "Loop 1: build a small prototype of %P."}, {3, "Loop 1: the customer reviews the prototype."},
          {0, "Loop 2: plan the next, bigger version using the feedback."}, {1, "Loop 2: analyse the new risks."}, {2, "Loop 2: build a more complete version."}, {3, "Loop 2: customer review again."},
          {2, "Final loop: the finished %P is built. Spiral repeats four steps in loops and puts risk analysis first. Best for big, risky projects."}}},
        {"V-Model", {"Requirements  <->  Acceptance Testing", "System Design  <->  System Testing", "Architecture Design  <->  Integration Testing", "Module Design  <->  Unit Testing", "Coding (the bottom of the V)"},
         {{0, "Left side, going down: gather requirements for %P and plan the acceptance tests now."}, {1, "Design the whole system and plan the system tests."}, {2, "Design the architecture and plan the integration tests."}, {3, "Design each module and plan the unit tests."},
          {4, "Bottom of the V: write the code of %P."}, {3, "Right side, going up: unit testing checks each module against its design."}, {2, "Integration testing checks that modules work together."}, {1, "System testing checks the complete %P."},
          {0, "Acceptance testing: the client checks %P against the requirements. Every development phase has a matching test phase."}}},
        {"Iterative model", {"1. Plan", "2. Design", "3. Build", "4. Test", "5. Evaluate (feedback)"},
         {{0, "Version 1: plan only the core features of %P."}, {1, "Version 1: design them."}, {2, "Version 1: build them."}, {3, "Version 1: test them."}, {4, "Version 1 is evaluated and the feedback is collected."},
          {0, "Version 2: plan more features using that feedback."}, {2, "Version 2: design and build them on top of version 1."}, {3, "Version 2: test."}, {4, "Version 2 is evaluated."},
          {2, "Version 3: the final features are added. %P grows version by version, and each version is a working product."}}}};
    std::vector<std::string> mn;
    for (size_t k = 0; k < models.size(); ++k) {
        Model md = models[k]; mn.push_back(md.name);
        T(k ? "Software Development Life Cycle (SDLC)#" + S(k) : "Software Development Life Cycle (SDLC)", "Type a project name or pick one below", "Library App", md.code,
          {"No LeetCode match. Compare two models for the same project and decide which fits better."},
          [md](const std::string& in) {
              std::string p = in; while (!p.empty() && isspace((unsigned char)p.back())) p.pop_back();
              if (p.empty()) p = "Library App"; if (p.size() > 28) p.resize(28); Steps s; int n = 0;
              for (auto& e : md.seq) { std::string t = e.second; for (size_t i; (i = t.find("%P")) != std::string::npos;) t.replace(i, 2, "\"" + p + "\"");
                  s.push_back(mk(e.first, t, {{"Project", p}, {"Model", md.name}, {"Step", S(++n) + " of " + S(md.seq.size())}})); }
              return s; });
        m[k ? "Software Development Life Cycle (SDLC)#" + S(k) : "Software Development Life Cycle (SDLC)"].presets = {"Library App", "Online Shop", "Hospital System", "Banking App", "Weather App"};
    }
    names("Software Development Life Cycle (SDLC)", mn);

    /* ---------- Flowcharts with real shapes ---------- */
    const std::vector<std::string> flowCode = {"START", "READ n", "IF n MOD 2 = 0 THEN", "    PRINT \"Even\"", "ELSE", "    PRINT \"Odd\"", "END"};
    const std::string F01 = "{\"w\":320,\"h\":350,\"n\":[[\"s\",160,25,\"START\"],[\"i\",160,85,\"%1\"],[\"d\",160,165,\"%2\"],[\"i\",85,255,\"%3\"],[\"i\",235,255,\"%4\"],[\"s\",160,325,\"END\"]],\"e\":[[0,\"b\",1,\"t\",\"\"],[1,\"b\",2,\"t\",\"\"],[2,\"l\",3,\"t\",\"Yes\"],[2,\"r\",4,\"t\",\"No\"],[3,\"b\",5,\"l\",\"\"],[4,\"b\",5,\"r\",\"\"]]}";
    auto fill = [&](std::string t, std::vector<std::string> v) { for (int i = 0; i < 4; ++i) { std::string key = "%" + S(i + 1); size_t p = t.find(key); if (p != std::string::npos) t.replace(p, 2, v[i]); } return t; };
    I("Flowcharts and Pseudocode", "Enter an integer", "7", flowCode, {"LeetCode #9 (Palindrome Number)", "LeetCode #412 (Fizz Buzz)"},
      [](const Ints& v) {
          int n = v.empty() ? 7 : v[0]; bool ev = n % 2 == 0; Vars nv = {{"n", S(n)}}; Steps s;
          s.push_back(nd(0, 0, "Oval (terminator) = the START of the flowchart.", nv));
          s.push_back(nd(1, 1, "Parallelogram = input or output. READ n takes your number: n = " + S(n) + ".", nv));
          s.push_back(nd(2, 2, "Diamond = decision. It asks one question with two exits. " + S(n) + " MOD 2 = " + S(((n % 2) + 2) % 2) + ", so n % 2 == 0 is " + (ev ? "YES." : "NO."), nv));
          s.push_back(nd(ev ? 3 : 5, ev ? 3 : 4, std::string("Follow the ") + (ev ? "Yes" : "No") + " arrow to another parallelogram (output): PRINT " + (ev ? "Even." : "Odd."), nv));
          s.push_back(nd(6, 5, "Arrows are the flow lines. Both paths meet at the oval END.", nv)); return s; });
    m["Flowcharts and Pseudocode"].flow = fill(F01, {"READ n", "n % 2 == 0 ?", "PRINT Even", "PRINT Odd"});
    I("Flowcharts and Pseudocode#1", "Enter two integers a, b", "12, 30", {"START", "READ a, b", "IF a > b THEN", "    PRINT a", "ELSE", "    PRINT b", "END"}, {"LeetCode #1979 (Find Greatest Common Divisor of Array)", "LeetCode #414 (Third Maximum Number)"},
      [](const Ints& v) {
          int a = v.size() > 0 ? v[0] : 12, b = v.size() > 1 ? v[1] : 30; bool g = a > b; Vars nv = {{"a", S(a)}, {"b", S(b)}}; Steps s;
          s.push_back(nd(0, 0, "Oval = START.", {})); s.push_back(nd(1, 1, "Parallelogram = input. READ a, b stores a = " + S(a) + " and b = " + S(b) + ".", nv));
          s.push_back(nd(2, 2, "Diamond = decision: is a > b ? " + S(a) + " > " + S(b) + " is " + (g ? "YES." : "NO."), nv));
          s.push_back(nd(g ? 3 : 5, g ? 3 : 4, std::string("The ") + (g ? "Yes" : "No") + " arrow leads to the output box: PRINT " + (g ? "a = " + S(a) : "b = " + S(b)) + ".", nv));
          s.push_back(nd(6, 5, "Both branches join at END.", nv)); return s; });
    m["Flowcharts and Pseudocode#1"].flow = fill(F01, {"READ a, b", "a > b ?", "PRINT a", "PRINT b"});
    I("Flowcharts and Pseudocode#2", "Enter n (1 to 6): sum of 1 to n", "4", {"START", "READ n", "sum = 0, i = 1", "WHILE i <= n", "    sum = sum + i", "    i = i + 1", "PRINT sum", "END"}, {"LeetCode #1480 (Running Sum of 1d Array)", "LeetCode #202 (Happy Number)"},
      [](const Ints& v) {
          int n = std::clamp(v.empty() ? 4 : v[0], 1, 6), sum = 0; Steps s;
          s.push_back(nd(0, 0, "Oval = START.")); s.push_back(nd(1, 1, "Parallelogram = input: n = " + S(n) + ".", {{"n", S(n)}}));
          s.push_back(nd(2, 2, "Rectangle = process (a calculation or assignment). Set sum = 0 and i = 1.", {{"sum", "0"}, {"i", "1"}}));
          for (int i = 1; i <= n; ++i) {
              s.push_back(nd(3, 3, "Diamond: i <= n ? " + S(i) + " <= " + S(n) + " is YES, so go down.", {{"i", S(i)}, {"sum", S(sum)}}));
              sum += i; s.push_back(nd(4, 4, "Process: sum = sum + i = " + S(sum) + ".", {{"i", S(i)}, {"sum", S(sum)}}));
              s.push_back(nd(5, 5, "Process: i = i + 1. The arrow on the left goes BACK up to the diamond. That backward arrow is the loop.", {{"i", S(i + 1)}, {"sum", S(sum)}}));
          }
          s.push_back(nd(3, 3, S(n + 1) + " <= " + S(n) + " is NO, so leave the loop through the right exit.", {{"i", S(n + 1)}, {"sum", S(sum)}}));
          s.push_back(nd(6, 6, "Parallelogram = output: PRINT " + S(sum) + ".", {{"sum", S(sum)}})); s.push_back(nd(7, 7, "Oval = END.", {{"sum", S(sum)}})); return s; });
    m["Flowcharts and Pseudocode#2"].flow = "{\"w\":360,\"h\":430,\"n\":[[\"s\",140,25,\"START\"],[\"i\",140,80,\"READ n\"],[\"p\",140,140,\"sum=0, i=1\"],[\"d\",140,225,\"i <= n ?\"],[\"p\",140,320,\"sum = sum + i\"],[\"p\",140,385,\"i = i + 1\"],[\"i\",285,320,\"PRINT sum\"],[\"s\",285,395,\"END\"]],\"e\":[[0,\"b\",1,\"t\",\"\"],[1,\"b\",2,\"t\",\"\"],[2,\"b\",3,\"t\",\"\"],[3,\"b\",4,\"t\",\"Yes\"],[4,\"b\",5,\"t\",\"\"],[5,\"l\",3,\"l\",\"\"],[3,\"r\",6,\"t\",\"No\"],[6,\"b\",7,\"t\",\"\"]]}";
    names("Flowcharts and Pseudocode", {"Even or odd (decision)", "Larger of two numbers (decision)", "Sum of 1 to n (loop)"});

    /* ---------- Loops ---------- */
    I("Loops#1", "Enter a whole number (0 to 999999)", "4096", {"int count = 0;", "while (n != 0) {", "    n = n / 10;", "    count++;", "}", "printf(\"%d\", count);"}, {"LeetCode #1295 (Find Numbers with Even Number of Digits)", "LeetCode #258 (Add Digits)", "LeetCode #7 (Reverse Integer)"},
      [](const Ints& v) {
          int n0 = std::min(std::abs(v.empty() ? 4096 : v[0]), 999999), n = n0, c = 0; Steps s;
          s.push_back(mk(0, "count starts at 0.", {{"n", S(n)}, {"count", "0"}}));
          for (;;) {
              s.push_back(mk(1, "Is n != 0 ? " + S(n) + " != 0 is " + (n ? "true." : "false, so the loop ends. while tests BEFORE the body."), {{"n", S(n)}, {"count", S(c)}}));
              if (!n) break; n /= 10;
              s.push_back(mk(2, "n = n / 10 removes the last digit: n = " + S(n) + ".", {{"n", S(n)}, {"count", S(c)}}));
              ++c; s.push_back(mk(3, "count++ makes count = " + S(c) + ".", {{"n", S(n)}, {"count", S(c)}}));
          }
          s.push_back(mk(5, (n0 == 0 ? "n was 0, so the body never ran (count is 0). " : "") + std::string("Prints ") + S(c) + ".", {{"count", S(c)}})); return s; });
    I("Loops#2", "Enter a start number (-5 to 8)", "3", {"int n = INPUT;", "do {", "    printf(\"%d \", n);", "    n--;", "} while (n > 0);"}, {"LeetCode #1342 (Number of Steps to Reduce a Number to Zero)", "LeetCode #509 (Fibonacci Number)"},
      [](const Ints& v) {
          int n = std::clamp(v.empty() ? 3 : v[0], -5, 8); std::string out; Steps s; s.push_back(mk(0, "n = " + S(n) + ".", {{"n", S(n)}}));
          for (int g = 0; g < 14; ++g) {
              s.push_back(mk(1, g ? "Back to the top of the body." : "do-while enters the body FIRST, without checking anything.", {{"n", S(n)}, {"Screen", out}}));
              out += S(n) + " "; s.push_back(mk(2, "Prints " + S(n) + ".", {{"n", S(n)}, {"Screen", out}}));
              --n; s.push_back(mk(3, "n-- makes n = " + S(n) + ".", {{"n", S(n)}, {"Screen", out}}));
              bool go = n > 0; s.push_back(mk(4, "Check AFTER the body: n > 0 is " + std::string(go ? "true, repeat." : "false, stop. (Even if n started as 0 or less, the body ran once.)"), {{"n", S(n)}, {"Screen", out}}));
              if (!go) break;
          }
          return s; });
    names("Loops", {"for: sum of 1 to n", "while: count the digits", "do-while: countdown (runs at least once)"});

    /* ---------- Conditional statements ---------- */
    I("Conditional Statements#1", "Enter three integers a, b, c", "8, 15, 11", {"int max = a;", "if (b > max)", "    max = b;", "if (c > max)", "    max = c;", "printf(\"%d\", max);"}, {"LeetCode #414 (Third Maximum Number)", "LeetCode #1822 (Sign of the Product of an Array)"},
      [](const Ints& v) {
          int a = v.size() > 0 ? v[0] : 8, b = v.size() > 1 ? v[1] : 15, c = v.size() > 2 ? v[2] : 11, mx = a; Steps s;
          s.push_back(mk(0, "Assume a is the largest: max = " + S(a) + ".", {{"a", S(a)}, {"b", S(b)}, {"c", S(c)}, {"max", S(mx)}}));
          bool g = b > mx; s.push_back(mk(1, "Is b > max ? " + S(b) + " > " + S(mx) + " is " + (g ? "true." : "false."), {{"max", S(mx)}})); if (g) { mx = b; s.push_back(mk(2, "max = b = " + S(mx) + ".", {{"max", S(mx)}})); }
          g = c > mx; s.push_back(mk(3, "Is c > max ? " + S(c) + " > " + S(mx) + " is " + (g ? "true." : "false."), {{"max", S(mx)}})); if (g) { mx = c; s.push_back(mk(4, "max = c = " + S(mx) + ".", {{"max", S(mx)}})); }
          s.push_back(mk(5, "Prints " + S(mx) + ". Two separate ifs: both conditions are always tested.", {{"max", S(mx)}})); return s; });
    I("Conditional Statements#2", "Enter a, b and op (1 +, 2 -, 3 *, 4 /)", "12, 4, 3", {"int r;", "switch (op) {", "    case 1: r = a + b; break;", "    case 2: r = a - b; break;", "    case 3: r = a * b; break;", "    case 4: r = a / b; break;", "    default: printf(\"Invalid\");", "}", "printf(\"%d\", r);"}, {"LeetCode #1342 (Number of Steps to Reduce a Number to Zero)", "LeetCode #29 (Divide Two Integers)"},
      [](const Ints& v) {
          int a = v.size() > 0 ? v[0] : 12, b = v.size() > 1 && v[1] ? v[1] : 4, op = v.size() > 2 ? v[2] : 3; Steps s; Vars base = {{"a", S(a)}, {"b", S(b)}, {"op", S(op)}};
          s.push_back(mk(0, "r will hold the result.", base)); s.push_back(mk(1, "switch jumps straight to the case that equals op = " + S(op) + ".", base));
          if (op < 1 || op > 4) { s.push_back(mk(6, "No case matched, so default runs: prints Invalid.", base)); s.push_back(mk(7, "End of the switch.", base)); return s; }
          int r = op == 1 ? a + b : op == 2 ? a - b : op == 3 ? a * b : a / b; const char* sy[] = {"", "+", "-", "*", "/"};
          s.push_back(mk(1 + op, "case " + S(op) + ": r = " + S(a) + " " + sy[op] + " " + S(b) + " = " + S(r) + ". break leaves the switch. Without break, execution would fall into the next case.", {{"r", S(r)}}));
          s.push_back(mk(8, "Prints " + S(r) + ".", {{"r", S(r)}})); return s; });
    names("Conditional Statements", {"if-else-if: positive, negative or zero", "if: largest of three numbers", "switch: mini calculator"});

    /* ---------- Arrays ---------- */
    I("Arrays#1", "Enter the key first, then up to 8 numbers", "7, 4, 9, 7, 5", {"int pos = -1;", "for (int i = 0; i < n; i++) {", "    if (a[i] == key) {", "        pos = i;  break;", "    }", "}", "printf(\"%d\", pos);"}, {"LeetCode #1 (Two Sum)", "LeetCode #704 (Binary Search)", "LeetCode #35 (Search Insert Position)"},
      [](const Ints& v) {
          int key = v.empty() ? 7 : v[0]; Ints a(v.begin() + (v.empty() ? 0 : 1), v.end()); if (a.empty()) a = {4, 9, 7, 5}; if (a.size() > 8) a.resize(8); int pos = -1; Steps s;
          s.push_back(mkArr(0, "Looking for key = " + S(key) + ". pos = -1 means not found yet.", {{"key", S(key)}, {"pos", "-1"}}, a, -1));
          for (int i = 0; i < (int)a.size(); ++i) { bool e = a[i] == key;
              s.push_back(mkArr(2, "Is a[" + S(i) + "] == key ? " + S(a[i]) + " == " + S(key) + " is " + (e ? "true." : "false."), {{"i", S(i)}, {"key", S(key)}, {"pos", "-1"}}, a, i));
              if (e) { pos = i; s.push_back(mkArr(3, "pos = " + S(i) + " and break stops the loop at the FIRST match.", {{"pos", S(pos)}}, a, i)); break; } }
          s.push_back(mkArr(6, pos < 0 ? "The loop finished with no match. Prints -1." : "Prints the position " + S(pos) + ".", {{"pos", S(pos)}}, a, pos)); return s; });
    I("Arrays#2", "Enter up to 8 numbers", "1, 2, 3, 4, 5", {"int i = 0, j = n - 1;", "while (i < j) {", "    int t = a[i]; a[i] = a[j]; a[j] = t;", "    i++;  j--;", "}"}, {"LeetCode #344 (Reverse String)", "LeetCode #189 (Rotate Array)", "LeetCode #905 (Sort Array By Parity)"},
      [](Ints a) {
          if (a.empty()) a = {1, 2, 3, 4, 5}; if (a.size() > 8) a.resize(8); int i = 0, j = a.size() - 1; Steps s;
          s.push_back(mkArr(0, "i starts at the left end, j at the right end.", {{"i", "0"}, {"j", S(j)}}, a, 0));
          while (i < j) {
              s.push_back(mkArr(1, "Is i < j ? " + S(i) + " < " + S(j) + " is true.", {{"i", S(i)}, {"j", S(j)}}, a, i));
              std::swap(a[i], a[j]); s.push_back(mkArr(2, "Swap a[" + S(i) + "] and a[" + S(j) + "]. The array now reads as shown.", {{"i", S(i)}, {"j", S(j)}}, a, j));
              ++i; --j; s.push_back(mkArr(3, "i++ and j-- move the two ends toward the middle.", {{"i", S(i)}, {"j", S(j)}}, a, i));
          }
          s.push_back(mkArr(1, "Is i < j ? " + S(i) + " < " + S(j) + " is false. The array is reversed in place with no extra array.", {{"i", S(i)}, {"j", S(j)}}, a, -1)); return s; });
    names("Arrays", {"Find the largest element", "Linear search for a key", "Reverse an array in place"});

    /* ---------- Recursion ---------- */
    auto rec = [](bool pw, const Ints& v) {
        int x = 0, n; if (pw) { x = std::clamp(v.size() > 0 ? v[0] : 2, -9, 9); n = std::clamp(v.size() > 1 ? v[1] : 3, 0, 6); } else n = std::clamp(v.empty() ? 4 : v[0], 0, 8);
        Steps s; std::vector<std::string> st;
        auto name = [&](int k) { return pw ? "power(" + S(x) + ", " + S(k) + ")" : "sum(" + S(k) + ")"; };
        auto push = [&](Step z) { z.stack = st; s.push_back(z); };
        for (int k = n;; --k) { st.push_back(name(k)); push(mk(1, "Inside " + name(k) + ": is n == 0 ? " + (k == 0 ? "Yes, base case." : "No."), {{"n", S(k)}})); if (k == 0) break; push(mk(2, "Calls " + name(k - 1) + " and waits for its result.", {{"n", S(k)}})); }
        int res = pw ? 1 : 0; push(mk(1, "Base case returns " + S(res) + ".", {{"return", S(res)}})); st.pop_back();
        for (int k = 1; k <= n; ++k) { int prev = res; res = pw ? x * prev : k + prev;
            push(mk(2, name(k) + " returns " + (pw ? S(x) + " * " + S(prev) : S(k) + " + " + S(prev)) + " = " + S(res) + ".", {{"n", S(k)}, {"return", S(res)}})); st.pop_back(); }
        push(mk(3, "Stack is empty. Result = " + S(res) + ".", {{"result", S(res)}})); return s; };
    I("Recursion#1", "Enter n (0 to 8): sum of 1 to n", "4", {"int sum(int n) {", "    if (n == 0) return 0;", "    return n + sum(n - 1);", "}"}, {"LeetCode #509 (Fibonacci Number)", "LeetCode #206 (Reverse Linked List)"}, [rec](const Ints& v) { return rec(false, v); });
    I("Recursion#2", "Enter x and n (n from 0 to 6): x to the power n", "2, 3", {"int power(int x, int n) {", "    if (n == 0) return 1;", "    return x * power(x, n - 1);", "}"}, {"LeetCode #50 (Pow(x, n))", "LeetCode #231 (Power of Two)", "LeetCode #326 (Power of Three)"}, [rec](const Ints& v) { return rec(true, v); });
    names("Recursion", {"Factorial", "Sum of 1 to n", "Power: x to the n"});
}
