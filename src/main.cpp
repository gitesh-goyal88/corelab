#include <cstdlib>
#include <string>
#include "content.hpp"
#include "http_server.hpp"
#include "simulations.hpp"
#include "simulations2.hpp"
#include "simulations3.hpp"
#include "simulations4.hpp"
#include "simulations5.hpp"
#include "physics.hpp"
#include "physics2.hpp"
#include "mathsim1.hpp"
#include "mathsim2.hpp"
#include "mathsim3.hpp"
#include "mathsim4.hpp"
#include "simulations6.hpp"
#include "simulations7.hpp"
#include "simulations8.hpp"
#include "simulations9.hpp"
#include "ds1.hpp"
#include "ds2.hpp"
#include "ds3.hpp"
#include "ds4.hpp"
#include "ds5.hpp"
#include "ds6.hpp"
#include "ds7.hpp"
#include "ds8.hpp"
#include "sdf2_1.hpp"
#include "sdf2_2.hpp"
#include "sdf2_3.hpp"
#include "sdf2_4.hpp"
#include "reallife.hpp"
#include "cross_apps.hpp"
#include "benchmark_progress.hpp"

using namespace std;

static string handleCurriculum(const string&) {
    return curriculumJson();
}

static string handleSimulate(const string& qs) {
    return simulateJson(queryParam(qs, "topic"), queryParam(qs, "input"), atoi(queryParam(qs, "opt").c_str()));
}

static string handleRealLife(const string& qs) {
    return realLifeJson(queryParam(qs, "topic"));
}

static string handlePhysics(const string& qs) {
    return physicsJson(queryParam(qs, "topic"), queryParam(qs, "p"));
}

static string handleCampusNavigator(const string& qs) {
    return CampusNavigator::findShortestPath(queryParam(qs, "from"), queryParam(qs, "to"));
}

static string handleCoursePrereq(const string& qs) {
    return CoursePrereqChecker::checkPrerequisites(queryParam(qs, "course"));
}

static string handleAutocomplete(const string& qs) {
    return AutocompleteTrie::queryJson(queryParam(qs, "q"));
}

static string handleTaskScheduler(const string& qs) {
    return TaskScheduler::runAction(queryParam(qs, "action"), atoi(queryParam(qs, "priority").c_str()), queryParam(qs, "name"));
}

static string handleBenchmark(const string& qs) {
    return BenchmarkRunner::runBenchmark(queryParam(qs, "struct"), atoi(queryParam(qs, "n").c_str()));
}

static string handleProgress(const string& qs) {
    string act = queryParam(qs, "action");
    if (act == "save") {
        bool ok = ProgressStore::getInstance().save(
            queryParam(qs, "topic"),
            queryParam(qs, "status"),
            atoi(queryParam(qs, "score").c_str())
        );
        return string("{\"saved\":") + (ok ? "true" : "false") + "}";
    }
    return ProgressStore::getInstance().toJson();
}

int main(int argc, char** argv) {
    int port = (argc > 1) ? atoi(argv[1]) : 8080;

    // Build lookup tables once before any browser connects
    sims();
    physMap();
    simulateJson("Loops", "", 0);
    physicsJson("Double Slit", "");

    HttpServer server(port, "web");
    server.api("/api/curriculum", handleCurriculum);
    server.api("/api/simulate", handleSimulate);
    server.api("/api/reallife", handleRealLife);
    server.api("/api/physics", handlePhysics);
    server.api("/api/campus_navigator", handleCampusNavigator);
    server.api("/api/course_prereq", handleCoursePrereq);
    server.api("/api/autocomplete", handleAutocomplete);
    server.api("/api/task_scheduler", handleTaskScheduler);
    server.api("/api/benchmark", handleBenchmark);
    server.api("/api/progress", handleProgress);
    server.run();

    return 0;
}
