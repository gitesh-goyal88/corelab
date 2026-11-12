// Software Development 2 (Semester 2) — Modules 1 & 2: OOP Introduction + C++ OOP Concepts
#pragma once
#include <algorithm>
#include <cmath>
#include <sstream>
#include "sdf2_2.hpp"

inline void addSDF2_2(std::map<std::string, Sim>& m);

inline void addSDF2_1(std::map<std::string, Sim>& m) {
    addSDF2_2(m);

    const std::vector<std::string> R = {
        "cppreference: C++ language|https://en.cppreference.com/w/cpp",
        "GeeksforGeeks: C++ OOP|https://www.geeksforgeeks.org/object-oriented-programming-in-cpp/",
        "LearnCpp.com|https://www.learncpp.com/"
    };

    auto I = [&](const std::string& key, const char* lab, const char* def,
                 std::vector<std::string> code, std::vector<std::string> pr,
                 std::function<Steps(const Ints&)> f) {
        m[key] = Sim{lab, def, code, R, pr, f, nullptr, "", {}};
    };
    auto T = [&](const std::string& key, const char* lab, const char* def,
                 std::vector<std::string> code, std::vector<std::string> pr,
                 std::function<Steps(const std::string&)> f) {
        m[key] = Sim{lab, def, code, R, pr, nullptr, f, "", {}};
    };
    auto names = [&](const char* t, std::vector<std::string> n) { programNames()[t] = n; };

    /* ===== Module 1: Introduction to OOP ===== */

    // 1. Procedural vs OOP
    names("Procedural Programming vs Object-Oriented Programming",
          {"Procedural style (C)", "OOP style (C++)"});
    I("Procedural Programming vs Object-Oriented Programming", "Enter a bank balance and deposit amount", "1000, 500",
      {"// Procedural: data and functions are separate",
       "float balance = 1000;",
       "void deposit(float* b, float amt) { *b += amt; }",
       "deposit(&balance, 500);",
       "printf(\"Balance: %.2f\", balance);"},
      {"Explain the difference between a struct-based C program and a class-based C++ program.",
       "Name two advantages of OOP over procedural programming."},
      [](const Ints& v) {
          float bal = v.size() > 0 ? v[0] : 1000;
          float dep = v.size() > 1 ? v[1] : 500;
          Steps s;
          s.push_back(mk(0, "PROCEDURAL: data (balance) and logic (deposit) live separately. Any code can access and modify the balance directly — there is no protection.", {{"balance", std::to_string((int)bal)}}));
          s.push_back(mk(1, "float balance = " + S((int)bal) + "; — a plain global variable. Nothing stops another function from corrupting it.", {{"balance", S((int)bal)}}));
          s.push_back(mk(2, "deposit() takes a pointer to the balance and adds to it. The function is not tied to the data.", {{"balance", S((int)bal)}, {"deposit", S((int)dep)}}));
          s.push_back(mk(3, "Call: deposit(&balance, " + S((int)dep) + "). The address is passed explicitly — the function and data are loosely coupled.", {{"balance", S((int)(bal+dep))}}));
          s.push_back(mk(4, "Result: balance = " + S((int)(bal+dep)) + ". Works, but any part of the program can bypass deposit() and change balance directly.", {{"balance", S((int)(bal+dep))}}));
          return s;
      });
    I("Procedural Programming vs Object-Oriented Programming#1", "Enter a bank balance and deposit amount", "1000, 500",
      {"// OOP: data and methods are bundled inside a class",
       "class BankAccount {",
       "    float balance;",
       "public:",
       "    BankAccount(float b) : balance(b) {}",
       "    void deposit(float amt) { balance += amt; }",
       "    float getBalance() { return balance; }",
       "};",
       "BankAccount acc(1000);",
       "acc.deposit(500);",
       "cout << acc.getBalance();"},
      {"Compare the two programs: which is safer and why?",
       "What does 'encapsulation' mean in the OOP version?"},
      [](const Ints& v) {
          int bal = v.size() > 0 ? v[0] : 1000;
          int dep = v.size() > 1 ? v[1] : 500;
          Steps s;
          s.push_back(mk(0, "OOP: class BankAccount bundles data (balance) and behaviour (deposit) together — this is called ENCAPSULATION.", {}));
          s.push_back(mk(1, "The class definition is a blueprint. It describes what a BankAccount looks like and what it can do.", {}));
          s.push_back(mk(2, "balance is private — only methods inside the class can touch it. External code cannot accidentally corrupt it.", {{"balance", "private"}}));
          s.push_back(mk(3, "public: methods form the interface — the only way the outside world can interact with the data.", {}));
          s.push_back(mk(4, "Constructor BankAccount(float b) : balance(b) {} initialises the object when it is created.", {{"balance", S(bal)}}));
          s.push_back(mk(7, "acc.deposit(" + S(dep) + ") — the object itself handles the update. No raw pointers, no external access.", {{"balance", S(bal + dep)}}));
          s.push_back(mk(8, "acc.getBalance() = " + S(bal + dep) + ". The balance is exposed only through a controlled interface.", {{"balance", S(bal + dep)}}));
          s.push_back(mk(9, "OOP advantages: encapsulation (data safety), reusability (create many BankAccount objects), and maintainability (change internals without affecting callers).", {}));
          return s;
      });

    // 2. Characteristics of OOP Languages
    T("Characteristics of Object-Oriented Languages", "Type a characteristic to explore (encapsulation / abstraction / inheritance / polymorphism)", "encapsulation",
      {"// The 4 pillars of OOP",
       "// 1. Encapsulation  — bundle data + methods; hide internals",
       "// 2. Abstraction    — expose only what matters; hide complexity",
       "// 3. Inheritance    — derive a class from another; reuse code",
       "// 4. Polymorphism   — same call, different behaviour at runtime"},
      {"Give one real-life example of each of the four OOP characteristics.",
       "Which characteristic lets you call speak() on a Dog and a Cat object the same way?"},
      [](const std::string& in) {
          std::string w = in;
          for (char& c : w) c = (char)tolower((unsigned char)c);
          Steps s;
          auto add = [&](int l, const std::string& n, Vars v) { s.push_back(mk(l, n, v)); };

          if (w.find("encap") != std::string::npos || w.empty()) {
              add(0, "ENCAPSULATION: wrapping data and methods together inside a class. The private keyword hides implementation details from outside code.", {{"keyword", "private / public / protected"}});
              add(0, "Example: a BankAccount class keeps balance private and exposes only deposit() and withdraw(). Outside code cannot set balance directly.", {{"access", "controlled via methods"}});
          }
          if (w.find("abstr") != std::string::npos) {
              add(1, "ABSTRACTION: showing only essential features and hiding unnecessary details. A class interface is an abstraction of the implementation.", {{"keyword", "interface / abstract class"}});
              add(1, "Example: you call sort() on a vector without knowing the algorithm. The complexity is hidden behind a simple interface.", {{"detail hidden", "sorting algorithm"}});
          }
          if (w.find("inher") != std::string::npos) {
              add(2, "INHERITANCE: a derived (child) class acquires members of a base (parent) class. Avoids duplicating code.", {{"keyword", "class Dog : public Animal"}});
              add(2, "Example: Animal has eat() and sleep(). Dog inherits those and adds bark(). The parent's code is reused.", {{"reused", "eat(), sleep()"}});
          }
          if (w.find("poly") != std::string::npos) {
              add(3, "POLYMORPHISM: 'many forms'. The same function call behaves differently depending on the actual object type at runtime.", {{"keyword", "virtual, override"}});
              add(3, "Example: Animal* a = new Dog(); a->speak(); calls Dog::speak(), not Animal::speak(), even through a base pointer — this is DYNAMIC DISPATCH.", {{"resolved at", "runtime"}});
          }
          if (s.empty()) {
              add(0, "Type one of: encapsulation, abstraction, inheritance, polymorphism to explore that pillar.", {});
              add(1, "All four work together to make programs modular, reusable and maintainable — the core promise of OOP.", {});
              add(2, "Encapsulation protects data. Abstraction hides complexity. Inheritance reuses code. Polymorphism enables flexibility.", {});
              add(3, "Together they let you model real-world entities as objects and build large software systems safely.", {});
          }
          return s;
      });

    // 3. Separation of Behaviour and Implementation
    I("Separation of Behaviour and Implementation", "Enter a circle radius", "5",
      {"// BEHAVIOUR declared in the class (header)",
       "class Circle {",
       "    double radius;",
       "public:",
       "    Circle(double r);       // declared here",
       "    double area() const;    // declared here",
       "};",
       "// IMPLEMENTATION defined separately (cpp file)",
       "Circle::Circle(double r) : radius(r) {}",
       "double Circle::area() const { return 3.14159 * radius * radius; }"},
      {"Why is it good practice to put declarations in .h and definitions in .cpp?",
       "What is the :: operator called and what does it do here?"},
      [](const Ints& v) {
          double r = v.empty() ? 5 : std::abs(v[0]);
          if (r == 0) r = 5;
          double area = 3.14159 * r * r;
          Steps s;
          s.push_back(mk(0, "BEHAVIOUR (what the class can do) is declared in the class body — the interface a user sees.", {}));
          s.push_back(mk(1, "Circle { double radius; ... } — the class declares its data member and its public interface.", {{"radius", S((int)r)}}));
          s.push_back(mk(3, "Circle(double r); and area() const; are DECLARATIONS — they promise the methods exist without saying how.", {}));
          s.push_back(mk(7, "IMPLEMENTATION (how it works) is written separately using the scope resolution operator ::.", {}));
          s.push_back(mk(8, "Circle::Circle(double r) : radius(r) {} — sets radius = " + S((int)r) + " using an initialiser list. The body is separate from the declaration.", {{"radius", S((int)r)}}));
          s.push_back(mk(9, "Circle::area() computes π × r² = 3.14159 × " + S((int)r) + "² = " + std::to_string(area).substr(0, 7) + ". The formula is hidden from callers.", {{"area", std::to_string(area).substr(0, 7)}}));
          s.push_back(mk(9, "Benefit: callers only see the interface (.h file). The implementation (.cpp) can be changed, optimised, or even kept as a compiled library — callers recompile without change.", {}));
          return s;
      });

    /* ===== Module 2: OOP Concepts Using C++ ===== */

    // 4. Classes and Objects
    I("Classes and Objects", "Enter length and width of a Rectangle", "8, 5",
      {"class Rectangle {",
       "    int length, width;",
       "public:",
       "    Rectangle(int l, int w) : length(l), width(w) {}",
       "    int area()      { return length * width; }",
       "    int perimeter() { return 2 * (length + width); }",
       "};",
       "Rectangle r(8, 5);",
       "cout << r.area();       // 40",
       "cout << r.perimeter();  // 26"},
      {"What is the difference between a class and an object?",
       "LeetCode #1603 (Design Parking System)"},
      [](const Ints& v) {
          int l = v.size() > 0 ? std::max(1, v[0]) : 8;
          int w = v.size() > 1 ? std::max(1, v[1]) : 5;
          Steps s;
          s.push_back(mk(0, "CLASS: a user-defined blueprint/type. It defines members (data) and methods (functions). No memory is allocated for the data yet.", {}));
          s.push_back(mk(1, "length and width are DATA MEMBERS — private by default, so outside code cannot read or change them directly.", {{"length", "?"}, {"width", "?"}}));
          s.push_back(mk(2, "public: members are the class's interface — accessible from anywhere.", {}));
          s.push_back(mk(3, "Constructor Rectangle(int l, int w) is called when an object is created. It initialises length = " + S(l) + " and width = " + S(w) + ".", {{"length", S(l)}, {"width", S(w)}}));
          s.push_back(mk(7, "OBJECT r is an instance of Rectangle. Memory is now allocated for r.length and r.width.", {{"r.length", S(l)}, {"r.width", S(w)}}));
          s.push_back(mk(8, "r.area() = length × width = " + S(l) + " × " + S(w) + " = " + S(l * w) + ". The dot operator (.) accesses the method on object r.", {{"area", S(l * w)}}));
          s.push_back(mk(9, "r.perimeter() = 2 × (" + S(l) + " + " + S(w) + ") = " + S(2 * (l + w)) + ".", {{"perimeter", S(2 * (l + w))}}));
          s.push_back(mk(9, "You can create many Rectangle objects from one class, each with its own length/width. That is the power of objects.", {}));
          return s;
      });

    // 5. Internal Representation of Objects
    I("Internal Representation of Objects", "Enter two integers for a Point object", "3, 7",
      {"class Point {",
       "public:",
       "    int x, y;",
       "    Point(int a, int b) : x(a), y(b) {}",
       "};",
       "Point p(3, 7);",
       "// In memory: [x=3][y=7] (two consecutive ints)",
       "// &p == &p.x  (object address = first member's address)",
       "// sizeof(Point) == 2 * sizeof(int) = 8 bytes"},
      {"What does sizeof(Point) return and why?",
       "Why does &p == &p.x for the first data member?"},
      [](const Ints& v) {
          int x = v.size() > 0 ? v[0] : 3;
          int y = v.size() > 1 ? v[1] : 7;
          Steps s;
          s.push_back(mk(0, "A class with only data members is stored in memory like a struct: each member occupies consecutive bytes.", {}));
          s.push_back(mk(1, "int x, y; — two int members. On a 64-bit system int is typically 4 bytes.", {{"sizeof(int)", "4 bytes"}}));
          s.push_back(mk(5, "Point p(" + S(x) + ", " + S(y) + "); — the constructor runs. Memory is allocated on the stack for the object.", {{"p.x", S(x)}, {"p.y", S(y)}}));
          s.push_back(mk(6, "Memory layout: [p.x = " + S(x) + "][p.y = " + S(y) + "] — the two ints stored side by side.", {{"p.x @offset 0", S(x)}, {"p.y @offset 4", S(y)}}));
          s.push_back(mk(7, "&p == &p.x because the first member starts at the beginning of the object. The address of the object IS the address of its first field.", {{"&p", "0x1000 (example)"}, {"&p.x", "0x1000 (same)"},  {"&p.y", "0x1004 (+4 bytes)"}}));
          s.push_back(mk(8, "sizeof(Point) = 8 bytes (2 × 4). Member functions (methods) are NOT stored inside each object — they live in code memory and are shared among all instances.", {{"sizeof(Point)", "8 bytes"}, {"methods stored", "once in code segment"}}));
          return s;
      });

    // 6. Constructors
    names("Constructors", {"Default constructor", "Parameterised constructor", "Copy constructor"});
    I("Constructors", "No input needed", "",
      {"class Box {",
       "    int side;",
       "public:",
       "    Box()         : side(1) {}       // default",
       "    Box(int s)    : side(s) {}       // parameterised",
       "    Box(const Box& b) : side(b.side) {} // copy",
       "    int volume()  { return side*side*side; }",
       "};"},
      {"When is the copy constructor called automatically?",
       "What is a constructor initialiser list and why prefer it over assignment in the body?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "A CONSTRUCTOR is a special member function with the same name as the class. It runs automatically when an object is created to set initial state.", {}));
          s.push_back(mk(3, "DEFAULT constructor Box() : side(1) {} — takes no arguments. Called when you write 'Box b;'. Sets side = 1.", {{"side", "1"}, {"volume", "1"}}));
          s.push_back(mk(4, "PARAMETERISED constructor Box(int s) — called when you write 'Box b(5);'. Sets side = 5.", {{"side", "5"}, {"volume", "125"}}));
          s.push_back(mk(5, "COPY constructor Box(const Box& b) — called when you write 'Box c = b;'. Copies b.side into c.side.", {{"side", "copied from source"}}));
          s.push_back(mk(5, "The : side(val) syntax is an INITIALISER LIST. It initialises the member before the constructor body runs. Preferred because it is more efficient (direct init, not assign-after-default).", {}));
          s.push_back(mk(6, "volume() = side³. For Box b(4): volume = 64. The constructor guarantees the object is always in a valid state.", {{"side", "4"}, {"volume", "64"}}));
          return s;
      });
    I("Constructors#1", "Enter a side length for Box", "4",
      {"Box b;        // default: side=1",
       "Box b2(4);    // parameterised: side=4",
       "Box b3 = b2;  // copy: side copied from b2",
       "cout << b.volume();   // 1",
       "cout << b2.volume();  // 64",
       "cout << b3.volume();  // 64"},
      {"What volume do b, b2 and b3 have after the three constructions?"},
      [](const Ints& v) {
          int s2 = v.empty() ? 4 : std::max(1, std::min(v[0], 10));
          Steps s;
          s.push_back(mk(0, "Box b; — default constructor runs. side = 1. volume = 1.", {{"b.side", "1"}, {"b.volume()", "1"}}));
          s.push_back(mk(1, "Box b2(" + S(s2) + "); — parameterised constructor. side = " + S(s2) + ". volume = " + S(s2*s2*s2) + ".", {{"b2.side", S(s2)}, {"b2.volume()", S(s2*s2*s2)}}));
          s.push_back(mk(2, "Box b3 = b2; — copy constructor. b3.side = b2.side = " + S(s2) + ". A new object with the same state.", {{"b3.side", S(s2)}, {"b3.volume()", S(s2*s2*s2)}}));
          s.push_back(mk(3, "b.volume() = 1.", {{"output", "1"}}));
          s.push_back(mk(4, "b2.volume() = " + S(s2*s2*s2) + ".", {{"output", S(s2*s2*s2)}}));
          s.push_back(mk(5, "b3.volume() = " + S(s2*s2*s2) + ". Changing b2.side later would NOT change b3 (copied by value).", {{"output", S(s2*s2*s2)}}));
          return s;
      });
    names("Constructors", {"Default / parameterised / copy (concept)", "Object construction trace"});

    // 7. Destructors
    I("Destructors", "No input needed", "",
      {"class Buffer {",
       "    int* data;",
       "public:",
       "    Buffer(int n) { data = new int[n]; }",
       "    ~Buffer()     { delete[] data; }   // destructor",
       "};",
       "{",
       "    Buffer b(100);  // constructor: allocates 100 ints",
       "}  // end of scope: ~Buffer() runs automatically"},
      {"What happens if you forget to write a destructor when the class owns heap memory?",
       "In what order do destructors run when multiple objects go out of scope?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "DESTRUCTOR: a special method named ~ClassName(). It has no parameters and no return type. It runs automatically when an object is destroyed.", {}));
          s.push_back(mk(1, "int* data — a raw pointer to heap memory. Someone must free this.", {{"data", "heap pointer"}}));
          s.push_back(mk(2, "Constructor Buffer(int n): data = new int[n] allocates n ints on the heap.", {{"heap", "100 ints allocated"}}));
          s.push_back(mk(3, "~Buffer(): delete[] data frees the memory. The [] is essential — it was allocated with new[].", {{"heap", "freed"}}));
          s.push_back(mk(6, "Block { Buffer b(100); } — b is a local object. The constructor runs at line 8.", {{"b.data", "allocated"}}));
          s.push_back(mk(7, "} — end of block. b goes out of scope. The destructor ~Buffer() runs AUTOMATICALLY.", {{"~Buffer()", "runs"}, {"b.data", "freed"}}));
          s.push_back(mk(7, "If you forget the destructor, delete[] is never called — MEMORY LEAK. The heap memory is gone until the program exits.", {{"leak", "100 ints lost per object"}}));
          s.push_back(mk(7, "RAII (Resource Acquisition Is Initialisation): tie resource lifetime to object lifetime. Constructors acquire, destructors release.", {}));
          return s;
      });

    // 8. Function Overloading
    names("Function Overloading", {"Overload by parameter type", "Overload by parameter count"});
    I("Function Overloading", "Enter two numbers", "4, 7",
      {"int    add(int a, int b)    { return a + b; }",
       "double add(double a, double b) { return a + b; }",
       "string add(string a, string b) { return a + b; }",
       "// Same name, different parameter types",
       "cout << add(4, 7);          // calls int version",
       "cout << add(4.5, 1.5);      // calls double version",
       "cout << add(\"Hi\", \" there\"); // calls string version"},
      {"LeetCode #2235 (Add Two Integers)",
       "What determines which overload the compiler picks?"},
      [](const Ints& v) {
          int a = v.size() > 0 ? v[0] : 4;
          int b = v.size() > 1 ? v[1] : 7;
          Steps s;
          s.push_back(mk(0, "FUNCTION OVERLOADING: multiple functions share the same name but have different parameter lists (type, number, or order). The compiler picks the right one at COMPILE TIME.", {}));
          s.push_back(mk(0, "add(int, int) — integer version.", {{"signature", "add(int, int)"}}));
          s.push_back(mk(1, "add(double, double) — floating-point version. Same name, different parameter types.", {{"signature", "add(double, double)"}}));
          s.push_back(mk(2, "add(string, string) — string concatenation version.", {{"signature", "add(string, string)"}}));
          s.push_back(mk(4, "add(" + S(a) + ", " + S(b) + ") — both ints, so the int overload is called. Result = " + S(a + b) + ".", {{"called", "add(int,int)"}, {"result", S(a + b)}}));
          s.push_back(mk(5, "add(4.5, 1.5) — both doubles, so the double overload. Result = 6.0.", {{"called", "add(double,double)"}, {"result", "6.0"}}));
          s.push_back(mk(6, "add(\"Hi\", \" there\") — both strings. Result = \"Hi there\".", {{"called", "add(string,string)"}, {"result", "\"Hi there\""}}));
          s.push_back(mk(6, "Return type alone does NOT distinguish overloads — only parameters do.", {{"note", "return type is not enough"}}));
          return s;
      });
    I("Function Overloading#1", "Enter a radius (for area overloads)", "5",
      {"double area(double r) { return 3.14159 * r * r; }         // circle",
       "double area(double l, double w) { return l * w; }         // rectangle",
       "double area(double a, double b, double c) {               // triangle (Heron)",
       "    double s = (a+b+c)/2;",
       "    return sqrt(s*(s-a)*(s-b)*(s-c)); }"},
      {"What is the difference between overloading and overriding?"},
      [](const Ints& v) {
          double r = v.empty() ? 5.0 : std::max(1, v[0]);
          double circleA = 3.14159 * r * r;
          double rectA = r * (r + 2);
          double a2 = r, b2 = r + 1, c2 = r + 2;
          double s2 = (a2 + b2 + c2) / 2;
          double triA = std::sqrt(s2 * (s2 - a2) * (s2 - b2) * (s2 - c2));
          Steps s;
          s.push_back(mk(0, "area(double r) — one arg: computes circle area π×r².", {{"signature", "area(double)"}}));
          s.push_back(mk(1, "area(double l, double w) — two args: computes rectangle area.", {{"signature", "area(double,double)"}}));
          s.push_back(mk(2, "area(double a, double b, double c) — three args: Heron's formula for a triangle.", {{"signature", "area(double,double,double)"}}));
          s.push_back(mk(0, "area(" + S((int)r) + ") = π×" + S((int)r) + "² = " + std::to_string(circleA).substr(0, 7) + ".", {{"result", std::to_string(circleA).substr(0, 7)}}));
          s.push_back(mk(1, "area(" + S((int)r) + ", " + S((int)r + 2) + ") = " + S((int)r) + "×" + S((int)r + 2) + " = " + std::to_string(rectA).substr(0, 7) + ".", {{"result", std::to_string(rectA).substr(0, 7)}}));
          s.push_back(mk(2, "area(" + S((int)a2) + ", " + S((int)b2) + ", " + S((int)c2) + ") = " + std::to_string(triA).substr(0, 7) + " (Heron).", {{"result", std::to_string(triA).substr(0, 7)}}));
          return s;
      });

    // 9. Operator Overloading
    I("Operator Overloading", "Enter real and imaginary parts of two complex numbers: a1, b1, a2, b2", "3, 4, 1, 2",
      {"class Complex {",
       "    double re, im;",
       "public:",
       "    Complex(double r, double i) : re(r), im(i) {}",
       "    Complex operator+(const Complex& o) const {",
       "        return Complex(re + o.re, im + o.im);",
       "    }",
       "    void print() { cout << re << \"+\" << im << \"i\"; }",
       "};",
       "Complex a(3,4), b(1,2);",
       "Complex c = a + b;   // calls operator+"},
      {"LeetCode #537 (Complex Number Multiplication)",
       "Which operators cannot be overloaded in C++?"},
      [](const Ints& v) {
          double r1 = v.size() > 0 ? v[0] : 3, i1 = v.size() > 1 ? v[1] : 4;
          double r2 = v.size() > 2 ? v[2] : 1, i2 = v.size() > 3 ? v[3] : 2;
          auto fmt = [](double r, double i) {
              std::ostringstream o; o << r << (i >= 0 ? "+" : "") << i << "i"; return o.str();
          };
          Steps s;
          s.push_back(mk(0, "OPERATOR OVERLOADING: define what standard operators (+, -, *, etc.) mean for your class. a + b becomes a call to operator+(b).", {}));
          s.push_back(mk(1, "re and im — private data members for the real and imaginary parts.", {}));
          s.push_back(mk(2, "Constructor initialises re = " + std::to_string(r1).substr(0,4) + ", im = " + std::to_string(i1).substr(0,4) + " for object a.", {{"a", fmt(r1, i1)}}));
          s.push_back(mk(3, "operator+(const Complex& o) is a member function. When you write a + b, the compiler calls a.operator+(b).", {}));
          s.push_back(mk(4, "It returns a NEW Complex with re = " + std::to_string(r1).substr(0,4) + " + " + std::to_string(r2).substr(0,4) + " = " + std::to_string(r1+r2).substr(0,4) + " and im = " + std::to_string(i1).substr(0,4) + " + " + std::to_string(i2).substr(0,4) + " = " + std::to_string(i1+i2).substr(0,4) + ".", {{"result re", std::to_string(r1+r2).substr(0,4)}, {"result im", std::to_string(i1+i2).substr(0,4)}}));
          s.push_back(mk(9, "Complex a(" + fmt(r1,i1) + "), b(" + fmt(r2,i2) + ");", {{"a", fmt(r1,i1)}, {"b", fmt(r2,i2)}}));
          s.push_back(mk(10, "c = a + b — calls a.operator+(b). Result c = " + fmt(r1+r2, i1+i2) + ".", {{"c", fmt(r1+r2, i1+i2)}}));
          return s;
      });

    // 10. Static Functions
    I("Static Functions", "No input needed", "",
      {"class Counter {",
       "    static int count;        // shared by ALL objects",
       "public:",
       "    Counter()  { count++; }",
       "    ~Counter() { count--; }",
       "    static int getCount() { return count; }",
       "};",
       "int Counter::count = 0;      // must define outside",
       "Counter a, b, c;",
       "cout << Counter::getCount(); // 3 — no object needed"},
      {"Why can a static member function not use 'this'?",
       "What is the difference between a static data member and an instance data member?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "STATIC member: belongs to the CLASS, not to any specific object. All objects share the same static variable.", {}));
          s.push_back(mk(1, "static int count — one copy in memory, shared by every Counter object.", {{"count", "shared"}}));
          s.push_back(mk(2, "Constructor Counter() { count++; } — called each time an object is created.", {}));
          s.push_back(mk(3, "Destructor ~Counter() { count--; } — called when an object is destroyed.", {}));
          s.push_back(mk(4, "static int getCount() — a STATIC method. It can be called without any object: Counter::getCount(). It has no 'this' pointer.", {}));
          s.push_back(mk(6, "int Counter::count = 0; — the static member must be defined OUTSIDE the class body (memory is allocated here).", {{"count", "0"}}));
          s.push_back(mk(7, "Counter a — constructor runs, count becomes 1.", {{"count", "1"}}));
          s.push_back(mk(7, "Counter b — constructor runs, count becomes 2.", {{"count", "2"}}));
          s.push_back(mk(7, "Counter c — constructor runs, count becomes 3.", {{"count", "3"}}));
          s.push_back(mk(8, "Counter::getCount() = 3. Called using the class name, not an object — static functions do not need 'this'.", {{"output", "3"}}));
          return s;
      });

    // 11. Friend Functions
    I("Friend Functions", "Enter two Box side lengths", "3, 5",
      {"class Box {",
       "    int side;",
       "public:",
       "    Box(int s) : side(s) {}",
       "    friend int totalVolume(Box a, Box b); // friend declaration",
       "};",
       "// friend defined outside — can access private side",
       "int totalVolume(Box a, Box b) {",
       "    return a.side*a.side*a.side + b.side*b.side*b.side;",
       "}",
       "Box x(3), y(5);",
       "cout << totalVolume(x, y); // 27 + 125 = 152"},
      {"Why is friend considered a 'controlled break' of encapsulation?",
       "What is the difference between a friend function and a member function?"},
      [](const Ints& v) {
          int s1 = v.size() > 0 ? std::max(1, std::min(v[0], 10)) : 3;
          int s2 = v.size() > 1 ? std::max(1, std::min(v[1], 10)) : 5;
          int vol1 = s1 * s1 * s1, vol2 = s2 * s2 * s2;
          Steps s;
          s.push_back(mk(0, "FRIEND function: a free function (not a member) that is explicitly granted access to a class's private members.", {}));
          s.push_back(mk(1, "int side — private. Normally only member functions can read this.", {{"side", "private"}}));
          s.push_back(mk(3, "friend int totalVolume(Box, Box); — the class 'befriends' totalVolume. This declaration gives the function access to private side.", {}));
          s.push_back(mk(6, "totalVolume is defined OUTSIDE the class — it is NOT a member. It takes Box objects by value.", {}));
          s.push_back(mk(7, "Inside totalVolume: a.side and b.side are accessible because of the friend declaration.", {{"a.side", S(s1)}, {"b.side", S(s2)}}));
          s.push_back(mk(8, "a.side³ = " + S(vol1) + ", b.side³ = " + S(vol2) + ".", {{"vol(a)", S(vol1)}, {"vol(b)", S(vol2)}}));
          s.push_back(mk(10, "totalVolume(x, y) = " + S(vol1) + " + " + S(vol2) + " = " + S(vol1 + vol2) + ".", {{"output", S(vol1 + vol2)}}));
          s.push_back(mk(10, "Friend is a controlled break of encapsulation — only specific, named functions gain access. Useful for operator<< overloads and tightly coupled classes.", {}));
          return s;
      });
}
