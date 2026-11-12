// Software Development 2 (Semester 2) — Modules 3 & 4: Inheritance + Polymorphism
#pragma once
#include "sdf2_3.hpp"

inline void addSDF2_3(std::map<std::string, Sim>& m);

inline void addSDF2_2(std::map<std::string, Sim>& m) {
    addSDF2_3(m);

    const std::vector<std::string> R = {
        "cppreference: C++ Inheritance|https://en.cppreference.com/w/cpp/language/derived_class",
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

    /* ===== Module 3: Inheritance Using C++ ===== */

    // 12. Base Class and Derived Class
    I("Base Class and Derived Class", "Enter speed and fuel for a Vehicle/Car", "120, 50",
      {"class Vehicle {         // BASE class",
       "protected:",
       "    int speed, fuel;",
       "public:",
       "    Vehicle(int s, int f) : speed(s), fuel(f) {}",
       "    void info() { cout << \"Speed:\" << speed << \" Fuel:\" << fuel; }",
       "};",
       "class Car : public Vehicle {  // DERIVED class",
       "    int doors;",
       "public:",
       "    Car(int s, int f, int d) : Vehicle(s, f), doors(d) {}",
       "    void carInfo() { info(); cout << \" Doors:\" << doors; }",
       "};",
       "Car c(120, 50, 4);",
       "c.carInfo();"},
      {"What does 'protected' mean? How does it differ from 'private' in inheritance?",
       "What is the difference between IS-A and HAS-A relationships?"},
      [](const Ints& v) {
          int speed = v.size() > 0 ? std::max(1, v[0]) : 120;
          int fuel  = v.size() > 1 ? std::max(1, v[1]) : 50;
          int doors = 4;
          Steps s;
          s.push_back(mk(0, "BASE class Vehicle: the parent. Defines common attributes (speed, fuel) and behaviour (info()) for all vehicles.", {}));
          s.push_back(mk(1, "protected: members are like private, BUT derived classes can access them. Outside code still cannot.", {{"speed", "protected"}, {"fuel", "protected"}}));
          s.push_back(mk(2, "Vehicle constructor sets speed = " + S(speed) + ", fuel = " + S(fuel) + ".", {{"speed", S(speed)}, {"fuel", S(fuel)}}));
          s.push_back(mk(3, "info() prints the base attributes.", {}));
          s.push_back(mk(5, "DERIVED class Car : public Vehicle — Car IS-A Vehicle. Car inherits all public and protected members of Vehicle.", {}));
          s.push_back(mk(6, "Car adds its own member: int doors = " + S(doors) + ".", {{"doors", S(doors)}}));
          s.push_back(mk(7, "Car constructor uses an INITIALISER LIST to call the base constructor: Vehicle(s, f). The base part is initialised first.", {{"speed", S(speed)}, {"fuel", S(fuel)}, {"doors", S(doors)}}));
          s.push_back(mk(10, "c.carInfo() calls base's info() (inherited) then prints doors = " + S(doors) + ".", {{"output", "Speed:" + S(speed) + " Fuel:" + S(fuel) + " Doors:" + S(doors)}}));
          s.push_back(mk(10, "Inheritance hierarchy: Vehicle (base) → Car (derived). Car object contains a Vehicle sub-object plus its own doors member.", {}));
          return s;
      });

    // 13. Method Overriding
    names("Method Overriding", {"Without virtual (hides, not overrides)", "With virtual (true override)"});
    I("Method Overriding", "No input needed", "",
      {"class Animal {",
       "public:",
       "    void speak() { cout << \"Animal sound\"; }  // non-virtual",
       "};",
       "class Dog : public Animal {",
       "public:",
       "    void speak() { cout << \"Woof!\"; }  // hides base method",
       "};",
       "Animal* a = new Dog();",
       "a->speak();  // prints 'Animal sound' — WRONG (no virtual)"},
      {"What is the difference between hiding and overriding?",
       "Why do you get 'Animal sound' even though a points to a Dog?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "Without 'virtual': base has speak() as a non-virtual method.", {}));
          s.push_back(mk(1, "void speak() in Animal — compiled as a plain method. The call is resolved at COMPILE TIME based on the pointer type (Animal*).", {}));
          s.push_back(mk(3, "Dog::speak() HIDES Animal::speak() — it is a different function with the same name, not an override.", {}));
          s.push_back(mk(5, "Animal* a = new Dog(); — a Dog object, but accessed through an Animal pointer.", {{"pointer type", "Animal*"}, {"actual object", "Dog"}}));
          s.push_back(mk(6, "a->speak() — compiler sees Animal*, looks up Animal::speak() at compile time. Prints 'Animal sound'. The Dog's speak() is never called.", {{"output", "Animal sound"}}));
          s.push_back(mk(6, "This is METHOD HIDING. The derived method shadows the base method but does NOT replace it polymorphically.", {}));
          return s;
      });
    I("Method Overriding#1", "No input needed", "",
      {"class Animal {",
       "public:",
       "    virtual void speak() { cout << \"Animal sound\"; }  // virtual!",
       "};",
       "class Dog : public Animal {",
       "public:",
       "    void speak() override { cout << \"Woof!\"; }",
       "};",
       "Animal* a = new Dog();",
       "a->speak();  // prints 'Woof!' — CORRECT (virtual dispatch)"},
      {"What does the 'override' keyword do? Is it required?",
       "What is early binding vs late binding?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "With 'virtual': speak() is a virtual method in Animal. The call is resolved at RUNTIME (late binding / dynamic dispatch).", {}));
          s.push_back(mk(1, "virtual void speak() — the compiler marks this in the vtable so the right version is called at runtime.", {}));
          s.push_back(mk(3, "void speak() override — 'override' tells the compiler: 'I intend to override a base virtual'. If there is no matching virtual base method, it is a compile error.", {}));
          s.push_back(mk(5, "Animal* a = new Dog(); — Dog object, Animal pointer.", {{"object", "Dog"}, {"pointer", "Animal*"}}));
          s.push_back(mk(6, "a->speak() — runtime checks the vtable of the actual Dog object and calls Dog::speak(). Prints 'Woof!'.", {{"output", "Woof!"}}));
          s.push_back(mk(6, "This is true METHOD OVERRIDING with POLYMORPHISM. Same pointer type, different behaviour based on the actual object — the core of runtime polymorphism.", {}));
          return s;
      });

    // 14. Private Inheritance
    I("Private Inheritance", "No input needed", "",
      {"class Engine {",
       "public:",
       "    void start() { cout << \"Engine started\"; }",
       "    int power()  { return 200; }",
       "};",
       "class Car : private Engine {  // private inheritance",
       "public:",
       "    void drive() { start(); }  // OK: used internally",
       "};",
       "Car c;",
       "c.drive();      // OK",
       "c.start();      // ERROR: start() is private in Car"},
      {"When would you choose private inheritance over composition?",
       "What is the difference between 'Car : public Engine' and 'Car : private Engine'?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "PRIVATE INHERITANCE: the base class's public and protected members become PRIVATE in the derived class.", {}));
          s.push_back(mk(0, "Engine has start() and power() — both public. Car privately inherits Engine.", {}));
          s.push_back(mk(5, "class Car : private Engine — Car IS internally implemented using Engine, but the relationship is hidden.", {}));
          s.push_back(mk(6, "Car::drive() can call start() — OK because drive() is a member of Car and inherits start() privately.", {{"internal", "start() accessible"}}));
          s.push_back(mk(8, "c.drive() — works fine. It calls Engine::start() internally.", {{"output", "Engine started"}}));
          s.push_back(mk(9, "c.start() — COMPILE ERROR. start() is not in Car's public interface (it became private due to private inheritance).", {{"error", "start() is inaccessible"}}));
          s.push_back(mk(9, "Private inheritance models 'implemented-in-terms-of', not 'is-a'. Often composition (HAS-A) is clearer — but private inheritance allows overriding virtual functions from the base.", {}));
          return s;
      });

    // 15. Public Inheritance
    I("Public Inheritance", "Enter salary for Employee/Manager", "50000",
      {"class Employee {",
       "public:",
       "    string name;",
       "    int salary;",
       "    Employee(string n, int s) : name(n), salary(s) {}",
       "    virtual void display() { cout << name << \" : \" << salary; }",
       "};",
       "class Manager : public Employee {",
       "    int teamSize;",
       "public:",
       "    Manager(string n, int s, int t)",
       "        : Employee(n, s), teamSize(t) {}",
       "    void display() override {",
       "        Employee::display();",
       "        cout << \" (team: \" << teamSize << \")\"; }",
       "};"},
      {"What is the Liskov Substitution Principle and how does public inheritance support it?",
       "Why can a Manager* be used wherever an Employee* is expected?"},
      [](const Ints& v) {
          int sal = v.empty() ? 50000 : std::max(1000, v[0]);
          int mgr_sal = sal + 20000;
          Steps s;
          s.push_back(mk(0, "PUBLIC INHERITANCE: the base class's public members remain public in the derived class. The IS-A relationship is preserved. Manager IS-A Employee.", {}));
          s.push_back(mk(1, "Employee has name and salary.", {{"salary", S(sal)}}));
          s.push_back(mk(5, "class Manager : public Employee — Manager publicly inherits Employee. Manager::display() can override Employee::display().", {}));
          s.push_back(mk(10, "Employee e(\"Alice\", " + S(sal) + "); — base object.", {{"name", "Alice"}, {"salary", S(sal)}}));
          s.push_back(mk(10, "Manager m(\"Bob\", " + S(mgr_sal) + ", 8); — derived object. Base constructor called first.", {{"name", "Bob"}, {"salary", S(mgr_sal)}, {"team", "8"}}));
          s.push_back(mk(10, "Employee* p = &m; — a Manager can be used through an Employee pointer (Liskov Substitution Principle).", {{"pointer", "Employee* → Manager object"}}));
          s.push_back(mk(10, "p->display() calls Manager::display() (virtual), prints 'Bob : " + S(mgr_sal) + " (team: 8)'.", {{"output", "Bob : " + S(mgr_sal) + " (team: 8)"}}));
          return s;
      });

    // 16. Multiple Inheritance
    I("Multiple Inheritance", "No input needed", "",
      {"class Flyable {",
       "public:",
       "    virtual void fly() { cout << \"Flying\"; }",
       "};",
       "class Swimmable {",
       "public:",
       "    virtual void swim() { cout << \"Swimming\"; }",
       "};",
       "class Duck : public Flyable, public Swimmable {",
       "public:",
       "    void fly()  override { cout << \"Duck flies!\"; }",
       "    void swim() override { cout << \"Duck swims!\"; }",
       "};",
       "Duck d; d.fly(); d.swim();"},
      {"What is the Diamond Problem in multiple inheritance? How does C++ solve it with virtual base classes?",
       "Name a real-world scenario that naturally models multiple inheritance."},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "MULTIPLE INHERITANCE: a class can inherit from more than one base class. Duck inherits both Flyable and Swimmable.", {}));
          s.push_back(mk(0, "Flyable has fly(). Swimmable has swim(). Both are independent base classes.", {}));
          s.push_back(mk(5, "class Duck : public Flyable, public Swimmable — Duck inherits from both. It gains fly() and swim() from each parent.", {}));
          s.push_back(mk(6, "Duck overrides both methods. It satisfies the IS-A relationship for both interfaces.", {}));
          s.push_back(mk(10, "d.fly() — Duck::fly() is called. Output: 'Duck flies!'", {{"output", "Duck flies!"}}));
          s.push_back(mk(11, "d.swim() — Duck::swim() is called. Output: 'Duck swims!'", {{"output", "Duck swims!"}}));
          s.push_back(mk(11, "DIAMOND PROBLEM: if two bases themselves share a common grandparent, there is ambiguity. Solved with 'virtual' base class inheritance (class A : virtual public Base).", {{"issue", "ambiguous grandparent"}, {"solution", "virtual base class"}}));
          return s;
      });

    /* ===== Module 4: Polymorphism Using C++ ===== */

    // 17. Virtual Functions
    I("Virtual Functions", "No input needed", "",
      {"class Shape {",
       "public:",
       "    virtual double area() = 0;   // pure virtual",
       "    virtual void draw() { cout << \"Drawing shape\"; }",
       "    virtual ~Shape() {}",
       "};",
       "class Circle : public Shape {",
       "    double r;",
       "public:",
       "    Circle(double r) : r(r) {}",
       "    double area() override { return 3.14159 * r * r; }",
       "    void draw()   override { cout << \"Drawing circle r=\" << r; }",
       "};",
       "Shape* s = new Circle(5);",
       "s->draw();  // calls Circle::draw() — virtual dispatch"},
      {"Why must the destructor be virtual when using polymorphism?",
       "What is vtable and vptr?"},
      [](const Ints& v) {
          (void)v;
          double r = 5;
          Steps s;
          s.push_back(mk(0, "VIRTUAL FUNCTION: declared with 'virtual' in the base. The correct version is selected at RUNTIME based on the actual object type (dynamic dispatch).", {}));
          s.push_back(mk(0, "Shape is an abstract base. area() = 0 makes it pure virtual.", {}));
          s.push_back(mk(1, "virtual void draw() — overridable in derived classes, but has a default body.", {}));
          s.push_back(mk(2, "virtual ~Shape() — VIRTUAL DESTRUCTOR. Essential when deleting through a base pointer; ensures the derived destructor is called.", {}));
          s.push_back(mk(5, "Circle stores r = " + S((int)r) + " and overrides both area() and draw().", {{"r", S((int)r)}}));
          s.push_back(mk(11, "Shape* s = new Circle(5); — s points to a Circle object on the heap.", {{"object", "Circle(5)"}, {"pointer type", "Shape*"}}));
          s.push_back(mk(12, "s->draw() — the vptr inside the Circle object points to Circle's vtable. The runtime looks up draw() there and calls Circle::draw().", {{"output", "Drawing circle r=5"}}));
          s.push_back(mk(12, "Without 'virtual', s->draw() would call Shape::draw() — wrong. With virtual, it calls the correct Circle::draw() regardless of pointer type.", {}));
          return s;
      });

    // 18. Pure Virtual Functions
    I("Pure Virtual Functions", "No input needed", "",
      {"class Shape {",
       "public:",
       "    virtual double area() = 0;    // pure virtual",
       "    virtual double perimeter() = 0; // pure virtual",
       "};",
       "// Shape is ABSTRACT — cannot be instantiated",
       "// Shape s;  // ERROR!",
       "class Square : public Shape {",
       "    double side;",
       "public:",
       "    Square(double s) : side(s) {}",
       "    double area()      override { return side * side; }",
       "    double perimeter() override { return 4 * side; }",
       "};",
       "Shape* p = new Square(6);",
       "cout << p->area(); // 36"},
      {"What makes a class abstract? Can an abstract class have a constructor?",
       "Why is '= 0' used for pure virtual functions?"},
      [](const Ints& v) {
          double side = v.empty() ? 6 : std::max(1, v[0]);
          Steps s;
          s.push_back(mk(0, "PURE VIRTUAL FUNCTION: virtual void foo() = 0; — the '= 0' means 'no body defined in this class; derived classes MUST override this'.", {}));
          s.push_back(mk(1, "area() = 0 and perimeter() = 0 — both pure virtual. Any class with at least one pure virtual function is ABSTRACT.", {}));
          s.push_back(mk(5, "Shape is abstract. You cannot create a Shape object — it is just an interface (a contract).", {{"Shape s", "ERROR — abstract"}}));
          s.push_back(mk(6, "Square inherits Shape and must override both area() and perimeter(), or it too becomes abstract.", {}));
          s.push_back(mk(9, "Square(" + std::to_string(side).substr(0,4) + ") — side = " + std::to_string(side).substr(0,4) + ".", {{"side", std::to_string(side).substr(0,4)}}));
          s.push_back(mk(10, "area() = side² = " + std::to_string(side*side).substr(0,7) + ".", {{"area", std::to_string(side*side).substr(0,7)}}));
          s.push_back(mk(11, "perimeter() = 4 × side = " + std::to_string(4*side).substr(0,7) + ".", {{"perimeter", std::to_string(4*side).substr(0,7)}}));
          s.push_back(mk(13, "Shape* p = new Square(6); p->area() = " + std::to_string(side*side).substr(0,7) + ".", {{"output", std::to_string(side*side).substr(0,7)}}));
          return s;
      });

    // 19. Abstract Classes
    T("Abstract Classes", "Type a shape (circle / rectangle / triangle) to see its abstract interface", "circle",
      {"// Abstract class = has at least one pure virtual function",
       "class Shape {",
       "    virtual double area() = 0;",
       "    virtual double perimeter() = 0;",
       "    virtual void draw() = 0;",
       "};",
       "// Concrete classes provide implementations",
       "class Circle : public Shape { ... };",
       "class Rectangle : public Shape { ... };"},
      {"What is the difference between an abstract class and an interface in C++?",
       "Why is an abstract class useful even if it has no data?"},
      [](const std::string& in) {
          std::string w = in;
          for (char& c : w) c = (char)tolower((unsigned char)c);
          Steps s;
          s.push_back(mk(0, "ABSTRACT CLASS: a class with at least one pure virtual function. It defines a contract that derived classes must fulfil.", {}));
          s.push_back(mk(1, "area() = 0, perimeter() = 0, draw() = 0 — three pure virtual functions. Shape cannot be instantiated.", {{"status", "abstract"}}));
          s.push_back(mk(2, "An abstract class acts as an INTERFACE: it promises that any Shape subclass will have area(), perimeter() and draw().", {}));
          if (w.find("circle") != std::string::npos || w.empty()) {
              s.push_back(mk(3, "Circle : public Shape. Provides: area() = π r², perimeter() = 2π r, draw() = render a circle.", {{"area", "π r²"}, {"perimeter", "2π r"}}));
          }
          if (w.find("rect") != std::string::npos) {
              s.push_back(mk(3, "Rectangle : public Shape. Provides: area() = l × w, perimeter() = 2(l+w), draw() = render a rectangle.", {{"area", "l × w"}, {"perimeter", "2(l+w)"}}));
          }
          if (w.find("tri") != std::string::npos) {
              s.push_back(mk(3, "Triangle : public Shape. Provides: area() = ½ b h (Heron's formula), perimeter() = a+b+c, draw() = render a triangle.", {{"area", "½ b h"}}));
          }
          s.push_back(mk(3, "Using Shape* pointers, you can store Circles, Rectangles, Triangles all in one array and call area() uniformly — POLYMORPHISM.", {}));
          s.push_back(mk(3, "If a derived class does not override all pure virtuals, it remains abstract and cannot be instantiated.", {}));
          return s;
      });

    // 20. Dynamic Dispatch
    I("Dynamic Dispatch", "No input needed", "",
      {"class Animal {",
       "public:",
       "    virtual void speak() { cout << \"...\"; }",
       "};",
       "class Dog : public Animal {",
       "public:",
       "    void speak() override { cout << \"Woof!\"; }",
       "};",
       "class Cat : public Animal {",
       "public:",
       "    void speak() override { cout << \"Meow!\"; }",
       "};",
       "Animal* animals[] = { new Dog, new Cat, new Dog };",
       "for (auto* a : animals) a->speak();"},
      {"What is the difference between static dispatch and dynamic dispatch?",
       "What overhead does dynamic dispatch add?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "DYNAMIC DISPATCH: the method call is resolved at RUNTIME based on the actual object type stored at the pointer. Enabled by 'virtual'.", {}));
          s.push_back(mk(0, "Animal has virtual speak() — the vtable mechanism is activated.", {}));
          s.push_back(mk(3, "Dog::speak() → 'Woof!', Cat::speak() → 'Meow!'. Each class has its own vtable with its own function pointers.", {}));
          s.push_back(mk(10, "Animal* animals[] = { new Dog, new Cat, new Dog }; — an array of base pointers, each pointing to a different derived object.", {{"animals[0]", "Dog"}, {"animals[1]", "Cat"}, {"animals[2]", "Dog"}}));
          s.push_back(mk(11, "Loop iteration 1: animals[0] is a Dog. The vptr inside the Dog object leads to Dog's vtable → Dog::speak() → 'Woof!'", {{"output", "Woof!"}}));
          s.push_back(mk(11, "Loop iteration 2: animals[1] is a Cat. vptr → Cat's vtable → Cat::speak() → 'Meow!'", {{"output", "Meow!"}}));
          s.push_back(mk(11, "Loop iteration 3: animals[2] is a Dog again → 'Woof!'", {{"output", "Woof!"}}));
          s.push_back(mk(11, "The same loop code handles Dog, Cat, and any future Animal subclass without modification — OPEN/CLOSED PRINCIPLE.", {}));
          return s;
      });

    // 21. Internal Representation of Method Tables (vtable / vptr)
    I("Internal Representation of Method Tables", "No input needed", "",
      {"// Each class with virtuals gets ONE vtable (a table of function pointers)",
       "// Each object gets ONE vptr pointing to its class's vtable",
       "class A { public: virtual void f(); virtual void g(); };",
       "class B : public A { public: void f() override; };",
       "// A's vtable: [ &A::f, &A::g ]",
       "// B's vtable: [ &B::f, &A::g ]   // B::g not overridden",
       "// Memory per object: [vptr | data members]",
       "// a->f() : load vptr, index 0, call → A::f",
       "// b->f() : load vptr, index 0, call → B::f"},
      {"How many vtables exist for a class hierarchy? How many vptrs per object?",
       "What is the overhead of a virtual call compared to a direct call?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "VTABLE (Virtual Table): a static table of function pointers, one table per class. Created by the compiler.", {}));
          s.push_back(mk(1, "VPTR (Virtual Pointer): a hidden pointer added by the compiler to EVERY object of a class with virtual functions. Points to the class's vtable.", {}));
          s.push_back(mk(2, "class A has virtual f() and g(). A's vtable: slot 0 = &A::f, slot 1 = &A::g.", {{"A's vtable[0]", "&A::f"}, {"A's vtable[1]", "&A::g"}}));
          s.push_back(mk(3, "class B overrides f() but not g(). B's vtable: slot 0 = &B::f (overridden), slot 1 = &A::g (inherited unchanged).", {{"B's vtable[0]", "&B::f"}, {"B's vtable[1]", "&A::g"}}));
          s.push_back(mk(4, "Memory layout of object A: [vptr → A's vtable] [data members...]", {{"object A layout", "[vptr][data]"}}));
          s.push_back(mk(5, "Memory layout of object B: [vptr → B's vtable] [inherited data][B's data]", {{"object B layout", "[vptr][A data][B data]"}}));
          s.push_back(mk(6, "a->f(): load vptr from object, go to A's vtable, read slot 0 (&A::f), call it. Two memory indirections.", {{"steps", "1.load vptr 2.index vtable 3.call"}}));
          s.push_back(mk(7, "b->f() through A*: load vptr (now points to B's vtable), read slot 0 (&B::f), call it → B::f is called. This is dynamic dispatch.", {{"called", "B::f"}}));
          s.push_back(mk(8, "Cost: virtual calls add ~1 extra memory dereference compared to direct calls. Usually negligible except in tight inner loops.", {{"overhead", "~1 pointer dereference"}}));
          return s;
      });

    // 22. RTTI
    I("RTTI", "No input needed", "",
      {"#include <typeinfo>",
       "Animal* a = new Dog();",
       "// typeid: get type name at runtime",
       "cout << typeid(*a).name();      // 'Dog' (mangled)",
       "// dynamic_cast: safe downcasting",
       "Dog* d = dynamic_cast<Dog*>(a); // succeeds",
       "Cat* c = dynamic_cast<Cat*>(a); // returns nullptr",
       "if (d) d->fetch();",
       "if (!c) cout << \"Not a Cat!\";"},
      {"What is the difference between static_cast and dynamic_cast?",
       "When is RTTI disabled and what is the performance impact?"},
      [](const Ints& v) {
          Steps s;
          s.push_back(mk(0, "RTTI (Run-Time Type Information): C++ mechanism to discover the actual type of an object through a base pointer at runtime.", {}));
          s.push_back(mk(0, "#include <typeinfo> enables typeid and dynamic_cast.", {}));
          s.push_back(mk(1, "Animal* a = new Dog(); — a Dog object through an Animal pointer.", {{"object", "Dog"}, {"pointer", "Animal*"}}));
          s.push_back(mk(2, "typeid(*a).name() — dereferences a and asks the vtable for the runtime type. Returns the mangled name for 'Dog'.", {{"output", "Dog (mangled)"}}));
          s.push_back(mk(3, "dynamic_cast<Dog*>(a) — tries to cast a to Dog*. The runtime checks whether the actual object is a Dog (or derived from Dog). Succeeds → returns valid Dog*.", {{"result", "valid Dog*"}}));
          s.push_back(mk(4, "dynamic_cast<Cat*>(a) — the actual object is a Dog, not a Cat. FAILS → returns nullptr (safe failure).", {{"result", "nullptr"}}));
          s.push_back(mk(5, "if (d) d->fetch(); — guard with nullptr check before using the cast pointer.", {}));
          s.push_back(mk(6, "if (!c) cout << \"Not a Cat!\"; — RTTI let us safely detect the wrong type.", {{"output", "Not a Cat!"}}));
          s.push_back(mk(6, "RTTI requires the class to have at least one virtual function (the vtable stores type info). Can be disabled with -fno-rtti for speed-critical code.", {{"cost", "small runtime overhead"}}));
          return s;
      });
}
