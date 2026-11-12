// Basic Electronics: circuit analysis, diodes, Zener, BJT, MOSFET, op-amps.
// Units used everywhere: volts (V), kilo-ohms (kOhm), milliamps (mA), microfarads (uF), milliseconds (ms).
#pragma once
#ifdef IN
#undef IN
#endif
#ifdef OUT
#undef OUT
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <cmath>
#include "simulations3.hpp"

using D = std::vector<double>;
#define P(i, d) (v.size() > (size_t)(i) ? v[i] : (d))
#define ST(l, n, ...) s.push_back(mk(l, n, Vars{__VA_ARGS__}))
inline D nums(const std::string& in) {
    D r; const char* c = in.c_str();
    while (*c) { if (strchr("0123456789+-.", *c)) { char* e; double x = std::strtod(c, &e); if (e == c) { ++c; continue; } r.push_back(x); c = e; } else ++c; }
    return r;
}
inline std::string G(double x) { char b[40]; std::snprintf(b, sizeof b, "%.4g", std::fabs(x) < 1e-15 ? 0.0 : x); return b; }
constexpr double VT = 0.02585;
inline double pz(double x, double d) { return x > 0 ? x : d; }
inline double par(double a, double b) { return a * b / (a + b); }
inline double clipV(double x, double sat) { return std::max(-sat, std::min(sat, x)); }

inline void addElectronics(std::map<std::string, Sim>& m) {
    const std::vector<std::string> ER = {
        "All About Circuits: Free Textbook|https://www.allaboutcircuits.com/textbook/",
        "Electronics Tutorials|https://www.electronics-tutorials.ws/",
        "Khan Academy: Electrical Engineering|https://www.khanacademy.org/science/electrical-engineering"};
    auto E = [&](const char* key, const char* lab, const char* def, std::vector<std::string> f, const char* pr, std::function<void(const D&, Steps&)> fn) {
        Sim sm{lab, def, f, ER, {pr}, nullptr, nullptr, "", {}};
        sm.runS = [fn](const std::string& in) { D v = nums(in); Steps s; fn(v, s); return s; };
        m[key] = sm; };


    /* ================= Basic Circuit Analysis ================= */
    E("Kirchhoff's Laws", "Enter V (volts), R1, R2, R3 (kOhm). R2 and R3 are in parallel, in series with R1", "12, 2, 3, 6",
      {"Req = R1 + (R2 || R3)", "I = V / Req", "KVL: V - I*R1 - V23 = 0", "I2 = V23 / R2,   I3 = V23 / R3", "KCL: I = I2 + I3   (check)"},
      "Try V = 24, R1 = 4, R2 = 6, R3 = 12 and predict I before you run it.",
      [](const D& v, Steps& s) {
          double V = P(0, 12), R1 = pz(P(1, 2), 2), R2 = pz(P(2, 3), 3), R3 = pz(P(3, 6), 6), pp = par(R2, R3), Req = R1 + pp, I = V / Req, V23 = V - I * R1, I2 = V23 / R2, I3 = V23 / R3;
          ST(0, "R2 || R3 = " + G(pp) + " kOhm, so Req = " + G(R1) + " + " + G(pp) + " = " + G(Req) + " kOhm.", {"R2 || R3", G(pp) + " kOhm"}, {"Req", G(Req) + " kOhm"});
          ST(1, "Ohm's law: I = " + G(V) + " / " + G(Req) + " = " + G(I) + " mA. All of this current flows through R1.", {"I", G(I) + " mA"});
          ST(2, "KVL: the voltages around the loop add up to zero. V23 = " + G(V) + " - " + G(I * R1) + " = " + G(V23) + " V.", {"V across R1", G(I * R1) + " V"}, {"V23", G(V23) + " V"});
          ST(3, "R2 and R3 share the same voltage, so I2 = " + G(I2) + " mA and I3 = " + G(I3) + " mA.", {"I2", G(I2) + " mA"}, {"I3", G(I3) + " mA"});
          ST(4, "KCL: current entering a node equals current leaving it. " + G(I2) + " + " + G(I3) + " = " + G(I2 + I3) + " mA = I.", {"I2 + I3", G(I2 + I3) + " mA"}, {"I", G(I) + " mA"}); });

    E("Voltage Divider", "Enter Vin (V), R1, R2 (kOhm), RL (kOhm, 0 = no load)", "12, 4, 8, 0",
      {"Vout = Vin * R2 / (R1 + R2)", "I = Vin / (R1 + R2)", "V1 = I*R1,   V2 = I*R2", "KVL check: V1 + V2 = Vin", "With load: R2' = R2 || RL, Vout' = Vin * R2' / (R1 + R2')"},
      "Try Vin = 9, R1 = 6, R2 = 3, then add RL = 3 and see how the output drops.",
      [](const D& v, Steps& s) {
          double Vi = P(0, 12), R1 = pz(P(1, 4), 4), R2 = pz(P(2, 8), 8), RL = P(3, 0), Vo = Vi * R2 / (R1 + R2), I = Vi / (R1 + R2);
          ST(0, "The output is the fraction R2 / (R1 + R2) of the input: " + G(Vi) + " * " + G(R2) + " / " + G(R1 + R2) + " = " + G(Vo) + " V.", {"Vout", G(Vo) + " V"});
          ST(1, "Series resistors carry the same current: I = " + G(Vi) + " / " + G(R1 + R2) + " = " + G(I) + " mA.", {"I", G(I) + " mA"});
          ST(2, "The bigger resistor gets the bigger share: V1 = " + G(I * R1) + " V, V2 = " + G(I * R2) + " V.", {"V1", G(I * R1) + " V"}, {"V2", G(I * R2) + " V"});
          ST(3, "Check: " + G(I * R1) + " + " + G(I * R2) + " = " + G(I * R1 + I * R2) + " V = Vin.", {"V1 + V2", G(I * R1 + I * R2) + " V"});
          if (RL > 0) { double R2p = par(R2, RL), Vl = Vi * R2p / (R1 + R2p); ST(4, "A load RL = " + G(RL) + " kOhm in parallel with R2 gives R2' = " + G(R2p) + " kOhm, so the output falls from " + G(Vo) + " V to " + G(Vl) + " V. A divider works well only when RL is much larger than R2.", {"R2'", G(R2p) + " kOhm"}, {"Vout (loaded)", G(Vl) + " V"}); }
          else ST(4, "No load was given (RL = 0), so the divider is unloaded. Enter an RL to see the loading effect.", {"RL", "none"}); });

    E("Current Divider", "Enter total current I (mA), R1, R2 (kOhm) in parallel", "9, 6, 3",
      {"I1 = I * R2 / (R1 + R2)", "I2 = I * R1 / (R1 + R2)", "Check: I1 + I2 = I", "Same voltage: V = I1*R1 = I2*R2"},
      "Try I = 12, R1 = 2, R2 = 6. Which branch carries more current?",
      [](const D& v, Steps& s) {
          double I = P(0, 9), R1 = pz(P(1, 6), 6), R2 = pz(P(2, 3), 3), I1 = I * R2 / (R1 + R2), I2 = I * R1 / (R1 + R2);
          ST(0, "I1 is the share that goes through R1. It uses the OTHER resistor on top: " + G(I) + " * " + G(R2) + " / " + G(R1 + R2) + " = " + G(I1) + " mA.", {"I1", G(I1) + " mA"});
          ST(1, "I2 = " + G(I) + " * " + G(R1) + " / " + G(R1 + R2) + " = " + G(I2) + " mA. The smaller resistor takes the larger current.", {"I2", G(I2) + " mA"});
          ST(2, "KCL check: " + G(I1) + " + " + G(I2) + " = " + G(I1 + I2) + " mA.", {"I1 + I2", G(I1 + I2) + " mA"});
          ST(3, "Both branches see the same voltage: " + G(I1 * R1) + " V = " + G(I2 * R2) + " V.", {"V", G(I1 * R1) + " V"}); });

    const char* twoSrcLabel = "Enter Va, Vb (V), then R1, R2, R3 (kOhm). Va-R1, Vb-R3 and R2 to ground meet at one node";
    E("Nodal Analysis", twoSrcLabel, "10, 4, 2, 4, 4",
      {"Choose the ground (0 V). Call the middle node V", "KCL at V:  (V-Va)/R1 + V/R2 + (V-Vb)/R3 = 0", "V * (1/R1 + 1/R2 + 1/R3) = Va/R1 + Vb/R3", "V = (Va/R1 + Vb/R3) / (1/R1 + 1/R2 + 1/R3)", "I1 = (Va-V)/R1,  I2 = V/R2,  I3 = (Vb-V)/R3"},
      "Try Va = 12, Vb = 0, R1 = R2 = R3 = 3. Predict V first.",
      [](const D& v, Steps& s) {
          double Va = P(0, 10), Vb = P(1, 4), R1 = pz(P(2, 2), 2), R2 = pz(P(3, 4), 4), R3 = pz(P(4, 4), 4), gs = 1 / R1 + 1 / R2 + 1 / R3, nu = Va / R1 + Vb / R3, V = nu / gs;
          ST(0, "Nodal analysis uses KCL with node voltages as unknowns. Only one unknown here: V.", {"Va", G(Va) + " V"}, {"Vb", G(Vb) + " V"});
          ST(1, "Currents leaving the node add to zero. Each current is (voltage difference) / resistance.", {"unknown", "V"});
          ST(2, "Collect the V terms: V * " + G(gs) + " = " + G(nu) + ".", {"1/R1 + 1/R2 + 1/R3", G(gs) + " mS"}, {"Va/R1 + Vb/R3", G(nu) + " mA"});
          ST(3, "V = " + G(nu) + " / " + G(gs) + " = " + G(V) + " V.", {"V", G(V) + " V"});
          ST(4, "I1 = " + G((Va - V) / R1) + " mA, I2 = " + G(V / R2) + " mA, I3 = " + G((Vb - V) / R3) + " mA. KCL holds: I1 + I3 = " + G((Va - V) / R1 + (Vb - V) / R3) + " = I2.", {"I1", G((Va - V) / R1) + " mA"}, {"I2", G(V / R2) + " mA"}, {"I3", G((Vb - V) / R3) + " mA"}); });

    E("Mesh Analysis", "Enter V1, V2 (V), then R1, R2, R3 (kOhm). R2 is shared by both loops", "10, 4, 2, 4, 4",
      {"Mesh 1: (R1 + R2)*I1 - R2*I2 = V1", "Mesh 2: -R2*I1 + (R2 + R3)*I2 = -V2", "Determinant D = (R1+R2)(R2+R3) - R2^2", "I1 = (V1*(R2+R3) - R2*V2) / D", "I2 = (R2*V1 - (R1+R2)*V2) / D", "Current in R2 = I1 - I2"},
      "Try V1 = 12, V2 = 6, R1 = 1, R2 = 2, R3 = 3 and check the signs of I1 and I2.",
      [](const D& v, Steps& s) {
          double V1 = P(0, 10), V2 = P(1, 4), R1 = pz(P(2, 2), 2), R2 = pz(P(3, 4), 4), R3 = pz(P(4, 4), 4), Dt = (R1 + R2) * (R2 + R3) - R2 * R2, I1 = (V1 * (R2 + R3) - R2 * V2) / Dt, I2 = (R2 * V1 - (R1 + R2) * V2) / Dt;
          ST(0, "Mesh analysis uses KVL with loop currents. Loop 1 includes V1, R1 and the shared R2.", {"R1 + R2", G(R1 + R2)});
          ST(1, "Loop 2 includes R2, R3 and V2 (against the loop direction, so it is -V2).", {"R2 + R3", G(R2 + R3)});
          ST(2, "Two equations, two unknowns. D = " + G(R1 + R2) + " * " + G(R2 + R3) + " - " + G(R2 * R2) + " = " + G(Dt) + ".", {"D", G(Dt)});
          ST(3, "I1 = " + G(I1) + " mA.", {"I1", G(I1) + " mA"});
          ST(4, "I2 = " + G(I2) + " mA. A negative value only means the real current goes against the arrow we drew.", {"I2", G(I2) + " mA"});
          ST(5, "The shared resistor carries I1 - I2 = " + G(I1 - I2) + " mA.", {"I(R2)", G(I1 - I2) + " mA"}); });

    E("Superposition", twoSrcLabel, "10, 4, 2, 4, 4",
      {"Only Va on (Vb = a wire):  V' = Va * (R2||R3) / (R1 + R2||R3)", "Only Vb on (Va = a wire):  V'' = Vb * (R2||R1) / (R3 + R2||R1)", "V = V' + V''", "Check against nodal analysis"},
      "Try Va = 6, Vb = 6, R1 = R3 = 2, R2 = 1. By symmetry both sources contribute equally.",
      [](const D& v, Steps& s) {
          double Va = P(0, 10), Vb = P(1, 4), R1 = pz(P(2, 2), 2), R2 = pz(P(3, 4), 4), R3 = pz(P(4, 4), 4), a = Va * par(R2, R3) / (R1 + par(R2, R3)), b = Vb * par(R2, R1) / (R3 + par(R2, R1)), Vn = (Va / R1 + Vb / R3) / (1 / R1 + 1 / R2 + 1 / R3);
          ST(0, "Switch off Vb by replacing it with a wire. R2 and R3 are now parallel: V' = " + G(a) + " V.", {"V'", G(a) + " V"});
          ST(1, "Switch off Va and keep Vb. R2 and R1 are now parallel: V'' = " + G(b) + " V.", {"V''", G(b) + " V"});
          ST(2, "In a linear circuit the total response is the sum of the responses to each source alone: " + G(a) + " + " + G(b) + " = " + G(a + b) + " V.", {"V", G(a + b) + " V"});
          ST(3, "Nodal analysis gives " + G(Vn) + " V, the same answer.", {"V (nodal)", G(Vn) + " V"}); });

    const char* thLabel = "Enter V (volts), R1 (series), R2 (shunt), RL (load), all resistors in kOhm";
    E("Thevenin's Theorem", thLabel, "12, 4, 4, 6",
      {"Remove the load RL (open circuit)", "Vth = V * R2 / (R1 + R2)", "Rth = R1 || R2   (source replaced by a short)", "IL = Vth / (Rth + RL)", "VL = IL * RL"},
      "Try V = 20, R1 = 5, R2 = 20, RL = 4. What load RL would take maximum power (RL = Rth)?",
      [](const D& v, Steps& s) {
          double V = P(0, 12), R1 = pz(P(1, 4), 4), R2 = pz(P(2, 4), 4), RL = pz(P(3, 6), 6), Vth = V * R2 / (R1 + R2), Rth = par(R1, R2), IL = Vth / (Rth + RL);
          ST(0, "Thevenin's theorem: any linear circuit seen from two terminals equals one voltage source Vth in series with one resistor Rth. First take the load off.", {"RL", "removed"});
          ST(1, "Open-circuit voltage at the terminals: Vth = " + G(V) + " * " + G(R2) + " / " + G(R1 + R2) + " = " + G(Vth) + " V.", {"Vth", G(Vth) + " V"});
          ST(2, "Turn the source off (a wire) and look back in: R1 and R2 are in parallel, Rth = " + G(Rth) + " kOhm.", {"Rth", G(Rth) + " kOhm"});
          ST(3, "Put the load back on the simple equivalent circuit: IL = " + G(Vth) + " / (" + G(Rth) + " + " + G(RL) + ") = " + G(IL) + " mA.", {"IL", G(IL) + " mA"});
          ST(4, "VL = " + G(IL) + " * " + G(RL) + " = " + G(IL * RL) + " V. Maximum power goes to the load when RL = Rth.", {"VL", G(IL * RL) + " V"}); });

    E("Norton's Theorem", thLabel, "12, 4, 4, 6",
      {"Short the load terminals", "IN = V / R1   (short-circuit current)", "RN = R1 || R2   (same as Rth)", "IL = IN * RN / (RN + RL)   (current divider)", "Vth = IN * RN   (Thevenin and Norton are equivalent)"},
      "Use the same numbers as Thevenin's Theorem and confirm that IL comes out identical.",
      [](const D& v, Steps& s) {
          double V = P(0, 12), R1 = pz(P(1, 4), 4), R2 = pz(P(2, 4), 4), RL = pz(P(3, 6), 6), IN = V / R1, RN = par(R1, R2), IL = IN * RN / (RN + RL);
          ST(0, "Norton's theorem: the circuit equals a current source IN in parallel with a resistor RN. Short the load terminals to find IN.", {"RL", "shorted"});
          ST(1, "With the output shorted, R2 is bypassed, so IN = " + G(V) + " / " + G(R1) + " = " + G(IN) + " mA.", {"IN", G(IN) + " mA"});
          ST(2, "RN is found the same way as Rth: R1 || R2 = " + G(RN) + " kOhm.", {"RN", G(RN) + " kOhm"});
          ST(3, "The source current splits between RN and RL: IL = " + G(IN) + " * " + G(RN) + " / " + G(RN + RL) + " = " + G(IL) + " mA.", {"IL", G(IL) + " mA"});
          ST(4, "IN * RN = " + G(IN * RN) + " V equals Vth, so the two theorems are the same circuit in two forms.", {"Vth", G(IN * RN) + " V"}); });

    /* ================= PN Junction Diode ================= */
    E("PN Junction", "Enter log10 of Na and Nd (doping per cm^3, 14 to 19)", "16, 15",
      {"Vt = kT/q = 25.85 mV   (300 K)", "Vbi = Vt * ln(Na * Nd / ni^2),   ni = 1.5e10 per cm^3", "W = sqrt( 2*eps*Vbi/q * (1/Na + 1/Nd) )", "xn = W*Na/(Na+Nd),   xp = W*Nd/(Na+Nd)"},
      "Raise both dopings by 100 times (add 2 to each exponent). Does Vbi rise and the depletion region shrink?",
      [](const D& v, Steps& s) {
          double la = std::clamp(P(0, 16), 14.0, 19.0), ld = std::clamp(P(1, 15), 14.0, 19.0), Na = std::pow(10, la), Nd = std::pow(10, ld), Vbi = VT * (std::log(10.0) * (la + ld) - 2 * std::log(1.5e10)),
                 eps = 11.7 * 8.854e-14, W = std::sqrt(2 * eps * Vbi / 1.602e-19 * (1 / Na + 1 / Nd)), xn = W * Na / (Na + Nd), xp = W * Nd / (Na + Nd);
          ST(0, "A P region (holes, Na) touches an N region (electrons, Nd). Thermal voltage Vt = 25.85 mV at room temperature.", {"Na", "1e" + G(la) + " /cm3"}, {"Nd", "1e" + G(ld) + " /cm3"});
          ST(1, "Electrons and holes diffuse across and leave fixed ions behind. This builds a barrier: Vbi = " + G(Vbi) + " V.", {"Vbi", G(Vbi) + " V"});
          ST(2, "The region emptied of free carriers is the depletion region: W = " + G(W * 1e4) + " um.", {"W", G(W * 1e4) + " um"});
          ST(3, "The depletion region reaches further into the LIGHTER doped side: xn = " + G(xn * 1e4) + " um, xp = " + G(xp * 1e4) + " um.", {"xn (N side)", G(xn * 1e4) + " um"}, {"xp (P side)", G(xp * 1e4) + " um"}); });

    E("Diode Biasing (Forward and Reverse)", "Enter source voltage V (negative = reverse) and series resistor R (kOhm)", "5, 1",
      {"Forward (V >= 0.7): Id = (V - 0.7) / R", "0 < V < 0.7: barrier not overcome, Id ~ 0", "Reverse (V < 0): Id = tiny leakage, diode is open", "Voltage across R = V - Vd"},
      "Try V = 3, R = 0.5. Then try V = -5. What changes?",
      [](const D& v, Steps& s) {
          double V = P(0, 5), R = pz(P(1, 1), 1);
          if (V >= 0.7) { double I = (V - 0.7) / R; ST(0, "Forward bias: the P side is positive. The barrier is overcome, so the diode conducts and drops about 0.7 V: Id = (" + G(V) + " - 0.7) / " + G(R) + " = " + G(I) + " mA.", {"Id", G(I) + " mA"}, {"Vd", "0.7 V"});
              ST(3, "The resistor takes the rest: " + G(I * R) + " V. The resistor is needed to limit the current.", {"V across R", G(I * R) + " V"}); }
          else if (V >= 0) { ST(1, "The source is below the 0.7 V barrier, so almost no current flows (a few nA).", {"Id", "~0"}); ST(3, "The diode takes nearly the whole " + G(V) + " V.", {"Vd", G(V) + " V"}); }
          else { ST(2, "Reverse bias: the depletion region widens and blocks current. Only a leakage of nA flows.", {"Id", "~0 (leakage)"}); ST(3, "The diode drops nearly all of " + G(V) + " V, and R drops almost nothing (until breakdown).", {"Vd", G(V) + " V"}); } });

    E("Diode I-V Characteristics", "Enter up to 6 voltages across the diode (V)", "-1, 0, 0.3, 0.5, 0.6, 0.7",
      {"Id = Is * (exp(V / (n*Vt)) - 1)", "Is = 1e-12 A,  n = 1,  Vt = 25.85 mV", "V < 0: Id ~ -Is   (reverse saturation current)", "V near 0.7 V: the current rises very sharply (knee)"},
      "Add 0.8 V to your list. How much does the current grow for every extra 0.06 V? (about 10 times)",
      [](const D& v, Steps& s) {
          D xs = v.empty() ? D{-1, 0, 0.3, 0.5, 0.6, 0.7} : v; if (xs.size() > 6) xs.resize(6); ST(1, "We use the Shockley diode equation with Is = 1 pA.", {"Is", "1e-12 A"}, {"Vt", "25.85 mV"});
          for (double x : xs) { x = std::clamp(x, -5.0, 0.9); double I = 1e-12 * (std::exp(x / VT) - 1) * 1e3;
              ST(x < 0 ? 2 : 0, "V = " + G(x) + " V gives Id = " + G(I) + " mA" + (x < -0.1 ? " (about -Is, the leakage)." : x < 0.4 ? " (almost nothing yet)." : x < 0.65 ? " (starting to turn on)." : " (the knee: now it conducts strongly)."), {"V", G(x) + " V"}, {"Id", G(I) + " mA"}); } });

    E("Half-Wave Rectifier", "Enter peak input Vm (V), load RL (kOhm), frequency f (Hz)", "10, 1, 50",
      {"Vp = Vm - 0.7   (one diode drop)", "Vdc = Vp / pi,    Vrms = Vp / 2", "Idc = Vdc / RL", "Efficiency = 40.6 %,  ripple factor = 1.21", "PIV = Vm,   ripple frequency = f"},
      "Try Vm = 20 and RL = 2. Predict Vdc before you run it.",
      [](const D& v, Steps& s) {
          double Vm = pz(P(0, 10), 10), RL = pz(P(1, 1), 1), f = pz(P(2, 50), 50), Vp = std::max(0.0, Vm - 0.7), Vdc = Vp / M_PI;
          ST(0, "Only the positive half-cycle passes. The silicon diode drops 0.7 V, so the output peak is " + G(Vp) + " V.", {"Vp", G(Vp) + " V"});
          ST(1, "The average (DC) value is Vp / pi = " + G(Vdc) + " V. The RMS value is Vp / 2 = " + G(Vp / 2) + " V.", {"Vdc", G(Vdc) + " V"}, {"Vrms", G(Vp / 2) + " V"});
          ST(2, "Idc = " + G(Vdc) + " / " + G(RL) + " = " + G(Vdc / RL) + " mA.", {"Idc", G(Vdc / RL) + " mA"});
          ST(3, "Only 40.6 % of the input power becomes DC. The ripple is large (factor 1.21).", {"Efficiency", "40.6 %"}, {"Ripple factor", "1.21"});
          ST(4, "During the negative half-cycle the diode must withstand the full peak: PIV = " + G(Vm) + " V. The ripple has the same frequency as the input: " + G(f) + " Hz.", {"PIV", G(Vm) + " V"}, {"Ripple frequency", G(f) + " Hz"}); });

    E("Full-Wave Rectifier", "Enter peak input Vm (V), load RL (kOhm), frequency f (Hz). A bridge rectifier is used", "10, 1, 50",
      {"Vp = Vm - 1.4   (two diodes conduct)", "Vdc = 2*Vp / pi,    Vrms = Vp / sqrt(2)", "Idc = Vdc / RL", "Efficiency = 81.2 %,  ripple factor = 0.48", "PIV = Vm,   ripple frequency = 2f"},
      "Use the same numbers as the Half-Wave Rectifier and compare Vdc. It should be about double.",
      [](const D& v, Steps& s) {
          double Vm = pz(P(0, 10), 10), RL = pz(P(1, 1), 1), f = pz(P(2, 50), 50), Vp = std::max(0.0, Vm - 1.4), Vdc = 2 * Vp / M_PI;
          ST(0, "Both half-cycles are used. In a bridge, TWO diodes conduct at a time, so the drop is 1.4 V: Vp = " + G(Vp) + " V.", {"Vp", G(Vp) + " V"});
          ST(1, "Vdc = 2 * Vp / pi = " + G(Vdc) + " V (double the half-wave value). Vrms = " + G(Vp / std::sqrt(2.0)) + " V.", {"Vdc", G(Vdc) + " V"}, {"Vrms", G(Vp / std::sqrt(2.0)) + " V"});
          ST(2, "Idc = " + G(Vdc) + " / " + G(RL) + " = " + G(Vdc / RL) + " mA.", {"Idc", G(Vdc / RL) + " mA"});
          ST(3, "Efficiency rises to 81.2 % and the ripple factor falls to 0.48.", {"Efficiency", "81.2 %"}, {"Ripple factor", "0.48"});
          ST(4, "PIV = " + G(Vm) + " V. The ripple frequency doubles to " + G(2 * f) + " Hz, which is easier to filter.", {"PIV", G(Vm) + " V"}, {"Ripple frequency", G(2 * f) + " Hz"}); });

    E("Clipper", "Enter peak input Vm (V) and reference voltage Vref (V)", "10, 3",
      {"Positive clipper with a reference: the diode conducts when Vin > Vref + 0.7", "Vin > Vref + 0.7:  Vout = Vref + 0.7", "Vin <= Vref + 0.7:  Vout = Vin", "Output swing:  -Vm  to  Vref + 0.7"},
      "Try Vref = -2. The clip level moves below zero.",
      [](const D& v, Steps& s) {
          double Vm = pz(P(0, 10), 10), Vr = P(1, 3), lim = Vr + 0.7;
          ST(0, "A clipper cuts off part of the waveform. The diode and the reference battery set the clip level at " + G(Vr) + " + 0.7 = " + G(lim) + " V.", {"Clip level", G(lim) + " V"});
          ST(1, "At the positive peak (+" + G(Vm) + " V): " + (Vm > lim ? "the diode conducts and the output is held at " + G(lim) + " V." : "the peak is below the clip level, so nothing is clipped."), {"Vin (peak)", G(Vm) + " V"}, {"Vout", G(std::min(Vm, lim)) + " V"});
          ST(2, "At the negative peak (-" + G(Vm) + " V) the diode is off, so the output follows the input.", {"Vin", G(-Vm) + " V"}, {"Vout", G(-Vm) + " V"});
          ST(3, "The output swings from " + G(-Vm) + " V up to " + G(std::min(Vm, lim)) + " V.", {"Vmin", G(-Vm) + " V"}, {"Vmax", G(std::min(Vm, lim)) + " V"}); });

    E("Clamper", "Enter peak input Vm (V) and DC reference Vref (V, 0 for a plain clamper)", "5, 0",
      {"Negative peak: the diode conducts and charges C to Vc = Vm - 0.7 + Vref", "Vout = Vin + Vc   (the diode is off after that)", "Output swing: (Vref - 0.7)  to  (2*Vm - 0.7 + Vref)", "The waveform shape is the same; only the DC level moves"},
      "Try Vm = 8 and Vref = 2. The whole waveform shifts up by an extra 2 V.",
      [](const D& v, Steps& s) {
          double Vm = pz(P(0, 5), 5), Vr = P(1, 0), Vc = Vm - 0.7 + Vr;
          ST(0, "On the first negative peak the diode conducts and the capacitor charges to Vc = " + G(Vm) + " - 0.7 + " + G(Vr) + " = " + G(Vc) + " V.", {"Vc", G(Vc) + " V"});
          ST(1, "After that the diode stays off and the capacitor (which cannot discharge quickly) acts like a battery in series: Vout = Vin + " + G(Vc) + ".", {"Vc", G(Vc) + " V"});
          ST(2, "The output goes from " + G(-Vm + Vc) + " V to " + G(Vm + Vc) + " V.", {"Vmin", G(-Vm + Vc) + " V"}, {"Vmax", G(Vm + Vc) + " V"});
          ST(3, "The shape is unchanged. The clamper only adds a DC level (" + G(Vc) + " V), so the waveform now sits above ground.", {"DC shift", G(Vc) + " V"}); });

    /* ================= Zener Diode ================= */
    E("Zener Diode Applications (Shunt Regulator)", "Enter Vin, Vz (V), series Rs and load RL (kOhm)", "12, 5.1, 0.5, 1",
      {"IT = (Vin - Vz) / Rs", "IL = Vz / RL", "Iz = IT - IL     (must stay above about 1 mA)", "Pz = Vz * Iz", "If Iz <= 0 the Zener is OFF: Vout = Vin * RL / (Rs + RL)"},
      "Try RL = 0.3. The load takes more current. Does the Zener still regulate?",
      [](const D& v, Steps& s) {
          double Vi = P(0, 12), Vz = pz(P(1, 5.1), 5.1), Rs = pz(P(2, 0.5), 0.5), RL = pz(P(3, 1), 1), IT = (Vi - Vz) / Rs, IL = Vz / RL, Iz = IT - IL;
          ST(0, "The Zener is in parallel with the load and holds the output at Vz. Rs carries the total current: IT = (" + G(Vi) + " - " + G(Vz) + ") / " + G(Rs) + " = " + G(IT) + " mA.", {"IT", G(IT) + " mA"});
          ST(1, "The load takes IL = " + G(Vz) + " / " + G(RL) + " = " + G(IL) + " mA.", {"IL", G(IL) + " mA"});
          if (Iz > 1) { ST(2, "Whatever the load does not use goes through the Zener: Iz = " + G(Iz) + " mA. This is above the minimum, so the output stays at " + G(Vz) + " V.", {"Iz", G(Iz) + " mA"}, {"Vout", G(Vz) + " V"}); ST(3, "The Zener dissipates Pz = " + G(Vz * Iz) + " mW. Check it is below the rated power.", {"Pz", G(Vz * Iz) + " mW"}); }
          else { double Vo = Vi * RL / (Rs + RL); ST(4, "Iz is too small (" + G(Iz) + " mA). The Zener drops out of breakdown and no longer regulates. The output becomes " + G(Vo) + " V.", {"Iz", G(Iz) + " mA"}, {"Vout", G(Vo) + " V"}); } });

    auto zen = [](double Vi, double Vz, double Rs, double RL, double rz) { return (Vi / Rs + Vz / rz) / (1 / Rs + 1 / rz + 1 / RL); };
    E("Line Regulation", "Enter Vin1, Vin2 (V), Vz (V), Rs, RL, rz (kOhm; rz is the Zener's dynamic resistance)", "10, 14, 5.1, 0.5, 1, 0.02",
      {"Vout = (Vin/Rs + Vz/rz) / (1/Rs + 1/rz + 1/RL)", "dVout = Vout(Vin2) - Vout(Vin1)", "Line regulation = dVout / dVin   (mV per V)", "Smaller rz  ->  better regulation"},
      "Try rz = 0.1. Regulation gets worse because the Zener is less ideal.",
      [zen](const D& v, Steps& s) {
          double a = pz(P(0, 10), 10), b = pz(P(1, 14), 14), Vz = pz(P(2, 5.1), 5.1), Rs = pz(P(3, 0.5), 0.5), RL = pz(P(4, 1), 1), rz = pz(P(5, 0.02), 0.02), o1 = zen(a, Vz, Rs, RL, rz), o2 = zen(b, Vz, Rs, RL, rz);
          ST(0, "Line regulation tells how steady the output is when the INPUT changes (load fixed). At Vin = " + G(a) + " V: Vout = " + G(o1) + " V.", {"Vout1", G(o1) + " V"});
          ST(0, "At Vin = " + G(b) + " V: Vout = " + G(o2) + " V.", {"Vout2", G(o2) + " V"});
          ST(1, "dVout = " + G(o2) + " - " + G(o1) + " = " + G((o2 - o1) * 1000) + " mV for dVin = " + G(b - a) + " V.", {"dVout", G((o2 - o1) * 1000) + " mV"});
          ST(2, "Line regulation = " + G((o2 - o1) * 1000 / (b - a == 0 ? 1 : b - a)) + " mV/V. The input changed by " + G(100 * (b - a) / a) + " % but the output moved only " + G(100 * (o2 - o1) / o1) + " %.", {"Line regulation", G((o2 - o1) * 1000 / (b - a == 0 ? 1 : b - a)) + " mV/V"});
          ST(3, "Roughly dVout / dVin = rz / (Rs + rz) when RL is large. A small rz keeps the output steady.", {"rz", G(rz) + " kOhm"}); });

    E("Load Regulation", "Enter Vin, Vz (V), Rs, RL_full (heavy load), RL_light (kOhm), rz (kOhm)", "12, 5.1, 0.5, 0.5, 10, 0.02",
      {"Vout = (Vin/Rs + Vz/rz) / (1/Rs + 1/rz + 1/RL)", "Light load: Vnl = Vout at the larger RL", "Full load: Vfl = Vout at the smaller RL", "Load regulation = (Vnl - Vfl) / Vfl * 100 %"},
      "Make RL_full smaller (0.3). A heavier load pulls the output down more.",
      [zen](const D& v, Steps& s) {
          double Vi = P(0, 12), Vz = pz(P(1, 5.1), 5.1), Rs = pz(P(2, 0.5), 0.5), Rf = pz(P(3, 0.5), 0.5), Rn = pz(P(4, 10), 10), rz = pz(P(5, 0.02), 0.02), vn = zen(Vi, Vz, Rs, Rn, rz), vf = zen(Vi, Vz, Rs, Rf, rz);
          ST(1, "Load regulation tells how steady the output is when the LOAD changes (input fixed). Light load RL = " + G(Rn) + " kOhm gives " + G(vn) + " V.", {"Vnl", G(vn) + " V"});
          ST(2, "Heavy load RL = " + G(Rf) + " kOhm gives " + G(vf) + " V.", {"Vfl", G(vf) + " V"});
          ST(3, "Load regulation = (" + G(vn) + " - " + G(vf) + ") / " + G(vf) + " = " + G(100 * (vn - vf) / vf) + " %.", {"Load regulation", G(100 * (vn - vf) / vf) + " %"});
          ST(0, "The Zener absorbs the current the load does not use, so the output barely changes: " + G((vn - vf) * 1000) + " mV for a large change in load.", {"Change", G((vn - vf) * 1000) + " mV"}); });

    /* ================= BJT ================= */
    E("BJT Operation", "Enter VBE and VCE (volts) of an NPN transistor", "0.7, 5",
      {"EB junction is forward biased if VBE >= 0.7 V", "CB junction is reverse biased if VCB = VCE - VBE > 0", "Cutoff: both junctions reverse (switch OFF)", "Active: EB forward, CB reverse (amplifier)", "Saturation: both forward, VCE ~ 0.2 V (switch ON)"},
      "Try VBE = 0.7 and VCE = 0.1. Which region is that?",
      [](const D& v, Steps& s) {
          double be = P(0, 0.7), ce = P(1, 5), cb = ce - be; bool eb = be >= 0.7;
          ST(0, "VBE = " + G(be) + " V, so the emitter-base junction is " + (eb ? "forward biased: the emitter injects electrons into the thin base." : "not forward biased: no electrons are injected."), {"VBE", G(be) + " V"});
          ST(1, "VCB = " + G(ce) + " - " + G(be) + " = " + G(cb) + " V, so the collector-base junction is " + (cb > 0 ? "reverse biased: it sweeps electrons from the base into the collector." : "forward biased."), {"VCB", G(cb) + " V"});
          if (!eb) ST(2, "Region: CUTOFF. No base current, so IC is about 0. The transistor is an open switch.", {"Region", "Cutoff"});
          else if (ce > 0.2 && cb > 0) ST(3, "Region: ACTIVE. IC = beta * IB. Use this region to amplify.", {"Region", "Active"});
          else ST(4, "Region: SATURATION. IC no longer follows beta * IB. The transistor is a closed switch.", {"Region", "Saturation"}); });

    E("BJT Characteristics", "Enter IB (uA), beta, VCE (V)", "40, 100, 5",
      {"Active region: IC = beta * IB * (1 + VCE / VA),   VA = 100 V", "IE = IB + IC", "alpha = IC / IE = beta / (beta + 1)", "Saturation (VCE < 0.2 V): IC is smaller than beta * IB"},
      "Double IB to 80 uA. The output curve moves up by the same factor.",
      [](const D& v, Steps& s) {
          double Ib = pz(P(0, 40), 40), b = pz(P(1, 100), 100), ce = P(2, 5), Ic = b * Ib * (1 + std::max(ce, 0.0) / 100) / 1000;
          ST(0, ce < 0.2 ? "VCE is below 0.2 V: this is the saturation region, so IC drops below beta * IB. (The simple formula is shown only for comparison.)" : "Each curve of IC against VCE is for one base current. In the active region it is almost flat: IC = " + G(b) + " * " + G(Ib) + " uA = " + G(Ic) + " mA (a small slope comes from the Early effect).", {"IB", G(Ib) + " uA"}, {"IC", G(Ic) + " mA"});
          ST(1, "IE = IB + IC = " + G(Ib / 1000) + " + " + G(Ic) + " = " + G(Ib / 1000 + Ic) + " mA.", {"IE", G(Ib / 1000 + Ic) + " mA"});
          ST(2, "alpha = " + G(b / (b + 1)) + ", just under 1: almost all of the emitter current reaches the collector.", {"alpha", G(b / (b + 1))}); });

    E("BJT Biasing", "Enter Vcc (V), Rb, Rc (kOhm), beta. Fixed bias circuit", "12, 470, 2.2, 100",
      {"IB = (Vcc - 0.7) / Rb", "IC = beta * IB", "VCE = Vcc - IC * Rc", "Load line ends: VCE = Vcc  and  IC = Vcc / Rc", "If VCE < 0.2 V the transistor is saturated"},
      "Try Rb = 100. The Q-point moves toward saturation.",
      [](const D& v, Steps& s) {
          double Vc = P(0, 12), Rb = pz(P(1, 470), 470), Rc = pz(P(2, 2.2), 2.2), b = pz(P(3, 100), 100), Ib = (Vc - 0.7) / Rb, Ic = b * Ib, Vce = Vc - Ic * Rc;
          ST(0, "Biasing sets the DC operating point (Q-point). The base resistor sets IB = (" + G(Vc) + " - 0.7) / " + G(Rb) + " = " + G(Ib * 1000) + " uA.", {"IB", G(Ib * 1000) + " uA"});
          ST(1, "IC = " + G(b) + " * " + G(Ib * 1000) + " uA = " + G(Ic) + " mA.", {"IC", G(Ic) + " mA"});
          ST(2, "VCE = " + G(Vc) + " - " + G(Ic) + " * " + G(Rc) + " = " + G(Vce) + " V.", {"VCE", G(Vce) + " V"});
          ST(3, "The load line joins VCE = " + G(Vc) + " V and IC = " + G(Vc / Rc) + " mA. The Q-point (" + G(Vce) + " V, " + G(Ic) + " mA) lies on it.", {"IC(sat)", G(Vc / Rc) + " mA"}, {"VCE(cutoff)", G(Vc) + " V"});
          ST(4, Vce < 0.2 ? "VCE is below 0.2 V: the transistor is SATURATED, so the real IC is only about " + G((Vc - 0.2) / Rc) + " mA. Increase Rb." : "VCE is a good value, near the middle of the load line, so the transistor is in the ACTIVE region and can amplify.", {"Region", Vce < 0.2 ? "Saturation" : "Active"}); });

    E("BJT Stability", "Enter beta, Rb (kOhm, base resistance), Re (kOhm, emitter resistor), dICO (uA, leakage change)", "100, 10, 1, 10",
      {"Fixed bias:  S = 1 + beta", "Emitter-stabilised:  S = (1+beta)(1 + Rb/Re) / (1 + beta + Rb/Re)", "dIC = S * dICO", "Smaller S = more stable Q-point"},
      "Try Re = 5. A larger emitter resistor lowers S and makes the circuit more stable.",
      [](const D& v, Steps& s) {
          double b = pz(P(0, 100), 100), Rb = pz(P(1, 10), 10), Re = pz(P(2, 1), 1), dI = P(3, 10), S1 = 1 + b, S2 = (1 + b) * (1 + Rb / Re) / (1 + b + Rb / Re);
          ST(0, "The stability factor S = dIC / dICO says how much IC drifts when leakage current ICO rises with temperature. A fixed bias circuit has S = " + G(S1) + ", which is poor.", {"S (fixed bias)", G(S1)});
          ST(1, "An emitter resistor gives negative feedback: S = " + G(S2) + ", which is much better.", {"S (with Re)", G(S2)});
          ST(2, "If ICO grows by " + G(dI) + " uA, IC drifts by " + G(S1 * dI) + " uA (fixed bias) but only " + G(S2 * dI) + " uA (with Re).", {"dIC fixed", G(S1 * dI) + " uA"}, {"dIC with Re", G(S2 * dI) + " uA"});
          ST(3, "The lowest possible S is 1 (a perfectly stable circuit).", {"best S", "1"}); });

    /* ================= MOSFET ================= */
    E("MOSFET Operation", "Enter VGS, VDS, Vt (volts) for an n-channel enhancement MOSFET", "3, 5, 2",
      {"Overdrive Vov = VGS - Vt", "VGS < Vt: cutoff (no channel)", "VGS > Vt and VDS < Vov: triode (channel acts like a resistor)", "VGS > Vt and VDS >= Vov: saturation (channel pinched off, an amplifier)"},
      "Try VGS = 3, VDS = 0.5, Vt = 2. Which region does that give?",
      [](const D& v, Steps& s) {
          double g = P(0, 3), d = P(1, 5), t = P(2, 2), ov = g - t;
          ST(0, "The gate voltage above the threshold is the overdrive: Vov = " + G(g) + " - " + G(t) + " = " + G(ov) + " V.", {"Vov", G(ov) + " V"});
          if (ov <= 0) ST(1, "VGS is not above Vt, so no inversion layer (channel) forms. The MOSFET is OFF: cutoff.", {"Region", "Cutoff"});
          else if (d < ov) ST(2, "VDS (" + G(d) + ") is below Vov (" + G(ov) + "): TRIODE region. The channel reaches the drain and the device acts as a voltage-controlled resistor.", {"Region", "Triode"});
          else ST(3, "VDS (" + G(d) + ") is at least Vov (" + G(ov) + "): SATURATION. The channel pinches off near the drain and ID depends on VGS, not VDS.", {"Region", "Saturation"}); });

    E("MOSFET Characteristics", "Enter VGS, VDS, Vt (volts) and k (mA/V^2)", "4, 6, 2, 0.5",
      {"Vov = VGS - Vt", "Cutoff: ID = 0", "Triode: ID = k * (2*Vov*VDS - VDS^2)", "Saturation: ID = k * Vov^2"},
      "Double VGS - Vt. In saturation ID becomes 4 times larger because the law is square.",
      [](const D& v, Steps& s) {
          double g = P(0, 4), d = std::max(0.0, P(1, 6)), t = P(2, 2), k = pz(P(3, 0.5), 0.5), ov = g - t;
          ST(0, "Vov = " + G(g) + " - " + G(t) + " = " + G(ov) + " V.", {"Vov", G(ov) + " V"});
          if (ov <= 0) ST(1, "Cutoff, so ID = 0.", {"ID", "0"});
          else if (d < ov) { double I = k * (2 * ov * d - d * d); ST(2, "Triode (VDS < Vov): ID = " + G(k) + " * (2 * " + G(ov) + " * " + G(d) + " - " + G(d * d) + ") = " + G(I) + " mA.", {"ID", G(I) + " mA"}); }
          else { double I = k * ov * ov; ST(3, "Saturation (VDS >= Vov): ID = " + G(k) + " * " + G(ov) + "^2 = " + G(I) + " mA. It stays the same however large VDS gets (ideally).", {"ID", G(I) + " mA"}); } });

    E("MOSFET Biasing", "Enter VDD (V), R1, R2, RD (kOhm), Vt (V), k (mA/V^2). Voltage-divider bias", "12, 200, 100, 2, 2, 0.5",
      {"VG = VDD * R2 / (R1 + R2)    (no gate current, so a pure divider)", "VGS = VG   (source is grounded)", "ID = k * (VGS - Vt)^2", "VDS = VDD - ID * RD", "Saturation holds if VDS >= VGS - Vt"},
      "Try RD = 4. The drain voltage drops. Does the MOSFET stay in saturation?",
      [](const D& v, Steps& s) {
          double Vd = P(0, 12), R1 = pz(P(1, 200), 200), R2 = pz(P(2, 100), 100), RD = pz(P(3, 2), 2), Vt = P(4, 2), k = pz(P(5, 0.5), 0.5), VG = Vd * R2 / (R1 + R2), ov = VG - Vt, Id = ov > 0 ? k * ov * ov : 0, Vds = Vd - Id * RD;
          ST(0, "The gate draws no current, so R1 and R2 form a plain divider: VG = " + G(Vd) + " * " + G(R2) + " / " + G(R1 + R2) + " = " + G(VG) + " V.", {"VG", G(VG) + " V"});
          ST(1, "The source is grounded, so VGS = " + G(VG) + " V.", {"VGS", G(VG) + " V"});
          ST(2, ov > 0 ? "ID = " + G(k) + " * (" + G(VG) + " - " + G(Vt) + ")^2 = " + G(Id) + " mA." : "VGS is not above Vt, so ID = 0 and the MOSFET is off.", {"ID", G(Id) + " mA"});
          ST(3, "VDS = " + G(Vd) + " - " + G(Id) + " * " + G(RD) + " = " + G(Vds) + " V.", {"VDS", G(Vds) + " V"});
          ST(4, ov <= 0 ? "Cutoff: raise R2 or lower R1." : Vds >= ov ? "VDS (" + G(Vds) + ") >= Vov (" + G(ov) + "): the MOSFET is in SATURATION, a good bias for amplifying." : "VDS is below Vov: the MOSFET is in the TRIODE region. Reduce RD or VGS.", {"Region", ov <= 0 ? "Cutoff" : Vds >= ov ? "Saturation" : "Triode"}); });

    /* ================= Op-Amp ================= */
    E("Op-Amp Parameters", "Enter closed-loop gain, GBW (MHz), slew rate (V/us), output peak (V), CMRR (dB)", "10, 1, 0.5, 5, 90",
      {"Closed-loop bandwidth: BW = GBW / gain", "Full-power bandwidth: fmax = SR / (2*pi*Vpk)", "Usable bandwidth = the smaller of the two", "CMRR = Ad / Ac = 10^(dB / 20)"},
      "Raise the gain to 100. Bandwidth falls by 10 times (the gain-bandwidth trade-off).",
      [](const D& v, Steps& s) {
          double A = pz(P(0, 10), 10), gbw = pz(P(1, 1), 1), sr = pz(P(2, 0.5), 0.5), vp = pz(P(3, 5), 5), cm = P(4, 90), bw = gbw / A * 1000, fm = sr / (2 * M_PI * vp) * 1000;
          ST(0, "Gain and bandwidth trade off: BW = " + G(gbw) + " MHz / " + G(A) + " = " + G(bw) + " kHz.", {"BW (small signal)", G(bw) + " kHz"});
          ST(1, "A large output cannot change faster than the slew rate: fmax = " + G(sr) + " / (2*pi*" + G(vp) + ") = " + G(fm) + " kHz.", {"fmax (slew limit)", G(fm) + " kHz"});
          ST(2, "Above the smaller limit the output is distorted: usable bandwidth = " + G(std::min(bw, fm)) + " kHz (limited by " + (fm < bw ? "slew rate" : "gain-bandwidth") + ").", {"Usable", G(std::min(bw, fm)) + " kHz"});
          ST(3, "CMRR of " + G(cm) + " dB means the op-amp amplifies the difference " + G(std::pow(10, cm / 20)) + " times more than a common-mode signal. Higher is better.", {"Ad/Ac", G(std::pow(10, cm / 20))}); });

    E("Ideal Op-Amp", "Enter V+ (volts at the non-inverting input), R1, Rf (kOhm). Shown in a non-inverting amplifier", "2, 1, 4",
      {"Ideal: A = infinity, Rin = infinity, Rout = 0, bandwidth = infinity", "Rule 1: no current enters either input (I+ = I- = 0)", "Rule 2 (virtual short): V- = V+", "Then the current through R1 is V- / R1 and Vout = V- + I*Rf"},
      "Change V+ to 3. The inverting input follows it exactly: V- = V+.",
      [](const D& v, Steps& s) {
          double Vp = P(0, 2), R1 = pz(P(1, 1), 1), Rf = pz(P(2, 4), 4), I = Vp / R1, Vo = Vp + I * Rf;
          ST(0, "An ideal op-amp has infinite gain, infinite input resistance, zero output resistance and infinite bandwidth.", {"A", "infinity"}, {"Rin", "infinity"}, {"Rout", "0"});
          ST(1, "Infinite Rin means no current flows into the two inputs.", {"I+", "0"}, {"I-", "0"});
          ST(2, "With negative feedback the output moves until the inputs are equal, because a finite output over infinite gain needs a zero difference: V- = V+ = " + G(Vp) + " V.", {"V-", G(Vp) + " V"});
          ST(3, "R1 now has " + G(Vp) + " V across it, so I = " + G(I) + " mA. This same current passes through Rf, so Vout = " + G(Vp) + " + " + G(I) + " * " + G(Rf) + " = " + G(Vo) + " V.", {"I", G(I) + " mA"}, {"Vout", G(Vo) + " V"}); });

    E("Op-Amp Equivalent Circuit", "Enter V+, V- (V), open-loop gain A, Rout and RL (kOhm)", "0.00005, 0, 100000, 0.075, 10",
      {"Differential input: Vd = V+ - V-", "Internal source: A * Vd", "Rin is very large (input current ~ 0)", "Vout = A * Vd * RL / (Rout + RL), limited to +/- Vsat (12 V)"},
      "Try V+ = 0.0002. The output tries to reach 20 V but clips at the supply limit.",
      [](const D& v, Steps& s) {
          double p = P(0, 0.00005), n = P(1, 0), A = pz(P(2, 100000), 100000), Ro = P(3, 0.075), RL = pz(P(4, 10), 10), Vd = p - n, src = A * Vd, Vo = src * RL / (Ro + RL), Vc = clipV(Vo, 12);
          ST(0, "The input stage only sees the difference: Vd = " + G(p) + " - " + G(n) + " = " + G(Vd) + " V.", {"Vd", G(Vd) + " V"});
          ST(1, "A controlled voltage source inside makes A * Vd = " + G(A) + " * " + G(Vd) + " = " + G(src) + " V.", {"A * Vd", G(src) + " V"});
          ST(2, "The input looks like a huge resistance (MOhm), so almost no current is drawn from the source.", {"Rin", "very large"});
          ST(3, "Rout and RL form a divider: " + G(src) + " * " + G(RL) + " / " + G(Ro + RL) + " = " + G(Vo) + " V." + (std::fabs(Vo) > 12 ? " This exceeds the supply, so the output saturates at " + G(Vc) + " V." : " A real output is slightly below the ideal because of Rout."), {"Vout", G(Vc) + " V"}); });

    E("Inverting Amplifier", "Enter Vin (V), Rin and Rf (kOhm)", "1, 1, 10",
      {"Virtual ground: V- = V+ = 0 V", "I = Vin / Rin   (this current goes into Rf)", "Vout = 0 - I * Rf = -(Rf / Rin) * Vin", "Gain = -Rf / Rin   (the output is inverted)", "Output saturates at +/- 12 V"},
      "Try Vin = 2 with the default resistors. Gain -10 gives -20 V, but the output clips at -12 V.",
      [](const D& v, Steps& s) {
          double Vi = P(0, 1), Ri = pz(P(1, 1), 1), Rf = pz(P(2, 10), 10), I = Vi / Ri, Vo = -I * Rf, Vc = clipV(Vo, 12);
          ST(0, "The non-inverting input is grounded, and feedback forces the inverting input to the same level. It is a virtual ground.", {"V-", "0 V"});
          ST(1, "So Rin has Vin across it: I = " + G(Vi) + " / " + G(Ri) + " = " + G(I) + " mA. No current enters the op-amp, so all of it goes through Rf.", {"I", G(I) + " mA"});
          ST(2, "The output drops by I * Rf below the virtual ground: Vout = -" + G(I) + " * " + G(Rf) + " = " + G(Vo) + " V.", {"Vout (ideal)", G(Vo) + " V"});
          ST(3, "Gain = -" + G(Rf) + " / " + G(Ri) + " = " + G(-Rf / Ri) + ". The minus sign means a 180-degree phase inversion.", {"Gain", G(-Rf / Ri)});
          ST(4, std::fabs(Vo) > 12 ? "The ideal result is beyond the supply, so the real output saturates at " + G(Vc) + " V." : "The result is within the supply limits, so it is not clipped.", {"Vout (real)", G(Vc) + " V"}); });

    E("Non-Inverting Amplifier", "Enter Vin (V), R1 (to ground) and Rf (feedback) in kOhm", "1, 1, 9",
      {"V- = V+ = Vin   (virtual short)", "I = Vin / R1   (through R1 to ground)", "Vout = Vin + I * Rf", "Gain = 1 + Rf / R1   (never below 1, no inversion)", "Output saturates at +/- 12 V"},
      "Make Rf = 0 (or very small). The gain drops to 1: that is a voltage follower.",
      [](const D& v, Steps& s) {
          double Vi = P(0, 1), R1 = pz(P(1, 1), 1), Rf = std::max(0.0, P(2, 9)), I = Vi / R1, Vo = Vi + I * Rf, Vc = clipV(Vo, 12);
          ST(0, "The signal drives the non-inverting input, so the feedback holds the inverting input at the same voltage: V- = " + G(Vi) + " V.", {"V-", G(Vi) + " V"});
          ST(1, "R1 goes to ground, so it carries I = " + G(Vi) + " / " + G(R1) + " = " + G(I) + " mA.", {"I", G(I) + " mA"});
          ST(2, "That current also flows through Rf, lifting the output: Vout = " + G(Vi) + " + " + G(I) + " * " + G(Rf) + " = " + G(Vo) + " V.", {"Vout (ideal)", G(Vo) + " V"});
          ST(3, "Gain = 1 + " + G(Rf) + " / " + G(R1) + " = " + G(1 + Rf / R1) + ". The output is in phase with the input.", {"Gain", G(1 + Rf / R1)});
          ST(4, std::fabs(Vo) > 12 ? "The ideal result exceeds the supply, so the output saturates at " + G(Vc) + " V." : "The output is within the supply limits.", {"Vout (real)", G(Vc) + " V"}); });

    E("Voltage Follower", "Enter source Vs (V), source resistance Rs and load RL (kOhm)", "5, 10, 1",
      {"Without buffer: Vload = Vs * RL / (Rs + RL)", "Buffer: V- = Vout = V+ = Vs", "Rin = very large, so no current is drawn from the source", "Vout = Vs   (gain = 1), and the op-amp can drive RL easily"},
      "Make RL very small, such as 0.1. Direct connection loses almost everything, but the buffer still gives Vs.",
      [](const D& v, Steps& s) {
          double Vs = P(0, 5), Rs = pz(P(1, 10), 10), RL = pz(P(2, 1), 1), Vd = Vs * RL / (Rs + RL);
          ST(0, "Connect the weak source straight to the load and they form a divider: " + G(Vs) + " * " + G(RL) + " / " + G(Rs + RL) + " = " + G(Vd) + " V. A lot of the signal is lost.", {"Vload (direct)", G(Vd) + " V"});
          ST(1, "The output is wired directly to the inverting input. Feedback forces Vout = V- = V+ = " + G(Vs) + " V.", {"Vout", G(Vs) + " V"});
          ST(2, "The non-inverting input draws almost no current, so the source does not see the load and there is no voltage drop across Rs.", {"I from source", "~0"});
          ST(3, "Gain = 1. The follower acts as a buffer: high input resistance, low output resistance. The load now gets " + G(Vs) + " V instead of " + G(Vd) + " V.", {"Gain", "1"}, {"Vload (buffer)", G(Vs) + " V"}); });

    E("Summing Amplifier (Summer)", "Enter V1, V2, V3 (V), R1, R2, R3 and Rf (kOhm)", "1, 2, 3, 10, 10, 10, 10",
      {"Virtual ground at the inverting input: V- = 0 V", "I1 = V1/R1,  I2 = V2/R2,  I3 = V3/R3", "All currents add at the inverting node:  If = I1 + I2 + I3", "Vout = -Rf * (V1/R1 + V2/R2 + V3/R3)", "With R1 = R2 = R3 = Rf:  Vout = -(V1 + V2 + V3)"},
      "Try V1 = 2, V2 = -1, V3 = 0.5. A negative input subtracts.",
      [](const D& v, Steps& s) {
          double V1 = P(0, 1), V2 = P(1, 2), V3 = P(2, 3), R1 = pz(P(3, 10), 10), R2 = pz(P(4, 10), 10), R3 = pz(P(5, 10), 10), Rf = pz(P(6, 10), 10), a = V1 / R1, b = V2 / R2, c = V3 / R3, Vo = -Rf * (a + b + c);
          ST(0, "The inverting input is a virtual ground, so each input resistor sees its own source voltage with no interaction between sources.", {"V-", "0 V"});
          ST(1, "I1 = " + G(a) + " mA, I2 = " + G(b) + " mA, I3 = " + G(c) + " mA.", {"I1", G(a) + " mA"}, {"I2", G(b) + " mA"}, {"I3", G(c) + " mA"});
          ST(2, "No current enters the op-amp, so they all go through Rf: If = " + G(a + b + c) + " mA.", {"If", G(a + b + c) + " mA"});
          ST(3, "Vout = -" + G(Rf) + " * " + G(a + b + c) + " = " + G(Vo) + " V" + (std::fabs(Vo) > 12 ? ", beyond the supply, so it clips at " + G(clipV(Vo, 12)) + " V." : "."), {"Vout", G(clipV(Vo, 12)) + " V"});
          ST(4, "With equal resistors this is a simple adder that outputs the negative sum: " + G(-(V1 + V2 + V3)) + " V. Unequal R values give a weighted sum (the basis of a DAC).", {"-(V1+V2+V3)", G(-(V1 + V2 + V3)) + " V"}); });

    E("Comparator", "Enter Vin, Vref and the saturation voltage Vsat (V)", "2, 1.5, 12",
      {"Vd = Vin - Vref", "Vd > 0:  Vout = +Vsat", "Vd < 0:  Vout = -Vsat", "No feedback: even a tiny difference swings the output fully"},
      "Try Vin = 1.4999 and Vin = 1.5001 with Vref = 1.5. The output flips completely.",
      [](const D& v, Steps& s) {
          double Vi = P(0, 2), Vr = P(1, 1.5), Vs = pz(P(2, 12), 12), Vd = Vi - Vr;
          ST(0, "No feedback is used, so the full open-loop gain applies. The difference is Vd = " + G(Vi) + " - " + G(Vr) + " = " + G(Vd) + " V.", {"Vd", G(Vd) + " V"});
          if (Vd > 0) ST(1, "Vin is above Vref, so the output saturates high: +" + G(Vs) + " V.", {"Vout", "+" + G(Vs) + " V"}); else if (Vd < 0) ST(2, "Vin is below Vref, so the output saturates low: -" + G(Vs) + " V.", {"Vout", "-" + G(Vs) + " V"}); else ST(3, "Vd is exactly 0: the output is at the switching point.", {"Vout", "switching"});
          ST(3, "The comparator turns an analog voltage into a digital-like HIGH or LOW. It is used for zero-crossing detection and level detection.", {"Type", "1-bit ADC"}); });

    E("Difference Amplifier", "Enter V1 (inverting side), V2 (non-inverting side), R1 and Rf (kOhm). Resistors are matched", "2, 3, 10, 50",
      {"V+ = V2 * Rf / (R1 + Rf)", "V- = V+ (virtual short)", "KCL at the inverting node: (V1 - V-)/R1 = (V- - Vout)/Rf", "Vout = (Rf / R1) * (V2 - V1)", "A common input (V1 = V2) gives Vout = 0"},
      "Set V1 = V2 = 4. The output is 0, so the common signal is rejected. Only the difference is amplified.",
      [](const D& v, Steps& s) {
          double V1 = P(0, 2), V2 = P(1, 3), R1 = pz(P(2, 10), 10), Rf = pz(P(3, 50), 50), Vp = V2 * Rf / (R1 + Rf), Vo = Rf / R1 * (V2 - V1);
          ST(0, "R1 and Rf divide V2 at the non-inverting input: V+ = " + G(V2) + " * " + G(Rf) + " / " + G(R1 + Rf) + " = " + G(Vp) + " V.", {"V+", G(Vp) + " V"});
          ST(1, "Feedback makes the inverting input equal: V- = " + G(Vp) + " V.", {"V-", G(Vp) + " V"});
          ST(2, "The current through R1 is (" + G(V1) + " - " + G(Vp) + ") / " + G(R1) + " = " + G((V1 - Vp) / R1) + " mA and it all flows on through Rf.", {"I", G((V1 - Vp) / R1) + " mA"});
          ST(3, "Vout = (" + G(Rf) + " / " + G(R1) + ") * (" + G(V2) + " - " + G(V1) + ") = " + G(Vo) + " V" + (std::fabs(Vo) > 12 ? ", but the real output clips at " + G(clipV(Vo, 12)) + " V." : "."), {"Vout", G(clipV(Vo, 12)) + " V"});
          ST(4, std::fabs(V1 - V2) < 1e-12 ? "V1 = V2, so the output is 0: the common-mode part is cancelled." : "A signal common to both inputs cancels. This is why it is used to reject noise (the idea behind CMRR).", {"Gain", G(Rf / R1)}); });

    E("Integrator", "Enter constant Vin (V), R (kOhm), C (uF) and total time T (ms)", "2, 10, 1, 4",
      {"Vout(t) = -(1 / (R*C)) * integral of Vin dt", "Constant input:  Vout(t) = -Vin * t / (R*C)", "R * C (kOhm * uF) is a time in ms", "The output cannot go beyond +/- 12 V"},
      "Try a larger C (such as 4 uF). The slope of the ramp becomes 4 times smaller.",
      [](const D& v, Steps& s) {
          double Vi = P(0, 2), R = pz(P(1, 10), 10), C = pz(P(2, 1), 1), T = pz(P(3, 4), 4), RC = R * C;
          ST(0, "A capacitor in the feedback path integrates the current Vin / R. The output starts at 0 V.", {"t", "0 ms"}, {"Vout", "0 V"});
          ST(2, "R * C = " + G(R) + " * " + G(C) + " = " + G(RC) + " ms. The slope of the output is -Vin / RC = " + G(-Vi / RC) + " V/ms.", {"RC", G(RC) + " ms"}, {"slope", G(-Vi / RC) + " V/ms"});
          for (int k = 1; k <= 4; ++k) { double t = T * k / 4, o = -Vi * t / RC; ST(1, "At t = " + G(t) + " ms: Vout = -" + G(Vi) + " * " + G(t) + " / " + G(RC) + " = " + G(o) + " V" + (std::fabs(o) > 12 ? ", but the real output is clipped at " + G(clipV(o, 12)) + " V." : "."), {"t", G(t) + " ms"}, {"Vout", G(clipV(o, 12)) + " V"}); }
          ST(3, "A constant input produces a straight ramp. A real circuit also needs a resistor across C so the output does not drift into saturation.", {"Output shape", "linear ramp"}); });

    E("Differentiator", "Enter ramp slope (V/ms), R (kOhm), C (uF), sine amplitude Vm (V) and frequency f (Hz)", "0.5, 10, 0.1, 1, 100",
      {"Vout(t) = -R * C * dVin/dt", "Ramp input: dVin/dt is constant, so Vout = -R*C*slope", "Sine input Vm*sin(w*t):  Vout = -R*C*w*Vm*cos(w*t)", "Higher frequency means a larger output (noise gets amplified)"},
      "Double the frequency. The amplitude of the sine output doubles too.",
      [](const D& v, Steps& s) {
          double sl = P(0, 0.5), R = pz(P(1, 10), 10), C = pz(P(2, 0.1), 0.1), Vm = P(3, 1), f = pz(P(4, 100), 100), RC = R * C, o = -RC * sl, A = RC / 1000 * 2 * M_PI * f * std::fabs(Vm);
          ST(0, "A differentiator outputs the rate of change of the input. The capacitor is at the input and the resistor is in feedback. RC = " + G(RC) + " ms.", {"RC", G(RC) + " ms"});
          ST(1, "A ramp rising at " + G(sl) + " V/ms has a constant slope, so Vout = -" + G(RC) + " * " + G(sl) + " = " + G(o) + " V (a constant level).", {"Vout (ramp in)", G(clipV(o, 12)) + " V"});
          ST(2, "A sine of " + G(Vm) + " V at " + G(f) + " Hz comes out as a cosine (90 degrees shifted, inverted) with amplitude RC*w*Vm = " + G(A) + " V.", {"Output amplitude", G(A) + " V"});
          ST(3, "The amplitude grows with frequency, so high-frequency noise is amplified. Practical circuits add a small series resistor to limit this.", {"Gain at f", G(A / (Vm == 0 ? 1 : std::fabs(Vm)))}); });
}
