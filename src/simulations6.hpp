// Step-by-step matrix topics: row echelon form, rank, Gauss elimination (shown as tables).
#pragma once
#include "simulations4.hpp"

inline void addMatrixSteps(std::map<std::string, Sim>& m) {
    const std::vector<std::string> R = {"Paul's Online Math Notes: Linear Algebra|https://tutorial.math.lamar.edu/Classes/LinAlg/LinAlg.aspx", "3Blue1Brown: Essence of Linear Algebra|https://www.3blue1brown.com/topics/linear-algebra",
                                        "MIT OpenCourseWare 18.06 Linear Algebra|https://ocw.mit.edu/courses/18-06-linear-algebra-spring-2010/", "Khan Academy: Linear Algebra|https://www.khanacademy.org/math/linear-algebra"};
    auto reg = [&](const char* key, std::vector<std::string> code, const char* pr, int mode) {
        Sim sm{"Enter a matrix of 3 rows, row by row: 9 numbers for A, or 12 numbers for [A | b]", "2, 1, -1, 8, -3, -1, 2, -11, -2, 1, 2, -3", code, R, {pr}, nullptr, nullptr, "", {}};
        sm.runS = [mode](const std::string& in) {
            D v = nums(in); int cols = v.size() >= 12 ? 4 : 3; double M[3][4] = {}; for (int i = 0; i < 3; ++i) for (int j = 0; j < cols; ++j) { size_t k = i * cols + j; M[i][j] = k < v.size() ? v[k] : 0; }
            Steps s; auto fm = [](double x) { return G(std::fabs(x) < 1e-9 ? 0.0 : x); };
            auto snap = [&](int line, std::string note, int hi, Vars vs = {}) { Step st = mk(line, note, vs); for (int i = 0; i < 3; ++i) for (int j = 0; j < cols; ++j) st.mat.push_back(fm(M[i][j])); st.mc = cols; st.hi = hi; st.cols = cols == 4 ? 3 : 0; s.push_back(st); };
            snap(0, std::string("The starting matrix") + (cols == 4 ? " (the bar separates A from the right-hand side b)." : "."), -1);
            int r0 = 0; std::string pcs;
            for (int c = 0; c < 3 && r0 < 3; ++c) {
                int pr = -1; for (int i = r0; i < 3; ++i) if (std::fabs(M[i][c]) > 1e-9) { pr = i; break; }
                if (pr < 0) { snap(3, "Column " + S(c + 1) + " has no non-zero entry in the remaining rows, so there is no pivot here. Move to the next column.", -1); continue; }
                if (pr != r0) { for (int j = 0; j < cols; ++j) std::swap(M[r0][j], M[pr][j]); snap(1, "Swap R" + S(r0 + 1) + " and R" + S(pr + 1) + " so the pivot sits in row " + S(r0 + 1) + ".", r0 * cols + c); }
                snap(0, "Pivot: " + fm(M[r0][c]) + " in row " + S(r0 + 1) + ", column " + S(c + 1) + ".", r0 * cols + c, {{"pivot", fm(M[r0][c])}});
                for (int i = r0 + 1; i < 3; ++i) if (std::fabs(M[i][c]) > 1e-9) { double f = M[i][c] / M[r0][c]; for (int j = 0; j < cols; ++j) M[i][j] -= f * M[r0][j]; M[i][c] = 0; snap(2, "R" + S(i + 1) + " → R" + S(i + 1) + " − (" + fm(f) + ")·R" + S(r0 + 1) + ", which makes the entry below the pivot zero.", i * cols + c, {{"multiplier", fm(f)}}); }
                pcs += (pcs.empty() ? "" : ", ") + S(c + 1); ++r0;
            }
            int rA = 0, rAb = 0; for (int i = 0; i < 3; ++i) { bool nzA = false, nzAll = false; for (int j = 0; j < cols; ++j) if (std::fabs(M[i][j]) > 1e-9) { nzAll = true; if (j < 3) nzA = true; } rA += nzA; rAb += nzAll; }
            snap(4, "Row echelon form reached (staircase shape). Pivot columns: " + (pcs.empty() ? std::string("none") : pcs) + ".", -1);
            if (mode == 1) snap(5, "Rank = number of non-zero rows. rank(A) = " + S(rA) + (cols == 4 ? ",  rank([A | b]) = " + S(rAb) + "." : "."), -1, {{"rank(A)", S(rA)}});
            if (mode == 2) {
                if (cols < 4) { snap(6, "No right-hand side b was given, so there is nothing to solve. Enter 12 numbers to see the full solution. rank(A) = " + S(rA) + ".", -1); return s; }
                if (rA != rAb) { snap(6, "rank(A) = " + S(rA) + " but rank([A | b]) = " + S(rAb) + ": a row reads 0 = non-zero, so the system has NO solution.", -1); return s; }
                if (rA < 3) { snap(6, "rank(A) = rank([A | b]) = " + S(rA) + " < 3 unknowns: the system has INFINITELY MANY solutions (free variables).", -1); return s; }
                snap(6, "rank(A) = rank([A | b]) = 3 = number of unknowns, so there is exactly ONE solution.", -1);
                double x3 = M[2][3] / M[2][2]; snap(5, "Back substitution, last equation: " + fm(M[2][2]) + "·x3 = " + fm(M[2][3]) + "  →  x3 = " + fm(x3) + ".", 2 * cols + 2, {{"x3", fm(x3)}});
                double x2 = (M[1][3] - M[1][2] * x3) / M[1][1]; snap(5, "Middle equation: " + fm(M[1][1]) + "·x2 + " + fm(M[1][2]) + "·x3 = " + fm(M[1][3]) + "  →  x2 = " + fm(x2) + ".", cols + 1, {{"x3", fm(x3)}, {"x2", fm(x2)}});
                double x1 = (M[0][3] - M[0][1] * x2 - M[0][2] * x3) / M[0][0]; snap(5, "First equation: " + fm(M[0][0]) + "·x1 + " + fm(M[0][1]) + "·x2 + " + fm(M[0][2]) + "·x3 = " + fm(M[0][3]) + "  →  x1 = " + fm(x1) + ".", 0, {{"x1", fm(x1)}, {"x2", fm(x2)}, {"x3", fm(x3)}});
                snap(5, "Solution: x1 = " + fm(x1) + ", x2 = " + fm(x2) + ", x3 = " + fm(x3) + ".", -1, {{"x1", fm(x1)}, {"x2", fm(x2)}, {"x3", fm(x3)}});
            }
            return s; };
        m[key] = sm; };
    std::vector<std::string> ref = {"Pick a pivot: a non-zero entry in the first unfinished column", "If needed, swap rows so the pivot is in the top row of the remaining part", "Make zeros below the pivot:  Rj → Rj − (aj / pivot)·Ri", "If a column has no pivot, move one column to the right", "Stop when the matrix has the staircase shape: the row echelon form"};
    reg("Row Echelon Form", ref, "Try the identity-like matrix 1 2 3, 2 4 6, 1 0 1 and see a column without a pivot.", 0);
    auto rk = ref; rk.push_back("Rank = number of non-zero rows in the echelon form"); reg("Rank of a Matrix", rk, "Enter 1 2 3, 2 4 6, 3 6 9 (all rows proportional): the rank is 1.", 1);
    auto ga = rk; ga[5] = "Back substitution: solve from the last equation upward"; ga.push_back("Compare rank(A) with rank([A | b]) to decide whether a solution exists"); reg("Gauss Elimination", ga, "Default system gives x = 2, y = 3, z = −1. Change the last number to make the system inconsistent.", 2);
}
