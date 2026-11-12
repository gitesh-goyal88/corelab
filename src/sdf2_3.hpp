// Software Development 2 (Semester 2) — Modules 5 & 6: UML + Exceptions, Templates, STL
#pragma once
#include "sdf2_4.hpp"

inline void addSDF2_4(std::map<std::string, Sim>& m);

inline void addSDF2_3(std::map<std::string, Sim>& m) {
    addSDF2_4(m);

    const std::vector<std::string> R = {
        "cppreference: C++ Templates|https://en.cppreference.com/w/cpp/language/templates",
        "cppreference: C++ Exceptions|https://en.cppreference.com/w/cpp/language/exceptions",
        "GeeksforGeeks: C++ STL|https://www.geeksforgeeks.org/the-c-standard-template-library-stl/"
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

    /* ===== Module 5: UML and Relationship Implementation ===== */

    // 23. Models, Views and Model Elements
    T("Models, Views and Model Elements", "Type a UML element to explore (class / interface / package / note / diagram)", "class",
      {"// UML = Unified Modelling Language",
       "// Model: abstract description of a system",
       "// View: a diagram (structural / behavioural)",
       "// Model elements: class, interface, package, note, ...",
       "// Structural views: class, component, deployment diagrams",
       "// Behavioural views: sequence, activity, state diagrams"},
      {"Name two structural diagram types and two behavioural diagram types in UML.",
       "What is the difference between a model and a view in UML?"},
      [](const std::string& in) {
          std::string w = in;
          for (char& c : w) c = (char)tolower((unsigned char)c);
          Steps s;
          s.push_back(mk(0, "UML (Unified Modelling Language): a standard visual language to model software systems. It has diagrams (views) built from elements.", {}));
          s.push_back(mk(1, "MODEL: the full abstract description of the system — all classes, relationships, behaviour. Too complex to show in one picture.", {}));
          s.push_back(mk(2, "VIEW: a specific diagram that shows one aspect of the model. The same model element (e.g. a class) can appear in many views.", {}));
          if (w.find("class") != std::string::npos || w.empty()) {
              s.push_back(mk(3, "MODEL ELEMENT — Class: a blueprint for objects. Shown as a rectangle with three compartments: name | attributes | operations.", {{"notation", "┌──────┐ | name | | +attr | | +op() |"}}));
          }
          if (w.find("inter") != std::string::npos) {
              s.push_back(mk(3, "MODEL ELEMENT — Interface: an abstract type with no data, only operation signatures. Shown as a class with <<interface>> stereotype or a lollipop.", {{"notation", "<<interface>>"}}));
          }
          if (w.find("pack") != std::string::npos) {
              s.push_back(mk(3, "MODEL ELEMENT — Package: a namespace that groups related model elements. Shown as a folder tab.", {{"notation", "tab-folder"}}));
          }
          if (w.find("note") != std::string::npos) {
              s.push_back(mk(3, "MODEL ELEMENT — Note: a free-form comment attached to any element. Shown as a folded corner rectangle.", {{"notation", "dog-eared box"}}));
          }
          if (w.find("diag") != std::string::npos) {
              s.push_back(mk(3, "STRUCTURAL views: Class Diagram, Component Diagram, Deployment Diagram — show static structure.", {}));
              s.push_back(mk(3, "BEHAVIOURAL views: Sequence Diagram, Activity Diagram, State Machine Diagram — show dynamic behaviour.", {}));
          }
          s.push_back(mk(3, "Class diagrams are the most used view in OOP. They show classes, attributes, methods and relationships (association, aggregation, composition, inheritance).", {}));
          return s;
      });

    // 24. Class Diagrams
    T("Class Diagrams", "Type a visibility symbol to explore (+ public / - private / # protected / ~ package)", "+",
      {"// UML Class Diagram notation:",
       "// ClassName",
       "// ----------",
       "// - name : Type     (- = private)",
       "// # age  : int      (# = protected)",
       "// ----------",
       "// + getName() : String   (+ = public)",
       "// + setName(n:String)"},
      {"Draw a class diagram for a Student class with private id/name and public getters.",
       "What do the three compartments of a class box represent?"},
      [](const std::string& in) {
          std::string w = in;
          Steps s;
          s.push_back(mk(0, "CLASS DIAGRAM: shows classes, their attributes, operations and relationships. The standard notation uses a rectangle with three sections.", {}));
          s.push_back(mk(1, "Section 1 — Class name: shown in bold, centred. Abstract class names are italicised.", {{"example", "BankAccount"}}));
          s.push_back(mk(2, "Section 2 — Attributes: visibility name : Type = defaultValue. Types can be primitive (int, string) or class types.", {{"example", "- balance : double = 0.0"}}));
          s.push_back(mk(3, "Section 3 — Operations: visibility name(params) : returnType.", {{"example", "+ deposit(amount : double) : void"}}));
          if (!w.empty() && w[0] == '+') {
              s.push_back(mk(0, "+ (plus) = PUBLIC: accessible by any code. Interface members are always public.", {{"symbol", "+"}}));
          } else if (!w.empty() && w[0] == '-') {
              s.push_back(mk(0, "- (minus) = PRIVATE: accessible only within the class itself.", {{"symbol", "-"}}));
          } else if (!w.empty() && w[0] == '#') {
              s.push_back(mk(0, "# (hash) = PROTECTED: accessible within the class and its subclasses.", {{"symbol", "#"}}));
          } else if (!w.empty() && w[0] == '~') {
              s.push_back(mk(0, "~ (tilde) = PACKAGE: accessible within the same package (rarely used in C++).", {{"symbol", "~"}}));
          }
          s.push_back(mk(3, "Static members are underlined in UML. Abstract operations are italicised.", {{"static", "underlined"}, {"abstract", "italicised"}}));
          s.push_back(mk(3, "Relationships between classes are shown as lines connecting the boxes: solid line = association, diamond = aggregation/composition, triangle = inheritance.", {}));
          return s;
      });

    // 25. Association
    I("Association", "No input needed", "",
      {"// Association: 'uses a' relationship — objects know about each other",
       "class Teacher {",
       "public:",
       "    std::string name;",
       "    void teach() { }",
       "};",
       "class Student {",
       "    Teacher* teacher;   // pointer — lives independently",
       "public:",
       "    Student(Teacher* t) : teacher(t) {}",
       "    void learn() { teacher->teach(); }",
       "};",
       "// UML: Student ———> Teacher  (plain arrow)"},
      {"What multiplicity would you write for a Teacher who teaches many Students?",
       "What is the difference between unidirectional and bidirectional association?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "ASSOCIATION: a 'uses-a' or 'knows-about' relationship. Objects of one class hold a reference/pointer to objects of another class.", {}));
          s.push_back(mk(0, "Teacher and Student are INDEPENDENT — both can exist without the other.", {}));
          s.push_back(mk(1, "Teacher has a name and a teach() method.", {{"Teacher", "independent object"}}));
          s.push_back(mk(3, "Student holds a Teacher* pointer — it knows about its teacher, but does not own it. The Teacher was created elsewhere.", {{"Student.teacher", "pointer to Teacher"}}));
          s.push_back(mk(5, "Student::learn() calls teacher->teach() — uses the association.", {}));
          s.push_back(mk(5, "If the Student is destroyed, the Teacher lives on. Destroying the Teacher does not destroy the Student.", {{"lifetime", "independent"}}));
          s.push_back(mk(5, "UML: Student ———> Teacher. A plain arrow. Multiplicity can be added: Student (0..*) ———> (1) Teacher means many students per one teacher.", {{"multiplicity", "0..* → 1"}}));
          return s;
      });

    // 26. Aggregation
    I("Aggregation", "No input needed", "",
      {"// Aggregation: 'has-a' (weak ownership) — parts can outlive the whole",
       "class Engine { public: std::string type; };",
       "class Car {",
       "    Engine* engine;    // pointer — engine can exist without car",
       "public:",
       "    Car(Engine* e) : engine(e) {}",
       "    // destructor does NOT delete engine",
       "    ~Car() {}",
       "};",
       "Engine eng;",
       "{ Car c(&eng); }  // Car destroyed, Engine still alive",
       "// UML: Car <>———> Engine  (hollow diamond at Car end)"},
      {"What is the difference between aggregation and composition in UML?",
       "Give a real-world example where the parts outlive the whole."},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "AGGREGATION: a 'has-a' relationship where the PART can exist independently of the WHOLE. Weak ownership.", {}));
          s.push_back(mk(0, "Engine is a separate object. Car does not create or destroy the Engine.", {}));
          s.push_back(mk(1, "Engine has a type. It is created independently.", {{"Engine", "independent lifetime"}}));
          s.push_back(mk(2, "Car holds Engine* — a pointer. The Car does NOT own the Engine.", {{"Car.engine", "borrowed reference"}}));
          s.push_back(mk(3, "~Car() does NOT delete engine. The Engine lives on when the Car is destroyed.", {{"destructor", "does not delete"}}));
          s.push_back(mk(5, "Engine eng; created on the stack.", {{"eng", "alive"}}));
          s.push_back(mk(6, "{ Car c(&eng); } — Car is created, then destroyed at }. Engine eng remains alive.", {{"eng", "still alive after Car destroyed"}}));
          s.push_back(mk(6, "UML: Car <>———> Engine. HOLLOW diamond at the Car end (whole), arrow pointing to Engine (part).", {{"UML", "hollow diamond"}}));
          return s;
      });

    // 27. Composition
    I("Composition", "No input needed", "",
      {"// Composition: 'owns-a' (strong ownership) — part cannot outlive the whole",
       "class Heart {",
       "public:",
       "    Heart() { cout << \"Heart created\"; }",
       "    ~Heart() { cout << \"Heart destroyed\"; }",
       "};",
       "class Person {",
       "    Heart heart;   // object by value — owned, NOT a pointer",
       "public:",
       "    Person()  { }",
       "    ~Person() { }  // Heart is destroyed automatically",
       "};",
       "// UML: Person <filled diamond>———> Heart"},
      {"Why is 'by value' used instead of 'by pointer' in composition?",
       "Can a Heart belong to two Person objects simultaneously?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "COMPOSITION: the WHOLE owns the PART. The part's lifetime is tied to the whole. Part cannot exist without the whole.", {}));
          s.push_back(mk(0, "Heart is completely owned by Person. It cannot be shared with another Person.", {}));
          s.push_back(mk(2, "Heart is stored BY VALUE inside Person — not a pointer. When Person is created, Heart is created automatically.", {{"Heart", "by value inside Person"}}));
          s.push_back(mk(5, "Person p; — Person constructor runs, then Heart() runs. 'Heart created'.", {{"output", "Heart created"}}));
          s.push_back(mk(6, "When p goes out of scope: ~Person() runs, then ~Heart() runs automatically. 'Heart destroyed'.", {{"output", "Heart destroyed"}}));
          s.push_back(mk(6, "The Heart cannot outlive the Person, and no other Person can share this Heart — EXCLUSIVE STRONG OWNERSHIP.", {{"ownership", "exclusive, strong"}}));
          s.push_back(mk(6, "UML: Person <FILLED diamond>———> Heart. Filled (solid) diamond = composition. Hollow diamond = aggregation.", {{"UML", "filled diamond"}}));
          return s;
      });

    // 28. Inheritance and Its Implementation (UML)
    I("Inheritance and Its Implementation", "No input needed", "",
      {"// UML: open triangle arrow = inheritance",
       "// Animal <|—— Dog     (Dog inherits Animal)",
       "class Animal {",
       "public:",
       "    std::string name;",
       "    virtual void speak() = 0;",
       "    virtual ~Animal() = default;",
       "};",
       "class Dog : public Animal {",
       "public:",
       "    Dog(std::string n) { name = n; }",
       "    void speak() override { cout << name << \" says Woof!\"; }",
       "};",
       "Animal* a = new Dog(\"Rex\");",
       "a->speak();  // Rex says Woof!"},
      {"Draw the UML class diagram for Animal, Dog, Cat hierarchy.",
       "In C++, what does the colon (:) syntax do in class Dog : public Animal?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "INHERITANCE in UML: shown as an OPEN TRIANGLE arrowhead pointing from child to parent. 'Dog ——|> Animal' means Dog inherits Animal.", {{"UML arrow", "open triangle"}}));
          s.push_back(mk(1, "Abstract class Animal — speak() is pure virtual. Animal is shown in italics in UML.", {{"Animal", "abstract (italic)"}}));
          s.push_back(mk(3, "Animal has name and virtual ~Animal() = default (virtual destructor for safe polymorphic deletion).", {}));
          s.push_back(mk(5, "class Dog : public Animal — C++ implementation of the inheritance arrow.", {}));
          s.push_back(mk(6, "Dog::speak() override provides the concrete implementation.", {{"Dog::speak", "\"Woof!\""}}));
          s.push_back(mk(10, "Animal* a = new Dog(\"Rex\"); — Liskov substitution: a Dog can be used wherever an Animal is expected.", {{"name", "Rex"}}));
          s.push_back(mk(11, "a->speak() — dynamic dispatch via vtable → Dog::speak() → 'Rex says Woof!'", {{"output", "Rex says Woof!"}}));
          s.push_back(mk(11, "In a full UML diagram: Animal (abstract) at top, Dog and Cat as children connected by inheritance arrows. Shared attributes (name) stay in Animal.", {}));
          return s;
      });

    /* ===== Module 6: Exceptions, Templates and STL ===== */

    // 29. Exceptions
    I("Exceptions", "Enter a numerator and denominator", "10, 0",
      {"void riskyOp(int a, int b) {",
       "    if (b == 0)",
       "        throw std::runtime_error(\"Division by zero!\");",
       "    cout << a / b;",
       "}",
       "try {",
       "    riskyOp(10, 0);",
       "} catch (const std::runtime_error& e) {",
       "    cout << \"Caught: \" << e.what();",
       "}"},
      {"LeetCode #1240 (Tiling a Rectangle with the Fewest Squares) — no good exception match; focus on design questions.",
       "What is the difference between throwing an int, a string, and an exception object?"},
      [](const Ints& v) {
          int a = v.size() > 0 ? v[0] : 10;
          int b = v.size() > 1 ? v[1] : 0;
          Steps s;
          s.push_back(mk(0, "EXCEPTION: an error signal that interrupts normal program flow. C++ exceptions separate error handling from normal code.", {}));
          s.push_back(mk(1, "riskyOp checks if b == 0.", {{"a", S(a)}, {"b", S(b)}}));
          if (b == 0) {
              s.push_back(mk(2, "b == 0 is TRUE — throw std::runtime_error(\"Division by zero!\") is executed. The function STOPS immediately.", {{"exception", "runtime_error"}}));
              s.push_back(mk(5, "The exception propagates UP the call stack. Control jumps to the nearest matching catch block.", {{"stack unwind", "happening"}}));
              s.push_back(mk(7, "catch (const std::runtime_error& e) — matches! e.what() returns \"Division by zero!\"", {{"caught", "runtime_error"}}));
              s.push_back(mk(8, "Output: 'Caught: Division by zero!' Program continues after the catch block.", {{"output", "Caught: Division by zero!"}}));
          } else {
              s.push_back(mk(2, "b = " + S(b) + " ≠ 0 — no exception thrown.", {}));
              s.push_back(mk(3, "Normal execution: a / b = " + S(a) + " / " + S(b) + " = " + S(a / b) + " is printed.", {{"output", S(a / b)}}));
              s.push_back(mk(5, "The try block completes normally. The catch block is SKIPPED.", {{"catch", "skipped"}}));
          }
          return s;
      });

    // 30. Try, Catch and Throw
    names("Try, Catch and Throw", {"Catching by type", "Multiple catch blocks"});
    I("Try, Catch and Throw", "Enter an integer (try 0 for divide-by-zero, negative for domain error)", "0",
      {"try {",
       "    if (n == 0) throw std::runtime_error(\"zero!\");",
       "    if (n < 0)  throw std::domain_error(\"negative!\");",
       "    cout << 100 / n;",
       "} catch (const std::domain_error& e) {",
       "    cout << \"Domain: \" << e.what();",
       "} catch (const std::runtime_error& e) {",
       "    cout << \"Runtime: \" << e.what();",
       "} catch (...) {",
       "    cout << \"Unknown exception\";",
       "}"},
      {"Why should catch blocks be ordered from most-specific to most-general?",
       "What does catch(...) catch?"},
      [](const Ints& v) {
          int n = v.empty() ? 0 : v[0];
          Steps s;
          s.push_back(mk(0, "TRY block: wraps code that might throw. If an exception is thrown, control leaves the try block immediately.", {{"n", S(n)}}));
          s.push_back(mk(1, "Check n == 0: " + std::string(n == 0 ? "TRUE." : "false.")));
          if (n == 0) {
              s.push_back(mk(1, "throw std::runtime_error(\"zero!\") — execution leaves the try block.", {{"thrown", "runtime_error"}}));
              s.push_back(mk(4, "catch(domain_error) — does NOT match runtime_error. Skipped.", {}));
              s.push_back(mk(5, "catch(runtime_error) — MATCHES. e.what() = \"zero!\"", {{"output", "Runtime: zero!"}}));
          } else if (n < 0) {
              s.push_back(mk(2, "n < 0 is TRUE. throw std::domain_error(\"negative!\").", {{"thrown", "domain_error"}}));
              s.push_back(mk(4, "catch(domain_error) — MATCHES first (most specific). e.what() = \"negative!\"", {{"output", "Domain: negative!"}}));
          } else {
              s.push_back(mk(2, "n = " + S(n) + " > 0. No exception thrown.", {}));
              s.push_back(mk(3, "100 / " + S(n) + " = " + S(100 / n) + ". All catch blocks skipped.", {{"output", S(100 / n)}}));
          }
          s.push_back(mk(8, "catch(...) is the catch-all. It catches ANY type. Must be LAST. Used as a safety net.", {{"catch(...)", "catches everything"}}));
          return s;
      });
    I("Try, Catch and Throw#1", "Enter two integers for safe array access (index, size)", "7, 5",
      {"int safeGet(int* arr, int size, int idx) {",
       "    if (idx < 0 || idx >= size)",
       "        throw std::out_of_range(\"index \" + to_string(idx));",
       "    return arr[idx];",
       "}",
       "try {",
       "    cout << safeGet(arr, 5, 7);",
       "} catch (const std::out_of_range& e) {",
       "    cerr << \"Error: \" << e.what();",
       "}"},
      {"What is std::out_of_range and where does it fit in the exception hierarchy?"},
      [](const Ints& v) {
          int idx = v.size() > 0 ? v[0] : 7;
          int sz  = v.size() > 1 ? std::max(1, v[1]) : 5;
          Steps s;
          s.push_back(mk(0, "safeGet checks bounds before accessing. This is defensive programming.", {{"index", S(idx)}, {"size", S(sz)}}));
          s.push_back(mk(1, "Is idx (" + S(idx) + ") < 0 or >= size (" + S(sz) + ")? " + (idx < 0 || idx >= sz ? "YES — out of range!" : "No — valid index.")));
          if (idx < 0 || idx >= sz) {
              s.push_back(mk(2, "throw std::out_of_range(\"index \" + " + S(idx) + "). Function stops.", {{"thrown", "out_of_range: index " + S(idx)}}));
              s.push_back(mk(6, "catch(std::out_of_range& e) — matches. e.what() = 'index " + S(idx) + "'.", {{"output", "Error: index " + S(idx)}}));
          } else {
              s.push_back(mk(3, "Index is valid. Returns arr[" + S(idx) + "].", {{"accessed", "arr[" + S(idx) + "]"}}));
          }
          return s;
      });

    // 31. Re-throwing Exceptions
    I("Re-throwing Exceptions", "No input needed", "",
      {"void log_and_rethrow() {",
       "    try {",
       "        throw std::runtime_error(\"disk full\");",
       "    } catch (const std::exception& e) {",
       "        cerr << \"[LOG] Exception: \" << e.what();",
       "        throw;   // re-throw the SAME exception",
       "    }",
       "}",
       "try {",
       "    log_and_rethrow();",
       "} catch (const std::exception& e) {",
       "    cout << \"Handled: \" << e.what();",
       "}"},
      {"What does 'throw;' (with no operand) do? Why is it preferred over 'throw e;' ?",
       "What is exception slicing and why does 'throw;' avoid it?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "RE-THROWING: catching an exception, doing some work (logging), then re-throwing it to a higher handler.", {}));
          s.push_back(mk(1, "Inner try: throw std::runtime_error(\"disk full\") is thrown.", {{"thrown", "runtime_error: disk full"}}));
          s.push_back(mk(2, "Inner catch catches it as std::exception& (base class reference).", {{"caught", "exception&"}}));
          s.push_back(mk(3, "cerr << ... — logs the error. '[LOG] Exception: disk full'", {{"logged", "disk full"}}));
          s.push_back(mk(4, "throw; — RE-THROWS the ORIGINAL exception object (runtime_error) without copying. The type is preserved.", {{"re-thrown", "original runtime_error"}}));
          s.push_back(mk(4, "If you wrote 'throw e;' instead, e is of type std::exception — you would SLICE the derived type info away. 'throw;' avoids slicing.", {{"slicing", "avoided by throw;"}}));
          s.push_back(mk(8, "Outer catch catches the re-thrown exception. e.what() = 'disk full'. Output: 'Handled: disk full'.", {{"output", "Handled: disk full"}}));
          return s;
      });

    // 32. Exception and Inheritance
    I("Exception and Inheritance", "No input needed", "",
      {"// Exception hierarchy",
       "class AppError : public std::exception {",
       "    std::string msg;",
       "public:",
       "    AppError(std::string m) : msg(m) {}",
       "    const char* what() const noexcept override { return msg.c_str(); }",
       "};",
          "class DatabaseError : public AppError {",
       "public:",
       "    DatabaseError(std::string m) : AppError(\"DB: \" + m) {}",
       "};",
       "throw DatabaseError(\"connection lost\");",
       "// catch (AppError&) catches DatabaseError too!"},
      {"Why should you catch by reference (const std::exception&) not by value?",
       "Design an exception hierarchy for a file I/O system."},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "EXCEPTION HIERARCHY: custom exceptions can inherit from std::exception or each other. A catch for a base class catches derived exceptions too.", {}));
          s.push_back(mk(1, "AppError : public std::exception — a custom application error. Overrides what() to return a message.", {}));
          s.push_back(mk(2, "what() noexcept — the override must be noexcept (doesn't throw). Returns the error string.", {{"what()", "returns message"}}));
          s.push_back(mk(5, "DatabaseError : public AppError — a more specific error. Passes 'DB: ' prefix to AppError.", {{"DatabaseError", "is-a AppError"}}));
          s.push_back(mk(10, "throw DatabaseError(\"connection lost\") — throws a DatabaseError.", {{"thrown", "DatabaseError"}}));
          s.push_back(mk(10, "catch (const DatabaseError& e) — catches only DatabaseError. Most specific, so put FIRST.", {}));
          s.push_back(mk(10, "catch (const AppError& e) — catches DatabaseError TOO (IS-A relationship). Less specific, put SECOND.", {}));
          s.push_back(mk(10, "catch (const std::exception& e) — catches everything derived from std::exception. Catch-all safety net. Put LAST.", {}));
          s.push_back(mk(10, "Catch by REFERENCE, never by value — avoids object slicing and copying.", {{"best practice", "catch by const ref"}}));
          return s;
      });

    // 33. Function Templates
    I("Function Templates", "Enter two numbers to compare", "7, 3",
      {"template <typename T>",
       "T maxOf(T a, T b) {",
       "    return (a > b) ? a : b;",
       "}",
       "// Compiler generates specific versions as needed:",
       "maxOf(7, 3);       // maxOf<int>(7, 3)    -> 7",
       "maxOf(2.5, 4.1);   // maxOf<double>(2.5, 4.1) -> 4.1",
       "maxOf('x', 'a');   // maxOf<char>('x','a')    -> 'x'"},
      {"LeetCode #35 (Search Insert Position) — template thinking.",
       "What is template instantiation? When does it happen?"},
      [](const Ints& v) {
          int a = v.size() > 0 ? v[0] : 7;
          int b = v.size() > 1 ? v[1] : 3;
          Steps s;
          s.push_back(mk(0, "FUNCTION TEMPLATE: a blueprint for a family of functions. The compiler generates specific versions (instantiations) for each type used.", {}));
          s.push_back(mk(1, "template <typename T> — T is a TYPE PARAMETER, a placeholder for the actual type supplied by the caller.", {}));
          s.push_back(mk(2, "T maxOf(T a, T b) — works for any type T that supports operator>.", {}));
          s.push_back(mk(3, "return (a > b) ? a : b — same logic, works for int, double, char, string, ...", {}));
          s.push_back(mk(5, "maxOf(" + S(a) + ", " + S(b) + ") — compiler deduces T = int, instantiates maxOf<int>. Result = " + S(std::max(a, b)) + ".", {{"T", "int"}, {"result", S(std::max(a, b))}}));
          s.push_back(mk(6, "maxOf(2.5, 4.1) — T = double, instantiates maxOf<double>. Result = 4.1.", {{"T", "double"}, {"result", "4.1"}}));
          s.push_back(mk(7, "maxOf('x', 'a') — T = char. 'x' > 'a' because ASCII 120 > 97. Result = 'x'.", {{"T", "char"}, {"result", "'x'"}}));
          s.push_back(mk(5, "TEMPLATE INSTANTIATION: the compiler generates a real function from the template for each distinct T. Happens at compile time (no runtime cost).", {{"instantiation", "compile time"}}));
          return s;
      });

    // 34. Function Template Overloading
    I("Function Template Overloading", "No input needed", "",
      {"template <typename T>",
       "void print(T x) { cout << x; }         // general",
       "",
       "template <typename T>",
       "void print(T* p) { cout << \"ptr: \" << *p; } // pointer overload",
       "",
       "void print(const char* s) { cout << \"str: \" << s; } // non-template",
       "",
       "int x = 42;",
       "print(x);    // general:  42",
       "print(&x);   // pointer:  ptr: 42",
       "print(\"hi\"); // non-template preferred: str: hi"},
      {"What is the resolution order: non-template specialisation vs template vs template overload?",
       "What is explicit template specialisation?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "FUNCTION TEMPLATE OVERLOADING: you can have multiple template functions with the same name but different parameter patterns.", {}));
          s.push_back(mk(1, "template<T> print(T x) — general: prints any value.", {{"signature", "print(T)"}}));
          s.push_back(mk(3, "template<T> print(T* p) — pointer overload: dereferences and prints. More specific than the general.", {{"signature", "print(T*)"}}));
          s.push_back(mk(5, "void print(const char* s) — a NON-TEMPLATE function. The compiler prefers an exact non-template match over templates.", {{"signature", "print(const char*)"}}));
          s.push_back(mk(7, "print(42) — int, not a pointer. General template: prints 42.", {{"called", "print<int>"}, {"output", "42"}}));
          s.push_back(mk(8, "print(&x) — int*, matches pointer template. Dereferences to get 42. Prints 'ptr: 42'.", {{"called", "print<int*>"}, {"output", "ptr: 42"}}));
          s.push_back(mk(9, "print(\"hi\") — const char*. The non-template exact match is preferred over templates. Prints 'str: hi'.", {{"called", "non-template"}, {"output", "str: hi"}}));
          s.push_back(mk(9, "Resolution order: 1) exact non-template match, 2) template specialisation, 3) template with implicit conversions.", {}));
          return s;
      });

    // 35. Class Templates
    I("Class Templates", "Enter up to 5 numbers for a typed stack", "10, 20, 30",
      {"template <typename T, int MaxSize = 10>",
       "class Stack {",
       "    T data[MaxSize];",
       "    int top = -1;",
       "public:",
       "    void push(T val) { data[++top] = val; }",
       "    T    pop()       { return data[top--]; }",
       "    bool empty()     { return top == -1; }",
       "    int  size()      { return top + 1; }",
       "};",
       "Stack<int> s;  s.push(10); s.push(20);",
       "cout << s.pop(); // 20"},
      {"LeetCode #232 (Implement Queue using Stacks)",
       "What is the difference between a class template and a template class?"},
      [](const Ints& v) {
          Ints items = v.empty() ? Ints{10, 20, 30} : v;
          if ((int)items.size() > 8) items.resize(8);
          Steps s;
          s.push_back(mk(0, "CLASS TEMPLATE: a blueprint for a family of classes. Stack<int>, Stack<double>, Stack<string> are all generated from one template.", {}));
          s.push_back(mk(0, "template <typename T, int MaxSize = 10> — T = element type, MaxSize = compile-time capacity with default 10.", {}));
          s.push_back(mk(1, "T data[MaxSize] — the array element type adapts to T.", {}));
          s.push_back(mk(2, "int top = -1 — stack pointer, -1 means empty.", {{"top", "-1"}}));
          s.push_back(mk(10, "Stack<int> s; — compiler generates a Stack for int. s is empty.", {{"top", "-1"}}));
          Ints arr;
          for (int i = 0; i < (int)items.size(); ++i) {
              arr.push_back(items[i]);
              Step st = mkArr(3, "s.push(" + S(items[i]) + ") — top becomes " + S(i) + ".", {{"top", S(i)}}, arr, i);
              s.push_back(st);
          }
          if (!arr.empty()) {
              int popped = arr.back(); arr.pop_back();
              s.push_back(mkArr(4, "s.pop() — returns " + S(popped) + " (last pushed). top decrements.", {{"popped", S(popped)}, {"top", S((int)arr.size()-1)}}, arr, -1));
              s.push_back(mk(6, "s.size() = " + S((int)arr.size()) + ".", {{"size", S((int)arr.size())}}));
          }
          return s;
      });

    // 36. Collection Classes
    T("Collection Classes", "Type a collection to explore (vector / list / map / set / queue / stack)", "vector",
      {"// STL collection classes",
       "#include <vector>   // dynamic array",
       "#include <list>     // doubly linked list",
       "#include <map>      // sorted key-value pairs",
       "#include <set>      // sorted unique values",
       "#include <queue>    // FIFO adapter",
       "#include <stack>    // LIFO adapter"},
      {"Which collection do you use for O(1) random access? O(1) insert at front?",
       "What is the time complexity of inserting into std::set?"},
      [](const std::string& in) {
          std::string w = in;
          for (char& c : w) c = (char)tolower((unsigned char)c);
          Steps s;
          s.push_back(mk(0, "STL COLLECTION CLASSES: ready-made generic containers. All are class templates — parameterised by element type.", {}));
          if (w.find("vector") != std::string::npos || w.empty()) {
              s.push_back(mk(1, "std::vector<T>: dynamic array. Random access O(1), push_back O(1) amortised, insert/erase at middle O(n). Most-used container.", {{"access", "O(1)"}, {"push_back", "O(1) amortised"}}));
          }
          if (w.find("list") != std::string::npos) {
              s.push_back(mk(1, "std::list<T>: doubly linked list. Insert/erase anywhere O(1) (given iterator), no random access, O(n) search.", {{"insert", "O(1) with iterator"}, {"access", "O(n)"}}));
          }
          if (w.find("map") != std::string::npos) {
              s.push_back(mk(1, "std::map<K,V>: red-black tree. Keys sorted, find/insert/erase O(log n). std::unordered_map gives O(1) average with hash.", {{"find", "O(log n)"}}));
          }
          if (w.find("set") != std::string::npos) {
              s.push_back(mk(1, "std::set<T>: sorted unique elements. Insert/find/erase O(log n). std::unordered_set for O(1) average.", {{"unique", "yes"}, {"sorted", "yes"}}));
          }
          if (w.find("queue") != std::string::npos) {
              s.push_back(mk(1, "std::queue<T>: FIFO adapter over deque. push() at back, pop()/front() at front — O(1) each.", {{"FIFO", "push=back, pop=front"}}));
          }
          if (w.find("stack") != std::string::npos) {
              s.push_back(mk(1, "std::stack<T>: LIFO adapter. push()/pop()/top() — all O(1).", {{"LIFO", "push/pop/top"}}));
          }
          s.push_back(mk(1, "All containers work with iterators and range-based for loops. All support begin(), end(), size(), empty().", {}));
          return s;
      });

    // 37. Iteration Protocols
    I("Iteration Protocols", "Enter up to 6 numbers", "5, 2, 8, 1, 9, 3",
      {"std::vector<int> v = {5, 2, 8, 1, 9, 3};",
       "// 1. Iterator (classic)",
       "for (auto it = v.begin(); it != v.end(); ++it)",
       "    cout << *it << \" \";",
       "// 2. Range-based for (C++11)",
       "for (int x : v) cout << x << \" \";",
       "// 3. Index-based",
       "for (size_t i = 0; i < v.size(); i++) cout << v[i] << \" \";"},
      {"What is an iterator and how does it model a generalised pointer?",
       "LeetCode #941 (Valid Mountain Array)"},
      [](const Ints& v) {
          Ints items = v.empty() ? Ints{5, 2, 8, 1, 9, 3} : v;
          if ((int)items.size() > 8) items.resize(8);
          Steps s;
          s.push_back(mkArr(0, "std::vector v initialised with " + S((int)items.size()) + " elements.", {{"size", S((int)items.size())}}, items, -1));
          s.push_back(mk(1, "ITERATOR: an object that generalises a pointer. begin() = pointer to first element, end() = one-past-last (sentinel).", {{"begin()", "→ first"}, {"end()", "→ past-last"}}));
          for (int i = 0; i < (int)items.size(); ++i) {
              s.push_back(mkArr(2, "it → v[" + S(i) + "] = " + S(items[i]) + ". *it dereferences like a pointer.", {{"*it", S(items[i])}, {"i", S(i)}}, items, i));
          }
          s.push_back(mk(2, "it == v.end(): loop ends. Total printed: all " + S((int)items.size()) + " elements.", {}));
          s.push_back(mkArr(4, "RANGE-BASED FOR (C++11): 'for (int x : v)' — the compiler expands this to the iterator loop automatically. Cleanest syntax.", {{"syntax", "for (auto x : container)"}}, items, -1));
          s.push_back(mkArr(6, "INDEX-BASED: 'for (size_t i = 0; i < v.size(); i++) v[i]' — only works for random-access containers (vector, array, deque).", {{"type", "random-access only"}}, items, -1));
          return s;
      });

    // 38. Standard Template Library (STL)
    I("Standard Template Library (STL)", "Enter up to 6 numbers to sort and search", "5, 2, 8, 1, 9, 3",
      {"#include <algorithm>  // sort, find, binary_search, ...",
       "#include <vector>",
       "std::vector<int> v = {5, 2, 8, 1, 9, 3};",
       "std::sort(v.begin(), v.end());   // O(n log n)",
       "auto it = std::find(v.begin(), v.end(), 8); // O(n)",
       "bool found = std::binary_search(v.begin(), v.end(), 8); // O(log n)"},
      {"LeetCode #704 (Binary Search)",
       "Name five STL algorithms and their time complexities."},
      [](const Ints& v) {
          Ints items = v.empty() ? Ints{5, 2, 8, 1, 9, 3} : v;
          if ((int)items.size() > 8) items.resize(8);
          Ints sorted_items = items;
          std::sort(sorted_items.begin(), sorted_items.end());
          int target = items.size() > 1 ? items[1] : (items.empty() ? 8 : items[0]);
          Steps s;
          s.push_back(mkArr(0, "STL = Standard Template Library. Three parts: Containers, Algorithms, Iterators. Algorithms operate on iterators — they work with ANY container.", {}, items, -1));
          s.push_back(mkArr(1, "#include <algorithm> — provides sort, find, binary_search, min_element, max_element, reverse, count, accumulate, ...", {}, items, -1));
          s.push_back(mkArr(2, "v before sort: " + [&]{ std::string r; for (int x : items) r += S(x) + " "; return r; }(), {}, items, -1));
          s.push_back(mkArr(3, "std::sort(v.begin(), v.end()) — sorts in-place. O(n log n) introsort. After sort: " + [&]{ std::string r; for (int x : sorted_items) r += S(x) + " "; return r; }(), {{"complexity", "O(n log n)"}}, sorted_items, -1));
          int findIdx = -1;
          for (int i = 0; i < (int)sorted_items.size(); ++i) if (sorted_items[i] == target) { findIdx = i; break; }
          s.push_back(mkArr(4, "std::find(begin, end, " + S(target) + ") — linear scan. Returns iterator to first match. O(n)." + (findIdx >= 0 ? " Found at index " + S(findIdx) + "." : " Not found."), {{"complexity", "O(n)"}, {"found at", findIdx >= 0 ? S(findIdx) : "not found"}}, sorted_items, findIdx));
          s.push_back(mkArr(5, "std::binary_search(begin, end, " + S(target) + ") — requires sorted. O(log n). Found: " + std::string(findIdx >= 0 ? "true." : "false."), {{"complexity", "O(log n)"}, {"found", findIdx >= 0 ? "true" : "false"}}, sorted_items, findIdx));
          s.push_back(mk(5, "STL algorithms are DECOUPLED from containers — they take iterators. The same std::sort works on vector, deque, and plain arrays.", {{"key design", "iterators decouple algorithm from container"}}));
          return s;
      });
}
