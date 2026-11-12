// Interactive Physics 1 simulations. The browser sends slider values; this code computes the curves, patterns and readouts.
#pragma once
#include <cmath>
#include <array>
#include <complex>
#include "simulations.hpp"

constexpr double PH = 6.62607015e-34, PC = 2.99792458e8, PKB = 1.380649e-23, PQE = 1.602176634e-19, PME = 9.1093837e-31, PHB = PH / (2 * M_PI), PMUB = 9.2740100783e-24;
struct PParam { std::string id, label, unit; double mn, mx, st, df; };
struct PSeries { std::string name; std::vector<double> x, y; bool dash = false, label = false, fill = false, dots = false, arrow = false; };
struct PMark { double x; std::string label; };
struct PPlot { std::string title, xl, yl; std::vector<PSeries> s; std::vector<PMark> marks; bool equal = false, spec = false; double xmin = NAN, xmax = NAN, ymin = NAN, ymax = NAN;
               std::vector<std::array<double, 4>> arrows; int hn = 0; std::vector<double> heat; };
struct PSurf { int nx = 0, ny = 0; double x0 = -1, x1 = 1, y0 = -1, y1 = 1, zmin = NAN, zmax = NAN; std::vector<double> z; std::string note;
               struct Poly { std::string name; std::vector<double> x, y, z; bool arrow = false; int col = 0; }; std::vector<Poly> curves;
               struct Pt { double x, y, z; std::string label; }; std::vector<Pt> pts; };
struct PRes {
    std::vector<PPlot> plots; Vars ro; std::string ex;
    std::vector<double> strip, radial; double color = 550, sx0 = 0, sx1 = 0, rmax = 0; std::string sunit, snote;
    PSurf surf; bool hasSurf = false;
};
struct Phys { std::vector<PParam> par; std::function<void(const std::vector<double>&, PRes&)> f; std::string practice; bool math = false; };
using PhysMap = std::map<std::string, Phys>;
#define PHF [](const std::vector<double>& p, PRes& r)

inline PPlot& newPlot(PRes& r, std::string t, std::string xl, std::string yl) { r.plots.push_back(PPlot()); PPlot& p = r.plots.back(); p.title = t; p.xl = xl; p.yl = yl; return p; }
inline PSeries& addS(PPlot& p, std::string n, bool dash = false, bool lab = false) { p.s.push_back(PSeries()); PSeries& s = p.s.back(); s.name = n; s.dash = dash; s.label = lab; return s; }
template <class F> void sample(PSeries& s, double a, double b, int n, F f) { for (int i = 0; i < n; ++i) { double x = a + (b - a) * i / (n - 1); s.x.push_back(x); s.y.push_back(f(x)); } }
template <class F> void fillStrip(PRes& r, double x0, double x1, int n, F f) { r.strip.clear(); for (int i = 0; i < n; ++i) r.strip.push_back(std::min(1.0, std::max(0.0, f(x0 + (x1 - x0) * (i + 0.5) / n)))); r.sx0 = x0; r.sx1 = x1; }
inline void ro(PRes& r, const std::string& a, const std::string& b) { r.ro.push_back({a, b}); }
inline double sinc2(double b) { return std::fabs(b) < 1e-9 ? 1.0 : std::pow(std::sin(b) / b, 2); }
inline double airy(double v) { return std::fabs(v) < 1e-9 ? 1.0 : std::pow(2 * std::cyl_bessel_j(1.0, std::fabs(v)) / std::fabs(v), 2); }
inline double planckB(double lam, double T, double hf = 1.0) { double h = PH * hf, a = h * PC / (lam * PKB * T); return a > 700 ? 0.0 : 2 * h * PC * PC / std::pow(lam, 5) / std::expm1(a) * 1e-12; }  // kW m^-2 sr^-1 nm^-1
inline std::string Gs(double x) { char b[40]; std::snprintf(b, sizeof b, "%.4g", x); return b; }
inline std::string sgn(double x, const char* u) { return Gs(x) + u; }
inline void addPhysics2(PhysMap& m);
inline void addMath1(PhysMap& m);
inline void addMath2(PhysMap& m);
inline void addMath3(PhysMap& m);
inline void addMath4(PhysMap& m);

inline PhysMap& physMap() {
    static PhysMap m; if (!m.empty()) return m;
    auto add = [&](const char* k, std::vector<PParam> par, std::function<void(const std::vector<double>&, PRes&)> f, const char* pr) { m[k] = Phys{par, f, pr}; };

    /* ================= Physical Optics ================= */
    add("Interference", {{"lam", "Wavelength", "nm", 400, 700, 5, 550}, {"d", "Slit separation d", "mm", 0.1, 2, 0.05, 0.5}, {"D", "Screen distance D", "m", 0.5, 3, 0.1, 1.5}},
        PHF { double b = p[0] * 1e-9 * p[2] / (p[1] * 1e-3) * 1e3; auto& pl = newPlot(r, "Intensity on the screen", "x (mm)", "I / I0");
              sample(addS(pl, "I"), -5 * b, 5 * b, 500, [&](double x) { return std::pow(std::cos(M_PI * x / b), 2); });
              r.color = p[0]; fillStrip(r, -5 * b, 5 * b, 400, [&](double x) { return std::pow(std::cos(M_PI * x / b), 2); }); r.sunit = "mm";
              ro(r, "Fringe width  β = λD/d", sgn(b, " mm")); ro(r, "Bright fringes at", "x = m·β  (path difference = mλ)"); ro(r, "Dark fringes at", "x = (m + 1/2)·β");
              r.ex = "Two coherent beams meet on the screen. Where the path difference is a whole number of wavelengths they add (bright); at half-wavelengths they cancel (dark). Halve d and the fringes spread twice as wide."; },
        "Double d and watch β halve. Then switch from 400 nm to 700 nm: red fringes are wider than violet ones.");

    add("Intensity Distribution", {{"I1", "Intensity of beam 1", "", 0.5, 10, 0.5, 4}, {"I2", "Intensity of beam 2", "", 0.5, 10, 0.5, 1}, {"dl", "Phase difference δ", "deg", 0, 720, 5, 90}},
        PHF { double I1 = p[0], I2 = p[1], c = 2 * std::sqrt(I1 * I2); auto& pl = newPlot(r, "I = I1 + I2 + 2√(I1·I2)·cos δ", "phase difference δ (deg)", "intensity");
              sample(addS(pl, "I"), 0, 720, 400, [&](double d) { return I1 + I2 + c * std::cos(d * M_PI / 180); }); pl.marks.push_back({p[2], "δ"}); pl.ymin = 0;
              double I = I1 + I2 + c * std::cos(p[2] * M_PI / 180);
              ro(r, "I at your δ", Gs(I)); ro(r, "Imax (δ = 0°, 360°)", Gs(std::pow(std::sqrt(I1) + std::sqrt(I2), 2))); ro(r, "Imin (δ = 180°, 540°)", Gs(std::pow(std::sqrt(I1) - std::sqrt(I2), 2))); ro(r, "Fringe visibility V", Gs(c / (I1 + I2)));
              r.ex = "Amplitudes add, not intensities. Equal beams give perfect darkness (V = 1). Make the beams unequal and the dark fringes get brighter, so contrast falls."; },
        "Set I1 = I2 and check that Imin = 0. Then set I1 = 9, I2 = 1 and find the visibility.");

    add("Fresnel's Biprism", {{"lam", "Wavelength", "nm", 400, 700, 5, 589}, {"a", "Slit to biprism a", "cm", 10, 60, 1, 30}, {"b", "Biprism to screen b", "cm", 50, 150, 5, 70}, {"mu", "Refractive index μ", "", 1.4, 1.7, 0.01, 1.5}, {"al", "Prism angle α", "deg", 0.1, 1.0, 0.05, 0.5}},
        PHF { double lam = p[0] * 1e-9, a = p[1] * 1e-2, b = p[2] * 1e-2, al = p[4] * M_PI / 180, d = 2 * a * (p[3] - 1) * al, beta = lam * (a + b) / d, ov = 2 * b * (p[3] - 1) * al;
              auto f = [&](double x) { return std::pow(std::cos(M_PI * x / (beta * 1e3)), 2); }; double w = std::min(5 * beta * 1e3, ov * 1e3 / 2);
              auto& pl = newPlot(r, "Fringes in the overlap region", "x (mm)", "I / I0"); sample(addS(pl, "I"), -w, w, 500, f); r.color = p[0]; fillStrip(r, -w, w, 400, f); r.sunit = "mm";
              ro(r, "Distance between virtual sources d = 2a(μ−1)α", sgn(d * 1e3, " mm")); ro(r, "Fringe width β = λ(a+b)/d", sgn(beta * 1e3, " mm")); ro(r, "Width of the overlap region", sgn(ov * 1e3, " mm")); ro(r, "Number of fringes seen", Gs(ov / beta));
              r.ex = "The biprism bends light from one slit into two overlapping beams that look like they come from two virtual sources a distance d apart. A larger prism angle or index makes d bigger and the fringes finer."; },
        "Measure β, then compute λ = βd/(a+b). Increase α and watch β shrink while more fringes fit in the overlap.");

    add("Newton's Rings", {{"lam", "Wavelength", "nm", 400, 700, 5, 589}, {"R", "Radius of curvature R", "m", 0.5, 3, 0.1, 1}, {"mu", "Index of film μ", "", 1.0, 1.5, 0.05, 1.0}},
        PHF { double lam = p[0] * 1e-9, R = p[1], mu = p[2], rmax = std::sqrt(12 * lam * R / mu); r.color = p[0]; r.rmax = rmax * 1e3; r.snote = "Reflected light: the centre is dark (n = 0)";
              for (int i = 0; i < 220; ++i) { double rr = rmax * (i + 0.5) / 220; r.radial.push_back(std::pow(std::sin(M_PI * mu * rr * rr / (R * lam)), 2)); }
              auto& pl = newPlot(r, "D² of the dark rings is proportional to n", "ring number n", "D² (mm²)"); sample(addS(pl, "D²"), 0, 12, 13, [&](double n) { return 4 * n * lam * R / mu * 1e6; });
              double sl = 4 * lam * R / mu * 1e6; ro(r, "Radius of the n-th dark ring", "rn = √(nλR/μ)"); ro(r, "r for n = 5", sgn(std::sqrt(5 * lam * R / mu) * 1e3, " mm")); ro(r, "r for n = 10", sgn(std::sqrt(10 * lam * R / mu) * 1e3, " mm"));
              ro(r, "Slope of D² against n", sgn(sl, " mm²")); ro(r, "R from the slope = slope·μ/4λ", sgn(sl * 1e-6 * mu / (4 * lam), " m"));
              r.ex = "A lens on a flat plate makes an air wedge. Light reflected from the top and bottom of the wedge interferes in circular fringes. Because t = r²/2R, ring areas grow in equal steps, so rings crowd together farther out."; },
        "Raise μ to 1.33 (water) and see the rings shrink by √1.33. Use the slope of the D² line to recover R.");

    add("Michelson Interferometer", {{"lam", "Wavelength", "nm", 400, 700, 5, 589}, {"d", "Mirror separation d", "mm", 0.02, 0.5, 0.01, 0.2}, {"dd", "Mirror moved by Δd", "µm", 0, 20, 0.5, 5}},
        PHF { double lam = p[0] * 1e-9, d = p[1] * 1e-3, dd = p[2] * 1e-6, tm = 0.2; r.color = p[0]; r.rmax = 1; r.snote = "Circular fringes seen in the telescope (centre to 11°)";
              for (int i = 0; i < 260; ++i) { double th = tm * (i + 0.5) / 260; r.radial.push_back(std::pow(std::cos(2 * M_PI * d * std::cos(th) / lam), 2)); }
              auto& pl = newPlot(r, "Intensity at the centre as one mirror moves", "mirror position shift (µm)", "I / I0"); sample(addS(pl, "I(centre)"), 0, 4 * p[0] / 2 * 1e-3 * 2, 300, [&](double x) { return std::pow(std::cos(2 * M_PI * (d + x * 1e-6) / lam), 2); });
              pl.marks.push_back({p[2] > 0 && p[2] < 4 * p[0] * 1e-3 ? p[2] : -1, ""}); pl.marks.clear();
              double N = 2 * dd / lam; ro(r, "Path difference at the centre", sgn(2 * d * 1e3, " mm")); ro(r, "Order of the central fringe", Gs(2 * d / lam)); ro(r, "Fringes that cross the centre (N = 2Δd/λ)", Gs(N)); ro(r, "Wavelength from N: λ = 2Δd/N", Gs(p[0]) + " nm"); ro(r, "Rings visible", Gs(2 * d * (1 - std::cos(tm)) / lam));
              r.ex = "A beam splitter sends light to two mirrors and recombines it. Moving one mirror by λ/2 changes the path by λ, so one fringe passes the centre. Counting fringes measures tiny lengths and wavelengths."; },
        "Move the mirror by 10 µm at 589 nm and count about 34 fringes. Then try a larger d and see more rings appear.");

    add("Fraunhofer Diffraction", {{"lam", "Wavelength", "nm", 400, 700, 5, 550}, {"a", "Circular aperture diameter a", "mm", 0.05, 1, 0.05, 0.2}, {"f", "Screen distance f", "m", 0.5, 3, 0.1, 1}},
        PHF { double lam = p[0] * 1e-9, a = p[1] * 1e-3, f = p[2], r1 = 1.22 * lam * f / a * 1e3, rm = 4.4 * r1; r.color = p[0]; r.rmax = rm; r.snote = "Airy pattern (brightness boosted so the faint rings show)";
              for (int i = 0; i < 260; ++i) { double rr = rm * (i + 0.5) / 260; r.radial.push_back(std::pow(airy(M_PI * a * (rr * 1e-3) / (lam * f)), 0.4)); }
              auto& pl = newPlot(r, "Airy pattern: I = [2·J1(x)/x]²", "r on the screen (mm)", "I / I0"); sample(addS(pl, "I"), 0, rm, 400, [&](double rr) { return airy(M_PI * a * (rr * 1e-3) / (lam * f)); }); pl.marks.push_back({r1, "1st dark ring"});
              ro(r, "Radius of the first dark ring  1.22λf/a", sgn(r1, " mm")); ro(r, "Angular radius  θ = 1.22λ/a", sgn(1.22 * lam / a * 1e3, " mrad")); ro(r, "Light inside the central disc", "83.8 %"); ro(r, "Brightest ring is only", "1.75 % of the centre");
              r.ex = "Far-field (Fraunhofer) diffraction by a round hole gives a bright disc, the Airy disc, surrounded by faint rings. A smaller hole spreads the pattern wider, which is the diffraction limit of every lens."; },
        "Halve the aperture and the Airy disc doubles in radius. Compare with a slit using the Single Slit topic.");

    add("Single Slit", {{"lam", "Wavelength", "nm", 400, 700, 5, 550}, {"a", "Slit width a", "µm", 20, 200, 5, 80}, {"D", "Screen distance D", "m", 0.5, 3, 0.1, 1.5}},
        PHF { double lam = p[0] * 1e-9, a = p[1] * 1e-6, D = p[2], x1 = lam * D / a * 1e3; auto f = [&](double x) { return sinc2(M_PI * a * x * 1e-3 / (lam * D)); };
              auto& pl = newPlot(r, "I = I0·(sin β / β)²,  β = πa·sinθ/λ", "x (mm)", "I / I0"); sample(addS(pl, "I"), -5 * x1, 5 * x1, 700, f); for (int m = -4; m <= 4; ++m) if (m) pl.marks.push_back({m * x1, "m=" + S(m)});
              r.color = p[0]; fillStrip(r, -5 * x1, 5 * x1, 500, [&](double x) { return std::pow(f(x), 0.5); }); r.sunit = "mm";
              ro(r, "Minima at  a·sinθ = mλ  → x", sgn(x1, " mm × m")); ro(r, "Width of the central maximum  2λD/a", sgn(2 * x1, " mm")); ro(r, "First side maximum", "4.5 % of the centre"); ro(r, "Central max is", "twice as wide as the side maxima");
              r.ex = "Each point of the slit acts as a source (Huygens). Waves from the top and bottom halves cancel when a·sinθ = λ. A narrower slit gives a wider pattern: squeeze the light and it spreads out."; },
        "Make the slit 2× narrower: the central maximum doubles in width. Compare the strip brightness with the plot.");

    add("Double Slit", {{"lam", "Wavelength", "nm", 400, 700, 5, 550}, {"d", "Slit separation d", "µm", 100, 600, 10, 300}, {"a", "Slit width a", "µm", 20, 150, 5, 60}, {"D", "Screen distance D", "m", 0.5, 3, 0.1, 1.5}},
        PHF { double lam = p[0] * 1e-9, d = p[1] * 1e-6, a = p[2] * 1e-6, D = p[3], xs = lam * D / a * 1e3, bt = lam * D / d * 1e3;
              auto env = [&](double x) { return sinc2(M_PI * a * x * 1e-3 / (lam * D)); }; auto f = [&](double x) { return std::pow(std::cos(M_PI * d * x * 1e-3 / (lam * D)), 2) * env(x); };
              auto& pl = newPlot(r, "Fringes (cos²) under the single-slit envelope", "x (mm)", "I / I0"); sample(addS(pl, "I"), -3 * xs, 3 * xs, 900, f); sample(addS(pl, "envelope", true), -3 * xs, 3 * xs, 300, env);
              r.color = p[0]; fillStrip(r, -3 * xs, 3 * xs, 500, [&](double x) { return std::pow(f(x), 0.7); }); r.sunit = "mm";
              ro(r, "Fringe spacing  λD/d", sgn(bt, " mm")); ro(r, "Half-width of the envelope  λD/a", sgn(xs, " mm")); ro(r, "Bright fringes in the central maximum", Gs(2 * std::floor(d / a - 1e-9) + 1)); ro(r, "d / a", Gs(d / a) + (std::fabs(d / a - std::round(d / a)) < 0.01 ? "  (whole number: every " + S((int)std::round(d / a)) + "th fringe is missing)" : ""));
              r.ex = "Interference between the two slits (fine fringes) is multiplied by the diffraction of each slit (the slow envelope). Where the envelope is zero a bright fringe can be missing."; },
        "Set d = 300 µm and a = 100 µm: d/a = 3 so the 3rd order fringe is missing.");

    add("Diffraction Grating", {{"lam", "Wavelength", "nm", 400, 700, 5, 550}, {"N", "Lines per mm", "", 100, 1000, 50, 600}, {"Ns", "Number of lines lit", "", 2, 20, 1, 6}, {"m", "Order for the readouts", "", 1, 4, 1, 1}},
        PHF { double lam = p[0] * 1e-9, d = 1e-3 / p[1], a = d / 3; int Ns = (int)p[2], mo = (int)p[3];
              auto f = [&](double s) { double g = M_PI * d * s / lam, den = std::sin(g), arr = std::fabs(den) < 1e-9 ? 1.0 : std::pow(std::sin(Ns * g) / (Ns * den), 2); return sinc2(M_PI * a * s / lam) * arr; };
              auto& pl = newPlot(r, "Intensity against sin θ", "sin θ", "I / I0"); sample(addS(pl, "I"), -1, 1, 3000, f); int mm = (int)std::floor(d / lam + 1e-9); for (int k = -mm; k <= mm; ++k) pl.marks.push_back({k * lam / d, "m=" + S(k)});
              ro(r, "Grating spacing d = 1/N", sgn(d * 1e6, " µm")); ro(r, "Highest order  m ≤ d/λ", S(mm));
              if (mo * lam / d <= 1) ro(r, "Angle of order " + S(mo), sgn(std::asin(mo * lam / d) * 180 / M_PI, "°")); else ro(r, "Angle of order " + S(mo), "this order does not exist");
              ro(r, "Resolving power  R = m·N", Gs(mo * Ns)); ro(r, "Smallest resolvable Δλ = λ/(mN)", sgn(p[0] / (mo * Ns), " nm")); ro(r, "Principal peaks / secondary maxima", "peaks at d·sinθ = mλ; " + S(Ns - 2) + " weak maxima between");
              r.ex = "Many slits make the bright orders very sharp: the more lines you illuminate, the narrower each peak and the better two close wavelengths separate. Orders sit where d·sinθ = mλ."; },
        "Light up 20 lines instead of 6: peaks get 3× sharper. Use 1000 lines/mm at 700 nm and count the orders.");

    add("Polarization", {{"I0", "Unpolarised intensity I0", "", 10, 100, 5, 100}, {"th", "Analyser angle θ", "deg", 0, 180, 1, 30}},
        PHF { double I1 = p[0] / 2, th = p[1] * M_PI / 180; auto& pl = newPlot(r, "Malus's law: I = I1·cos²θ", "angle θ between polarizer and analyser (deg)", "transmitted intensity");
              sample(addS(pl, "I"), 0, 360, 360, [&](double d) { return I1 * std::pow(std::cos(d * M_PI / 180), 2); }); pl.marks.push_back({p[1], "θ"}); pl.ymin = 0;
              ro(r, "After the polarizer (unpolarised in)", Gs(I1) + "  (= I0/2)"); ro(r, "After the analyser", Gs(I1 * std::pow(std::cos(th), 2))); ro(r, "Maximum at", "θ = 0° and 180°"); ro(r, "Extinction at", "θ = 90° (crossed)");
              r.ex = "A polarizer passes only light vibrating along its axis, so natural light loses half its intensity. A second polarizer then passes cos²θ of what is left (Malus's law)."; },
        "Put the analyser at 60°: the intensity is one quarter of I1. Find where the output is zero.");

    add("Birefringence", {{"cr", "Crystal (0 calcite, 1 quartz, 2 rutile)", "", 0, 2, 1, 0}, {"t", "Thickness t", "µm", 1, 500, 1, 100}, {"lam", "Wavelength", "nm", 400, 700, 5, 589}},
        PHF { static const double no[3] = {1.6584, 1.5443, 2.616}, ne[3] = {1.4864, 1.5534, 2.903}; static const char* nm[3] = {"calcite", "quartz", "rutile"}; int c = (int)std::lround(p[0]); double dn = ne[c] - no[c], t = p[1] * 1e-6, lam = p[2] * 1e-9;
              auto& pl = newPlot(r, "Retardation Γ = Δn·t / λ", "thickness t (µm)", "retardation (waves)"); sample(addS(pl, "Γ"), 0, 500, 100, [&](double x) { return std::fabs(dn) * x * 1e-6 / lam; }); pl.marks.push_back({p[1], "t"});
              ro(r, "Crystal", std::string(nm[c]) + (dn < 0 ? " (negative: e-ray is faster)" : " (positive: e-ray is slower)")); ro(r, "no (ordinary)", Gs(no[c])); ro(r, "ne (extraordinary)", Gs(ne[c])); ro(r, "Δn = ne − no", Gs(dn));
              ro(r, "Path difference Δn·t", sgn(std::fabs(dn) * t * 1e9, " nm")); ro(r, "Phase difference δ", sgn(360 * std::fabs(dn) * t / lam, "°")); ro(r, "Speeds  c/no and c/ne", Gs(PC / no[c] / 1e8) + " and " + Gs(PC / ne[c] / 1e8) + " ×10⁸ m/s");
              r.ex = "A birefringent crystal has two refractive indices. The ordinary and extraordinary rays travel at different speeds and gain a phase difference that grows with thickness."; },
        "Compare calcite (large Δn) with quartz (small Δn) at the same thickness.");

    add("Polarizers", {{"t1", "First polarizer angle", "deg", 0, 180, 1, 0}, {"t2", "Middle polarizer angle", "deg", 0, 180, 1, 45}, {"t3", "Last polarizer angle", "deg", 0, 180, 1, 90}},
        PHF { auto rad = [](double d) { return d * M_PI / 180; }; auto I = [&](double t2) { return 0.5 * std::pow(std::cos(rad(t2 - p[0])), 2) * std::pow(std::cos(rad(p[2] - t2)), 2); };
              auto& pl = newPlot(r, "Output (fraction of I0) as the middle polarizer turns", "middle polarizer angle (deg)", "I / I0"); sample(addS(pl, "three polarizers"), 0, 180, 361, I); pl.marks.push_back({p[1], "now"}); pl.ymin = 0;
              ro(r, "Output with the middle one", Gs(I(p[1])) + " × I0"); ro(r, "Output with the middle one removed", Gs(0.5 * std::pow(std::cos(rad(p[2] - p[0])), 2)) + " × I0"); ro(r, "Best middle angle", Gs((p[0] + p[2]) / 2) + "°");
              r.ex = "Two crossed polarizers block all light. Slip a third one between them at 45° and light comes through again (12.5 % of I0): the middle polarizer rotates the direction of vibration step by step."; },
        "Start with 0° and 90° and the middle at 0°: no light. Move the middle to 45° and light appears.");

    add("Compensators", {{"lam", "Wavelength", "nm", 400, 700, 5, 589}, {"dn", "Δn of quartz wedges", "", 0.001, 0.02, 0.001, 0.009}, {"al", "Wedge angle α", "deg", 0.5, 5, 0.1, 2}, {"G", "Retardation of sample", "waves", 0, 1, 0.05, 0.25}},
        PHF { double lam = p[0] * 1e-9, k = 2 * p[1] * std::tan(p[2] * M_PI / 180) / lam * 1e-3, G0 = p[3]; auto f = [&](double x) { return std::pow(std::sin(M_PI * (k * x + G0)), 2); };
              auto& pl = newPlot(r, "Babinet compensator between crossed polarizers", "position x along the compensator (mm)", "I / I0"); sample(addS(pl, "I"), -10, 10, 800, f);
              double sp = 1 / k, x0 = -G0 / k; if (x0 < -sp) x0 += sp * std::ceil(-x0 / sp); pl.marks.push_back({std::fmod(x0, sp) > 10 ? std::fmod(x0, sp) - sp : std::fmod(x0, sp), "zero order"});
              r.color = p[0]; fillStrip(r, -10, 10, 500, f); r.sunit = "mm";
              ro(r, "Fringe spacing  λ / (2Δn·tanα)", sgn(sp, " mm")); ro(r, "Retardation made by the wedge", "Γ(x) = 2Δn·x·tanα / λ"); ro(r, "Sample retardation", Gs(G0) + " waves"); ro(r, "Dark fringe shifted by", sgn(G0 * sp, " mm"));
              r.ex = "Two opposite quartz wedges make a retardation that changes steadily with position. Adding a sample shifts the dark fringes: the shift measured in fringe spacings is the sample's retardation in waves."; },
        "Set the sample retardation to 0.5 and see the dark fringe move half a fringe spacing.");

    add("Wave Plates", {{"G", "Retardation of the plate", "waves", 0, 1, 0.01, 0.25}, {"ph", "Input polarisation angle", "deg", 0, 90, 1, 45}},
        PHF { double dl = 2 * M_PI * p[0], ph = p[1] * M_PI / 180; auto& pl = newPlot(r, "Tip of the electric field after the plate (Ey against Ex)", "Ex", "Ey"); pl.equal = true;
              sample(addS(pl, "output"), 0, 2 * M_PI, 200, [&](double) { return 0.0; }); auto& s = pl.s.back(); s.x.clear(); s.y.clear(); for (int i = 0; i <= 200; ++i) { double t = 2 * M_PI * i / 200; s.x.push_back(std::cos(ph) * std::cos(t)); s.y.push_back(std::sin(ph) * std::cos(t - dl)); }
              auto& in = addS(pl, "input", true); in.x = {-std::cos(ph), std::cos(ph)}; in.y = {-std::sin(ph), std::sin(ph)};
              std::string st; if (std::fabs(std::sin(dl)) < 0.03 || p[1] < 1 || p[1] > 89) st = "linear"; else if (std::fabs(p[1] - 45) < 1 && std::fabs(std::cos(dl)) < 0.04) st = std::sin(dl) > 0 ? "circular (one handedness)" : "circular (opposite handedness)"; else st = "elliptical";
              ro(r, "Phase retardation δ", Gs(p[0] * 360) + "°"); ro(r, "Output polarisation", st);
              if (std::fabs(p[0] - 0.5) < 0.01) ro(r, "Half-wave plate", "linear light turned to angle −" + Gs(p[1]) + "°"); if (std::fabs(p[0] - 0.25) < 0.01) ro(r, "Quarter-wave plate", p[1] == 45 ? "linear → circular" : "linear → elliptical");
              r.ex = "A wave plate delays one component of the light by a fraction of a wavelength. A quarter-wave plate (0.25) turns 45° linear light into circular; a half-wave plate (0.5) flips the angle of linear light."; },
        "Set 0.25 and 45°: a circle. Set 0.5 and 30°: a line at −30°. Set 0.25 and 20°: an ellipse.");

    add("Retardation Plate", {{"lam", "Wavelength", "nm", 400, 700, 5, 550}, {"dn", "Δn of the plate", "", 0.001, 0.02, 0.001, 0.009}, {"t", "Plate thickness t", "µm", 1, 200, 1, 15}, {"th", "Plate axis angle θ", "deg", 0, 90, 1, 45}},
        PHF { double lam = p[0] * 1e-9, dn = p[1], th = p[3] * M_PI / 180; auto I = [&](double t) { return std::pow(std::sin(2 * th), 2) * std::pow(std::sin(M_PI * dn * t * 1e-6 / lam), 2); };
              auto& pl = newPlot(r, "Light through crossed polarizers with the plate between", "thickness t (µm)", "I / I0"); sample(addS(pl, "I"), 0, 200, 600, I); pl.marks.push_back({p[2], "t"}); pl.ymin = 0;
              ro(r, "Retardation of this plate", Gs(dn * p[2] * 1e-6 / lam) + " waves"); ro(r, "Quarter-wave thickness  λ/(4Δn)", sgn(lam / (4 * dn) * 1e6, " µm")); ro(r, "Half-wave thickness  λ/(2Δn)", sgn(lam / (2 * dn) * 1e6, " µm")); ro(r, "Transmitted fraction", Gs(I(p[2])));
              r.ex = "The thickness decides the retardation: Γ = Δn·t/λ. Between crossed polarizers the transmitted light is sin²2θ·sin²(πΓ), largest when the plate axis is at 45°."; },
        "Find the thickness of a half-wave plate: the plot reaches 1 there when θ = 45°.");

    add("Optical Activity", {{"al", "Specific rotation [α]", "°·dm⁻¹·(g/mL)⁻¹", 0, 100, 0.5, 66.5}, {"l", "Tube length l", "dm", 0.5, 4, 0.5, 2}, {"c", "Concentration c", "g/mL", 0, 1, 0.01, 0.2}},
        PHF { double th = p[0] * p[1] * p[2]; auto& pl = newPlot(r, "Biot's law: θ = [α]·l·c", "concentration c (g/mL)", "rotation θ (deg)"); sample(addS(pl, "θ"), 0, 1, 50, [&](double c) { return p[0] * p[1] * c; }); pl.marks.push_back({p[2], "c"});
              auto& q2 = newPlot(r, "Plane of polarisation before and after the tube", "", ""); q2.equal = true; auto& a = addS(q2, "incoming", true, true); a.x = {-1, 1}; a.y = {0, 0}; auto& b = addS(q2, "outgoing", false, true); b.x = {-std::cos(th * M_PI / 180), std::cos(th * M_PI / 180)}; b.y = {-std::sin(th * M_PI / 180), std::sin(th * M_PI / 180)};
              ro(r, "Rotation θ", sgn(th, "°") + " (clockwise = dextrorotatory)"); ro(r, "Law", "θ = [α]·l·c"); ro(r, "Rotation per dm of tube", sgn(p[0] * p[2], "°"));
              r.ex = "Some substances (sugar solutions, quartz) rotate the plane of polarisation. The angle grows with path length and concentration. [α] is a property of the substance and depends on wavelength."; },
        "Double the tube length: θ doubles. Use [α] = 66.5 (sucrose) and find c that gives 20°.");

    add("Polarimeter", {{"al", "Specific rotation [α]", "°·dm⁻¹·(g/mL)⁻¹", 10, 100, 0.5, 66.5}, {"l", "Tube length l", "dm", 0.5, 4, 0.5, 2}, {"c", "Unknown concentration c", "g/mL", 0, 0.6, 0.01, 0.2}, {"ps", "Analyser angle ψ (you turn it)", "deg", -90, 90, 0.5, 0}},
        PHF { double th = std::fmod(p[0] * p[1] * p[2], 180); if (th > 90) th -= 180; double eps = 4 * M_PI / 180, ps = p[3] * M_PI / 180, ths = th * M_PI / 180; auto A = [&](double q) { return std::pow(std::sin(q - ths + eps), 2); };
              auto B = [&](double q) { return std::pow(std::sin(q - ths - eps), 2); };
              auto& pl = newPlot(r, "Two halves of the half-shadow field", "analyser angle ψ (deg)", "brightness"); sample(addS(pl, "left half"), -90, 90, 400, [&](double d) { return A(d * M_PI / 180); }); sample(addS(pl, "right half", true), -90, 90, 400, [&](double d) { return B(d * M_PI / 180); });
              pl.marks.push_back({p[3], "ψ"}); double ia = A(ps), ib = B(ps); bool eq = std::fabs(ia - ib) < 0.004;
              ro(r, "Left / right brightness", Gs(ia) + " / " + Gs(ib)); ro(r, "Field", eq ? "balanced: both halves equal, read ψ" : (ia > ib ? "left half brighter" : "right half brighter, turn the other way")); ro(r, "Concentration from ψ  c = ψ/([α]·l)", eq ? Gs(p[3] / (p[0] * p[1])) + " g/mL" : "turn ψ until the halves match");
              r.ex = "In a half-shadow polarimeter the eye compares two half-fields. When they match in brightness the analyser angle equals the rotation θ, which gives the concentration. Turn ψ until the field balances."; },
        "Find the angle that balances the field and compute c. Then change the unknown c and repeat.");

    add("Resolving Power of Microscope", {{"lam", "Wavelength", "nm", 400, 700, 5, 550}, {"na", "Numerical aperture NA", "", 0.25, 1.4, 0.05, 0.65}, {"s", "Separation of two points", "nm", 100, 800, 10, 400}},
        PHF { double lam = p[0], dm = 0.61 * lam / p[1], s = p[2], vv = 2 * M_PI * p[1] / lam; auto I = [&](double x) { return airy(vv * x); }; double w = std::max(3 * dm, s);
              auto& pl = newPlot(r, "Images of two points (Airy patterns) and their sum", "position (nm)", "intensity"); sample(addS(pl, "point A", true), -w, w, 700, [&](double x) { return I(x + s / 2); }); sample(addS(pl, "point B", true), -w, w, 700, [&](double x) { return I(x - s / 2); }); sample(addS(pl, "sum"), -w, w, 700, [&](double x) { return I(x + s / 2) + I(x - s / 2); });
              double mid = 2 * I(s / 2), pk = I(0) + I(s); ro(r, "Smallest resolvable distance  d = 0.61λ/NA", sgn(dm, " nm")); ro(r, "Your separation", sgn(s, " nm") + (s >= dm ? "  → resolved" : "  → NOT resolved")); ro(r, "Dip between the peaks", Gs(mid / std::max(pk, 1e-9)) + " of the peak (Rayleigh: 0.735)"); ro(r, "Resolving power  1/d", Gs(1e3 / dm) + " per µm");
              r.ex = "Two points can be told apart if the centre of one Airy pattern lies no closer than the first dark ring of the other (Rayleigh criterion). Shorter wavelength and larger NA resolve finer detail."; },
        "Use NA = 1.4 (oil immersion) with 400 nm light to reach about 175 nm. Then slide the separation across the limit.");

    /* ================= Relativity ================= */
    add("Frame of Reference", {{"v", "Train speed v (relative to ground)", "m/s", 0, 40, 1, 20}, {"u", "Ball speed u (relative to train)", "m/s", -30, 30, 1, 10}, {"T", "Time shown", "s", 1, 10, 1, 5}},
        PHF { double v = p[0], u = p[1], T = p[2]; auto& pl = newPlot(r, "Position against time in the ground frame", "time t (s)", "position (m)"); sample(addS(pl, "train", false, true), 0, T, 2, [&](double t) { return v * t; }); sample(addS(pl, "ball (ground view)", false, true), 0, T, 2, [&](double t) { return (u + v) * t; }); sample(addS(pl, "ball (train view)", true, true), 0, T, 2, [&](double t) { return u * t; });
              ro(r, "Ball speed seen by a passenger", Gs(u) + " m/s"); ro(r, "Ball speed seen by an observer on the ground", Gs(u + v) + " m/s  (u + v)"); ro(r, "After " + Gs(T) + " s the ball is", Gs((u + v) * T) + " m from the ground origin");
              r.ex = "Motion is described relative to a frame of reference. The same ball has different speeds for different observers, and in everyday speeds the speeds simply add. Inertial frames move at constant velocity relative to each other."; },
        "Throw the ball backwards (u = −20) with the train at 20: the ground observer sees it at rest.");

    add("Galilean Transformations", {{"v", "Frame S' speed v", "m/s", 0, 40, 1, 20}, {"x", "Event position x", "m", 0, 100, 1, 50}, {"t", "Event time t", "s", 0, 10, 0.5, 3}, {"u", "Particle speed u in S", "m/s", -30, 50, 1, 25}},
        PHF { double v = p[0], x = p[1], t = p[2]; auto& pl = newPlot(r, "x' = x − vt for the event as time passes", "time t (s)", "x' (m)"); sample(addS(pl, "x' (v as chosen)"), 0, 10, 2, [&](double tt) { return x - v * tt; }); sample(addS(pl, "x (frame S)", true), 0, 10, 2, [&](double) { return x; }); pl.marks.push_back({t, "t"});
              auto& q2 = newPlot(r, "Speed of light in S' as Galileo predicts", "v / c", "(c − v)/c"); sample(addS(q2, "Galilean (c − v)"), 0, 0.9, 30, [](double b) { return 1 - b; }); sample(addS(q2, "experiment: always c", true), 0, 0.9, 2, [](double) { return 1.0; });
              ro(r, "x' = x − vt", Gs(x - v * t) + " m"); ro(r, "t' = t", Gs(t) + " s (time is absolute)"); ro(r, "u' = u − v", Gs(p[3] - v) + " m/s"); ro(r, "Acceleration", "a' = a (invariant)");
              r.ex = "Galilean transformations connect two frames in relative motion at everyday speeds: space coordinates shift, time is the same. They fail for light: they predict c − v, but experiments always measure c."; },
        "Change v and see u' change; look at the second graph to see where Galileo fails.");

    add("Michelson–Morley Experiment", {{"L", "Arm length L (with mirrors)", "m", 1, 30, 1, 11}, {"lam", "Wavelength", "nm", 400, 700, 5, 500}, {"v", "Earth's speed through the ether", "km/s", 0, 60, 1, 30}},
        PHF { double L = p[0], lam = p[1] * 1e-9; auto dN = [&](double v) { return 2 * L / lam * std::pow(v * 1e3 / PC, 2); }; auto& pl = newPlot(r, "Expected fringe shift when the apparatus turns by 90°", "ether wind speed (km/s)", "fringe shift ΔN"); sample(addS(pl, "predicted"), 0, 60, 120, dN); sample(addS(pl, "observed (< 0.01)", true), 0, 60, 2, [](double) { return 0.01; }); pl.marks.push_back({p[2], "v"});
              ro(r, "Predicted shift  ΔN = (2L/λ)(v/c)²", Gs(dN(p[2])) + " fringes"); ro(r, "Observed shift", "less than 0.01 fringes (essentially zero)"); ro(r, "Time difference between arms", sgn(L * std::pow(p[2] * 1e3 / PC, 2) / PC * 1e15, " fs"));
              r.ex = "If light travelled through an ether, the Earth's motion would make light take different times along the two arms and the fringes would shift when the instrument is rotated. No shift was found: the speed of light is the same in all frames."; },
        "Use L = 11 m, λ = 500 nm, v = 30 km/s to get the famous 0.4 fringes the 1887 experiment should have seen.");

    add("Lorentz Transformations", {{"b", "Frame speed β = v/c", "", 0, 0.99, 0.01, 0.6}, {"x", "Event position x", "light-seconds", -5, 5, 0.5, 3}, {"ct", "Event time ct", "light-seconds", 0, 8, 0.5, 4}},
        PHF { double b = p[0], g = 1 / std::sqrt(1 - b * b), x = p[1], ct = p[2], xp = g * (x - b * ct), ctp = g * (ct - b * x); auto& pl = newPlot(r, "Spacetime diagram (frame S axes; S' axes tilted)", "x (light-seconds)", "ct (light-seconds)"); pl.equal = true;
              sample(addS(pl, "light  x = ct", true, true), 0, 8, 2, [](double t) { return t; }); auto& l2 = addS(pl, "", true); l2.x = {0, -8}; l2.y = {0, 8}; auto& a1 = addS(pl, "ct' axis", false, true); a1.x = {0, 8 * b}; a1.y = {0, 8}; auto& a2 = addS(pl, "x' axis", false, true); a2.x = {0, 8}; a2.y = {0, 8 * b};
              auto& ev = addS(pl, "event", false, true); ev.x = {x}; ev.y = {ct};
              ro(r, "Lorentz factor γ", Gs(g)); ro(r, "x' = γ(x − vt)", Gs(xp) + " ls"); ro(r, "ct' = γ(ct − vx/c)", Gs(ctp) + " ls"); ro(r, "Invariant interval (ct)² − x²", Gs(ct * ct - x * x) + "  (same in both frames: " + Gs(ctp * ctp - xp * xp) + ")"); ro(r, "Moving clocks tick slower by", "Δt = γΔt'  → " + Gs(g)); ro(r, "Moving rods shorten to", "L0/γ → " + Gs(1 / g) + " × L0");
              r.ex = "Lorentz transformations keep the speed of light the same for every observer. The tilted axes show that the new frame disagrees about both position and time. Only the spacetime interval is shared."; },
        "Set β = 0.6 and x = 3, ct = 4: the event is on a light ray? Check the invariant interval.");

    add("Addition of Velocities", {{"u", "Speed u' inside the moving frame", "c", 0, 0.99, 0.01, 0.8}, {"v", "Speed v of the frame", "c", 0, 0.99, 0.01, 0.8}},
        PHF { double u = p[0], v = p[1]; auto& pl = newPlot(r, "Total speed against frame speed", "v / c", "speed seen by the ground / c"); sample(addS(pl, "relativistic", false, true), 0, 0.99, 100, [&](double w) { return (u + w) / (1 + u * w); }); sample(addS(pl, "Galilean u' + v", true, true), 0, 0.99, 100, [&](double w) { return u + w; }); pl.ymax = 2; pl.ymin = 0; pl.marks.push_back({v, "v"});
              ro(r, "Relativistic  (u' + v)/(1 + u'v/c²)", Gs((u + v) / (1 + u * v)) + " c"); ro(r, "Galilean  u' + v", Gs(u + v) + " c"); ro(r, "Light check (u' = 1)", Gs((1 + v) / (1 + v)) + " c always");
              r.ex = "At high speeds velocities do not add like numbers. The relativistic rule keeps every result below c, and a light beam has speed c for every observer."; },
        "Fire a beam at 0.9c from a ship moving at 0.9c: the ground sees only 0.994c.");

    add("Mass Variation with Velocity", {{"b", "Speed β = v/c", "", 0, 0.999, 0.001, 0.8}, {"m0", "Rest mass m0", "electron masses", 1, 2000, 1, 1}},
        PHF { double b = p[0], g = 1 / std::sqrt(1 - b * b); auto& pl = newPlot(r, "m = γ·m0", "β = v / c", "m / m0"); sample(addS(pl, "m/m0"), 0, 0.999, 300, [](double x) { return 1 / std::sqrt(1 - x * x); }); pl.ymax = 10; pl.ymin = 1; pl.marks.push_back({b, "β"});
              ro(r, "γ = 1/√(1 − v²/c²)", Gs(g)); ro(r, "Moving mass m = γ·m0", Gs(g * p[1]) + " electron masses"); ro(r, "Momentum p = γ·m0·v", Gs(g * p[1] * b) + " m_e·c"); ro(r, "Speed of an electron at this β", Gs(b * PC / 1e6) + " ×10⁶ m/s");
              r.ex = "The mass measured by a moving observer grows as m = m0/√(1 − v²/c²). It rises without limit near c, which is why no massive object can reach the speed of light."; },
        "Find β where the mass doubles (γ = 2, β = 0.866) and where it is 10 times.");

    add("Mass–Energy Relation", {{"m", "Mass converted", "g", 0.01, 1000, 0.01, 1}, {"b", "Speed β (for kinetic energy)", "c", 0, 0.99, 0.01, 0.5}},
        PHF { double m = p[0] * 1e-3, E = m * PC * PC, b = p[1], g = 1 / std::sqrt(1 - b * b); auto& pl = newPlot(r, "Kinetic energy in units of m0c²", "β = v / c", "K / (m0c²)"); sample(addS(pl, "relativistic (γ − 1)", false, true), 0, 0.99, 100, [](double x) { return 1 / std::sqrt(1 - x * x) - 1; }); sample(addS(pl, "classical ½β²", true, true), 0, 0.99, 100, [](double x) { return 0.5 * x * x; }); pl.ymax = 6; pl.marks.push_back({b, "β"});
              ro(r, "Energy E = mc²", Gs(E) + " J"); ro(r, "In kilowatt-hours", Gs(E / 3.6e6) + " kWh"); ro(r, "In tonnes of TNT", Gs(E / 4.184e9) + " t"); ro(r, "Kinetic energy at β = " + Gs(b) + "  (γ − 1)m0c²", Gs((g - 1) * E) + " J"); ro(r, "Classical ½mv² would give", Gs(0.5 * b * b * E) + " J");
              r.ex = "Mass is a form of energy: E = mc². One gram holds about 9 × 10¹³ joules. At speed the kinetic energy is (γ−1)m0c², which matches ½mv² only when v is small."; },
        "Convert 1 g and compare it with 21.5 kt of TNT. Push β to 0.9 and see how far the classical curve falls behind.");
    addPhysics2(m);
    addMath1(m);
    addMath2(m);
    addMath3(m);
    addMath4(m);
    return m;
}
