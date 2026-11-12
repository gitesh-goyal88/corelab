// Physics 1: atomic structure, radiation, quantum mechanics, and the JSON writer.
#pragma once
#include "simulations4.hpp"
#include "physics.hpp"

using cd = std::complex<double>;
static const char* const LET = "SPDFGH";
inline std::vector<cd> solveC(std::vector<std::vector<cd>> A) {
    int n = A.size();
    for (int i = 0; i < n; ++i) { int pv = i; for (int k = i + 1; k < n; ++k) if (std::abs(A[k][i]) > std::abs(A[pv][i])) pv = k; std::swap(A[i], A[pv]);
        for (int k = i + 1; k < n; ++k) { cd f = A[k][i] / A[i][i]; for (int j = i; j <= n; ++j) A[k][j] -= f * A[i][j]; } }
    std::vector<cd> x(n); for (int i = n - 1; i >= 0; --i) { cd s = A[i][n]; for (int j = i + 1; j < n; ++j) s -= A[i][j] * x[j]; x[i] = s / A[i][i]; } return x;
}
struct Barrier { cd r, A, B, t, k, kap; double a; };
inline Barrier solveBarrier(double E, double V0, double a) {   // E, V0 in eV, a in nm; wave numbers in 1/nm
    if (std::fabs(E - V0) < 1e-6) E = V0 + 1e-6;
    Barrier s; s.a = a; s.k = 5.1232 * std::sqrt(E); s.kap = std::sqrt(cd(V0 - E, 0)) * 5.1232; cd i(0, 1), k = s.k, K = s.kap, ea = std::exp(-K * a), eik = std::exp(i * k * a);
    std::vector<std::vector<cd>> M = {{1, -1, -ea, 0, -1}, {-i * k, K, -K * ea, 0, -i * k}, {0, ea, 1, -eik, 0}, {0, -K * ea, K, -i * k * eik, 0}};
    auto x = solveC(M); s.r = x[0]; s.A = x[1]; s.B = x[2]; s.t = x[3]; return s;
}
inline std::string jstr(double J) { return std::fabs(J - std::round(J)) < 1e-9 ? S((int)std::round(J)) : S((int)std::round(2 * J)) + "/2"; }

inline void addPhysics2(PhysMap& m) {
    auto add = [&](const char* k, std::vector<PParam> par, std::function<void(const std::vector<double>&, PRes&)> f, const char* pr) { m[k] = Phys{par, f, pr}; };

    /* ================= Atomic structure ================= */
    add("Spectral Lines", {{"Z", "Nuclear charge Z", "", 1, 4, 1, 1}, {"nl", "Lower level n", "", 1, 4, 1, 2}, {"nu", "Highest upper level", "", 3, 10, 1, 7}},
        PHF { int Z = (int)p[0], nl = (int)p[1], nu = std::max((int)p[2], nl + 2); const double R = 1.0973731568e7; auto lam = [&](int n) { return 1e9 / (R * Z * Z * (1.0 / (nl * nl) - 1.0 / (n * n))); };
              auto& pl = newPlot(r, "Emission lines (hydrogen-like atom)", "wavelength (nm)", ""); pl.spec = true; double lo = lam(nl + 1), hi = lam(nu); double rg = std::fabs(lo - hi); pl.xmin = std::min(lo, hi) - 0.1 * rg - 5; pl.xmax = std::max(lo, hi) + 0.1 * rg + 5; pl.ymin = 0; pl.ymax = 1;
              auto& b = addS(pl, ""); b.x = {pl.xmin, pl.xmax}; b.y = {0, 0}; for (int n = nl + 1; n <= nu; ++n) pl.marks.push_back({lam(n), S(n) + "→" + S(nl)});
              const char* nmx[5] = {"", "Lyman", "Balmer", "Paschen", "Brackett"};
              ro(r, "Series", std::string(nmx[nl]) + " series (ends on n = " + S(nl) + ")"); ro(r, "First line  " + S(nl + 1) + "→" + S(nl), sgn(lam(nl + 1), " nm")); ro(r, "Series limit (n → ∞)", sgn(1e9 / (R * Z * Z / (nl * nl)), " nm")); ro(r, "Photon energy of the first line", sgn(13.6057 * Z * Z * (1.0 / (nl * nl) - 1.0 / ((nl + 1) * (nl + 1))), " eV"));
              ro(r, "Region", nl == 1 ? "ultraviolet" : (nl == 2 ? "mostly visible" : "infrared"));
              r.ex = "1/λ = R·Z²(1/n1² − 1/n2²). An electron falling from a higher level n2 to n1 emits one photon of that wavelength. Lines crowd together near the series limit."; },
        "Choose Z = 1, lower level 2 (Balmer) and identify the 656 nm line (3→2). Raise Z to 2 for He⁺.");

    add("Spin and Orbital Angular Momentum", {{"l", "Orbital quantum number l", "", 0, 4, 1, 2}, {"ml", "Magnetic number ml", "", -4, 4, 1, 1}, {"ms", "Spin direction ms (−1 down, +1 up)", "", -1, 1, 2, 1}},
        PHF { int l = (int)p[0], ml = std::max(-l, std::min(l, (int)p[1])), up = p[2] > 0 ? 1 : -1; double L = std::sqrt(l * (l + 1.0)), S2 = std::sqrt(3.0) / 2;
              auto& pl = newPlot(r, "Orbital angular momentum vectors (units of ħ)", "", ""); pl.equal = true; sample(addS(pl, "|L|"), 0, 2 * M_PI, 120, [&](double) { return 0.0; }); auto& c = pl.s.back(); c.x.clear(); c.y.clear(); for (int i = 0; i <= 120; ++i) { double t = 2 * M_PI * i / 120; c.x.push_back(L * std::cos(t)); c.y.push_back(L * std::sin(t)); }
              for (int k = -l; k <= l; ++k) { auto& h = addS(pl, "", true); double w = std::sqrt(std::max(0.0, L * L - k * k)); h.x = {-w, w}; h.y = {(double)k, (double)k}; } auto& v = addS(pl, "L", false, true); v.x = {0, std::sqrt(std::max(0.0, L * L - ml * ml))}; v.y = {0, (double)ml};
              auto& q2 = newPlot(r, "Spin angular momentum (units of ħ)", "", ""); q2.equal = true; auto& c2 = addS(q2, "|S|"); for (int i = 0; i <= 120; ++i) { double t = 2 * M_PI * i / 120; c2.x.push_back(S2 * std::cos(t)); c2.y.push_back(S2 * std::sin(t)); }
              for (int k : {-1, 1}) { auto& h = addS(q2, "", true); h.x = {-0.707, 0.707}; h.y = {k * 0.5, k * 0.5}; } auto& sv = addS(q2, "S", false, true); sv.x = {0, 0.707}; sv.y = {0, up * 0.5};
              ro(r, "|L| = √(l(l+1)) ħ", Gs(L) + " ħ"); ro(r, "Lz = ml ħ", S(ml) + " ħ"); if (l) ro(r, "Angle between L and z", sgn(std::acos(ml / L) * 180 / M_PI, "°")); ro(r, "Orientations allowed (2l + 1)", S(2 * l + 1));
              ro(r, "|S| = √(s(s+1)) ħ", "0.866 ħ  (s = 1/2)"); ro(r, "Sz = ms ħ", up > 0 ? "+½ ħ" : "−½ ħ"); ro(r, "Total j = l ± ½", l ? jstr(l - 0.5) + " or " + jstr(l + 0.5) : "1/2");
              r.ex = "The length of L is fixed by l, but only 2l+1 tilts to the z-axis are allowed (space quantisation). L can never point exactly along z. The electron spin has a fixed length and two possible tilts, up or down."; },
        "Increase l to 3: there are 7 allowed tilts. Check that Lz is never equal to |L|.");

    add("Quantum Numbers", {{"n", "Principal n", "", 1, 5, 1, 2}, {"l", "Orbital l", "", 0, 4, 1, 1}, {"ml", "Magnetic ml", "", -4, 4, 1, 0}, {"ms", "Spin ms (−1 down, +1 up)", "", -1, 1, 2, 1}},
        PHF { int n = (int)p[0], l = std::min((int)p[1], n - 1), ml = std::max(-l, std::min(l, (int)p[2])); double ms = p[3] > 0 ? 0.5 : -0.5;
              auto R = [&](double rr) { double rho = 2 * rr / n, nm = std::sqrt(std::pow(2.0 / n, 3) * std::tgamma(n - l) / (2.0 * n * std::tgamma(n + l + 1))); return nm * std::exp(-rho / 2) * std::pow(rho, l) * std::assoc_laguerre(n - l - 1, 2 * l + 1, rho); };
              double rm = 2.4 * n * n + 10; auto& pl = newPlot(r, "Radial probability  r²R(r)²", "r (units of a0 = 0.0529 nm)", "probability density"); sample(addS(pl, "P(r)"), 0, rm, 500, [&](double rr) { return rr * rr * R(rr) * R(rr); }); double avg = 0.5 * (3.0 * n * n - l * (l + 1)); pl.marks.push_back({avg, "<r>"});
              ro(r, "State", "|" + S(n) + ", " + S(l) + ", " + S(ml) + ", " + (ms > 0 ? "+½" : "−½") + "⟩   → " + S(n) + (l < 6 ? std::string(1, "spdfgh"[l]) : "")); ro(r, "Allowed l", "0 … " + S(n - 1)); ro(r, "Allowed ml", "−" + S(l) + " … +" + S(l) + "  (" + S(2 * l + 1) + " orbitals)"); ro(r, "Electrons in this subshell", S(2 * (2 * l + 1))); ro(r, "Electrons in shell n", "2n² = " + S(2 * n * n));
              ro(r, "Energy En = −13.6/n²", sgn(-13.6057 / (n * n), " eV")); ro(r, "Mean radius", sgn(avg * 0.0529, " nm")); ro(r, "Radial nodes n − l − 1", S(n - l - 1));
              r.ex = "n sets the energy and size, l the shape and angular momentum, ml the orientation, and ms the spin. The number of radial nodes is n−l−1. Each set of four numbers can hold only one electron (Pauli exclusion)."; },
        "Try n = 3, l = 0, 1, 2 and count the radial nodes: 2, 1, 0.");

    add("Designation of States", {{"mu", "Multiplicity 2S + 1", "", 1, 4, 1, 3}, {"L", "Total orbital L (0=S, 1=P, 2=D …)", "", 0, 4, 1, 1}, {"A", "Spin-orbit constant A", "meV", 1, 20, 1, 5}},
        PHF { int mu = (int)p[0], L = (int)p[1]; double S2 = (mu - 1) / 2.0, A = p[2], lo = std::fabs(L - S2), hi = L + S2; std::string terms; auto& pl = newPlot(r, "Fine-structure levels (Landé interval rule)", "", "energy (meV)"); pl.xmin = 0; pl.xmax = 1.3;
              for (double J = lo; J <= hi + 1e-9; J += 1) { double E = A / 2 * (J * (J + 1) - L * (L + 1) - S2 * (S2 + 1)); auto& s = addS(pl, S(mu) + std::string(1, LET[L]) + jstr(J), false, true); s.x = {0, 1}; s.y = {E, E}; terms += S(mu) + std::string(1, LET[L]) + jstr(J) + "   "; }
              ro(r, "Term symbol", "^(2S+1) L_J  =  " + S(mu) + std::string(1, LET[L]) + "_J"); ro(r, "Levels (J values)", terms); ro(r, "Number of J levels", S((int)std::floor(hi - lo + 1e-9) + 1)); ro(r, "Spacing  E(J) − E(J−1)", "A·J  (the interval rule)"); ro(r, "Degeneracy of each level", "2J + 1"); ro(r, "Hund's rule", A > 0 ? "less than half-full shell: lowest J is lowest in energy" : "more than half-full: highest J lowest");
              r.ex = "An atomic state is written ^(2S+1)L_J: spin multiplicity, orbital letter (S, P, D, F…) and total J. Spin-orbit coupling splits one term into several J levels whose gaps grow in proportion to J."; },
        "Choose multiplicity 3 and L = 1 (³P): levels J = 0, 1, 2. Then try ²P: J = 1/2, 3/2.");

    add("Atoms in Magnetic Field", {{"B", "Magnetic field B", "T", 0, 5, 0.1, 1}, {"L", "Orbital L", "", 0, 3, 1, 1}, {"S", "Spin S", "", 0, 1.5, 0.5, 0.5}, {"J", "Total J", "", 0, 4.5, 0.5, 1.5}},
        PHF { double L = p[1], S2 = p[2], jmin = std::fabs(L - S2), jmax = L + S2, J = jmin + std::max(0.0, std::min(jmax - jmin, std::round(p[3] - jmin))); double g = J > 0 ? 1 + (J * (J + 1) + S2 * (S2 + 1) - L * (L + 1)) / (2 * J * (J + 1)) : 0.0; const double mb = 57.88;
              auto& pl = newPlot(r, "Energy of the sublevels  E = mJ·g·μB·B", "B (T)", "E (µeV)"); for (double mj = -J; mj <= J + 1e-9; mj += 1) { auto& s = addS(pl, "mJ=" + jstr(mj), false, true); s.x = {0, 5}; s.y = {0, mj * g * mb * 5}; } pl.marks.push_back({p[0], "B"});
              ro(r, "Term", S((int)(2 * S2 + 1)) + std::string(1, LET[std::min(5, (int)L)]) + jstr(J) + "  (J allowed: " + jstr(jmin) + " … " + jstr(jmax) + ")"); ro(r, "Landé g-factor", Gs(g)); ro(r, "Sublevels (2J + 1)", S((int)std::round(2 * J + 1))); ro(r, "Spacing g·μB·B", sgn(g * mb * p[0], " µeV") + " = " + Gs(g * mb * p[0] * 1e-6 * PQE / PH / 1e9) + " GHz"); ro(r, "μB", "57.88 µeV per tesla");
              r.ex = "A magnetic field lifts the 2J+1 degeneracy: each level splits into sublevels spaced g·μB·B apart. The g-factor depends on how L and S combine, so different terms split by different amounts."; },
        "Choose L = 1, S = 1/2, J = 3/2 (²P3/2): g = 4/3 and 4 sublevels. Compare J = 1/2: g = 2/3.");

    add("Zeeman Effect", {{"B", "Magnetic field B", "T", 0, 5, 0.1, 1}, {"lam", "Spectral line λ0", "nm", 300, 700, 1, 643.8}},
        PHF { double B = p[0], dl = p[1] * p[1] * 1e-18 * PQE * B / (4 * M_PI * PME * PC) * 1e12; auto& pl = newPlot(r, "Normal Zeeman triplet", "shift from λ0 (pm)", ""); double w = std::max(1.0, 2.5 * dl); pl.xmin = -w; pl.xmax = w; pl.ymin = 0; pl.ymax = 1; auto& b = addS(pl, ""); b.x = {-w, w}; b.y = {0, 0};
              pl.marks.push_back({-dl, "σ (Δm=−1)"}); pl.marks.push_back({0, "π (Δm=0)"}); pl.marks.push_back({dl, "σ (Δm=+1)"}); auto& q2 = newPlot(r, "Upper level (l = 1) splits into three", "", "shift of energy (µeV)"); q2.xmin = 0; q2.xmax = 1.4;
              for (int k = 1; k >= -1; --k) { auto& s = addS(q2, "m=" + S(k), false, true); s.x = {0, 1}; s.y = {k * 57.88 * B, k * 57.88 * B}; }
              ro(r, "Wavelength shift  Δλ = λ²eB/(4πmc)", sgn(dl, " pm")); ro(r, "Frequency shift  Δν = eB/(4πm)", sgn(PQE * B / (4 * M_PI * PME) / 1e9, " GHz")); ro(r, "Energy splitting μB·B", sgn(57.88 * B, " µeV")); ro(r, "Components", "π: light polarised along B; σ±: circular around B");
              r.ex = "In a magnetic field the spectral line splits into three: the original wavelength (π) and two shifted ones (σ±) separated by Δν = eB/4πm. The shift is proportional to B. This is the normal Zeeman effect."; },
        "Double B and check that the splitting doubles. Cadmium red line: 643.8 nm.");

    /* ================= Radiation ================= */
    auto lamRange = [](double T, double a, double b, int n, PSeries& s, std::function<double(double)> f) { sample(s, a, b, n, [&](double nm) { return f(nm * 1e-9); }); (void)T; };
    (void)lamRange;
    add("Black Body Radiation", {{"T", "Temperature T", "K", 500, 10000, 100, 5800}},
        PHF { double T = p[0], lm = 2.8977719e-3 / T * 1e9; auto& pl = newPlot(r, "Spectral radiance of a black body", "wavelength (nm)", "B (kW m⁻² sr⁻¹ nm⁻¹)"); sample(addS(pl, "Planck"), 100, 3000, 500, [&](double nm) { return planckB(nm * 1e-9, T); }); pl.marks.push_back({lm, "λmax"}); pl.marks.push_back({380, ""}); pl.marks.push_back({750, ""});
              double vis = 0, dn = 370.0 / 150; for (int i = 0; i < 150; ++i) vis += planckB((380 + (i + 0.5) * dn) * 1e-9, T) * 1e3 * dn; double tot = 5.670374419e-8 * std::pow(T, 4) / M_PI;
              ro(r, "Peak wavelength  λmax = b/T", sgn(lm, " nm") + (lm < 380 ? " (ultraviolet)" : lm > 750 ? " (infrared)" : " (visible)")); ro(r, "Total power per area  σT⁴", Gs(5.670374419e-8 * std::pow(T, 4)) + " W/m²"); ro(r, "Fraction in the visible band (380–750 nm)", Gs(100 * vis / tot) + " %"); ro(r, "Doubling T multiplies power by", "16");
              r.ex = "A perfect absorber is also a perfect emitter. Its spectrum depends only on temperature: a hotter body is brighter at every wavelength and its peak moves to shorter wavelengths. The two lines mark the visible band."; },
        "Set T = 5800 K (the Sun) and T = 3000 K (a lamp filament): note where the peak sits against the visible band.");

    add("Wien's Law", {{"T", "Temperature T", "K", 1000, 10000, 100, 3000}},
        PHF { double T = p[0], lm = 2.8977719e-3 / T * 1e9; auto wien = [&](double nm) { double l = nm * 1e-9, a = PH * PC / (l * PKB * T); return a > 700 ? 0.0 : 2 * PH * PC * PC / std::pow(l, 5) * std::exp(-a) * 1e-12; };
              auto& pl = newPlot(r, "Wien's approximation compared with Planck", "wavelength (nm)", "B (kW m⁻² sr⁻¹ nm⁻¹)"); sample(addS(pl, "Planck", false, true), 0.2 * lm, 6 * lm, 400, [&](double nm) { return planckB(nm * 1e-9, T); }); sample(addS(pl, "Wien", true, true), 0.2 * lm, 6 * lm, 400, wien);
              ro(r, "Wien's law", "B ≈ (2hc²/λ⁵)·e^(−hc/λkT)"); ro(r, "Error at the peak λmax", Gs(100 * (wien(lm) / planckB(lm * 1e-9, T) - 1)) + " %"); ro(r, "Error at 5 × λmax", Gs(100 * (wien(5 * lm) / planckB(5 * lm * 1e-9, T) - 1)) + " %");
              r.ex = "Wien's law fits the short-wavelength side of the black body curve well, but fails at long wavelengths, where it falls below the true curve."; },
        "Look at the right-hand tail: Wien drops off too fast there. Try T = 6000 K.");

    add("Rayleigh–Jeans Law", {{"T", "Temperature T", "K", 1000, 10000, 100, 5000}},
        PHF { double T = p[0], lm = 2.8977719e-3 / T * 1e9; auto rj = [&](double nm) { double l = nm * 1e-9; return 2 * PC * PKB * T / std::pow(l, 4) * 1e-12; }; double pk = planckB(lm * 1e-9, T);
              auto& pl = newPlot(r, "The ultraviolet catastrophe", "wavelength (nm)", "B (kW m⁻² sr⁻¹ nm⁻¹)"); sample(addS(pl, "Planck", false, true), 0.2 * lm, 10 * lm, 500, [&](double nm) { return planckB(nm * 1e-9, T); }); sample(addS(pl, "Rayleigh–Jeans", true, true), 0.2 * lm, 10 * lm, 500, rj); pl.ymax = 1.6 * pk; pl.ymin = 0;
              ro(r, "Rayleigh–Jeans law", "B = 2ckT/λ⁴  (classical)"); ro(r, "At λmax it is", Gs(rj(lm) / pk) + " × Planck"); ro(r, "At 5 × λmax", Gs(rj(5 * lm) / planckB(5 * lm * 1e-9, T)) + " × Planck (agrees)"); ro(r, "At 0.3 × λmax", Gs(rj(0.3 * lm) / planckB(0.3 * lm * 1e-9, T)) + " × Planck (way too high)");
              r.ex = "Classical physics predicts a curve that grows as 1/λ⁴ and so blows up at short wavelengths, the ultraviolet catastrophe. It works at long wavelengths only. Planck solved it by quantising the energy."; },
        "Raise T and see the agreement region move to shorter wavelengths.");

    add("Bose–Einstein Statistics", {{"mu", "Chemical potential μ", "kT", -3, 3, 0.1, 0}, {"T", "Temperature T", "K", 3, 6000, 1, 300}, {"nu", "Photon frequency ν", "THz", 0.1, 100, 0.1, 10}},
        PHF { double mu = p[0]; auto& pl = newPlot(r, "Average occupation of an energy state", "(ε) / kT", "occupation number"); pl.ymax = 5; pl.ymin = 0; addS(pl, "Bose–Einstein", false, true); addS(pl, "Fermi–Dirac", true, true); addS(pl, "Maxwell–Boltzmann", true, true); auto& be = pl.s[0]; auto& fd = pl.s[1]; auto& mb = pl.s[2];
              for (int i = 0; i <= 160; ++i) { double e = 8.0 * i / 160; be.x.push_back(e); be.y.push_back(e - std::min(mu, 0.0) > 0.02 ? 1 / std::expm1(e - std::min(mu, 0.0)) : NAN); fd.x.push_back(e); fd.y.push_back(1 / (std::exp(e - mu) + 1)); mb.x.push_back(e); mb.y.push_back(std::exp(mu - e)); }
              double x = PH * p[2] * 1e12 / (PKB * p[1]); pl.marks.push_back({x, "hν/kT"}); ro(r, "Photon (μ = 0): n = 1/(e^(hν/kT) − 1)", Gs(1 / std::expm1(x)) + " photons per mode"); ro(r, "hν / kT", Gs(x)); ro(r, "Bosons (BE)", "any number per state, μ ≤ 0 (bunch at low ε)"); ro(r, "Fermions (FD)", "at most 1 per state (Pauli)"); ro(r, "Classical (MB)", "dilute limit: all three agree when occupation ≪ 1");
              r.ex = "Identical bosons such as photons prefer to occupy the same state, so the occupation 1/(e^((ε−μ)/kT) − 1) rises steeply at low energy. Fermions are capped at 1. This distribution is what makes Planck's radiation law."; },
        "Move ν to a high value: the photon number falls toward the classical exponential tail.");

    add("Planck's Law", {{"T", "Temperature T", "K", 1000, 10000, 100, 5000}, {"hf", "Planck constant factor (1 = real h)", "× h", 0.05, 1, 0.05, 1}},
        PHF { double T = p[0], hf = p[1], lm0 = 2.8977719e-3 / T * 1e9; auto rj = [&](double nm) { double l = nm * 1e-9; return 2 * PC * PKB * T / std::pow(l, 4) * 1e-12; }; double pk = planckB(lm0 * 1e-9, T);
              auto& pl = newPlot(r, "B = 2hc²/λ⁵ · 1/(e^(hc/λkT) − 1)", "wavelength (nm)", "B (kW m⁻² sr⁻¹ nm⁻¹)"); sample(addS(pl, "Planck (your h)", false, true), 0.1 * lm0, 8 * lm0, 600, [&](double nm) { return planckB(nm * 1e-9, T, hf); }); sample(addS(pl, "classical (h → 0)", true, true), 0.1 * lm0, 8 * lm0, 600, rj); pl.ymax = 1.5 * pk; pl.ymin = 0;
              ro(r, "Peak wavelength with this h", sgn(lm0 * hf, " nm")); ro(r, "Energy of a photon at the peak", sgn(PH * hf * PC / (lm0 * hf * 1e-9) / PQE, " eV")); ro(r, "Total power", Gs(5.670374419e-8 * std::pow(T, 4) * std::pow(hf, 4)) + " W/m²");
              r.ex = "Planck assumed light energy comes in packets E = hν. Slide the h factor toward 0 and the quantum curve turns into the classical Rayleigh–Jeans line with its ultraviolet catastrophe."; },
        "Set the h factor to 0.2, then 1. At 1 the curve has the real peak and no catastrophe.");

    add("Wien's Displacement Law", {{"T", "Temperature T", "K", 500, 10000, 100, 5800}},
        PHF { double T = p[0], lm = 2.8977719e-3 / T * 1e9; auto& pl = newPlot(r, "Peaks move to shorter λ as T rises (λmax·T = constant)", "wavelength (nm)", "B (kW m⁻² sr⁻¹ nm⁻¹)"); double hi = 3.2 * 2.8977719e-3 / (0.6 * T) * 1e9; hi = std::max(hi, 2000.0);
              for (double f : {0.6, 1.0, 1.4}) { auto& s = addS(pl, "T = " + Gs(T * f) + " K", false, true); sample(s, 50, hi, 500, [&](double nm) { return planckB(nm * 1e-9, T * f); }); pl.marks.push_back({2.8977719e-3 / (T * f) * 1e9, ""}); }
              ro(r, "λmax = b/T,  b = 2.898×10⁻³ m·K", sgn(lm, " nm")); ro(r, "Frequency of the peak (per unit ν)", sgn(5.879e10 * T / 1e12, " THz")); ro(r, "Looks", lm < 380 ? "hotter than visible: white-blue and UV" : lm > 750 ? "peak in the infrared: dull red or invisible" : "peak in the visible range");
              r.ex = "When a body gets hotter its peak moves to shorter wavelengths, so metal glows red, then yellow, then white. The product λmax·T stays constant, which lets us read the temperature of stars from their colour."; },
        "Check the Sun (5800 K → 500 nm) and a person (310 K → about 9.3 µm, infrared).");

    /* ================= Quantum mechanics ================= */
    add("Wave–Particle Duality", {{"V", "Accelerating voltage V", "V", 10, 1000, 10, 100}, {"d", "Slit separation d", "nm", 5, 100, 1, 20}, {"L", "Screen distance L", "m", 0.5, 3, 0.1, 1}},
        PHF { double lam = PH / std::sqrt(2 * PME * PQE * p[0]), b = lam * p[2] / (p[1] * 1e-9) * 1e3; auto f = [&](double x) { return std::pow(std::cos(M_PI * x / b), 2); };
              auto& pl = newPlot(r, "Electron interference pattern", "x (mm)", "probability"); sample(addS(pl, "P(x)"), -5 * b, 5 * b, 500, f); r.color = 470; fillStrip(r, -5 * b, 5 * b, 400, f); r.sunit = "mm"; r.snote = "Accumulated electron impacts";
              ro(r, "de Broglie wavelength  λ = h/√(2meV)", sgn(lam * 1e12, " pm")); ro(r, "Fringe spacing  λL/d", sgn(b, " mm")); ro(r, "Momentum p = h/λ", Gs(PH / lam) + " kg·m/s"); ro(r, "Electron speed", Gs(std::sqrt(2 * PQE * p[0] / PME) / 1e6) + " ×10⁶ m/s");
              r.ex = "Electrons, fired one at a time, still build up an interference pattern. Each arrives as a particle but lands with a probability set by a wave of wavelength h/p. Higher voltage means a shorter wavelength and finer fringes."; },
        "Quadruple the voltage: λ halves and the fringes halve.");

    add("Compton Scattering", {{"lam", "X-ray wavelength λ", "pm", 10, 100, 0.5, 71.1}, {"th", "Scattering angle θ", "deg", 0, 180, 1, 90}},
        PHF { const double lc = 2.42631023867; double lam = p[0], th = p[1] * M_PI / 180, dl = lc * (1 - std::cos(th)), l2 = lam + dl, al = lc / lam; auto& pl = newPlot(r, "Wavelength shift  Δλ = (h/mc)(1 − cos θ)", "angle θ (deg)", "Δλ (pm)"); sample(addS(pl, "Δλ"), 0, 180, 91, [&](double d) { return lc * (1 - std::cos(d * M_PI / 180)); }); pl.marks.push_back({p[1], "θ"});
              auto& q2 = newPlot(r, "Scattered X-rays at this angle", "wavelength (pm)", "relative intensity"); auto g = [&](double x, double c, double h) { return h * std::exp(-std::pow(x - c, 2) / (2 * 0.25)); }; sample(addS(q2, "spectrum"), lam - 3, lam + 7, 400, [&](double x) { return g(x, lam, 0.3) + g(x, l2, 1.0); }); q2.marks.push_back({lam, "λ"}); q2.marks.push_back({l2, "λ'"});
              double phi = p[1] < 0.5 ? 90.0 : std::atan(1 / ((1 + al) * std::tan(th / 2))) * 180 / M_PI; ro(r, "Compton wavelength h/mc", "2.426 pm"); ro(r, "Shift Δλ", sgn(dl, " pm")); ro(r, "Scattered wavelength λ'", sgn(l2, " pm")); ro(r, "Kinetic energy given to the electron", sgn(1239.84 * (1 / lam - 1 / l2), " keV")); ro(r, "Recoil angle of the electron", sgn(phi, "°"));
              r.ex = "A photon colliding with a free electron loses energy, so its wavelength grows. The shift depends only on the scattering angle, not on the incoming wavelength. This showed that light carries momentum h/λ, like a particle."; },
        "Use Mo Kα (71.1 pm) at 90°: shift = 2.43 pm. At 180° the shift is largest, 4.85 pm.");

    add("Matter Waves", {{"K", "Kinetic energy", "eV", 1, 1000, 1, 100}, {"m", "Mass", "electron masses", 1, 2000, 1, 1}},
        PHF { double M = p[1] * PME, K = p[0] * PQE; auto lamK = [&](double e) { return PH / std::sqrt(2 * M * e * PQE) * 1e12; }; double lam = PH / std::sqrt(2 * M * K) * 1e12;
              auto& pl = newPlot(r, "The matter wave of the particle", "distance (pm)", "wave amplitude"); sample(addS(pl, "ψ"), 0, 4 * lam, 400, [&](double x) { return std::cos(2 * M_PI * x / lam); }); pl.marks.push_back({lam, "λ"});
              auto& q2 = newPlot(r, "Wavelength against energy", "kinetic energy (eV)", "λ (pm)"); sample(addS(q2, "λ"), 1, 1000, 200, lamK); q2.marks.push_back({p[0], "now"});
              ro(r, "de Broglie wavelength  λ = h/p", sgn(lam, " pm")); ro(r, "Momentum", Gs(std::sqrt(2 * M * K)) + " kg·m/s"); ro(r, "Speed", Gs(std::sqrt(2 * K / M) / 1e3) + " km/s"); ro(r, "Compare with atom spacing (~250 pm)", lam > 50 && lam < 1000 ? "diffraction by crystals is possible" : (lam < 50 ? "too short to diffract strongly" : "much larger than atoms"));
              r.ex = "Every moving particle has a wavelength λ = h/p. It is measurable for electrons but absurdly tiny for large objects. Electrons of 100 eV have λ ≈ 120 pm, ideal for diffraction from crystals."; },
        "A 54 eV electron has λ ≈ 167 pm (Davisson–Germer). Try a proton (mass 1836): same energy, much shorter λ.");

    add("Heisenberg's Uncertainty Principle", {{"dx", "Position spread Δx", "nm", 0.01, 2, 0.01, 0.1}, {"m", "Mass", "electron masses", 1, 2000, 1, 1}},
        PHF { double sx = p[0], sp = PHB / (2 * sx * 1e-9) / 1e-25, M = p[1] * PME; auto& pl = newPlot(r, "Position probability", "x (nm)", "|ψ|² (1/nm)"); sample(addS(pl, "|ψ|²"), -4 * sx, 4 * sx, 300, [&](double x) { return std::exp(-x * x / (2 * sx * sx)) / (sx * std::sqrt(2 * M_PI)); });
              auto& q2 = newPlot(r, "Momentum probability", "p (10⁻²⁵ kg·m/s)", "|φ|²"); sample(addS(q2, "|φ|²"), -4 * sp, 4 * sp, 300, [&](double x) { return std::exp(-x * x / (2 * sp * sp)) / (sp * std::sqrt(2 * M_PI)); });
              ro(r, "Δx", sgn(sx, " nm")); ro(r, "Δp ≥ ħ/(2Δx)", sgn(sp, " ×10⁻²⁵ kg·m/s")); ro(r, "Δx·Δp", Gs(PHB / 2) + " J·s  (= ħ/2, the minimum)"); ro(r, "Speed uncertainty Δv", sgn(sp * 1e-25 / M / 1e3, " km/s")); ro(r, "Confinement energy  (Δp)²/2m", sgn(std::pow(sp * 1e-25, 2) / (2 * M) / PQE, " eV"));
              r.ex = "You cannot know both position and momentum perfectly: Δx·Δp ≥ ħ/2. Squeeze the electron into a smaller region and the momentum spread widens. This is why electrons cannot collapse into the nucleus."; },
        "Shrink Δx from 0.1 nm to 0.01 nm: the momentum curve becomes ten times wider.");

    add("Schrödinger Wave Equation", {{"s0", "Initial width σ0", "nm", 0.1, 1, 0.05, 0.3}, {"k0", "Wave number k0", "nm⁻¹", 0, 20, 0.5, 5}, {"t", "Time t", "fs", 0, 10, 0.1, 2}},
        PHF { double s0 = p[0], k0 = p[1], t = p[2], hm = PHB / PME * 1e3, tau = hm * t / (2 * s0 * s0), v = hm * k0, w = hm * k0 * k0 / 2, st = s0 * std::sqrt(1 + tau * tau), c = v * t;
              auto psi = [&](double x) { cd i(0, 1); return std::pow(2 * M_PI * s0 * s0, -0.25) / std::sqrt(cd(1, tau)) * std::exp(-(x - c) * (x - c) / (4 * s0 * s0 * cd(1, tau))) * std::exp(i * (k0 * x - w * t)); };
              double lo = std::min(-5 * s0, c - 5 * st), hi = std::max(5 * s0, c + 5 * st); auto& pl = newPlot(r, "Free-particle wave packet", "x (nm)", "|ψ|²  and  Re ψ"); sample(addS(pl, "|ψ|²", false, true), lo, hi, 800, [&](double x) { return std::norm(psi(x)); });
              sample(addS(pl, "Re ψ", true), lo, hi, 800, [&](double x) { return psi(x).real(); }); sample(addS(pl, "|ψ|² at t = 0", true), lo, hi, 400, [&](double x) { return std::exp(-x * x / (2 * s0 * s0)) / (s0 * std::sqrt(2 * M_PI)); });
              ro(r, "Packet centre (v = ħk0/m)", sgn(c, " nm") + "   v = " + Gs(v) + " nm/fs"); ro(r, "Width σ(t) = σ0√(1 + (ħt/2mσ0²)²)", sgn(st, " nm") + "  (" + Gs(st / s0) + " × initial)"); ro(r, "Spreading time 2mσ0²/ħ", sgn(2 * s0 * s0 / hm, " fs")); ro(r, "Equation", "iħ ∂ψ/∂t = −(ħ²/2m) ∂²ψ/∂x²");
              r.ex = "The Schrödinger equation tells how ψ changes in time. For a free particle, a localised packet drifts at the group velocity ħk0/m and spreads, because it is made from waves with a range of momenta. |ψ|² is the probability density."; },
        "Move t from 0 to 10 fs and watch the packet drift and flatten. Smaller σ0 spreads faster.");

    add("Particle in a Box (1D and 3D)", {{"L", "Box length L", "nm", 0.2, 5, 0.1, 1}, {"n", "1D quantum number n", "", 1, 6, 1, 1}, {"nx", "3D: nx", "", 1, 5, 1, 1}, {"ny", "3D: ny", "", 1, 5, 1, 1}, {"nz", "3D: nz", "", 1, 5, 1, 2}},
        PHF { double L = p[0], E1 = PH * PH / (8 * PME * L * L * 1e-18) / PQE; int n = (int)p[1], nx = (int)p[2], ny = (int)p[3], nz = (int)p[4]; auto psi = [&](double x) { return std::sqrt(2 / L) * std::sin(n * M_PI * x / L); };
              auto& pl = newPlot(r, "Wavefunction and probability in the box", "x (nm)", "ψ (nm^-1/2)  and  |ψ|² (nm⁻¹)"); sample(addS(pl, "ψ", false, true), 0, L, 400, psi); sample(addS(pl, "|ψ|²", true, true), 0, L, 400, [&](double x) { return psi(x) * psi(x); });
              auto& q2 = newPlot(r, "Energy levels En = n²·E1", "", "energy (eV)"); q2.xmin = 0; q2.xmax = 1.4; for (int k = 1; k <= 6; ++k) { auto& s = addS(q2, "n=" + S(k), k != n, true); s.x = {0, 1}; s.y = {E1 * k * k, E1 * k * k}; }
              int N = nx * nx + ny * ny + nz * nz, deg = 0; for (int a = 1; a * a < N; ++a) for (int b = 1; a * a + b * b < N; ++b) { int c2 = N - a * a - b * b, c = (int)std::lround(std::sqrt((double)c2)); if (c >= 1 && c * c == c2) ++deg; }
              ro(r, "Ground-state energy  E1 = h²/8mL²", sgn(E1, " eV")); ro(r, "Energy of n = " + S(n), sgn(E1 * n * n, " eV")); ro(r, "Gap to the next level", sgn(E1 * (2 * n + 1), " eV")); ro(r, "Nodes inside the box", S(n - 1)); ro(r, "Photon for n → n−1", n > 1 ? sgn(1239.84 / (E1 * (2 * n - 1)), " nm") : "none (ground state)");
              ro(r, "3D box: E = E1(nx² + ny² + nz²)", sgn(E1 * N, " eV") + "  (sum = " + S(N) + ")"); ro(r, "3D degeneracy of that energy", S(deg) + " states");
              r.ex = "Confining a particle makes only standing waves possible: n half-wavelengths fit in the box, so energy is quantised and never zero. Energy grows as n² and as 1/L². In 3D different (nx, ny, nz) can share one energy (degeneracy)."; },
        "Shrink L to half: energies go up 4 times. In 3D try (1,1,2), (1,2,1), (2,1,1): they share an energy.");

    add("Potential Barrier", {{"E", "Particle energy E", "eV", 0.1, 10, 0.1, 2}, {"V0", "Barrier height V0", "eV", 1, 10, 0.1, 5}, {"a", "Barrier width a", "nm", 0.05, 2, 0.05, 0.5}},
        PHF { double E = p[0], V0 = p[1], a = p[2]; auto s = solveBarrier(E, V0, a); double T = std::norm(s.t), R = std::norm(s.r); double lo = -1.5, hi = a + 1.5;
              auto psi2 = [&](double x) { cd i(0, 1); cd v = x < 0 ? std::exp(i * s.k * x) + s.r * std::exp(-i * s.k * x) : (x <= a ? s.A * std::exp(-s.kap * x) + s.B * std::exp(s.kap * (x - a)) : s.t * std::exp(i * s.k * x)); return std::norm(v); };
              auto& pl = newPlot(r, "Probability density |ψ|²  (incident wave from the left)", "x (nm)", "|ψ|²"); sample(addS(pl, "|ψ|²"), lo, hi, 700, psi2); pl.marks.push_back({0, "barrier"}); pl.marks.push_back({a, ""}); pl.ymin = 0;
              auto& q2 = newPlot(r, "Transmission against barrier width", "width a (nm)", "T"); sample(addS(q2, "T"), 0.02, 2, 120, [&](double w) { return std::norm(solveBarrier(E, V0, w).t); }); q2.marks.push_back({a, "now"}); q2.ymin = 0; q2.ymax = 1;
              ro(r, "Transmission T", Gs(100 * T) + " %"); ro(r, "Reflection R", Gs(100 * R) + " %  (T + R = " + Gs(T + R) + ")"); ro(r, "κ inside the barrier", E < V0 ? sgn(s.kap.real(), " nm⁻¹") + "  (penetration depth " + Gs(1 / s.kap.real()) + " nm)" : "E > V0: no decay, but still some reflection"); if (E < V0) ro(r, "Thick-barrier estimate", Gs(100 * 16 * E * (V0 - E) / (V0 * V0) * std::exp(-2 * s.kap.real() * a)) + " %");
              r.ex = "Classically a particle with E < V0 can never pass. Quantum mechanically the wave decays inside the barrier but is not zero, so some of it comes out the other side: tunnelling. T falls roughly as e^(−2κa), so it is very sensitive to width."; },
        "Raise the width from 0.2 to 1 nm and watch T fall by orders of magnitude. Try E above V0 and see reflection anyway.");

    add("Tunnel Diode", {{"V", "Applied voltage V", "V", 0, 0.6, 0.01, 0.2}, {"Vp", "Peak voltage Vp", "V", 0.04, 0.12, 0.01, 0.07}, {"Ip", "Peak current Ip", "mA", 1, 10, 0.5, 5}},
        PHF { double Vp = p[1], Ip = p[2]; auto I = [&](double v) { return Ip * (v / Vp) * std::exp(1 - v / Vp) + 0.05 * Ip * std::exp((v - 0.4) / 0.045); }; auto& pl = newPlot(r, "Tunnel diode I–V curve", "voltage (V)", "current (mA)"); sample(addS(pl, "I"), 0, 0.6, 300, I);
              double vv = Vp, best = 1e9; for (double v = Vp; v < 0.5; v += 0.001) if (I(v) < best) { best = I(v); vv = v; } pl.marks.push_back({p[0], "V"}); pl.marks.push_back({Vp, "peak"}); pl.marks.push_back({vv, "valley"}); double h = 1e-4, g = (I(p[0] + h) - I(std::max(0.0, p[0] - h))) / (p[0] + h - std::max(0.0, p[0] - h));
              ro(r, "Current at your V", sgn(I(p[0]), " mA")); ro(r, "Peak", Gs(I(Vp)) + " mA at " + Gs(Vp) + " V"); ro(r, "Valley", Gs(best) + " mA at " + Gs(vv) + " V"); ro(r, "Peak-to-valley ratio", Gs(I(Vp) / best)); ro(r, "Dynamic resistance dV/dI", g < 0 ? Gs(1 / g) + " kΩ (negative!)" : Gs(1 / g) + " kΩ");
              ro(r, "Region", p[0] < Vp ? "tunnelling current rises" : (p[0] < vv ? "negative resistance: current falls as V rises" : "ordinary diode current takes over"));
              r.ex = "In a heavily doped PN junction electrons tunnel through the thin barrier. As the bias rises the bands slide out of line and the tunnelling current falls, giving a region of negative resistance that is used in fast oscillators and switches."; },
        "Sweep V from 0 to 0.6 V and find where the slope turns negative.");
}

inline std::string jn(double x) { if (!std::isfinite(x)) return "null"; char b[40]; std::snprintf(b, sizeof b, "%.6g", x); return b; }
inline std::string physicsJson(const std::string& topic, const std::string& pstr) {
    auto& mm = physMap(); auto it = mm.find(topic); if (it == mm.end()) return "{\"available\":false}";
    const Phys& ph = it->second; D v = nums(pstr); std::vector<double> p;
    for (size_t i = 0; i < ph.par.size(); ++i) { double x = i < v.size() ? v[i] : ph.par[i].df; p.push_back(std::max(ph.par[i].mn, std::min(ph.par[i].mx, x))); }
    PRes r; ph.f(p, r);
    auto arr = [](const std::vector<double>& a) { std::string j = "["; for (size_t i = 0; i < a.size(); ++i) j += (i ? "," : "") + jn(a[i]); return j + "]"; };
    std::string j = "{\"available\":true,\"kind\":\"physics\",\"params\":[";
    for (size_t i = 0; i < ph.par.size(); ++i) { const PParam& a = ph.par[i]; j += std::string(i ? "," : "") + "{\"label\":" + q(a.label) + ",\"unit\":" + q(a.unit) + ",\"min\":" + jn(a.mn) + ",\"max\":" + jn(a.mx) + ",\"step\":" + jn(a.st) + ",\"def\":" + jn(a.df) + ",\"val\":" + jn(p[i]) + "}"; }
    j += "],\"plots\":[";
    for (size_t k = 0; k < r.plots.size(); ++k) { PPlot& pl = r.plots[k];
        j += std::string(k ? "," : "") + "{\"title\":" + q(pl.title) + ",\"xl\":" + q(pl.xl) + ",\"yl\":" + q(pl.yl) + ",\"equal\":" + (pl.equal ? "true" : "false") + ",\"spec\":" + (pl.spec ? "true" : "false") + ",\"xmin\":" + jn(pl.xmin) + ",\"xmax\":" + jn(pl.xmax) + ",\"ymin\":" + jn(pl.ymin) + ",\"ymax\":" + jn(pl.ymax) + ",\"series\":[";
        for (size_t s = 0; s < pl.s.size(); ++s) j += std::string(s ? "," : "") + "{\"name\":" + q(pl.s[s].name) + ",\"dash\":" + (pl.s[s].dash ? "true" : "false") + ",\"label\":" + (pl.s[s].label ? "true" : "false") + ",\"fill\":" + (pl.s[s].fill ? "true" : "false") + ",\"dots\":" + (pl.s[s].dots ? "true" : "false") + ",\"arrow\":" + (pl.s[s].arrow ? "true" : "false") + ",\"x\":" + arr(pl.s[s].x) + ",\"y\":" + arr(pl.s[s].y) + "}";
        j += "],\"marks\":["; for (size_t s = 0; s < pl.marks.size(); ++s) j += std::string(s ? "," : "") + "{\"x\":" + jn(pl.marks[s].x) + ",\"label\":" + q(pl.marks[s].label) + "}";
        j += "],\"arrows\":["; for (size_t s = 0; s < pl.arrows.size(); ++s) j += std::string(s ? "," : "") + "[" + jn(pl.arrows[s][0]) + "," + jn(pl.arrows[s][1]) + "," + jn(pl.arrows[s][2]) + "," + jn(pl.arrows[s][3]) + "]";
        j += "],\"heat\":" + (pl.hn ? "{\"n\":" + S(pl.hn) + ",\"v\":" + arr(pl.heat) + "}" : std::string("null")) + "}"; }
    j += "],\"strip\":" + (r.strip.empty() ? std::string("null") : "{\"I\":" + arr(r.strip) + ",\"color\":" + jn(r.color) + ",\"x0\":" + jn(r.sx0) + ",\"x1\":" + jn(r.sx1) + ",\"unit\":" + q(r.sunit) + ",\"note\":" + q(r.snote) + "}");
    if (r.hasSurf) { PSurf& f = r.surf; j += ",\"surf\":{\"nx\":" + S(f.nx) + ",\"ny\":" + S(f.ny) + ",\"x0\":" + jn(f.x0) + ",\"x1\":" + jn(f.x1) + ",\"y0\":" + jn(f.y0) + ",\"y1\":" + jn(f.y1) + ",\"zmin\":" + jn(f.zmin) + ",\"zmax\":" + jn(f.zmax) + ",\"note\":" + q(f.note) + ",\"z\":" + arr(f.z) + ",\"curves\":[";
        for (size_t i = 0; i < f.curves.size(); ++i) j += std::string(i ? "," : "") + "{\"name\":" + q(f.curves[i].name) + ",\"arrow\":" + (f.curves[i].arrow ? "true" : "false") + ",\"col\":" + S(f.curves[i].col) + ",\"x\":" + arr(f.curves[i].x) + ",\"y\":" + arr(f.curves[i].y) + ",\"z\":" + arr(f.curves[i].z) + "}";
        j += "],\"pts\":["; for (size_t i = 0; i < f.pts.size(); ++i) j += std::string(i ? "," : "") + "{\"x\":" + jn(f.pts[i].x) + ",\"y\":" + jn(f.pts[i].y) + ",\"z\":" + jn(f.pts[i].z) + ",\"label\":" + q(f.pts[i].label) + "}"; j += "]}"; } else j += ",\"surf\":null";
    j += ",\"radial\":" + (r.radial.empty() ? std::string("null") : "{\"I\":" + arr(r.radial) + ",\"color\":" + jn(r.color) + ",\"rmax\":" + jn(r.rmax) + ",\"note\":" + q(r.snote) + "}");
    j += ",\"readouts\":["; for (size_t i = 0; i < r.ro.size(); ++i) j += std::string(i ? "," : "") + "[" + q(r.ro[i].first) + "," + q(r.ro[i].second) + "]";
    j += "],\"explain\":" + q(r.ex) + ",\"practice\":" + q(ph.practice) + ",\"refs\":[" + (ph.math ? q("Paul's Online Math Notes|https://tutorial.math.lamar.edu/") + "," + q("Khan Academy: Multivariable Calculus|https://www.khanacademy.org/math/multivariable-calculus") + "," + q("MIT OpenCourseWare 18.02 and 18.03|https://ocw.mit.edu/courses/18-02-multivariable-calculus-fall-2007/") + "," + q("3Blue1Brown: Essence of Linear Algebra|https://www.3blue1brown.com/topics/linear-algebra") + "," + q("Wolfram MathWorld|https://mathworld.wolfram.com/")
        : q("HyperPhysics (Georgia State)|http://hyperphysics.phy-astr.gsu.edu/hbase/index.html") + "," + q("Feynman Lectures on Physics (free online)|https://www.feynmanlectures.caltech.edu/") + "," + q("PhET Interactive Simulations|https://phet.colorado.edu/") + "," + q("LibreTexts Physics|https://phys.libretexts.org/")) + "]}";
    return j;
}
