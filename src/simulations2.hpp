// Remaining Software Development 1 topics.
#pragma once
#include <climits>
#include <cstring>
#include <iomanip>
#include "simulations.hpp"

inline std::string F2(double d) { std::ostringstream o; o << std::fixed << std::setprecision(2) << d; return o.str(); }
inline std::string join(const Ints& a, const std::string& sep) { std::string r; for (size_t i = 0; i < a.size(); ++i) r += (i ? sep : "") + S(a[i]); return r; }
inline Ints codes(const std::string& w, bool nul) { Ints a; for (char c : w) a.push_back((unsigned char)c); if (nul) a.push_back(0); return a; }
inline std::string firstWord(const std::string& in, size_t mx, const std::string& d) {
    std::string w; for (char c : in) { if (isspace((unsigned char)c)) { if (!w.empty()) break; } else w += c; }
    if (w.empty()) w = d; return w.substr(0, mx);
}

inline void addMore(std::map<std::string, Sim>& m) {
    const std::vector<std::string> R = {
        "Learn-C.org (interactive tutorial)|https://www.learn-c.org/",
        "GeeksforGeeks: C Programming|https://www.geeksforgeeks.org/c-programming-language/",
        "cppreference: C language|https://en.cppreference.com/w/c"};
    auto I = [&](const char* n, const char* lab, const char* def, std::vector<std::string> code, std::vector<std::string> pr, std::function<Steps(const Ints&)> f) {
        m[n] = Sim{lab, def, code, R, pr, f, nullptr}; };
    auto T = [&](const char* n, const char* lab, const char* def, std::vector<std::string> code, std::vector<std::string> pr, std::function<Steps(const std::string&)> f) {
        Sim s{lab, def, code, R, pr, nullptr, f}; m[n] = s; };

    I("Algorithms", "Enter two positive integers (Euclid's GCD)", "48, 18",
      {"while (b != 0) {", "    int r = a % b;", "    a = b;", "    b = r;", "}", "printf(\"%d\", a);   // GCD"},
      {"LeetCode #1979 (Find Greatest Common Divisor of Array)", "LeetCode #1071 (Greatest Common Divisor of Strings)", "LeetCode #9 (Palindrome Number)"},
      [](const Ints& v) {
          int a = v.size() > 0 ? std::abs(v[0]) : 48, b = v.size() > 1 ? std::abs(v[1]) : 18; if (!a && !b) { a = 48; b = 18; }
          Steps s; int g = 0;
          while (g++ < 40) {
              s.push_back(mk(0, "Is b != 0 ? " + S(b) + " != 0 is " + (b ? "true." : "false, so stop."), {{"a", S(a)}, {"b", S(b)}}));
              if (!b) break;
              int r = a % b;
              s.push_back(mk(1, "r = " + S(a) + " % " + S(b) + " = " + S(r) + ".", {{"a", S(a)}, {"b", S(b)}, {"r", S(r)}}));
              s.push_back(mk(2, "a = b = " + S(b) + ".", {{"a", S(b)}, {"b", S(b)}, {"r", S(r)}}));
              s.push_back(mk(3, "b = r = " + S(r) + ".", {{"a", S(b)}, {"b", S(r)}, {"r", S(r)}}));
              a = b; b = r;
          }
          s.push_back(mk(5, "The algorithm ends. GCD = " + S(a) + ".", {{"a", S(a)}})); return s; });

    I("Variables, Constants and Data Types", "Enter an integer", "66",
      {"int x = INPUT;", "float f = x / 4.0;", "char c = (char)x;", "const int MAX = 100;", "MAX = 5;   // not allowed"},
      {"LeetCode #7 (Reverse Integer)", "LeetCode #258 (Add Digits)"},
      [](const Ints& v) {
          int x = v.empty() ? 66 : v[0]; std::string c = (x >= 32 && x <= 126) ? std::string(1, (char)x) : "(not printable)"; Steps s;
          s.push_back(mk(0, "int needs 4 bytes and stores whole numbers.", {{"x (int, 4 bytes)", S(x)}}));
          s.push_back(mk(1, "float needs 4 bytes and keeps the fraction.", {{"x (int)", S(x)}, {"f (float, 4 bytes)", F2(x / 4.0)}}));
          s.push_back(mk(2, "char needs 1 byte and holds the ASCII code " + S(x) + " as a character.", {{"x (int)", S(x)}, {"c (char, 1 byte)", c}}));
          s.push_back(mk(3, "const fixes the value for the whole program.", {{"MAX (const int)", "100"}}));
          s.push_back(mk(4, "Compile error: assignment of read-only variable 'MAX'.", {{"MAX (const int)", "100 (unchanged)"}})); return s; });

    I("Input/Output Functions", "Enter two integers", "7, 5",
      {"int a, b;", "scanf(\"%d %d\", &a, &b);", "printf(\"Sum = %d\\n\", a + b);", "printf(\"Avg = %.2f\\n\", (a + b) / 2.0);"},
      {"LeetCode #2235 (Add Two Integers)", "LeetCode #1342 (Number of Steps to Reduce a Number to Zero)"},
      [](const Ints& v) {
          int a = v.size() > 0 ? v[0] : 7, b = v.size() > 1 ? v[1] : 5; Steps s;
          s.push_back(mk(0, "Two int boxes are created. They hold garbage until we store something.", {{"a", "?"}, {"b", "?"}}));
          s.push_back(mk(1, "scanf reads your input and stores it at the addresses &a and &b.", {{"a", S(a)}, {"b", S(b)}}));
          s.push_back(mk(2, "printf %d prints an int.", {{"a", S(a)}, {"b", S(b)}, {"Screen", "Sum = " + S(a + b)}}));
          s.push_back(mk(3, "%.2f prints a float with 2 decimals. (a + b) / 2.0 avoids integer division.", {{"Screen", "Avg = " + F2((a + b) / 2.0)}})); return s; });

    I("Operators and Operator Precedence", "Enter a, b, c (b must not be 0)", "10, 4, 3",
      {"int r = a + b * c - a / b;", "printf(\"%d\", r);"},
      {"LeetCode #29 (Divide Two Integers)", "LeetCode #2413 (Smallest Even Multiple)"},
      [](const Ints& v) {
          int a = v.size() > 0 ? v[0] : 10, b = v.size() > 1 && v[1] ? v[1] : 4, c = v.size() > 2 ? v[2] : 3; Steps s;
          Vars base = {{"a", S(a)}, {"b", S(b)}, {"c", S(c)}};
          s.push_back(mk(0, "Rule: * and / run before + and -. Equal levels go left to right.", base));
          Vars v1 = base; v1.push_back({"b * c", S(b * c)}); s.push_back(mk(0, "First: b * c = " + S(b) + " * " + S(c) + " = " + S(b * c) + ".", v1));
          Vars v2 = v1; v2.push_back({"a / b", S(a / b)}); s.push_back(mk(0, "Next: a / b = " + S(a) + " / " + S(b) + " = " + S(a / b) + " (integer division drops the fraction).", v2));
          Vars v3 = v2; v3.push_back({"a + b*c", S(a + b * c)}); s.push_back(mk(0, "Then, left to right: a + " + S(b * c) + " = " + S(a + b * c) + ".", v3));
          int r = a + b * c - a / b; Vars v4 = v3; v4.push_back({"r", S(r)}); s.push_back(mk(0, S(a + b * c) + " - " + S(a / b) + " = " + S(r) + ".", v4));
          s.push_back(mk(1, "Prints " + S(r) + ".", {{"r", S(r)}})); return s; });

    I("Multidimensional Arrays (2D and 3D)", "Enter up to 6 numbers for a 2 x 3 matrix", "1, 2, 3, 4, 5, 6",
      {"int a[2][3] = { values };", "int sum = 0;", "for (int i = 0; i < 2; i++)", "    for (int j = 0; j < 3; j++)", "        sum += a[i][j];", "printf(\"%d\", sum);   // a 3D array adds one more loop"},
      {"LeetCode #1572 (Matrix Diagonal Sum)", "LeetCode #867 (Transpose Matrix)", "LeetCode #54 (Spiral Matrix)"},
      [](Ints a) {
          if (a.empty()) a = {1, 2, 3, 4, 5, 6}; a.resize(6); int sum = 0; Steps s;
          auto P = [&](int l, std::string n, Vars v, int hi) { Step x = mkArr(l, n, v, a, hi); x.cols = 3; s.push_back(x); };
          P(0, "A 2D array is stored row by row: 2 rows, 3 columns.", {}, -1); P(1, "sum starts at 0.", {{"sum", "0"}}, -1);
          for (int i = 0; i < 2; ++i) {
              P(2, "Outer loop: row i = " + S(i) + ".", {{"i", S(i)}, {"sum", S(sum)}}, -1);
              for (int j = 0; j < 3; ++j) { int k = i * 3 + j; sum += a[k];
                  P(4, "a[" + S(i) + "][" + S(j) + "] = " + S(a[k]) + ", so sum = " + S(sum) + ".", {{"i", S(i)}, {"j", S(j)}, {"sum", S(sum)}}, k); }
          }
          P(5, "Prints " + S(sum) + ".", {{"sum", S(sum)}}, -1); return s; });

    T("Strings and String Library Functions", "Enter a word (max 12 letters, no spaces)", "hello",
      {"char s[] = INPUT;", "int len = 0;", "while (s[len] != '\\0')", "    len++;", "printf(\"%d\", len);   // strlen(s)"},
      {"LeetCode #344 (Reverse String)", "LeetCode #709 (To Lower Case)", "LeetCode #13 (Roman to Integer)"},
      [](const std::string& in) {
          std::string w = firstWord(in, 12, "hello"); Ints a = codes(w, true); int L = w.size(); Steps s;
          auto P = [&](int l, std::string n, Vars v, int hi) { Step x = mkArr(l, n, v, a, hi); x.chr = true; x.nm = "s"; s.push_back(x); };
          P(0, "A string is a char array that ends with the hidden character '\\0'.", {}, L); P(1, "len starts at 0.", {{"len", "0"}}, 0);
          for (int i = 0; i < L; ++i) {
              P(2, std::string("s[") + S(i) + "] is '" + w[i] + "', not '\\0', so continue.", {{"len", S(i)}}, i);
              P(3, "len++ makes len = " + S(i + 1) + ".", {{"len", S(i + 1)}}, i);
          }
          P(2, "s[" + S(L) + "] is '\\0', so the loop stops.", {{"len", S(L)}}, L);
          P(4, "Length = " + S(L) + ". strlen() does exactly this. strcpy, strcat and strcmp also walk until '\\0'.", {{"len", S(L)}}, -1); return s; });

    I("Functions and Parameter Passing", "Enter two integers to swap", "3, 8",
      {"void swapVal(int x, int y) { int t = x; x = y; y = t; }", "void swapPtr(int *x, int *y) { int t = *x; *x = *y; *y = t; }", "int a = A, b = B;", "swapVal(a, b);     // copies", "swapPtr(&a, &b);   // addresses", "printf(\"%d %d\", a, b);"},
      {"LeetCode #1342 (Number of Steps to Reduce a Number to Zero)", "LeetCode #2235 (Add Two Integers)"},
      [](const Ints& v) {
          int a = v.size() > 0 ? v[0] : 3, b = v.size() > 1 ? v[1] : 8; Steps s;
          s.push_back(mk(2, "main() has a and b.", {{"a", S(a)}, {"b", S(b)}}));
          s.push_back(mk(3, "Call by value: x and y are copies of a and b.", {{"main: a", S(a)}, {"main: b", S(b)}, {"x (copy)", S(a)}, {"y (copy)", S(b)}}));
          s.push_back(mk(0, "swapVal swaps only its copies. main's a and b do not change.", {{"main: a", S(a)}, {"main: b", S(b)}, {"x (copy)", S(b)}, {"y (copy)", S(a)}}));
          s.push_back(mk(4, "Call by reference: x and y hold the addresses &a and &b.", {{"main: a", S(a)}, {"main: b", S(b)}, {"x", "&a"}, {"y", "&b"}}));
          s.push_back(mk(1, "*x and *y reach the real variables, so the swap sticks.", {{"main: a", S(b)}, {"main: b", S(a)}, {"*x", S(b)}, {"*y", S(a)}}));
          s.push_back(mk(5, "Prints " + S(b) + " " + S(a) + ".", {{"a", S(b)}, {"b", S(a)}})); return s; });

    I("Pointers and Arrays", "Enter up to 8 numbers", "5, 10, 15, 20",
      {"int *p = a;", "int sum = 0;", "for (int i = 0; i < n; i++) {", "    sum += *p;", "    p++;", "}"},
      {"LeetCode #1480 (Running Sum of 1d Array)", "LeetCode #283 (Move Zeroes)", "LeetCode #27 (Remove Element)"},
      [](Ints a) {
          if (a.empty()) a = {5, 10, 15, 20}; if (a.size() > 8) a.resize(8); int n = a.size(), sum = 0; Steps s;
          s.push_back(mkArr(0, "The array name a is the address of a[0], so p points at a[0].", {{"p", "&a[0]"}}, a, 0));
          s.push_back(mkArr(1, "sum starts at 0.", {{"sum", "0"}}, a, 0));
          for (int i = 0; i < n; ++i) { sum += a[i];
              s.push_back(mkArr(3, "*p reads a[" + S(i) + "] = " + S(a[i]) + ". sum = " + S(sum) + ".", {{"p", "&a[" + S(i) + "]"}, {"*p", S(a[i])}, {"sum", S(sum)}}, a, i));
              s.push_back(mkArr(4, i + 1 < n ? "p++ moves to the next int, a[" + S(i + 1) + "]." : "p++ moves one past the last element.", {{"p", i + 1 < n ? "&a[" + S(i + 1) + "]" : "end"}, {"sum", S(sum)}}, a, i + 1 < n ? i + 1 : -1)); }
          return s; });

    T("Pointers and Strings", "Enter a word (max 10 letters)", "education",
      {"char *p = s;", "int vowels = 0;", "while (*p != '\\0') {", "    if (*p=='a'||*p=='e'||*p=='i'||*p=='o'||*p=='u') vowels++;", "    p++;", "}"},
      {"LeetCode #345 (Reverse Vowels of a String)", "LeetCode #125 (Valid Palindrome)", "LeetCode #1768 (Merge Strings Alternately)"},
      [](const std::string& in) {
          std::string w = firstWord(in, 10, "education"); Ints a = codes(w, true); int L = w.size(), c = 0; Steps s;
          auto P = [&](int l, std::string n, Vars v, int hi) { Step x = mkArr(l, n, v, a, hi); x.chr = true; x.nm = "s"; s.push_back(x); };
          P(0, "p points to the first character of s.", {{"p", "&s[0]"}}, 0); P(1, "vowels starts at 0.", {{"vowels", "0"}}, 0);
          for (int i = 0; i < L; ++i) { char ch = tolower(w[i]); bool v = strchr("aeiou", ch) != nullptr; if (v) ++c;
              P(3, std::string("*p is '") + w[i] + "': " + (v ? "a vowel, vowels = " + S(c) + "." : "not a vowel."), {{"p", "&s[" + S(i) + "]"}, {"*p", std::string(1, w[i])}, {"vowels", S(c)}}, i);
              P(4, "p++ moves to the next character.", {{"p", "&s[" + S(i + 1) + "]"}, {"vowels", S(c)}}, i + 1); }
          P(2, "*p is '\\0', so the loop ends. Vowels found: " + S(c) + ".", {{"vowels", S(c)}}, L); return s; });

    I("Dynamic Memory Allocation", "Enter n (1 to 6), the number of ints", "3",
      {"int *p = (int*)malloc(n * sizeof(int));", "for (int i = 0; i < n; i++) p[i] = i * i;", "p = (int*)realloc(p, 2 * n * sizeof(int));", "int *z = (int*)calloc(n, sizeof(int));", "free(p);  free(z);   // always release heap memory"},
      {"LeetCode #1929 (Concatenation of Array)", "LeetCode #88 (Merge Sorted Array)", "LeetCode #1470 (Shuffle the Array)"},
      [](const Ints& v) {
          int n = std::clamp(v.empty() ? 3 : v[0], 1, 6); Steps s; Ints a(n, INT_MIN);
          auto P = [&](int l, std::string t, Vars vs, int hi, const char* nm) { Step x = mkArr(l, t, vs, a, hi); x.nm = nm; s.push_back(x); };
          P(0, "malloc reserves " + S(n * 4) + " bytes on the heap. The contents are garbage (?).", {{"p", "heap block"}}, -1, "p");
          for (int i = 0; i < n; ++i) { a[i] = i * i; P(1, "p[" + S(i) + "] = " + S(i) + " * " + S(i) + " = " + S(a[i]) + ".", {{"i", S(i)}}, i, "p"); }
          a.resize(2 * n, INT_MIN); P(2, "realloc grows the block to " + S(2 * n) + " ints. Old values are kept; new cells are garbage.", {{"p", "bigger block"}}, -1, "p");
          Ints z(n, 0); Step x = mkArr(3, "calloc gives n cells that are all set to 0. (Shown as z.)", {{"z", "heap block"}}, z, -1); x.nm = "z"; s.push_back(x);
          a.clear(); P(4, "free() returns the memory. p and z now dangle; set them to NULL. Forgetting free causes a memory leak.", {{"p", "dangling"}, {"z", "dangling"}}, -1, "p"); return s; });

    I("Structures", "Enter roll number and marks", "101, 82",
      {"struct Student { int roll; float marks; char grade; };", "struct Student s1 = {ROLL, MARKS};", "s1.grade = (s1.marks >= 90) ? 'A' : (s1.marks >= 75) ? 'B' : 'C';", "printf(\"%d %c\", s1.roll, s1.grade);"},
      {"LeetCode #1603 (Design Parking System)", "LeetCode #1672 (Richest Customer Wealth)"},
      [](const Ints& v) {
          int r = v.size() > 0 ? v[0] : 101, mk_ = v.size() > 1 ? v[1] : 82; char g = mk_ >= 90 ? 'A' : mk_ >= 75 ? 'B' : 'C'; Steps s;
          s.push_back(mk(0, "A struct groups different types into one new type. This only defines the type; no memory is used yet. Size is 12 bytes with padding.", {{"Student", "int + float + char"}}));
          s.push_back(mk(1, "s1 is created. Members are set in order.", {{"s1.roll", S(r)}, {"s1.marks", S(mk_)}, {"s1.grade", "?"}}));
          s.push_back(mk(2, "The dot operator reaches a member. Marks " + S(mk_) + " give grade " + g + ".", {{"s1.roll", S(r)}, {"s1.marks", S(mk_)}, {"s1.grade", std::string(1, g)}}));
          s.push_back(mk(3, "Prints " + S(r) + " " + g + ".", {{"Screen", S(r) + " " + g}})); return s; });

    I("Unions", "Enter an integer (65 is 'A')", "65",
      {"union Data { int i; float f; char c; };", "union Data u;", "u.i = N;", "printf(\"%c\", u.c);", "u.f = 3.14;", "printf(\"%d\", u.i);   // garbage"},
      {"No LeetCode match. Write a program that prints the 4 bytes of an int using a union."},
      [](const Ints& v) {
          int n = v.empty() ? 65 : v[0]; Steps s; unsigned char b[4];
          auto P = [&](int l, std::string t, Vars vs, int hi) { Step x; x.line = l; x.note = t; x.vars = vs; x.hi = hi; x.nm = "byte"; for (int i = 0; i < 4; ++i) x.arr.push_back(b[i]); s.push_back(x); };
          std::memset(b, 0, 4); P(0, "All members share the same 4 bytes. The union's size is its largest member.", {{"sizeof(u)", "4"}}, -1);
          std::memcpy(b, &n, 4); P(2, "u.i = " + S(n) + " writes these 4 bytes (lowest byte first).", {{"u.i", S(n)}}, 0);
          P(3, "u.c reads only the first byte: " + S(b[0]) + ", which is the character '" + (b[0] >= 32 && b[0] < 127 ? std::string(1, (char)b[0]) : "?") + "'.", {{"u.c", S(b[0])}}, 0);
          float f = 3.14f; std::memcpy(b, &f, 4); int back; std::memcpy(&back, b, 4);
          P(4, "u.f = 3.14 overwrites the same bytes with the float's bit pattern.", {{"u.f", "3.14"}}, -1);
          P(5, "Reading u.i now reinterprets those bytes as an int: " + S(back) + ". Only the last-written member is valid.", {{"u.i", S(back)}}, -1); return s; });

    I("Arrays of Structures", "Enter up to 5 marks (one per student)", "72, 91, 85, 60",
      {"struct Student { int roll; float marks; };", "struct Student s[N];   // roll = index + 1", "int best = 0;", "for (int i = 1; i < n; i++)", "    if (s[i].marks > s[best].marks) best = i;", "printf(\"%d\", s[best].roll);"},
      {"LeetCode #506 (Relative Ranks)", "LeetCode #1331 (Rank Transform of an Array)", "LeetCode #1603 (Design Parking System)"},
      [](Ints a) {
          if (a.empty()) a = {72, 91, 85, 60}; if (a.size() > 5) a.resize(5); int n = a.size(), best = 0; Steps s;
          auto P = [&](int l, std::string t, Vars v, int hi) { Step x = mkArr(l, t, v, a, hi); x.nm = "s.marks"; s.push_back(x); };
          P(1, "An array of structs: each s[i] holds a roll number and marks. Cells show s[i].marks.", {}, -1); P(2, "best = 0, so assume student 1 is the topper.", {{"best", "0"}}, 0);
          for (int i = 1; i < n; ++i) { bool g = a[i] > a[best];
              P(4, "Is s[" + S(i) + "].marks > s[best].marks ? " + S(a[i]) + " > " + S(a[best]) + " is " + (g ? "true, so best = " + S(i) + "." : "false."), {{"i", S(i)}, {"best", S(g ? i : best)}}, i); if (g) best = i; }
          P(5, "Prints roll " + S(best + 1) + " with marks " + S(a[best]) + ".", {{"best", S(best)}}, best); return s; });

    I("Pointers to Structures", "Enter roll number and marks", "7, 68",
      {"struct Student s = {ROLL, MARKS};", "struct Student *p = &s;", "p->marks += 5;   // grace marks", "printf(\"%d\", (*p).roll);   // same as p->roll", "printf(\"%d\", p->marks);"},
      {"LeetCode #206 (Reverse Linked List)", "LeetCode #141 (Linked List Cycle)", "LeetCode #876 (Middle of the Linked List)"},
      [](const Ints& v) {
          int r = v.size() > 0 ? v[0] : 7, mks = v.size() > 1 ? v[1] : 68; Steps s;
          s.push_back(mk(0, "s is an ordinary struct variable.", {{"s.roll", S(r)}, {"s.marks", S(mks)}}));
          s.push_back(mk(1, "p stores the address of s.", {{"p", "&s"}, {"s.roll", S(r)}, {"s.marks", S(mks)}}));
          s.push_back(mk(2, "p->marks means (*p).marks: go to the struct p points to, then take marks.", {{"p->marks", S(mks + 5)}, {"s.marks", S(mks + 5)}}));
          s.push_back(mk(3, "p->roll and (*p).roll are the same thing: " + S(r) + ".", {{"p->roll", S(r)}}));
          s.push_back(mk(4, "Prints " + S(mks + 5) + ". The change shows in s because p points at s itself, not a copy.", {{"Screen", S(mks + 5)}})); return s; });

    I("Enumerations (enum)", "Enter a day number (0 = MON ... 6 = SUN)", "5",
      {"enum Day { MON, TUE, WED, THU, FRI, SAT, SUN };", "enum Day d = DAYNO;", "switch (d) {", "    case SAT: case SUN: printf(\"Weekend\"); break;", "    default: printf(\"Weekday\");", "}"},
      {"LeetCode #1185 (Day of the Week)", "LeetCode #1507 (Reformat Date)"},
      [](const Ints& v) {
          int d = std::clamp(v.empty() ? 5 : v[0], 0, 6); const char* nm[7] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"}; Steps s; Ints a = {0, 1, 2, 3, 4, 5, 6};
          Step x = mkArr(0, "enum gives names to integers. They start at 0 and go up by 1.", {{"MON", "0"}, {"SUN", "6"}}, a, -1); x.nm = "day"; s.push_back(x);
          x = mkArr(1, std::string("d = ") + nm[d] + ", which is stored as the integer " + S(d) + ".", {{"d", std::string(nm[d]) + " (" + S(d) + ")"}}, a, d); x.nm = "day"; s.push_back(x);
          x = mkArr(2, "switch compares d with each case.", {{"d", nm[d]}}, a, d); x.nm = "day"; s.push_back(x);
          bool we = d >= 5; x = mkArr(we ? 3 : 4, we ? "SAT or SUN matched: prints Weekend." : "No case matched, so default runs: prints Weekday.", {{"d", nm[d]}}, a, d); x.nm = "day"; s.push_back(x); return s; });

    I("File Handling", "Enter up to 6 numbers to save in a file", "10, 20, 30",
      {"FILE *fp = fopen(\"data.txt\", \"w\");", "for (int i = 0; i < n; i++) fprintf(fp, \"%d\\n\", a[i]);", "fclose(fp);", "fp = fopen(\"data.txt\", \"r\");", "while (fscanf(fp, \"%d\", &x) == 1) sum += x;", "fclose(fp);  printf(\"%d\", sum);"},
      {"No LeetCode match. Write a program that reads numbers from a file and prints their average."},
      [](Ints a) {
          if (a.empty()) a = {10, 20, 30}; if (a.size() > 6) a.resize(6); Steps s; int n = a.size(), sum = 0; Ints w;
          s.push_back(mk(0, "Mode \"w\" creates (or empties) data.txt for writing.", {{"data.txt", "(empty)"}}));
          for (int i = 0; i < n; ++i) { w.push_back(a[i]); s.push_back(mk(1, "fprintf writes " + S(a[i]) + " and a newline.", {{"data.txt", join(w, " ↵ ")}})); }
          s.push_back(mk(2, "fclose saves and releases the file.", {{"data.txt", join(w, " ↵ ")}}));
          s.push_back(mk(3, "Mode \"r\" opens it for reading from the start.", {{"data.txt", join(w, " ↵ ")}, {"cursor", "start"}}));
          for (int i = 0; i < n; ++i) { sum += a[i]; s.push_back(mk(4, "fscanf reads " + S(a[i]) + " and returns 1. sum = " + S(sum) + ".", {{"x", S(a[i])}, {"sum", S(sum)}})); }
          s.push_back(mk(4, "fscanf hits the end of file and does not return 1, so the loop stops.", {{"sum", S(sum)}}));
          s.push_back(mk(5, "File closed. Prints " + S(sum) + ".", {{"sum", S(sum)}})); return s; });

    T("Command-Line Arguments (argc, argv)", "Enter arguments (words separated by spaces)", "hello world 42",
      {"int main(int argc, char *argv[]) {", "    printf(\"%d\", argc);", "    for (int i = 0; i < argc; i++)", "        printf(\"%s\", argv[i]);", "}"},
      {"No LeetCode match. Write a program that adds all numbers given on the command line (use atoi)."},
      [](const std::string& in) {
          std::istringstream ss(in); std::vector<std::string> av = {"./prog"}; std::string w;
          while (ss >> w && av.size() < 7) av.push_back(w.substr(0, 12));
          int argc = av.size(); std::string cmd; for (auto& x : av) cmd += (cmd.empty() ? "" : " ") + x; Steps s;
          s.push_back(mk(0, "You run: " + cmd + ". The OS passes the words to main.", {{"Command", cmd}}));
          s.push_back(mk(1, "argc counts every word, including the program name.", {{"argc", S(argc)}}));
          for (int i = 0; i < argc; ++i) s.push_back(mk(3, "argv[" + S(i) + "] is the string \"" + av[i] + "\"." + (i == 0 ? " argv[0] is always the program name." : ""), {{"i", S(i)}, {"argv[i]", av[i]}, {"argc", S(argc)}}));
          s.push_back(mk(4, "argv[" + S(argc) + "] is NULL. All arguments are strings; use atoi() to turn \"42\" into a number.", {{"argv[" + S(argc) + "]", "NULL"}})); return s; });
}
