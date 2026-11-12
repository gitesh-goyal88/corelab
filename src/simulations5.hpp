// Circuit diagrams for the Electronics topics. A diagram is a list of parts and wires (drawn by the browser)
// plus a "map": for each formula line, which parts to highlight.
#pragma once
#include "simulations4.hpp"

struct Cir {
    int dx = 0; std::string e;
    void add(const std::string& s) { e += s + ","; }
    void W(std::vector<int> p, const char* id = "w") {
        std::string s = std::string("[\"W\",\"") + id + "\",[";
        for (size_t i = 0; i < p.size(); ++i) s += (i ? "," : "") + S(p[i] + (i % 2 == 0 ? dx : 0));
        add(s + "]]"); }
    // part types: R V A I C D Z Q M OA ; orientation h or v ; dir flips diode/polarity/op-amp inputs
    void Part(const char* t, const char* id, int x, int y, char o, std::string lab, const char* side = "", int dir = 1) {
        add(std::string("[\"") + t + "\",\"" + id + "\"," + S(x + dx) + "," + S(y) + ",\"" + o + "\"," + q(lab) + ",\"" + side + "\"," + S(dir) + "]"); }
    void N(int x, int y) { add("[\"N\"," + S(x + dx) + "," + S(y) + "]"); }
    void G(int x, int y) { add("[\"G\"," + S(x + dx) + "," + S(y) + "]"); }
    void T(int x, int y, std::string t, const char* an = "middle") { add("[\"T\"," + S(x + dx) + "," + S(y) + "," + q(t) + ",\"" + an + "\"]"); }
    void B(const char* id, int x, int y, int w, int h, std::string t, const char* cls) {
        add(std::string("[\"B\",\"") + id + "\"," + S(x + dx) + "," + S(y) + "," + S(w) + "," + S(h) + "," + q(t) + ",\"" + cls + "\"]"); }
    std::string out(int w, int h, std::vector<std::vector<std::string>> map) {
        if (!e.empty()) e.pop_back();
        std::string j = "{\"w\":" + S(w + dx) + ",\"h\":" + S(h) + ",\"el\":[" + e + "],\"map\":[";
        for (size_t i = 0; i < map.size(); ++i) { j += (i ? ",[" : "["); for (size_t k = 0; k < map[i].size(); ++k) j += (k ? "," : "") + q(map[i][k]); j += "]"; }
        return j + "]}"; }
};
inline std::string lb(const char* n, double x, const char* u = "k") { return std::string(n) + "=" + G(x) + u; }
using Map = std::vector<std::vector<std::string>>;

// inverting-style op-amp circuit: input part -> inverting input, feedback part around the op-amp
inline std::string invAmp(const char* inT, const char* inId, std::string inL, const char* fbT, const char* fbId, std::string fbL, std::string vin, Map map) {
    Cir c; c.dx = 30;
    c.Part("V", "Vin", 40, 160, 'v', vin, "l"); c.G(40, 190);
    c.W({40, 130, 40, 100, 60, 100}); c.Part(inT, inId, 90, 100, 'h', inL); c.W({120, 100, 160, 100}); c.N(130, 100);
    c.W({130, 100, 130, 50, 170, 50}); c.Part(fbT, fbId, 200, 50, 'h', fbL); c.W({230, 50, 270, 50, 270, 120, 340, 120}); c.N(270, 120);
    c.W({160, 140, 140, 140, 140, 190}); c.G(140, 190);
    c.Part("OA", "OA", 200, 120, 'h', "", "", 1); c.T(345, 124, "Vout", "start");
    return c.out(420, 220, map); }
// non-inverting op-amp circuit
inline std::string nonInv(std::string vin, std::string r1, std::string rf, Map map) {
    Cir c; c.dx = 30;
    c.Part("V", "Vin", 40, 150, 'v', vin, "l"); c.G(40, 180); c.W({40, 120, 40, 100, 160, 100});
    c.Part("OA", "OA", 200, 120, 'h', "", "", -1);
    c.W({160, 140, 130, 140, 130, 175}); c.N(130, 175); c.Part("R", "R1", 130, 205, 'v', r1, "l"); c.G(130, 235);
    c.W({130, 175, 170, 175}); c.Part("R", "Rf", 200, 175, 'h', rf); c.W({230, 175, 270, 175, 270, 120, 340, 120}); c.N(270, 120); c.T(345, 124, "Vout", "start");
    return c.out(420, 270, map); }
// transistor with two bias sources (operation / characteristics)
inline std::string trans2(const char* sym, std::string v1, std::string v2, Map map) {
    Cir c; c.dx = 40;
    c.Part("V", "V1", 50, 150, 'v', v1, "l"); c.W({50, 120, 110, 120}); c.Part(sym, sym, 140, 120, 'h', "");
    c.W({50, 180, 50, 230, 300, 230}); c.W({154, 150, 154, 230}); c.N(154, 230);
    c.W({154, 90, 154, 40, 300, 40, 300, 90}); c.Part("V", "V2", 300, 120, 'v', v2, "r"); c.W({300, 150, 300, 230});
    return c.out(440, 250, map); }

inline void addCircuits(std::map<std::string, Sim>& m) {
    auto DG = [&](const std::string& key, std::function<std::string(const D&)> fn) { m[key].diag = [fn](const std::string& in) { return fn(nums(in)); }; };
    const std::vector<std::string> ER = {"All About Circuits: Free Textbook|https://www.allaboutcircuits.com/textbook/", "Electronics Tutorials|https://www.electronics-tutorials.ws/", "Khan Academy: Electrical Engineering|https://www.khanacademy.org/science/electrical-engineering"};
    const std::string RC = "Resistor Combinations (Series / Parallel / Mixed)";

    /* ---------- Resistor combinations: choose series, parallel or combination ---------- */
    auto reg = [&](const std::string& key, const char* def, std::vector<std::string> f, std::function<void(const D&, Steps&)> fn) {
        Sim sm{"Enter V (volts), R1, R2, R3 (kOhm)", def, f, ER, {"Change one resistor and predict how Req changes before you run it."}, nullptr, nullptr, "", {}};
        sm.runS = [fn](const std::string& in) { D v = nums(in); Steps s; fn(v, s); return s; }; m[key] = sm; };
    reg(RC, "12, 2, 4, 6", {"Req = R1 + R2 + R3", "I = V / Req   (same current everywhere)", "V1 = I*R1,  V2 = I*R2,  V3 = I*R3", "KVL check: V1 + V2 + V3 = V"},
        [](const D& v, Steps& s) {
            double V = P(0, 12), a = pz(P(1, 2), 2), b = pz(P(2, 4), 4), c = pz(P(3, 6), 6), Rq = a + b + c, I = V / Rq;
            ST(0, "In series the resistances simply add: Req = " + G(a) + " + " + G(b) + " + " + G(c) + " = " + G(Rq) + " kOhm.", {"Req", G(Rq) + " kOhm"});
            ST(1, "One path means one current: I = " + G(V) + " / " + G(Rq) + " = " + G(I) + " mA.", {"I", G(I) + " mA"});
            ST(2, "Each resistor drops its own share: V1 = " + G(I * a) + " V, V2 = " + G(I * b) + " V, V3 = " + G(I * c) + " V. The biggest R takes the biggest voltage.", {"V1", G(I * a) + " V"}, {"V2", G(I * b) + " V"}, {"V3", G(I * c) + " V"});
            ST(3, "Check: " + G(I * a) + " + " + G(I * b) + " + " + G(I * c) + " = " + G(I * Rq) + " V = V.", {"sum of drops", G(I * Rq) + " V"}); });
    DG(RC, [](const D& v) { Cir c; c.dx = 50;
        c.Part("V", "V", 40, 100, 'v', lb("V", P(0, 12), "V"), "l"); c.W({40, 70, 40, 40, 80, 40}); c.Part("R", "R1", 110, 40, 'h', lb("R1", pz(P(1, 2), 2)));
        c.W({140, 40, 170, 40}); c.Part("R", "R2", 200, 40, 'h', lb("R2", pz(P(2, 4), 4))); c.W({230, 40, 260, 40}); c.Part("R", "R3", 290, 40, 'h', lb("R3", pz(P(3, 6), 6)));
        c.W({320, 40, 360, 40, 360, 160, 40, 160, 40, 130});
        return c.out(420, 190, {{"R1", "R2", "R3"}, {"V", "w"}, {"R1", "R2", "R3"}, {"V", "R1", "R2", "R3"}}); });

    reg(RC + "#1", "12, 6, 3, 2", {"1/Req = 1/R1 + 1/R2 + 1/R3", "Req = 1 / (1/R1 + 1/R2 + 1/R3)", "I = V / Req", "I1 = V/R1,  I2 = V/R2,  I3 = V/R3", "KCL check: I1 + I2 + I3 = I"},
        [](const D& v, Steps& s) {
            double V = P(0, 12), a = pz(P(1, 6), 6), b = pz(P(2, 3), 3), c = pz(P(3, 2), 2), g = 1 / a + 1 / b + 1 / c, Rq = 1 / g, I = V / Rq;
            ST(0, "In parallel the conductances (1/R) add: " + G(1 / a) + " + " + G(1 / b) + " + " + G(1 / c) + " = " + G(g) + " mS.", {"1/Req", G(g) + " mS"});
            ST(1, "Req = 1 / " + G(g) + " = " + G(Rq) + " kOhm. It is always smaller than the smallest resistor (" + G(std::min({a, b, c})) + " kOhm).", {"Req", G(Rq) + " kOhm"});
            ST(2, "I = " + G(V) + " / " + G(Rq) + " = " + G(I) + " mA from the source.", {"I", G(I) + " mA"});
            ST(3, "Every branch has the full " + G(V) + " V, so I1 = " + G(V / a) + " mA, I2 = " + G(V / b) + " mA, I3 = " + G(V / c) + " mA. The smallest R carries the biggest current.", {"I1", G(V / a) + " mA"}, {"I2", G(V / b) + " mA"}, {"I3", G(V / c) + " mA"});
            ST(4, "Check: " + G(V / a) + " + " + G(V / b) + " + " + G(V / c) + " = " + G(V / a + V / b + V / c) + " mA = I.", {"I1+I2+I3", G(V / a + V / b + V / c) + " mA"}); });
    DG(RC + "#1", [](const D& v) { Cir c; c.dx = 50;
        c.Part("V", "V", 40, 120, 'v', lb("V", P(0, 12), "V"), "l"); c.W({40, 90, 40, 40, 300, 40, 300, 90}); c.W({40, 150, 40, 200, 300, 200, 300, 150});
        const char* id[3] = {"R1", "R2", "R3"}; for (int i = 0; i < 3; ++i) { int x = 140 + 80 * i; c.Part("R", id[i], x, 120, 'v', lb(id[i], pz(P(i + 1, 3), 3)), "r"); c.W({x, 40, x, 90}); c.W({x, 150, x, 200}); }
        c.N(140, 40); c.N(220, 40); c.N(140, 200); c.N(220, 200);
        return c.out(420, 230, {{"R1", "R2", "R3"}, {"R1", "R2", "R3"}, {"V", "w"}, {"R1", "R2", "R3"}, {"V"}}); });

    reg(RC + "#2", "12, 2, 4, 6", {"R12 = R1 + R2   (series branch)", "Req = R12 || R3 = R12 * R3 / (R12 + R3)", "I = V / Req", "I12 = V / R12,   I3 = V / R3", "KCL check: I12 + I3 = I"},
        [](const D& v, Steps& s) {
            double V = P(0, 12), a = pz(P(1, 2), 2), b = pz(P(2, 4), 4), c = pz(P(3, 6), 6), r12 = a + b, Rq = par(r12, c), I = V / Rq;
            ST(0, "Simplify step by step. R1 and R2 are in series, so they collapse into R12 = " + G(a) + " + " + G(b) + " = " + G(r12) + " kOhm.", {"R12", G(r12) + " kOhm"});
            ST(1, "R12 and R3 are in parallel: Req = " + G(r12) + " * " + G(c) + " / " + G(r12 + c) + " = " + G(Rq) + " kOhm.", {"Req", G(Rq) + " kOhm"});
            ST(2, "I = " + G(V) + " / " + G(Rq) + " = " + G(I) + " mA.", {"I", G(I) + " mA"});
            ST(3, "Both branches see the full " + G(V) + " V. The R1-R2 branch carries I12 = " + G(V / r12) + " mA (the same current in R1 and R2) and R3 carries I3 = " + G(V / c) + " mA.", {"I12", G(V / r12) + " mA"}, {"I3", G(V / c) + " mA"});
            ST(4, "Check: " + G(V / r12) + " + " + G(V / c) + " = " + G(V / r12 + V / c) + " mA = I.", {"I12 + I3", G(V / r12 + V / c) + " mA"}); });
    DG(RC + "#2", [](const D& v) { Cir c; c.dx = 50;
        c.Part("V", "V", 40, 120, 'v', lb("V", P(0, 12), "V"), "l"); c.W({40, 90, 40, 40, 240, 40, 240, 90}); c.W({40, 150, 40, 200, 240, 200, 240, 150});
        c.W({140, 40, 140, 50}); c.Part("R", "R1", 140, 80, 'v', lb("R1", pz(P(1, 2), 2)), "r"); c.W({140, 110, 140, 120}); c.Part("R", "R2", 140, 150, 'v', lb("R2", pz(P(2, 4), 4)), "r"); c.W({140, 180, 140, 200});
        c.Part("R", "R3", 240, 120, 'v', lb("R3", pz(P(3, 6), 6)), "r"); c.N(140, 40); c.N(140, 200);
        return c.out(380, 230, {{"R1", "R2"}, {"R1", "R2", "R3"}, {"V", "w"}, {"R1", "R2", "R3"}, {"V"}}); });
    programNames()[RC] = {"Series", "Parallel", "Combination (series + parallel)"};

    /* ---------- Basic circuit analysis ---------- */
    DG("Kirchhoff's Laws", [](const D& v) { Cir c; c.dx = 50;
        c.Part("V", "V", 40, 120, 'v', lb("V", P(0, 12), "V"), "l"); c.W({40, 90, 40, 40, 90, 40}); c.Part("R", "R1", 120, 40, 'h', lb("R1", pz(P(1, 2), 2)));
        c.W({150, 40, 320, 40, 320, 90}); c.W({230, 40, 230, 90}); c.N(230, 40); c.Part("R", "R2", 230, 120, 'v', lb("R2", pz(P(2, 3), 3)), "r"); c.Part("R", "R3", 320, 120, 'v', lb("R3", pz(P(3, 6), 6)), "r");
        c.W({320, 150, 320, 200, 40, 200, 40, 150}); c.W({230, 150, 230, 200}); c.N(230, 200);
        return c.out(430, 230, {{"R1", "R2", "R3"}, {"V", "w"}, {"V", "R1", "R2", "R3"}, {"R2", "R3"}, {"R2", "R3", "V"}}); });

    DG("Voltage Divider", [](const D& v) { Cir c; c.dx = 50; double RL = P(3, 0);
        c.Part("V", "V", 40, 120, 'v', lb("Vin", P(0, 12), "V"), "l"); c.W({40, 90, 40, 40, 140, 40, 140, 50}); c.Part("R", "R1", 140, 80, 'v', lb("R1", pz(P(1, 4), 4)), "r");
        c.W({140, 110, 140, 130}); c.N(140, 120); c.Part("R", "R2", 140, 160, 'v', lb("R2", pz(P(2, 8), 8)), "r"); c.W({140, 190, 140, 220, 40, 220, 40, 150});
        if (RL > 0) { c.W({140, 120, 250, 120, 250, 140}); c.Part("R", "RL", 250, 170, 'v', lb("RL", RL), "r"); c.W({250, 200, 250, 220, 140, 220}); c.T(200, 112, "Vout"); }
        else { c.W({140, 120, 230, 120}); c.N(230, 120); c.T(240, 124, "Vout", "start"); }
        return c.out(380, 250, {{"R1", "R2"}, {"V", "w"}, {"R1", "R2"}, {"V", "R1", "R2"}, {"RL", "R2"}}); });

    DG("Current Divider", [](const D& v) { Cir c; c.dx = 50;
        c.Part("I", "I", 40, 120, 'v', lb("I", P(0, 9), "mA"), "l"); c.W({40, 90, 40, 40, 260, 40, 260, 90}); c.W({40, 150, 40, 200, 260, 200, 260, 150});
        c.W({160, 40, 160, 90}); c.W({160, 150, 160, 200}); c.N(160, 40); c.N(160, 200);
        c.Part("R", "R1", 160, 120, 'v', lb("R1", pz(P(1, 6), 6)), "r"); c.Part("R", "R2", 260, 120, 'v', lb("R2", pz(P(2, 3), 3)), "r");
        return c.out(380, 230, {{"R1", "R2"}, {"R1", "R2"}, {"I", "w"}, {"R1", "R2"}}); });

    auto twoSrc = [](const D& v, Map map) { Cir c; c.dx = 50;
        c.Part("V", "V1", 40, 120, 'v', lb("Va", P(0, 10), "V"), "l"); c.W({40, 90, 40, 40, 100, 40}); c.Part("R", "R1", 130, 40, 'h', lb("R1", pz(P(2, 2), 2)));
        c.W({160, 40, 280, 40}); c.Part("R", "R3", 310, 40, 'h', lb("R3", pz(P(4, 4), 4))); c.W({340, 40, 400, 40, 400, 90}); c.Part("V", "V2", 400, 120, 'v', lb("Vb", P(1, 4), "V"), "r");
        c.W({220, 40, 220, 90}); c.N(220, 40); c.Part("R", "R2", 220, 120, 'v', lb("R2", pz(P(3, 4), 4)), "r"); c.W({220, 150, 220, 200}); c.N(220, 200); c.T(226, 36, "V", "start");
        c.W({40, 150, 40, 200, 400, 200, 400, 150}); c.G(220, 200);
        return c.out(480, 240, map); };
    DG("Nodal Analysis", [twoSrc](const D& v) { return twoSrc(v, {{"R2"}, {"R1", "R2", "R3"}, {"R1", "R2", "R3"}, {"R2"}, {"R1", "R3", "V1", "V2"}}); });
    DG("Mesh Analysis", [twoSrc](const D& v) { return twoSrc(v, {{"V1", "R1", "R2"}, {"V2", "R2", "R3"}, {"R1", "R2", "R3"}, {"V1", "R1"}, {"V2", "R3"}, {"R2"}}); });
    DG("Superposition", [twoSrc](const D& v) { return twoSrc(v, {{"V1", "R1", "R2", "R3"}, {"V2", "R3", "R2", "R1"}, {"R2"}, {"R1", "R2", "R3", "V1", "V2"}}); });

    auto thev = [](const D& v, Map map) { Cir c; c.dx = 50;
        c.Part("V", "V", 40, 120, 'v', lb("V", P(0, 12), "V"), "l"); c.W({40, 90, 40, 40, 80, 40}); c.Part("R", "R1", 110, 40, 'h', lb("R1", pz(P(1, 4), 4)));
        c.W({140, 40, 320, 40, 320, 90}); c.W({200, 40, 200, 90}); c.N(200, 40); c.Part("R", "R2", 200, 120, 'v', lb("R2", pz(P(2, 4), 4)), "r"); c.W({200, 150, 200, 200}); c.N(200, 200);
        c.N(260, 40); c.N(260, 200); c.T(260, 28, "a"); c.T(260, 222, "b"); c.Part("R", "RL", 320, 120, 'v', lb("RL", pz(P(3, 6), 6)), "r"); c.W({320, 150, 320, 200, 40, 200, 40, 150});
        return c.out(430, 235, map); };
    DG("Thevenin's Theorem", [thev](const D& v) { return thev(v, {{"RL"}, {"V", "R2"}, {"R1", "R2"}, {"RL", "R1", "R2"}, {"RL"}}); });
    DG("Norton's Theorem", [thev](const D& v) { return thev(v, {{"RL"}, {"V", "R1"}, {"R1", "R2"}, {"RL", "R1", "R2"}, {"R1", "R2", "V"}}); });

    /* ---------- Diodes ---------- */
    DG("PN Junction", [](const D& v) { Cir c;
        c.B("P", 40, 50, 160, 80, "", "p"); c.B("N", 200, 50, 160, 80, "", "n"); c.B("dep", 150, 50, 100, 80, "", "dep");
        c.T(95, 75, "P-type"); c.T(95, 95, "(holes)"); c.T(305, 75, "N-type"); c.T(305, 95, "(electrons)");
        c.T(175, 70, "- - -"); c.T(225, 70, "+ + +"); c.T(175, 112, "xp"); c.T(225, 112, "xn"); c.W({150, 150, 250, 150}, "dep"); c.T(200, 172, "depletion width W");
        return c.out(400, 190, {{"P", "N"}, {"dep"}, {"dep"}, {"dep"}}); });

    DG("Diode Biasing (Forward and Reverse)", [](const D& v) { Cir c; c.dx = 40; double V = P(0, 5);
        c.Part("V", "V", 40, 120, 'v', lb("V", V, "V"), "l", V < 0 ? -1 : 1); c.W({40, 90, 40, 40, 170, 40}); c.Part("R", "R", 200, 40, 'h', lb("R", pz(P(1, 1), 1)));
        c.W({230, 40, 360, 40, 360, 90}); c.Part("D", "D", 360, 120, 'v', "D", "r"); c.W({360, 150, 360, 200, 40, 200, 40, 150});
        return c.out(440, 230, {{"V", "R", "D"}, {"V", "D"}, {"V", "D"}, {"R"}}); });
    DG("Diode I-V Characteristics", [](const D& v) { Cir c; c.dx = 40;
        c.Part("V", "V", 40, 120, 'v', "V (varied)", "l"); c.W({40, 90, 40, 40, 130, 40}); c.Part("D", "D", 160, 40, 'h', "diode"); c.W({190, 40, 280, 40, 280, 200, 40, 200, 40, 150}); c.T(285, 125, "Id", "start");
        return c.out(380, 230, {{"D", "V"}, {"D", "V"}, {"D", "V"}, {"D", "V"}}); });
    DG("Half-Wave Rectifier", [](const D& v) { Cir c; c.dx = 40;
        c.Part("A", "A", 40, 120, 'v', lb("Vm", pz(P(0, 10), 10), "V"), "l"); c.W({40, 90, 40, 40, 120, 40}); c.Part("D", "D", 150, 40, 'h', "D"); c.W({180, 40, 300, 40, 300, 90});
        c.Part("R", "RL", 300, 120, 'v', lb("RL", pz(P(1, 1), 1)), "r"); c.W({300, 150, 300, 200, 40, 200, 40, 150}); c.T(250, 28, "Vo");
        return c.out(400, 230, {{"A", "D"}, {"RL"}, {"RL", "D"}, {"A", "D", "RL"}, {"D"}}); });
    DG("Full-Wave Rectifier", [](const D& v) { Cir c; c.dx = 40;
        c.Part("A", "A", 40, 120, 'v', lb("Vm", pz(P(0, 10), 10), "V"), "l"); c.W({40, 90, 40, 60, 170, 60}); c.Part("D", "D1", 230, 60, 'h', "D1", "", 1); c.Part("D", "D2", 290, 120, 'v', "D2", "r", -1);
        c.Part("D", "D3", 230, 180, 'h', "D3", "", 1); c.Part("D", "D4", 170, 120, 'v', "D4", "l", -1);
        c.W({170, 60, 200, 60}); c.W({260, 60, 290, 60, 290, 90}); c.W({290, 150, 290, 180, 260, 180}); c.W({200, 180, 170, 180, 170, 150}); c.W({170, 90, 170, 60});
        c.W({40, 150, 40, 230, 290, 230, 290, 180}); c.W({290, 60, 350, 60, 350, 90}); c.Part("R", "RL", 350, 120, 'v', lb("RL", pz(P(1, 1), 1)), "r"); c.W({350, 150, 350, 260, 130, 260, 130, 180, 170, 180});
        c.T(318, 52, "+"); c.T(150, 196, "-");
        return c.out(440, 280, {{"D1", "D2", "D3", "D4", "A"}, {"RL"}, {"RL"}, {"A", "D1", "D2", "D3", "D4", "RL"}, {"D1", "D2", "D3", "D4"}}); });
    DG("Clipper", [](const D& v) { Cir c; c.dx = 40;
        c.Part("A", "A", 40, 120, 'v', lb("Vm", pz(P(0, 10), 10), "V"), "l"); c.W({40, 90, 40, 40, 80, 40}); c.Part("R", "R", 110, 40, 'h', "R"); c.W({140, 40, 280, 40}); c.N(180, 40);
        c.W({180, 40, 180, 50}); c.Part("D", "D", 180, 80, 'v', "D", "r"); c.W({180, 110, 180, 120}); c.Part("V", "Vref", 180, 150, 'v', lb("Vref", P(1, 3), "V"), "r"); c.W({180, 180, 180, 220});
        c.W({40, 150, 40, 220, 280, 220}); c.N(280, 40); c.N(280, 220); c.T(290, 44, "Vout", "start");
        return c.out(400, 250, {{"D", "Vref", "R"}, {"D", "Vref"}, {"R", "A"}, {"D", "Vref", "R", "A"}}); });
    DG("Clamper", [](const D& v) { Cir c; c.dx = 40;
        c.Part("A", "A", 40, 120, 'v', lb("Vm", pz(P(0, 5), 5), "V"), "l"); c.W({40, 90, 40, 40, 90, 40}); c.Part("C", "C", 120, 40, 'h', "C"); c.W({150, 40, 290, 40, 290, 90}); c.N(190, 40);
        c.W({190, 40, 190, 60}); c.Part("D", "D", 190, 90, 'v', "D", "r", -1); c.W({190, 120, 190, 130}); c.Part("V", "V", 190, 160, 'v', lb("Vref", P(1, 0), "V"), "r"); c.W({190, 190, 190, 220});
        c.Part("R", "RL", 290, 120, 'v', "RL", "r"); c.W({290, 150, 290, 220, 40, 220, 40, 150}); c.T(215, 28, "Vout", "start");
        return c.out(400, 250, {{"D", "C", "V", "A"}, {"C", "A"}, {"RL", "C"}, {"C"}}); });

    auto zener = [](const D& v, bool two, Map map) { Cir c; c.dx = 50; double RLv = two ? 1 : 1;
        c.Part("V", "V", 40, 120, 'v', "Vin", "l"); c.W({40, 90, 40, 40, 80, 40}); c.Part("R", "Rs", 110, 40, 'h', "Rs"); c.W({140, 40, 270, 40, 270, 90}); c.W({180, 40, 180, 90}); c.N(180, 40);
        c.Part("Z", "Z", 180, 120, 'v', "Zener", "r", -1); c.W({180, 150, 180, 200}); c.N(180, 200); c.Part("R", "RL", 270, 120, 'v', "RL", "r"); c.W({270, 150, 270, 200, 40, 200, 40, 150}); c.T(225, 28, "Vout");
        (void)v; (void)RLv; return c.out(390, 230, map); };
    DG("Zener Diode Applications (Shunt Regulator)", [zener](const D& v) { return zener(v, false, {{"Rs", "V"}, {"RL"}, {"Z"}, {"Z"}, {"Rs", "RL"}}); });
    DG("Line Regulation", [zener](const D& v) { return zener(v, false, {{"Z", "RL", "Rs"}, {"V", "Z"}, {"V", "Z"}, {"Z"}}); });
    DG("Load Regulation", [zener](const D& v) { return zener(v, false, {{"Z", "Rs"}, {"RL"}, {"RL"}, {"RL", "Z"}}); });

    /* ---------- BJT and MOSFET ---------- */
    DG("BJT Operation", [](const D& v) { return trans2("Q", lb("VBE", P(0, 0.7), "V"), lb("VCE", P(1, 5), "V"), {{"V1", "Q"}, {"V2", "Q"}, {"Q"}, {"Q"}, {"Q"}}); });
    DG("BJT Characteristics", [](const D& v) { return trans2("Q", lb("IB", pz(P(0, 40), 40), "uA"), lb("VCE", P(2, 5), "V"), {{"Q", "V2"}, {"Q"}, {"Q"}, {"Q"}}); });
    auto bjtBias = [](const D& v, bool re, Map map) { Cir c; c.dx = 85; int by = re ? 270 : 230;
        c.Part("V", "V", 20, 120, 'v', lb("Vcc", P(0, 12), "V"), "l"); c.W({20, 90, 20, 40, 154, 40}); c.W({110, 40, 110, 60}); c.N(110, 40); c.Part("R", "Rb", 110, 90, 'v', lb("Rb", pz(P(1, 470), 470)), "l");
        c.W({110, 120, 110, 160}); c.Part("R", "Rc", 154, 90, 'v', lb("Rc", pz(P(2, 2.2), 2.2)), "r"); c.W({154, 60, 154, 40}); c.W({154, 120, 154, 130});
        c.Part("Q", "Q", 140, 160, 'h', ""); c.W({20, 150, 20, by, 154, by});
        if (re) { c.W({154, 190, 154, 200}); c.Part("R", "Re", 154, 230, 'v', lb("Re", pz(P(3, 1), 1)), "r"); c.W({154, 260, 154, by}); } else c.W({154, 190, 154, by});
        return c.out(300, by + 20, map); };
    DG("BJT Biasing", [bjtBias](const D& v) { return bjtBias(v, false, {{"Rb"}, {"Rb", "Q"}, {"Rc", "Q"}, {"V", "Rc", "Q"}, {"Q"}}); });
    DG("BJT Stability", [bjtBias](const D& v) { D w = {12, P(1, 10), 2.2, P(2, 1)}; return bjtBias(w, true, {{"Rb", "Q"}, {"Re", "Rb", "Q"}, {"Q"}, {"Re"}}); });
    DG("MOSFET Operation", [](const D& v) { return trans2("M", lb("VGS", P(0, 3), "V"), lb("VDS", P(1, 5), "V"), {{"V1", "M"}, {"M"}, {"M"}, {"M", "V2"}}); });
    DG("MOSFET Characteristics", [](const D& v) { return trans2("M", lb("VGS", P(0, 4), "V"), lb("VDS", P(1, 6), "V"), {{"V1", "M"}, {"M"}, {"M", "V2"}, {"M", "V2"}}); });
    DG("MOSFET Biasing", [](const D& v) { Cir c; c.dx = 50;
        c.Part("V", "V", 50, 130, 'v', lb("VDD", P(0, 12), "V"), "l"); c.W({50, 100, 50, 40, 214, 40}); c.W({110, 40, 110, 60}); c.N(110, 40);
        c.Part("R", "R1", 110, 90, 'v', lb("R1", pz(P(1, 200), 200)), "r"); c.W({110, 120, 110, 170}); c.N(110, 150); c.W({110, 150, 170, 150}); c.Part("R", "R2", 110, 200, 'v', lb("R2", pz(P(2, 100), 100)), "r");
        c.W({110, 230, 110, 250}); c.Part("R", "RD", 214, 80, 'v', lb("RD", pz(P(3, 2), 2)), "r"); c.W({214, 50, 214, 40}); c.W({214, 110, 214, 120}); c.Part("M", "M", 200, 150, 'h', ""); c.W({214, 180, 214, 250});
        c.W({50, 160, 50, 250, 214, 250}); c.N(110, 250);
        return c.out(340, 275, {{"R1", "R2"}, {"M"}, {"M"}, {"RD", "M"}, {"M"}}); });

    /* ---------- Op-amps ---------- */
    DG("Op-Amp Parameters", [](const D& v) { Cir c; c.dx = 40; c.Part("OA", "OA", 160, 100, 'h', "", "", 1); c.W({60, 80, 120, 80}); c.W({60, 120, 120, 120}); c.W({200, 100, 300, 100});
        c.T(55, 84, "V-", "end"); c.T(55, 124, "V+", "end"); c.T(305, 104, "Vout", "start"); c.T(160, 168, "gain, bandwidth, slew rate, CMRR");
        return c.out(380, 190, {{"OA"}, {"OA"}, {"OA"}, {"OA"}}); });
    DG("Ideal Op-Amp", [](const D& v) { return nonInv(lb("V+", P(0, 2), "V"), lb("R1", pz(P(1, 1), 1)), lb("Rf", pz(P(2, 4), 4)), {{"OA"}, {"OA"}, {"OA"}, {"R1", "Rf"}}); });
    DG("Op-Amp Equivalent Circuit", [](const D& v) { Cir c; c.dx = 40;
        c.N(30, 60); c.N(30, 200); c.T(22, 56, "V+", "end"); c.T(22, 204, "V-", "end"); c.W({30, 60, 70, 60, 70, 100}); c.Part("R", "Rin", 70, 130, 'v', "Rin", "r"); c.W({70, 160, 70, 200, 30, 200});
        c.Part("V", "E", 190, 130, 'v', "A*Vd", "l"); c.W({190, 100, 190, 60, 220, 60}); c.Part("R", "Ro", 250, 60, 'h', lb("Ro", P(3, 0.075))); c.W({280, 60, 340, 60, 340, 100});
        c.Part("R", "RL", 340, 130, 'v', lb("RL", pz(P(4, 10), 10)), "r"); c.W({340, 160, 340, 200, 190, 200, 190, 160}); c.T(70, 236, "Vd = V+ - V-");
        return c.out(440, 250, {{"Rin"}, {"E"}, {"Rin"}, {"Ro", "RL"}}); });
    DG("Inverting Amplifier", [](const D& v) { return invAmp("R", "Rin", lb("Rin", pz(P(1, 1), 1)), "R", "Rf", lb("Rf", pz(P(2, 10), 10)), lb("Vin", P(0, 1), "V"), {{"OA"}, {"Rin"}, {"Rf"}, {"Rin", "Rf"}, {"OA"}}); });
    DG("Integrator", [](const D& v) { return invAmp("R", "Rin", lb("R", pz(P(1, 10), 10)), "C", "C", lb("C", pz(P(2, 1), 1), "u"), lb("Vin", P(0, 2), "V"), {{"C", "Rin"}, {"C"}, {"Rin", "C"}, {"OA"}}); });
    DG("Differentiator", [](const D& v) { return invAmp("C", "C", lb("C", pz(P(2, 0.1), 0.1), "u"), "R", "Rf", lb("R", pz(P(1, 10), 10)), "Vin (changing)", {{"C", "Rf"}, {"C", "Rf"}, {"C", "Rf"}, {"Rf"}}); });
    DG("Non-Inverting Amplifier", [](const D& v) { return nonInv(lb("Vin", P(0, 1), "V"), lb("R1", pz(P(1, 1), 1)), lb("Rf", std::max(0.1, P(2, 9))), {{"OA"}, {"R1"}, {"Rf"}, {"R1", "Rf"}, {"OA"}}); });
    DG("Voltage Follower", [](const D& v) { Cir c; c.dx = 30;
        c.Part("V", "Vs", 40, 150, 'v', lb("Vs", P(0, 5), "V"), "l"); c.G(40, 180); c.W({40, 120, 40, 100, 60, 100}); c.Part("R", "Rs", 90, 100, 'h', lb("Rs", pz(P(1, 10), 10))); c.W({120, 100, 160, 100});
        c.Part("OA", "OA", 200, 120, 'h', "", "", -1); c.W({160, 140, 130, 140, 130, 180, 270, 180, 270, 120}); c.N(270, 120); c.W({240, 120, 310, 120, 310, 140});
        c.Part("R", "RL", 310, 170, 'v', lb("RL", pz(P(2, 1), 1)), "r"); c.W({310, 200, 310, 215}); c.G(310, 215);
        return c.out(430, 250, {{"Rs", "RL"}, {"OA"}, {"Rs"}, {"OA", "RL"}}); });
    DG("Summing Amplifier (Summer)", [](const D& v) { Cir c; c.dx = 50;
        for (int i = 0; i < 3; ++i) { int y = 30 + 90 * i; std::string n = "V" + S(i + 1), r = "R" + S(i + 1);
            c.Part("V", n.c_str(), 40, y + 30, 'v', lb(n.c_str(), P(i, i + 1.0), "V"), "l"); c.G(40, y + 60); c.W({40, y, 70, y}); c.Part("R", r.c_str(), 100, y, 'h', lb(r.c_str(), pz(P(3 + i, 10), 10))); c.W({130, y, 150, y}); }
        c.W({150, 30, 150, 210}); c.N(150, 120); c.W({150, 120, 220, 120}); c.N(190, 120);
        c.Part("OA", "OA", 260, 140, 'h', "", "", 1); c.W({220, 120, 220, 120}); c.W({220, 160, 200, 160, 200, 185}); c.G(200, 185);
        c.W({190, 120, 190, 80, 230, 80}); c.Part("R", "Rf", 260, 80, 'h', lb("Rf", pz(P(6, 10), 10))); c.W({290, 80, 330, 80, 330, 140, 400, 140}); c.N(330, 140); c.T(405, 144, "Vout", "start");
        return c.out(480, 305, {{"OA"}, {"R1", "R2", "R3"}, {"Rf"}, {"Rf", "OA"}, {"R1", "R2", "R3", "Rf"}}); });
    DG("Comparator", [](const D& v) { Cir c; c.dx = 40;
        c.Part("V", "Vin", 40, 150, 'v', lb("Vin", P(0, 2), "V"), "l"); c.G(40, 180); c.W({40, 120, 40, 100, 160, 100});
        c.Part("V", "Vref", 100, 200, 'v', lb("Vref", P(1, 1.5), "V"), "l"); c.G(100, 230); c.W({100, 170, 100, 140, 160, 140});
        c.Part("OA", "OA", 200, 120, 'h', "", "", -1); c.W({240, 120, 320, 120}); c.T(325, 124, "Vout = +Vsat or -Vsat", "start");
        return c.out(520, 250, {{"OA"}, {"OA"}, {"OA"}, {"OA"}}); });
    DG("Difference Amplifier", [](const D& v) { Cir c; c.dx = 50;
        c.Part("V", "V1", 60, 150, 'v', lb("V1", P(0, 2), "V"), "l"); c.G(60, 180); c.W({60, 120, 80, 120}); c.Part("R", "R1", 110, 120, 'h', lb("R1", pz(P(2, 10), 10))); c.W({140, 120, 200, 120}); c.N(170, 120);
        c.W({170, 120, 170, 70, 210, 70}); c.Part("R", "Rf", 240, 70, 'h', lb("Rf", pz(P(3, 50), 50))); c.W({270, 70, 320, 70, 320, 140, 380, 140}); c.N(320, 140); c.T(385, 144, "Vout", "start");
        c.Part("V", "V2", 60, 240, 'v', lb("V2", P(1, 3), "V"), "l"); c.G(60, 270); c.W({60, 210, 80, 210}); c.Part("R", "R2", 110, 210, 'h', lb("R2", pz(P(2, 10), 10))); c.W({140, 210, 170, 210, 170, 160, 200, 160}); c.N(170, 210);
        c.W({170, 210, 170, 215}); c.Part("R", "R3", 170, 245, 'v', lb("R3", pz(P(3, 50), 50)), "r"); c.W({170, 275, 170, 285}); c.G(170, 285);
        c.Part("OA", "OA", 240, 140, 'h', "", "", 1);
        return c.out(480, 310, {{"R2", "R3"}, {"OA"}, {"R1", "Rf"}, {"OA", "Rf"}, {"R1", "R2", "R3", "Rf"}}); });
}
