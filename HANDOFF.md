# CoreLab: handoff notes (read this first)

CoreLab is a learning app for JIIT Sem 1-3 (Semester -> Subject -> Topic -> simulation -> references/practice).
Backend: C++17, no libraries (raw sockets). Frontend: one HTML/JS page. Run: `make && ./corelab 8080`, open the printed URL.

## Files (src/)
content.hpp (curriculum: semesters, subjects, topics, groups) | http_server.hpp (hardened server) | simulations.hpp (core types, JSON writer)
simulations2-3 (SD1 step-by-step, program choices "Topic#k") | simulations4-5 (Electronics + circuit diagrams) | physics.hpp/physics2.hpp (interactive slider engine, plots/heat/arrows/3D surf)
mathsim1-4 (Maths 1 + Sem 3 MFAI interactive) | simulations6-7 (matrix/simplex tableaus) | simulations8-9 (Workshop) | web/index.html (all drawing code)

## Three engines
1. Step-by-step: Sim{label,default,code lines,refs,practice,...}; runS(input)->Steps (line, note, vars, hl ids, mat table); diag(input) -> drawing JSON with a per-line highlight map; codeFn for input-dependent code; program choices registered as key "Topic#1", names via programNames()["Topic"].
2. Interactive sliders: PhysMap entry (mAdd/add) with PParam list and a PHF lambda filling PRes (plots, heat, arrows, fills, dots, surf, readouts, explanation).
3. Diagram builders: Cir (circuits, in simulations5.hpp) and Sk (workshop sketches, simulations8.hpp). Highlights glow; the rest dims.
Topic names in content.hpp must match the registered keys exactly and be unique across subjects.

## Gotchas learned the hard way
- Never keep a reference from addS()/newPlot() across another addS()/newPlot() (vector reallocation; AddressSanitizer found two bugs).
- `P(i,d)` is a macro in simulations4.hpp: do not name a method or variable P( ).
- Test with: build with `-fsanitize=address,undefined`, fuzz every topic through /api/simulate and /api/physics, and run the jsdom page tests.

## Status
Done and tested: Sem 1 (Software Development 1, Electronics, Physics 1, Mathematics 1); Sem 3 Mathematical Foundations of AI and DS.
Sem 2 WorkShop: all 16 topics built (carpentry, welding, sheet metal, fitting, machine shop incl. G-codes), sanitizer clean, page tests pass. Not yet polished: some drawings are plain (grooving/slotting/turning layers are small); check each with the render script.
Pending syllabi: Sem 2 Mathematics 2, Physics 2, Software Development 2; Sem 3 DBMS, Data Structures (needs binary search, stack, BFS, etc. from the original synopsis), Theory of Computation.
Open question: the Mathematics 1 Differential Equations line had a broken character where one topic name should be; ask the user what it was.

## v10 changes (Workshop polish)
- Fixed the narrowing warning (mathsim1.hpp:127). Build is warning-free with -Wall.
- Workshop drawings: Sk::out now emits "ws":1; index.html keeps labels/dimension lines readable when parts are highlighted. Added Sk::Arr (arrow).
- Redrawn: Turning (side + end view with pass rings), Facing (enlarged allowance), Grooving (detail inset), Slotting (end + plan view), Lap Joint (Welding), Cross-Lap Joint, V-Groove Fit; label fixes on Tray, Funnel, Butt Joint, Safety, G-Codes.
- jsonEscape now escapes control characters and replaces malformed UTF-8 (fuzz found invalid JSON for SDLC and the String topics).
- index.html: guards against a late response after the user has left the screen.
- tools/: fuzz.py (ASan fuzz of /api/simulate and /api/physics), pagetest.js (jsdom: every topic, every slider; needs `npm i jsdom`), render.js (Playwright screenshots of Workshop drawings).
- Still to build: Mathematics 1 Differential Equations (one topic, syllabus line had a broken character), Mathematics 2, Physics 2, Software Development 2, DBMS, Data Structures, Theory of Computation.

## v11 changes (Data Structures, part 1)
- Step engine: Step now has `tags` (small label under each array cell, e.g. L/M/R/top/front), `rl`/`rh` (active range, cells outside are dimmed) and `dg` (a per-step drawing, JSON from Sk::out, used for linked lists, trees, graphs). Page: index.html draws `dg` when present.
- New file src/ds1.hpp (addDS1): helpers `dsx::A` (array step), `dsx::L` (drawing step), `dsx::listDg` (linked-list drawing), `tg` (tags).
- Built: Module 1 (Overview, Array, Linked List, Stack, Queue) and Interpolation Search (program 0 = binary search with L/M/R pointers, program 1 = interpolation).
- Still to build in Data Structures: Module 2 (Median Search, Hashing, Hash Table, Chaining, Probing, Merge/Quick/Radix/Bucket/Counting Sort, complexity), Module 3 (trees, heaps), Module 4 (BST, AVL, RB, B-tree, B+), Module 5 (graphs), Module 6 (interval/segment tree, strings, suffix tree, tries, suffix array).

## v12 changes (Data Structures, Module 2 complete)
- New files: src/ds2.hpp (Merge/Quick/Radix/Bucket/Counting sort, Median Search, Time and Space Complexity), src/ds3.hpp (Hashing, Hash Table, Chaining, Probing). Chain: addDS1 -> addDS2 -> addDS3.
- Chaining uses a per-step drawing (dsx::chainDg). Sorts use tags + active range; merge/quick sort and recursion topics use the call-stack panel (Step.stack).
- Counting sort uses values 0..7 so the count table fits the panel.
- Still to build: Module 3 (Multi List, Binary Tree, K-ary Tree, Tree Traversals, Threaded Binary Tree, Priority Queue using Binary Heap, Binomial Heap, Fibonacci Heap, Heap Sort), Module 4 (BST, Balanced BST, AVL, Red-Black, Multiway Tree, B-Tree, B+ Tree), Module 5 (Graph fundamentals, Adjacency Matrix, Adjacency List, Graph Traversal, DFS, BFS, Shortest Path, MST), Module 6 (Interval Tree, Segment Tree, String Data Structures, Suffix Tree, Tries, Suffix Array).

## v13 changes (Data Structures complete: all 48 syllabus topics)
- Makefile now depends on src/*.hpp (before, header-only edits did not trigger a rebuild).
- New files: ds4.hpp (tree/forest drawing, Module 3), ds5.hpp (Module 4), ds6.hpp (graphs, Module 5), ds7.hpp (Module 6). Registration chain: addDS1 -> addDS2 -> addDS3 -> addDS4 -> addDS4b / addDS5 -> addDS5b / addDS6 -> addDS6b / addDS7 -> addDS7b.
- Drawing helpers: dsx::treeDg (any tree, boxed labels, thread arrows, leaf links), dsx::forestDg (binomial / Fibonacci forests; forest node i is id "t"+(i+1)), dsx::grDg (graphs), dsx::chainDg, dsx::listDg, dsx::mlDg. T() with anchor "mlight" draws white text on dark nodes.
- Not implemented (honest limits): B-tree / B+ tree DELETE, Ukkonen's O(n) suffix-tree build (naive insertion is used), suffix-array doubling construction (sorted by comparison).

## v14 changes (real-life usage)
- Every topic (all 238) now ends with a "Where it is used in real life" panel. Data lives in src/reallife_data.hpp (generated: topic -> list of uses, and `demo` = program index of a real-life scenario or -1). Endpoint: /api/reallife?topic=... (src/reallife.hpp). The page fills <div id="rl"> in both the step engine and the slider engine (loadRL in index.html).
- 12 real-life SCENARIO simulations are an extra last program in: Stack (browser Back), Queue (printer), Linked List (playlist), Priority Queue (ER triage), BFS (friend suggestions), Shortest Path (GPS), MST (cable network), Tree Traversals (folder size), Tries (search suggestions), B+ Tree (database index vs full scan), Interpolation Search (guess the number), Time Complexity (search at scale). Code in src/ds8.hpp (addDS8 is called at the end of addDS1 so program names exist). The panel shows a "Try it as a simulation" button for these.
- To add a scenario for another topic: register "<Topic>#k" in ds8.hpp, add programNames()[topic].push_back(label), and set demo = k for that topic in reallife_data.hpp.
- Tests: tools/fuzz.py now checks /api/reallife for all topics and hits program indexes up to 5; tools/pagetest.js checks the panel and the demo button (ONLY="Topic1|Topic2" restricts it).

## v15 changes (Red-Black tree: insert, search, delete)
- ds5.hpp: new engine in namespace dsx (after MWT): rbSnap, rbInsert(b,k,Steps*,rec), rbDelete(b,key,Steps*,rec), rbCheck (validates BST order, parent links, root black, no red-red, equal black heights), rbHeight/rbCount/rbBlackHeight. rec=false updates the tree silently (used to build the starting tree).
- "Red-Black (RB) Tree" now has 3 programs (names registered): 0 Insert (descent, red leaf, recolor / triangle / line cases, root black), 1 Search (key + up to 9 keys, colours shown but never used), 2 Delete (key + up to 9 keys: BST delete with successor copy, then double-black repair: case 1 red sibling, case 2 move up, case 3 near child red, case 4 far child red; a real double-black node is drawn blue).
- Delete default "5, 5, 30, 15, 55, 25, 60, 20, 35" walks through cases 1 -> 3 -> 4.
- Fixed: the old RB insert read the height from Bt::h, which plain inserts never update; RB now uses rbHeight (true height) and also reports black height.
- KNOWN, NOT FIXED: bstHeight() has the same stale-h problem for the plain BST topic ("Height is now 1" after every insert; also the Balanced BST "skewed tree" note). Fix = make bstHeight recursive like rbHeight.
- Test: tools/rb_stress.cpp (cd tools && g++ -std=c++17 -O1 -w -o rb_stress rb_stress.cpp && ./rb_stress): ~237k random inserts/deletes, every RB rule checked after every operation. Also passes tools/fuzz.py (ASan + UBSan, 0 findings).

