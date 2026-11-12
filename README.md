# CoreLab (C++ backend + browser GUI)

Build and run (Linux / macOS / WSL):

    make
    ./corelab 8080

Then open http://localhost:8080

- `src/content.hpp`     curriculum data (semesters, subjects, later topics)
- `src/http_server.hpp` dependency-free HTTP server
- `src/main.cpp`        routes (`/api/curriculum`)
- `web/index.html`      GUI, loads everything from the C++ API

Windows: 1 semester, 2 subject, 3 topic list, 4 simulation. Done: Sem 1 (all four subjects) and Sem 3 Mathematical Foundations of AI and DS. Run: make && ./corelab 8080
