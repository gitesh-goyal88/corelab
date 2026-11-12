// Software Development 2 (Semester 2) — Module 7: Introduction to Data Structures
#pragma once
#include "simulations.hpp"
#include "simulations8.hpp"

inline void addSDF2_4(std::map<std::string, Sim>& m) {

    const std::vector<std::string> R = {
        "cppreference: C++ containers|https://en.cppreference.com/w/cpp/container",
        "GeeksforGeeks: Data Structures|https://www.geeksforgeeks.org/data-structures/",
        "VisuAlgo: Data structure visualiser|https://visualgo.net/en"
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

    // Helper: draw a singly linked list using Sk
    auto listDraw = [](const std::vector<std::string>& vals, int head, const std::string& cap,
                       int newSlot = -1, const std::string& newVal = "") -> std::string {
        Sk k;
        const double bw = 44, bh = 32, gap = 22, x0 = 50, y = 100;
        int n = (int)vals.size();
        auto X = [&](int i) { return x0 + i * (bw + gap); };
        for (int i = 0; i < n; ++i) {
            k.Rect(("n" + S(i)).c_str(), X(i), y, bw, bh, "steel2");
            k.T(X(i) + bw / 2, y + bh / 2, vals[i]);
        }
        for (int i = 0; i < n; ++i) {
            if (i + 1 < n)
                k.Arr(X(i) + bw, y + bh * 0.4, X(i + 1), y + bh * 0.4, ("l" + S(i)).c_str());
            else {
                k.Arr(X(i) + bw, y + bh / 2, X(i) + bw + 14, y + bh / 2, ("l" + S(i)).c_str());
                k.T(X(i) + bw + 20, y + bh / 2, "NULL", "start");
            }
        }
        if (head >= 0 && head < n) {
            k.T(X(head) + bw / 2, y - 26, "head");
            k.Arr(X(head) + bw / 2, y - 18, X(head) + bw / 2, y - 1, "head");
        }
        if (!newVal.empty()) {
            int slot = newSlot >= 0 ? newSlot : n;
            double nx = X(std::min(slot, 7)), ny = y - 72;
            k.Rect("nw", nx, ny, bw, bh, "weld");
            k.T(nx + bw / 2, ny + bh / 2, newVal);
            k.T(nx - 6, ny + bh / 2, "new", "end");
        }
        double W = std::max(300.0, X(n) + 60);
        k.T(W / 2, y + bh + 24, cap);
        return k.out((int)W, (int)(y + bh + 50), {});
    };

    // Helper: draw array boxes
    auto arrDraw = [](const Ints& a, int hi, const std::string& nm) -> Step {
        return mkArr(0, "", {}, a, hi);
    };
    (void)arrDraw;

    /* ===== 39. Fundamentals of Data Structures ===== */
    T("Fundamentals of Data Structures", "Type a concept (array / linked list / stack / queue / tree / graph)", "array",
      {"// A data structure organises data for efficient access and modification",
       "// Primitive:  int, char, float, double",
       "// Non-primitive: array, linked list, stack, queue, tree, graph",
       "// Key operations: Traversal, Search, Insertion, Deletion, Sorting",
       "// Choice of structure determines algorithm efficiency"},
      {"Why is choosing the right data structure as important as choosing the right algorithm?",
       "What is the difference between linear and non-linear data structures?"},
      [](const std::string& in) {
          std::string w = in;
          for (char& c : w) c = (char)tolower((unsigned char)c);
          Steps s;
          s.push_back(mk(0, "DATA STRUCTURE: a way of organising, storing and retrieving data so that operations (insert, delete, search) are efficient.", {}));
          s.push_back(mk(1, "PRIMITIVE types (int, char, float): built-in, fixed size, stored directly.", {}));
          s.push_back(mk(2, "NON-PRIMITIVE: built from primitives. LINEAR (array, list, stack, queue) or NON-LINEAR (tree, graph).", {}));
          if (w.find("array") != std::string::npos || w.empty()) {
              s.push_back(mk(3, "ARRAY: contiguous memory, O(1) random access by index, O(n) insert/delete in middle. Fixed size.", {{"access", "O(1)"}, {"insert middle", "O(n)"}}));
          }
          if (w.find("link") != std::string::npos) {
              s.push_back(mk(3, "LINKED LIST: nodes connected by pointers. O(1) insert/delete at head, O(n) access by index. Dynamic size.", {{"insert head", "O(1)"}, {"access", "O(n)"}}));
          }
          if (w.find("stack") != std::string::npos) {
              s.push_back(mk(3, "STACK: LIFO — push and pop from the same end (top). Used in function calls, expression evaluation, undo.", {{"operations", "push/pop O(1)"}}));
          }
          if (w.find("queue") != std::string::npos) {
              s.push_back(mk(3, "QUEUE: FIFO — enqueue at rear, dequeue from front. Used in OS scheduling, BFS, print spoolers.", {{"operations", "enqueue/dequeue O(1)"}}));
          }
          if (w.find("tree") != std::string::npos) {
              s.push_back(mk(3, "TREE: hierarchical structure. Binary Search Tree: O(log n) search/insert/delete average. Used in file systems, databases.", {{"BST ops", "O(log n) avg"}}));
          }
          if (w.find("graph") != std::string::npos) {
              s.push_back(mk(3, "GRAPH: nodes + edges. Models networks, roads, social connections. DFS/BFS traversal O(V+E).", {{"traversal", "O(V+E)"}}));
          }
          s.push_back(mk(4, "Five fundamental operations: Traversal, Search, Insertion, Deletion, Sorting. Time complexity of each determines which structure to use.", {}));
          return s;
      });

    /* ===== 40. Dynamic Memory Allocation (C++) ===== */
    names("Dynamic Memory Allocation (C++)", {"new / delete for single object", "new[] / delete[] for array"});
    I("Dynamic Memory Allocation (C++)", "Enter a size n for a dynamic array", "5",
      {"int* p = new int(42);     // allocate one int on heap",
       "cout << *p;               // 42",
       "delete p;                 // free it",
       "",
       "int* arr = new int[n];    // allocate n ints",
       "for (int i=0; i<n; i++) arr[i] = i*i;",
       "delete[] arr;             // MUST use delete[]"},
      {"What happens if you use 'delete' instead of 'delete[]' on an array?",
       "What is a memory leak? How do smart pointers prevent it?"},
      [](const Ints& v) {
          int n = v.empty() ? 5 : std::max(1, std::min(v[0], 8));
          Ints arr(n);
          for (int i = 0; i < n; ++i) arr[i] = i * i;
          Steps s;
          s.push_back(mk(0, "HEAP vs STACK: local variables are on the stack (auto freed). 'new' allocates on the HEAP — lives until 'delete' is called.", {{"stack", "auto freed"}, {"heap", "manual free"}}));
          s.push_back(mk(0, "int* p = new int(42) — allocates one int on the heap, initialised to 42. p stores the heap address.", {{"*p", "42"}, {"address", "heap"}}));
          s.push_back(mk(1, "*p = 42. Dereference p to access the value.", {{"*p", "42"}}));
          s.push_back(mk(2, "delete p — frees the one int. p is now a dangling pointer. Set p = nullptr after deleting.", {{"heap", "freed"}}));
          s.push_back(mkArr(4, "int* arr = new int[" + S(n) + "] — allocates " + S(n) + " ints on the heap.", {{"n", S(n)}}, Ints(n, 0), -1));
          for (int i = 0; i < n; ++i) {
              Ints cur(n, 0); for (int j = 0; j <= i; ++j) cur[j] = j*j;
              s.push_back(mkArr(5, "arr[" + S(i) + "] = " + S(i) + "² = " + S(i*i) + ".", {{"i", S(i)}, {"arr[i]", S(i*i)}}, cur, i));
          }
          s.push_back(mkArr(6, "delete[] arr — frees ALL n ints. MUST use delete[] (with []) for arrays. Using plain delete gives undefined behaviour.", {{"note", "delete[] for arrays"}}, arr, -1));
          return s;
      });
    I("Dynamic Memory Allocation (C++)#1", "Enter allocation size (1–8)", "4",
      {"// Smart pointers (C++11) — automatic deletion",
       "#include <memory>",
       "auto p = std::make_unique<int>(99);",
       "// p is deleted automatically when it goes out of scope",
       "auto arr = std::make_unique<int[]>(n);",
       "// no delete needed!"},
      {"What is RAII and how do smart pointers implement it?",
       "What is the difference between unique_ptr and shared_ptr?"},
      [](const Ints& v) {
          int n = v.empty() ? 4 : std::max(1, std::min(v[0], 8));
          Steps s;
          s.push_back(mk(0, "SMART POINTERS: wrappers around raw pointers that automatically call delete when they go out of scope. No manual memory management needed.", {}));
          s.push_back(mk(1, "#include <memory> — brings in unique_ptr, shared_ptr, weak_ptr.", {}));
          s.push_back(mk(2, "auto p = std::make_unique<int>(99) — allocates one int (value 99) on the heap. p owns it exclusively.", {{"*p", "99"}, {"owner", "unique_ptr"}}));
          s.push_back(mk(3, "When p goes out of scope (function returns, block ends), unique_ptr's destructor calls delete automatically. RAII.", {{"delete", "automatic"}}));
          s.push_back(mk(4, "auto arr = std::make_unique<int[]>(" + S(n) + ") — dynamic array of " + S(n) + " ints. Freed automatically.", {{"size", S(n)}, {"freed", "automatic"}}));
          s.push_back(mk(4, "unique_ptr: sole owner — cannot be copied, only moved. shared_ptr: reference-counted, multiple owners. weak_ptr: non-owning observer.", {}));
          return s;
      });

    /* ===== 41. Linear and Non-Linear Data Structures ===== */
    T("Linear and Non-Linear Data Structures", "Type linear or nonlinear to explore", "linear",
      {"// LINEAR: elements arranged sequentially",
       "//   Array, Linked List, Stack, Queue, Deque",
       "// NON-LINEAR: elements arranged hierarchically or in a network",
       "//   Tree, Graph, Heap, Trie"},
      {"Give two examples each of linear and non-linear data structures.",
       "Why is a tree considered non-linear?"},
      [](const std::string& in) {
          std::string w = in;
          for (char& c : w) c = (char)tolower((unsigned char)c);
          Steps s;
          s.push_back(mk(0, "LINEAR DATA STRUCTURES: each element has exactly one predecessor and one successor (except the first and last).", {}));
          if (w.find("non") != std::string::npos || w.empty()) {
              s.push_back(mk(0, "NON-LINEAR DATA STRUCTURES: elements may have multiple predecessors or successors — hierarchy or network.", {}));
          }
          if (w.find("non") == std::string::npos || w.empty()) {
              s.push_back(mk(1, "ARRAY: fixed-size, contiguous memory. Sequential access or O(1) random access. Stack/Queue can be built on it.", {{"access", "O(1)"}}));
              s.push_back(mk(1, "LINKED LIST: nodes with data + next pointer. Sequential access only. Dynamic size. O(1) insert/delete at known position.", {{"access", "O(n)"}}));
              s.push_back(mk(1, "STACK (LIFO): push/pop at top. Function call stack, expression evaluation, undo.", {{"policy", "LIFO"}}));
              s.push_back(mk(1, "QUEUE (FIFO): enqueue at rear, dequeue at front. Printer queue, OS scheduler, BFS.", {{"policy", "FIFO"}}));
          }
          if (w.find("non") != std::string::npos || w.empty()) {
              s.push_back(mk(2, "TREE: root node with zero or more children; each node (except root) has exactly one parent. File systems, HTML DOM, decision trees.", {{"shape", "hierarchical"}}));
              s.push_back(mk(2, "GRAPH: nodes (vertices) connected by edges. Can be directed or undirected, weighted or unweighted. Social networks, maps, dependency graphs.", {{"shape", "network"}}));
              s.push_back(mk(2, "HEAP: complete binary tree with heap property (max or min). Used in priority queues, Dijkstra, heap sort.", {{"shape", "semi-structured tree"}}));
          }
          return s;
      });

    /* ===== 42. Limitations of Arrays ===== */
    I("Limitations of Arrays", "Enter array size n and an insert position", "6, 2",
      {"int a[6] = {10, 20, 30, 40, 50};  // 5 used, max 6",
       "// INSERT 99 at position 2 — must shift elements right",
       "for (int i = 4; i >= 2; i--)",
       "    a[i+1] = a[i];   // shift right",
       "a[2] = 99;",
       "// Cost: O(n) shifts. Array is FULL? Must reallocate."},
      {"List five limitations of arrays.",
       "How does std::vector overcome the fixed-size limitation?"},
      [](const Ints& v) {
          int n  = v.size() > 0 ? std::max(3, std::min(v[0], 8)) : 6;
          int pos = v.size() > 1 ? std::max(0, std::min(v[1], n - 2)) : 2;
          Ints a;
          for (int i = 0; i < n - 1; ++i) a.push_back((i + 1) * 10);
          a.push_back(0); // spare slot
          Steps s;
          s.push_back(mkArr(0, "Array of size " + S(n) + " with " + S(n-1) + " elements. Last cell [" + S(n-1) + "] is empty (spare).", {{"size", S(n)}, {"used", S(n-1)}}, a, -1));
          s.push_back(mk(0, "LIMITATION 1: FIXED SIZE — declared at compile time. Cannot grow beyond n = " + S(n) + ". std::vector overcomes this by doubling capacity.", {{"limit", "fixed size"}}));
          s.push_back(mk(0, "LIMITATION 2: WASTED SPACE — if you allocate more than you use (e.g., size=100, use=5), 95 slots are wasted.", {}));
          s.push_back(mk(0, "LIMITATION 3: SLOW INSERT/DELETE in the middle — every element after the position must shift.", {}));

          Ints shifting = a;
          for (int i = n - 2; i >= pos; --i) {
              shifting[i + 1] = shifting[i];
              s.push_back(mkArr(2, "Shift: a[" + S(i+1) + "] = a[" + S(i) + "] = " + S(shifting[i+1]) + ".", {{"i", S(i)}}, shifting, i + 1));
          }
          shifting[pos] = 99;
          s.push_back(mkArr(3, "a[" + S(pos) + "] = 99. Insert complete. Took " + S(n - 1 - pos) + " shifts — O(n).", {{"inserted", "99 at [" + S(pos) + "]"}}, shifting, pos));
          s.push_back(mk(3, "LIMITATION 4: HOMOGENEOUS — all elements must be the same type.", {}));
          s.push_back(mk(3, "LIMITATION 5: NO BUILT-IN BOUNDS CHECKING — a[-1] or a[100] is undefined behaviour in C/C++.", {}));
          return s;
      });

    /* ===== 43. Need for Linked Lists ===== */
    I("Need for Linked Lists", "Enter a value to insert at the front", "99",
      {"struct Node {",
       "    int data;",
       "    Node* next;",
       "};",
       "// Insert at front — O(1), no shifting",
       "Node* newNode = new Node{val, head};",
       "head = newNode;"},
      {"Why is insertion at the head of a linked list O(1) but O(n) for an array?",
       "What overhead does a linked list add compared to an array?"},
      [](const Ints& v) {
          int val = v.empty() ? 99 : v[0];
          std::vector<std::string> existing = {"30", "20", "10"};
          Steps s;
          // Show existing list
          auto dg0 = [&](const std::vector<std::string>& lst, int head, const std::string& cap,
                         int ns = -1, const std::string& nv = "") -> std::string {
              Sk k; int n = (int)lst.size();
              const double bw=44, bh=32, gap=22, x0=50, y=100;
              auto X=[&](int i){ return x0 + i*(bw+gap); };
              for (int i=0;i<n;++i){ k.Rect(("n"+S(i)).c_str(),X(i),y,bw,bh,"steel2"); k.T(X(i)+bw/2,y+bh/2,lst[i]); }
              for (int i=0;i<n;++i){ if(i+1<n) k.Arr(X(i)+bw,y+bh*0.4,X(i+1),y+bh*0.4,("l"+S(i)).c_str()); else { k.Arr(X(i)+bw,y+bh/2,X(i)+bw+14,y+bh/2,("l"+S(i)).c_str()); k.T(X(i)+bw+20,y+bh/2,"NULL","start"); } }
              if(head>=0&&head<n){ k.T(X(head)+bw/2,y-26,"head"); k.Arr(X(head)+bw/2,y-18,X(head)+bw/2,y-1,"head"); }
              if(!nv.empty()){ double nx=X(std::min(ns,7)),ny=y-72; k.Rect("nw",nx,ny,bw,bh,"weld"); k.T(nx+bw/2,ny+bh/2,nv); k.T(nx-6,ny+bh/2,"new","end"); }
              double W=std::max(300.0,X(n)+60); k.T(W/2,y+bh+24,cap);
              return k.out((int)W,(int)(y+bh+50),{});
          };

          Step s0 = mk(0, "Existing list: 30 → 20 → 10 → NULL. Each node stores data + a pointer to the next node.", {{"head", "30"}});
          s0.dg = dg0(existing, 0, "Before insert");
          s.push_back(s0);

          Step s1 = mk(1, "WHY LINKED LIST? Problem with arrays: inserting at front requires O(n) shifts. Linked list: O(1) — just adjust pointers.", {{"array cost", "O(n)"}, {"list cost", "O(1)"}});
          s1.dg = dg0(existing, 0, "Need: insert " + S(val) + " at front");
          s.push_back(s1);

          Step s2 = mk(5, "Node* newNode = new Node{" + S(val) + ", head}; — allocate new node with data=" + S(val) + " and next=head (points to 30).", {{"newNode.data", S(val)}, {"newNode.next", "→30"}});
          s2.dg = dg0(existing, 0, "new node created", 0, S(val));
          s.push_back(s2);

          existing.insert(existing.begin(), S(val));
          Step s3 = mk(6, "head = newNode; — head now points to the new node. No shifting, no resizing. O(1).", {{"head", S(val)}, {"cost", "O(1)"}});
          s3.dg = dg0(existing, 0, "After insert: " + S(val) + " → 30 → 20 → 10 → NULL");
          s.push_back(s3);

          s.push_back(mk(6, "KEY ADVANTAGE: dynamic size (grows/shrinks freely), O(1) insert/delete at any known position, no wasted preallocated space.", {}));
          s.push_back(mk(6, "KEY DISADVANTAGE: O(n) access by index (must traverse), extra memory per node (the pointer), poor cache locality.", {}));
          return s;
      });

    /* ===== 44. Types of Linked Lists ===== */
    names("Types of Linked Lists", {"Singly linked list", "Doubly linked list", "Circular linked list"});
    I("Types of Linked Lists", "Enter up to 4 values", "10, 20, 30, 40",
      {"// Singly Linked List: each node has one 'next' pointer",
       "struct SNode { int data; SNode* next; };",
       "// Traversal: O(n) forward only"},
      {"LeetCode #206 (Reverse Linked List)",
       "What is the advantage of a doubly linked list over a singly linked list?"},
      [listDraw](const Ints& v) {
          Ints items = v.empty() ? Ints{10,20,30,40} : v;
          if ((int)items.size() > 6) items.resize(6);
          Steps s;
          std::vector<std::string> strs;
          for (int x : items) strs.push_back(S(x));

          Step s0 = mk(0, "SINGLY LINKED LIST: each node stores data and a 'next' pointer. Traversal is FORWARD only.", {{"direction", "forward only"}});
          s0.dg = listDraw(strs, 0, "Singly Linked List");
          s.push_back(s0);

          for (int i = 0; i < (int)items.size(); ++i) {
              Step si = mk(1, "Visit node " + S(i) + ": data = " + S(items[i]) + ". Follow next pointer.", {{"i", S(i)}, {"data", S(items[i])}});
              si.dg = listDraw(strs, i, "Traversing — at node " + S(i));
              s.push_back(si);
          }
          Step sn = mk(1, "node.next == NULL — end of list. Traversal complete in O(n).", {{"cost", "O(n)"}});
          sn.dg = listDraw(strs, 0, "Traversal complete");
          s.push_back(sn);
          return s;
      });
    I("Types of Linked Lists#1", "Enter up to 4 values", "10, 20, 30, 40",
      {"// Doubly Linked List: each node has 'prev' AND 'next'",
       "struct DNode { int data; DNode* prev; DNode* next; };",
       "// Traverse forward AND backward",
       "// Delete node given its pointer: O(1)"},
      {"LeetCode #707 (Design Linked List)",
       "Why is delete O(1) for a doubly linked list but O(n) for singly?"},
      [](const Ints& v) {
          Ints items = v.empty() ? Ints{10,20,30,40} : v;
          if ((int)items.size() > 6) items.resize(6);
          int n = (int)items.size();
          Steps s;
          // Draw doubly linked list using Sk
          auto drawDL = [&](int hi, const std::string& cap) -> std::string {
              Sk k; const double bw=44, bh=32, gap=28, x0=50, y=110;
              auto X=[&](int i){ return x0 + i*(bw+gap); };
              for (int i=0;i<n;++i){ k.Rect(("n"+S(i)).c_str(),X(i),y,bw,bh,i==hi?"weld":"steel2"); k.T(X(i)+bw/2,y+bh/2,S(items[i])); }
              for (int i=0;i<n-1;++i){
                  k.Arr(X(i)+bw, y+bh*0.3, X(i+1), y+bh*0.3, ("fwd"+S(i)).c_str()); // forward
                  k.Arr(X(i+1), y+bh*0.7, X(i)+bw, y+bh*0.7, ("bwd"+S(i)).c_str()); // backward
              }
              // NULL at ends
              k.Arr(X(0),y+bh/2,X(0)-14,y+bh/2,"ll"); k.T(X(0)-20,y+bh/2,"NULL","end");
              k.Arr(X(n-1)+bw,y+bh/2,X(n-1)+bw+14,y+bh/2,"lr"); k.T(X(n-1)+bw+20,y+bh/2,"NULL","start");
              k.T(X(0)+bw/2,y-26,"head"); k.Arr(X(0)+bw/2,y-18,X(0)+bw/2,y-1,"hd");
              k.T(X(n-1)+bw/2,y-26,"tail"); k.Arr(X(n-1)+bw/2,y-18,X(n-1)+bw/2,y-1,"tl");
              double W=std::max(300.0,X(n)+60); k.T(W/2,y+bh+30,cap);
              return k.out((int)W,(int)(y+bh+56),{});
          };
          Step s0 = mk(0, "DOUBLY LINKED LIST: each node has 'prev' (previous) AND 'next' pointers. Traverse both directions.", {{"direction", "forward and backward"}});
          s0.dg = drawDL(-1, "Doubly Linked List");
          s.push_back(s0);
          Step sf = mk(1, "Forward traversal: head → " + [&]{ std::string r; for (int x : items) r += S(x) + " → "; return r + "NULL"; }(), {});
          sf.dg = drawDL(0, "Forward traversal");
          s.push_back(sf);
          Step sb = mk(1, "Backward traversal via 'prev': tail → " + [&]{ std::string r; for (int i=n-1;i>=0;--i) r += S(items[i]) + " → "; return r + "NULL"; }(), {});
          sb.dg = drawDL(n-1, "Backward traversal");
          s.push_back(sb);
          s.push_back(mk(1, "Delete a known node p: p->prev->next = p->next; p->next->prev = p->prev; delete p; — just 2 pointer updates, O(1). No need to find the predecessor (unlike singly).", {{"delete", "O(1) with pointer"}}));
          return s;
      });
    I("Types of Linked Lists#2", "Enter up to 4 values", "10, 20, 30, 40",
      {"// Circular Linked List: last node's next points back to head",
       "// Circular Doubly: also last->next = head, head->prev = last",
       "// Use case: round-robin scheduling, circular buffer"},
      {"LeetCode #708 (Insert into a Sorted Circular Linked List)",
       "What is a circular buffer and how does it use a circular linked list?"},
      [](const Ints& v) {
          Ints items = v.empty() ? Ints{10,20,30,40} : v;
          if ((int)items.size() > 6) items.resize(6);
          int n = (int)items.size();
          Steps s;
          auto drawCL = [&](int hi, const std::string& cap) -> std::string {
              Sk k; const double bw=44, bh=32, gap=28, x0=50, y=110;
              auto X=[&](int i){ return x0 + i*(bw+gap); };
              for (int i=0;i<n;++i){ k.Rect(("n"+S(i)).c_str(),X(i),y,bw,bh,i==hi?"weld":"steel2"); k.T(X(i)+bw/2,y+bh/2,S(items[i])); }
              for (int i=0;i<n-1;++i) k.Arr(X(i)+bw,y+bh*0.4,X(i+1),y+bh*0.4,("l"+S(i)).c_str());
              // Last node curves back to head
              double lx=X(n-1)+bw, hy=y+bh+28, hx=x0+bw/2;
              k.W("circ", {lx, y+bh/2, lx+10, y+bh/2, lx+10, hy, hx, hy, hx, y+bh});
              k.Arr(hx, y+bh+2, hx, y+bh-2, "circ");
              k.T(X(0)+bw/2,y-26,"head"); k.Arr(X(0)+bw/2,y-18,X(0)+bw/2,y-1,"hd");
              double W=std::max(300.0,X(n)+60); k.T(W/2,y+bh+56,cap);
              return k.out((int)W,(int)(y+bh+76),{});
          };
          Step s0 = mk(0, "CIRCULAR LINKED LIST: last node's 'next' points back to the HEAD, forming a ring. No NULL at the end.", {{"last.next", "→ head"}});
          s0.dg = drawCL(-1, "Circular Linked List");
          s.push_back(s0);
          Step s1 = mk(0, "To detect the end during traversal: start at head, stop when next == head again.", {{"termination", "current == head"}});
          s1.dg = drawCL(0, "Starting traversal from head");
          s.push_back(s1);
          s.push_back(mk(0, "USE CASES: Round-robin CPU scheduling (each process gets a turn, looping), Circular buffers in audio/network streams, Multiplayer board games (each player takes a turn).", {{"use", "round-robin, buffer"}}));
          return s;
      });

    /* ===== 45. Stack (SDF2) ===== */
    names("Stack (SDF2)", {"Array-based stack", "Linked-list-based stack"});
    I("Stack (SDF2)", "Enter up to 6 values to push, then one to pop", "5, 10, 3, 8",
      {"// Stack: LIFO — Last In, First Out",
       "int stk[MAX]; int top = -1;",
       "void push(int x) { stk[++top] = x; }",
       "int  pop()       { return stk[top--]; }",
       "int  peek()      { return stk[top]; }",
       "bool isEmpty()   { return top == -1; }"},
      {"LeetCode #20 (Valid Parentheses)",
       "LeetCode #155 (Min Stack)"},
      [](const Ints& v) {
          Ints items = v.empty() ? Ints{5,10,3,8} : v;
          if ((int)items.size() > 6) items.resize(6);
          Steps s;
          s.push_back(mk(0, "STACK: LIFO (Last In, First Out). Push adds to top, Pop removes from top. Used in function calls, expression evaluation, undo.", {{"policy", "LIFO"}}));
          s.push_back(mk(1, "int stk[MAX]; int top = -1; — top = -1 means empty. top tracks the index of the topmost element.", {{"top", "-1 (empty)"}}));

          Ints stk;
          for (int i = 0; i < (int)items.size(); ++i) {
              stk.push_back(items[i]);
              Step st = mkArr(2, "push(" + S(items[i]) + "): stk[" + S(i) + "] = " + S(items[i]) + ", top = " + S(i) + ".", {{"top", S(i)}, {"pushed", S(items[i])}}, stk, i);
              st.nm = "stk";
              s.push_back(st);
          }
          if (!stk.empty()) {
              int popped = stk.back(); stk.pop_back();
              int newTop = (int)stk.size() - 1;
              Step sp = mkArr(3, "pop(): returns stk[" + S(newTop+1) + "] = " + S(popped) + ", top decrements to " + S(newTop) + ".", {{"popped", S(popped)}, {"top", S(newTop)}}, stk, -1);
              sp.nm = "stk";
              s.push_back(sp);
              if (!stk.empty()) {
                  Step sk = mkArr(4, "peek(): returns stk[" + S(newTop) + "] = " + S(stk.back()) + " WITHOUT removing it.", {{"peek", S(stk.back())}}, stk, (int)stk.size()-1);
                  sk.nm = "stk";
                  s.push_back(sk);
              }
          }
          s.push_back(mk(5, "isEmpty(): top == -1 → " + std::string(stk.empty() ? "true (stack empty)." : "false (stack has elements)."), {{"empty", stk.empty() ? "true" : "false"}}));
          return s;
      });

    /* ===== 46. Queue (SDF2) ===== */
    names("Queue (SDF2)", {"Array-based queue (circular)", "Linked-list-based queue"});
    I("Queue (SDF2)", "Enter up to 5 values to enqueue", "5, 10, 3, 8, 2",
      {"// Queue: FIFO — First In, First Out",
       "int q[MAX]; int front = 0, rear = -1, size = 0;",
       "void enqueue(int x) { q[++rear % MAX] = x; size++; }",
       "int  dequeue()      { size--; return q[front++ % MAX]; }",
       "bool isEmpty()      { return size == 0; }"},
      {"LeetCode #232 (Implement Queue using Stacks)",
       "LeetCode #933 (Number of Recent Calls)"},
      [](const Ints& v) {
          Ints items = v.empty() ? Ints{5,10,3,8,2} : v;
          if ((int)items.size() > 6) items.resize(6);
          Steps s;
          s.push_back(mk(0, "QUEUE: FIFO (First In, First Out). Enqueue at REAR, Dequeue from FRONT. Like a real queue — first to arrive, first to leave.", {{"policy", "FIFO"}}));
          s.push_back(mk(1, "front = 0, rear = -1, size = 0. Empty queue.", {{"front", "0"}, {"rear", "-1"}, {"size", "0"}}));

          Ints q;
          for (int i = 0; i < (int)items.size(); ++i) {
              q.push_back(items[i]);
              Step st = mkArr(2, "enqueue(" + S(items[i]) + "): q[" + S(i) + "] = " + S(items[i]) + ", rear = " + S(i) + ", size = " + S(i+1) + ".", {{"rear", S(i)}, {"size", S(i+1)}}, q, i);
              st.nm = "q";
              s.push_back(st);
          }
          if (!q.empty()) {
              int front_val = q.front();
              Ints q2(q.begin() + 1, q.end());
              Step sd = mkArr(3, "dequeue(): removes q[front] = " + S(front_val) + " (the OLDEST element). front advances.", {{"dequeued", S(front_val)}, {"size", S((int)q2.size())}}, q2, 0);
              sd.nm = "q";
              s.push_back(sd);
          }
          s.push_back(mk(4, "isEmpty(): size == 0 → false. Applications: printer spooler, OS process scheduling, BFS traversal.", {{"empty", "false"}}));
          return s;
      });

    /* ===== 47. Stack and Queue Operations Using STL ===== */
    I("Stack and Queue Operations Using STL", "Enter up to 5 values", "3, 1, 4, 1, 5",
      {"#include <stack>",
       "#include <queue>",
       "std::stack<int> s;",
       "s.push(3); s.push(1); s.push(4);",
       "cout << s.top();  // 4 (LIFO)",
       "s.pop();",
       "std::queue<int> q;",
       "q.push(3); q.push(1); q.push(4);",
       "cout << q.front(); // 3 (FIFO)",
       "q.pop();"},
      {"LeetCode #232 (Implement Queue using Stacks)",
       "What underlying container does std::stack use by default?"},
      [](const Ints& v) {
          Ints items = v.empty() ? Ints{3,1,4,1,5} : v;
          if ((int)items.size() > 6) items.resize(6);
          Steps s;
          s.push_back(mk(0, "STL provides std::stack and std::queue as CONTAINER ADAPTERS — they wrap an underlying container (default: deque) and restrict operations.", {}));
          s.push_back(mk(1, "#include <stack> and #include <queue>.", {}));
          // Stack demo
          Ints stk;
          s.push_back(mk(2, "std::stack<int> s; — empty stack.", {{"s.size()", "0"}}));
          for (int x : items) {
              stk.push_back(x);
              Step st = mkArr(3, "s.push(" + S(x) + ") — top = " + S(x) + ".", {{"top", S(x)}, {"size", S((int)stk.size())}}, stk, (int)stk.size()-1);
              st.nm = "stk";
              s.push_back(st);
          }
          s.push_back(mk(4, "s.top() = " + S(stk.back()) + " — peek without removing. s.pop() removes top.", {{"s.top()", S(stk.back())}}));

          // Queue demo
          s.push_back(mk(5, "std::queue<int> q; — empty queue.", {{"q.size()", "0"}}));
          Ints q;
          for (int x : items) {
              q.push_back(x);
              Step qt = mkArr(6, "q.push(" + S(x) + ") — rear grows.", {{"front", S(q.front())}, {"size", S((int)q.size())}}, q, (int)q.size()-1);
              qt.nm = "q";
              s.push_back(qt);
          }
          s.push_back(mk(7, "q.front() = " + S(q.front()) + " — peek at front without removing.", {{"q.front()", S(q.front())}}));
          int deq = q.front(); q.erase(q.begin());
          Step qd = mkArr(8, "q.pop() — removes front element " + S(deq) + ". New front = " + (q.empty() ? "none" : S(q.front())) + ".", {{"removed", S(deq)}}, q, 0);
          qd.nm = "q";
          s.push_back(qd);
          return s;
      });

    /* ===== 48. Recursion (SDF2) ===== */
    names("Recursion (SDF2)", {"Factorial", "Fibonacci"});
    I("Recursion (SDF2)", "Enter n (0 to 10)", "5",
      {"int factorial(int n) {",
       "    if (n == 0) return 1;    // base case",
       "    return n * factorial(n-1); // recursive case",
       "}",
       "// factorial(5) = 5 * 4 * 3 * 2 * 1 = 120"},
      {"LeetCode #509 (Fibonacci Number)",
       "LeetCode #206 (Reverse Linked List) — recursive version"},
      [](const Ints& v) {
          int n = v.empty() ? 5 : std::max(0, std::min(v[0], 10));
          Steps s;
          s.push_back(mk(0, "RECURSION: a function that calls itself with a smaller problem. Requires a BASE CASE to stop. Maintains a call stack.", {}));
          s.push_back(mk(1, "Base case: factorial(0) = 1. Without a base case, recursion is infinite.", {{"base case", "n == 0 → return 1"}}));

          // Descent
          std::vector<std::string> callStack;
          for (int i = n; i >= 0; --i) {
              callStack.push_back("factorial(" + S(i) + ")");
              Step si = mk(i == 0 ? 1 : 2, i == 0 ? "BASE CASE reached: factorial(0) = 1. Recursion unwinds now." : "factorial(" + S(i) + ") calls factorial(" + S(i-1) + "). Push frame onto call stack.", {{"n", S(i)}});
              si.stack = callStack;
              s.push_back(si);
          }

          // Ascent
          long long result = 1;
          for (int i = 1; i <= n; ++i) {
              callStack.pop_back();
              result *= i;
              Step si = mk(2, "Return to factorial(" + S(i) + "): " + S(i) + " × " + S(result/i) + " = " + S(result) + ". Pop frame.", {{"n", S(i)}, {"result so far", S(result)}});
              si.stack = callStack;
              s.push_back(si);
          }
          s.push_back(mk(2, "Final answer: factorial(" + S(n) + ") = " + S(result) + ". Time O(n), Space O(n) for the call stack.", {{"factorial(" + S(n) + ")", S(result)}}));
          return s;
      });
    I("Recursion (SDF2)#1", "Enter n (0 to 12)", "7",
      {"int fib(int n) {",
       "    if (n <= 1) return n;",
       "    return fib(n-1) + fib(n-2);",
       "}",
       "// fib(0)=0, fib(1)=1, fib(2)=1, fib(3)=2, ..."},
      {"LeetCode #509 (Fibonacci Number)",
       "Why is the naive recursive Fibonacci O(2^n)? How does memoisation fix it?"},
      [](const Ints& v) {
          int n = v.empty() ? 7 : std::max(0, std::min(v[0], 12));
          Steps s;
          // Compute fib values
          std::vector<long long> fib(n + 1);
          fib[0] = 0; if (n > 0) fib[1] = 1;
          for (int i = 2; i <= n; ++i) fib[i] = fib[i-1] + fib[i-2];

          s.push_back(mk(0, "FIBONACCI via recursion: fib(n) = fib(n-1) + fib(n-2). Two recursive calls per invocation.", {}));
          s.push_back(mk(1, "Base case: fib(0) = 0, fib(1) = 1.", {{"fib(0)", "0"}, {"fib(1)", "1"}}));

          Vars chain;
          for (int i = 0; i <= n; ++i) chain.push_back({"fib(" + S(i) + ")", S(fib[i])});
          s.push_back(mk(2, "Computing fib(" + S(n) + ") = " + S(fib[n]) + ". The recursion tree has 2 branches at each node.", chain));
          s.push_back(mk(2, "PROBLEM: fib(n-2) is computed again in the right branch — REPEATED SUBPROBLEMS. Time = O(2^n).", {{"calls for fib(" + S(n) + ")", "~" + S((int)std::pow(2, n))}}));
          s.push_back(mk(2, "FIX (Memoisation): store computed values in a table. Each subproblem solved once → O(n) time, O(n) space.", {{"memoised complexity", "O(n)"}}));
          s.push_back(mk(2, "FIX (Bottom-up DP): compute fib(0), fib(1), ... iteratively. O(n) time, O(1) space.", {{"iterative complexity", "O(n) time, O(1) space"}}));
          return s;
      });

    /* ===== 49. Recursion Removal Using Stack ===== */
    I("Recursion Removal Using Stack", "Enter n for iterative factorial using an explicit stack", "5",
      {"// Every recursive call can be simulated with an explicit stack",
       "// Iterative factorial using a manual stack:",
       "int iterFactorial(int n) {",
       "    std::stack<int> stk;",
       "    while (n > 0) stk.push(n--);",
       "    int result = 1;",
       "    while (!stk.empty()) {",
       "        result *= stk.top(); stk.pop();",
       "    }",
       "    return result;",
       "}"},
      {"Why would you replace recursion with an explicit stack?",
       "What is tail recursion and how can the compiler optimise it?"},
      [](const Ints& v) {
          int n = v.empty() ? 5 : std::max(1, std::min(v[0], 10));
          Steps s;
          s.push_back(mk(0, "RECURSION REMOVAL: every recursive function can be rewritten using an explicit stack that mimics the call stack. Avoids stack overflow for deep recursion.", {}));
          s.push_back(mk(1, "Phase 1 — push values n down to 1 onto the stack (simulating the descent of recursive calls).", {}));

          Ints stk;
          for (int i = n; i >= 1; --i) {
              stk.insert(stk.begin(), i);
              Step st = mkArr(2, "push(" + S(i) + ") — simulates the recursive call frame for n=" + S(i) + ".", {{"n", S(i)}}, stk, 0);
              st.nm = "stk";
              s.push_back(st);
          }

          s.push_back(mk(5, "Phase 2 — pop values and multiply (simulating the return/unwind).", {}));
          long long result = 1;
          while (!stk.empty()) {
              int top = stk.front(); stk.erase(stk.begin());
              result *= top;
              Step st = mkArr(6, "pop() = " + S(top) + ". result = result × " + S(top) + " = " + S(result) + ".", {{"popped", S(top)}, {"result", S(result)}}, stk, -1);
              st.nm = "stk";
              s.push_back(st);
          }
          s.push_back(mk(7, "Result = " + S(result) + " = " + S(n) + "!. Same as recursive factorial. Explicit stack avoids system call-stack overflow.", {{"result", S(result)}}));
          return s;
      });

    /* ===== 50. Tower of Hanoi ===== */
    I("Tower of Hanoi", "Enter number of disks (1 to 7)", "3",
      {"void hanoi(int n, char from, char to, char aux) {",
       "    if (n == 1) {",
       "        cout << from << \" -> \" << to;",
       "        return;",
       "    }",
       "    hanoi(n-1, from, aux, to);  // move n-1 to aux",
       "    cout << from << \" -> \" << to; // move disk n",
       "    hanoi(n-1, aux, to, from);  // move n-1 to to",
       "}"},
      {"LeetCode #780 (Reaching Points) — recursive thinking.",
       "How many moves does Hanoi(n) take? Prove it."},
      [](const Ints& v) {
          int n = v.empty() ? 3 : std::max(1, std::min(v[0], 7));
          Steps s;
          s.push_back(mk(0, "TOWER OF HANOI: move " + S(n) + " disk(s) from peg A to peg C using peg B as auxiliary. Rule: never place a larger disk on a smaller one.", {{"disks", S(n)}, {"total moves", S((1 << n) - 1)}}));
          s.push_back(mk(1, "Base case: n == 1 — move the single disk from 'from' to 'to' directly.", {}));

          struct Move { std::string from, to; };
          std::vector<Move> moves;
          std::function<void(int, char, char, char)> solve = [&](int disks, char from, char to, char aux) {
              if (disks == 1) { moves.push_back({std::string(1,from), std::string(1,to)}); return; }
              solve(disks - 1, from, aux, to);
              moves.push_back({std::string(1,from), std::string(1,to)});
              solve(disks - 1, aux, to, from);
          };
          solve(n, 'A', 'C', 'B');

          std::vector<std::string> callStk;
          for (int i = 0; i < (int)moves.size(); ++i) {
              Step si = mk(i == 0 ? 2 : (i == (int)moves.size()/2 ? 3 : 5),
                           "Move " + S(i+1) + " of " + S((int)moves.size()) + ": Disk " + moves[i].from + " → " + moves[i].to + ".",
                           {{"move", S(i+1)}, {"from", moves[i].from}, {"to", moves[i].to}});
              s.push_back(si);
          }
          s.push_back(mk(5, "Total moves: 2^" + S(n) + " - 1 = " + S((1 << n) - 1) + ". Each disk doubles the number of moves. Time complexity: O(2^n).", {{"total", S((1<<n)-1)}, {"complexity", "O(2^n)"}}));
          return s;
      });

    /* ===== 51. N-Queens Problem ===== */
    I("N-Queens Problem", "Enter board size N (4 to 8)", "5",
      {"// Place N queens on an N×N board — no two attack each other",
       "void solve(int row, vector<int>& col) {",
       "    if (row == N) { printSolution(col); return; }",
       "    for (int c = 0; c < N; c++) {",
       "        if (isSafe(row, c, col)) {",
       "            col[row] = c;",
       "            solve(row + 1, col);",
       "            col[row] = -1;  // backtrack",
       "        }",
       "    }",
       "}"},
      {"LeetCode #51 (N-Queens)",
       "LeetCode #52 (N-Queens II)"},
      [](const Ints& v) {
          int N = v.empty() ? 5 : std::max(4, std::min(v[0], 8));
          Steps s;
          s.push_back(mk(0, "N-QUEENS: place N queens on an N×N board so no two queens are in the same row, column, or diagonal. Classic backtracking problem.", {{"N", S(N)}}));
          s.push_back(mk(1, "Strategy: place queens one row at a time, left to right, checking safety at each column.", {}));
          s.push_back(mk(2, "isSafe(row, col): check that no previously placed queen shares the column or diagonal.", {}));

          // Find first solution via backtracking
          std::vector<int> colPlaced(N, -1);
          std::vector<std::pair<int,int>> trace;
          std::vector<int> sol;
          bool found = false;
          std::function<void(int)> bt = [&](int row) {
              if (found) return;
              if (row == N) { sol = colPlaced; found = true; return; }
              for (int c = 0; c < N && !found; ++c) {
                  bool safe = true;
                  for (int r2 = 0; r2 < row; ++r2) {
                      if (colPlaced[r2] == c || std::abs(colPlaced[r2] - c) == std::abs(r2 - row)) { safe = false; break; }
                  }
                  if (safe) { colPlaced[row] = c; trace.push_back({row, c}); bt(row + 1); if (!found) { colPlaced[row] = -1; trace.push_back({-(row+1), c}); } }
              }
          };
          bt(0);

          int shown = 0;
          for (auto& t : trace) {
              if (shown++ > 14) break;
              if (t.first >= 0) {
                  std::vector<std::string> mat(N * N, ".");
                  for (int r = 0; r <= t.first; ++r) if (colPlaced[r] >= 0) mat[r * N + colPlaced[r]] = "Q";
                  Step si = mk(t.first == 0 ? 3 : 4, t.first == 0 ? "Row 0: Try queen at column " + S(t.second) + " — safe, place it." : "Row " + S(t.first) + ", col " + S(t.second) + " — safe, place queen.", {{"row", S(t.first)}, {"col", S(t.second)}});
                  si.mat = mat; si.mc = N;
                  s.push_back(si);
              } else {
                  Step sb = mk(6, "Backtrack from row " + S(-t.first - 1) + " — no safe column found. Remove queen, try next column.", {{"backtrack from row", S(-t.first - 1)}});
                  s.push_back(sb);
              }
          }
          if (found) {
              std::vector<std::string> mat(N * N, ".");
              for (int r = 0; r < N; ++r) mat[r * N + sol[r]] = "Q";
              Step sf = mk(7, "SOLUTION FOUND: " + S(N) + " queens placed safely.", {{"result", "valid placement"}});
              sf.mat = mat; sf.mc = N;
              s.push_back(sf);
          }
          return s;
      });

    /* ===== 52. Rat in a Maze ===== */
    I("Rat in a Maze", "Enter maze size (3 to 6) — maze is auto-generated", "4",
      {"// Find a path from (0,0) to (N-1,N-1) in a grid",
       "// 1 = open cell, 0 = wall",
       "bool solve(int x, int y, vector<vector<int>>& maze, vector<vector<int>>& sol) {",
       "    if (x == N-1 && y == N-1) { sol[x][y]=1; return true; }",
       "    if (isSafe(x, y, maze)) {",
       "        sol[x][y] = 1;  // mark",
       "        if (solve(x+1, y, ...) || solve(x, y+1, ...)) return true;",
       "        sol[x][y] = 0;  // backtrack",
       "    }",
       "    return false;",
       "}"},
      {"LeetCode #62 (Unique Paths)",
       "LeetCode #79 (Word Search) — same backtracking idea"},
      [](const Ints& v) {
          int N = v.empty() ? 4 : std::max(3, std::min(v[0], 6));
          // Build a solvable maze
          std::vector<std::vector<int>> maze(N, std::vector<int>(N, 1));
          // Add a few walls
          if (N >= 4) { maze[1][1] = 0; maze[2][3 % N] = 0; }
          if (N >= 5) { maze[3][1] = 0; }

          Steps s;
          auto matStr = [&](const std::vector<std::vector<int>>& sol, int cx, int cy) {
              std::vector<std::string> m;
              for (int r = 0; r < N; ++r)
                  for (int c = 0; c < N; ++c) {
                      if (r == cx && c == cy) m.push_back("R"); // rat
                      else if (sol[r][c] == 1) m.push_back("*");
                      else if (maze[r][c] == 0) m.push_back("#");
                      else m.push_back(".");
                  }
              return m;
          };

          s.push_back(mk(0, "RAT IN A MAZE: find a path from top-left (0,0) to bottom-right (" + S(N-1) + "," + S(N-1) + "). '.' = open, '#' = wall, 'R' = rat, '*' = path.", {{"N", S(N)}}));
          {
              std::vector<std::vector<int>> sol(N, std::vector<int>(N, 0));
              Step si = mk(0, "Initial maze: 1=open, 0=wall.", {}); si.mat = matStr(sol, 0, 0); si.mc = N;
              s.push_back(si);
          }

          // Solve with trace
          std::vector<std::vector<int>> sol(N, std::vector<int>(N, 0));
          std::vector<std::tuple<int,int,bool>> trace; // x, y, forward/backtrack
          std::function<bool(int,int)> bt = [&](int x, int y) -> bool {
              if (x < 0 || y < 0 || x >= N || y >= N || maze[x][y] == 0 || sol[x][y] == 1) return false;
              sol[x][y] = 1; trace.push_back({x, y, true});
              if (x == N-1 && y == N-1) return true;
              if (bt(x+1, y) || bt(x, y+1) || bt(x-1, y) || bt(x, y-1)) return true;
              sol[x][y] = 0; trace.push_back({x, y, false});
              return false;
          };
          bt(0, 0);

          int shown = 0;
          for (auto& [x, y, fwd] : trace) {
              if (shown++ > 16) break;
              std::vector<std::vector<int>> curSol = sol; // final sol for display
              // Rebuild current state from trace up to this point
              std::vector<std::vector<int>> partSol(N, std::vector<int>(N, 0));
              int k = 0;
              for (auto& [tx, ty, tfwd] : trace) {
                  if (k++ > shown) break;
                  if (tfwd) partSol[tx][ty] = 1;
                  else partSol[tx][ty] = 0;
              }
              Step si = mk(fwd ? 4 : 7, fwd ? "Move to (" + S(x) + "," + S(y) + ") — open cell, mark it." : "BACKTRACK from (" + S(x) + "," + S(y) + ") — dead end, unmark.", {{"pos", "(" + S(x) + "," + S(y) + ")"}});
              si.mat = matStr(partSol, x, y); si.mc = N;
              s.push_back(si);
          }
          Step sf = mk(3, "PATH FOUND! Route from (0,0) to (" + S(N-1) + "," + S(N-1) + ") marked with '*'.", {{"result", "path found"}});
          sf.mat = matStr(sol, N-1, N-1); sf.mc = N;
          s.push_back(sf);
          return s;
      });
}
