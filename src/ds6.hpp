// Data Structures (Semester 3), part 6: Module 5 (graphs).
#pragma once
#include "ds5.hpp"
#include <numeric>

using namespace std;

class GraphData {
public:
    int numVertices;
    bool isDirected;
    bool isWeighted;
    vector<array<int, 3>> edges;

    GraphData() {
        numVertices = 0;
        isDirected = false;
        isWeighted = false;
    }
};

static GraphData dsBuildGraphFromRaw(const Ints& rawValues, bool weighted, bool directed, int weightLo, int weightHi, int maxVertices) {
    GraphData graph;
    graph.isDirected = directed;
    graph.isWeighted = weighted;
    size_t stride = weighted ? 3 : 2;
    for (size_t i = 0; i + stride <= rawValues.size() && graph.edges.size() < 12; i += stride) {
        int u = (int)(abs((long long)rawValues[i]) % maxVertices);
        int v = (int)(abs((long long)rawValues[i + 1]) % maxVertices);
        int w = weighted ? clamp(rawValues[i + 2], weightLo, weightHi) : 1;
        if (u == v) continue;
        bool duplicate = false;
        for (size_t e = 0; e < graph.edges.size(); ++e) {
            int eu = graph.edges[e][0];
            int ev = graph.edges[e][1];
            if ((eu == u && ev == v) || (!directed && eu == v && ev == u)) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) continue;
        graph.edges.push_back({u, v, w});
    }
    for (size_t e = 0; e < graph.edges.size(); ++e) {
        graph.numVertices = max(graph.numVertices, max(graph.edges[e][0], graph.edges[e][1]) + 1);
    }
    return graph;
}

static GraphData dsParseGraph(const Ints& input, size_t startIndex, bool weighted, bool directed, Ints defaultValues,
                              int weightLo = 1, int weightHi = 99, int maxVertices = 8) {
    Ints rawValues;
    for (size_t i = startIndex; i < input.size() && rawValues.size() < 40; ++i) {
        rawValues.push_back(input[i]);
    }
    GraphData graph = dsBuildGraphFromRaw(rawValues, weighted, directed, weightLo, weightHi, maxVertices);
    if (graph.edges.empty() || graph.numVertices < 2) {
        graph = dsBuildGraphFromRaw(defaultValues, weighted, directed, weightLo, weightHi, maxVertices);
    }
    return graph;
}

static string dsDrawGraph(const GraphData& graph, const vector<int>& vertexState, const vector<string>& vertexLabels,
                          const vector<char>& chosenEdges, const string& caption) {
    Sk canvas;
    const double centerX = 180;
    const double centerY = 112;
    const double layoutRadius = 84;
    const double nodeRadius = 17;
    int vertexCount = graph.numVertices;
    vector<double> posX(vertexCount);
    vector<double> posY(vertexCount);
    for (int i = 0; i < vertexCount; ++i) {
        double angle = -3.14159265 / 2 + 2 * 3.14159265 * i / vertexCount;
        posX[i] = centerX + layoutRadius * cos(angle);
        posY[i] = centerY + layoutRadius * sin(angle);
    }
    for (size_t i = 0; i < graph.edges.size(); ++i) {
        int u = graph.edges[i][0];
        int v = graph.edges[i][1];
        double dx = posX[v] - posX[u];
        double dy = posY[v] - posY[u];
        double length = sqrt(dx * dx + dy * dy);
        if (length < 1) continue;
        dx /= length;
        dy /= length;
        string edgeId = "e" + S((int)i);
        double x1 = posX[u] + dx * nodeRadius;
        double y1 = posY[u] + dy * nodeRadius;
        double x2 = posX[v] - dx * nodeRadius;
        double y2 = posY[v] - dy * nodeRadius;
        if (i < chosenEdges.size() && chosenEdges[i]) {
            string chosenId = "ec" + S((int)i);
            canvas.W(chosenId.c_str(), {x1 - dy * 1.6, y1 + dx * 1.6, x2 - dy * 1.6, y2 + dx * 1.6});
            canvas.W(chosenId.c_str(), {x1 + dy * 1.6, y1 - dx * 1.6, x2 + dy * 1.6, y2 - dx * 1.6});
        }
        if (graph.isDirected) canvas.Arr(x1, y1, x2, y2, edgeId.c_str());
        else canvas.W(edgeId.c_str(), {x1, y1, x2, y2});
        if (graph.isWeighted) {
            double labelX = (x1 + x2) / 2 - dy * 9;
            double labelY = (y1 + y2) / 2 + dx * 9;
            canvas.T(labelX, labelY, S(graph.edges[i][2]));
        }
    }
    for (int i = 0; i < vertexCount; ++i) {
        string vertexId = "v" + S(i);
        int state = i < (int)vertexState.size() ? vertexState[i] : 0;
        const char* cls = state == 1 ? "blue" : state == 2 ? "weld" : "steel2";
        canvas.K(vertexId.c_str(), posX[i], posY[i], nodeRadius, cls);
        canvas.T(posX[i], posY[i], S(i), state == 1 ? "mlight" : "middle");
        if (i < (int)vertexLabels.size() && !vertexLabels[i].empty()) {
            double offX = posX[i] - centerX;
            double offY = posY[i] - centerY;
            double offLen = sqrt(offX * offX + offY * offY);
            if (offLen < 1) offLen = 1;
            canvas.T(posX[i] + offX / offLen * (nodeRadius + 17), posY[i] + offY / offLen * (nodeRadius + 14), vertexLabels[i]);
        }
    }
    canvas.T(180, 226, caption);
    return canvas.out(360, 236, {});
}

static vector<vector<pair<int, int>>> dsGraphAdj(const GraphData& graph) {
    vector<vector<pair<int, int>>> adj(graph.numVertices);
    for (size_t i = 0; i < graph.edges.size(); ++i) {
        adj[graph.edges[i][0]].push_back(make_pair(graph.edges[i][1], (int)i));
        if (!graph.isDirected) adj[graph.edges[i][1]].push_back(make_pair(graph.edges[i][0], (int)i));
    }
    for (size_t i = 0; i < adj.size(); ++i) {
        sort(adj[i].begin(), adj[i].end());
    }
    return adj;
}

static const Ints& dsDefaultGraphEdges() {
    static const Ints edges = {0, 1, 0, 2, 1, 3, 2, 3, 3, 4, 4, 5};
    return edges;
}

static const vector<string>& dsGraphRefs() {
    static const vector<string> refs = {
        "VisuAlgo: graph traversal, SSSP, MST|https://visualgo.net/en/dfsbfs",
        "GeeksforGeeks: Graph data structure|https://www.geeksforgeeks.org/graph-data-structure-and-algorithms/",
        "cp-algorithms: graphs|https://cp-algorithms.com/"};
    return refs;
}

static string dsFormatVertexList(const vector<int>& values, int upto) {
    string result;
    for (int i = 0; i < upto && i < (int)values.size(); ++i) {
        if (!result.empty()) result += "  ";
        result += "v" + S(i) + ":" + S(values[i]);
    }
    return result;
}

static vector<string> dsBuildAdjMatrixCells(int vertexCount, const vector<vector<int>>& matrix) {
    vector<string> cells;
    cells.push_back("");
    for (int j = 0; j < vertexCount; ++j) cells.push_back(S(j));
    for (int i = 0; i < vertexCount; ++i) {
        cells.push_back(S(i));
        for (int j = 0; j < vertexCount; ++j) cells.push_back(S(matrix[i][j]));
    }
    return cells;
}

static Step dsMatrixStep(int lineNum, const string& noteText, Vars varsList, const GraphData& graph,
                         const vector<int>& vertexState, const vector<char>& chosenEdges,
                         const vector<vector<int>>& matrix, int hiCell, vector<string> highlightIds,
                         const string& caption) {
    Step step = dsDiagramStep(lineNum, noteText, varsList, dsDrawGraph(graph, vertexState, {}, chosenEdges, caption), highlightIds);
    step.mat = dsBuildAdjMatrixCells(graph.numVertices, matrix);
    step.mc = graph.numVertices + 1;
    step.hi = hiCell;
    return step;
}

static int dsGraphStartVertex(const Ints& input, int vertexCount) {
    long long raw = input.empty() ? 0 : (long long)input[0];
    return (int)(abs(raw) % vertexCount);
}

static Step dsGraphStep(int lineNum, const string& noteText, Vars varsList, const GraphData& graph,
                        const vector<int>& vertexState, const vector<string>& vertexLabels,
                        const vector<char>& chosenEdges, const string& caption, vector<string> highlightIds) {
    return dsDiagramStep(lineNum, noteText, varsList, dsDrawGraph(graph, vertexState, vertexLabels, chosenEdges, caption), highlightIds);
}

static Steps simGraphFundamentalsHelper(bool isDirected, const Ints& input) {
    GraphData graph = dsParseGraph(input, 0, false, isDirected, dsDefaultGraphEdges());
    int vertexCount = graph.numVertices;
    vector<int> degree(vertexCount, 0);
    vector<int> inDegree(vertexCount, 0);
    vector<int> outDegree(vertexCount, 0);
    vector<int> vertexState(vertexCount, 0);
    vector<char> chosenEdges(graph.edges.size(), 0);
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        string("A graph is a set of VERTICES (circles) joined by EDGES (lines). ") +
            (isDirected ? "This one is DIRECTED: each edge is an arrow from one vertex to another."
                        : "This one is UNDIRECTED: an edge works in both directions."),
        {{"vertices V", S(vertexCount)}, {"edges E", S(graph.edges.size())}},
        dsDrawGraph(graph, vertexState, {}, chosenEdges, "V = " + S(vertexCount) + ", E = " + S(graph.edges.size())), {}));
    for (size_t i = 0; i < graph.edges.size(); ++i) {
        int u = graph.edges[i][0];
        int v = graph.edges[i][1];
        ++degree[u];
        ++degree[v];
        ++outDegree[u];
        ++inDegree[v];
        vertexState[u] = 2;
        vertexState[v] = 2;
        vector<string> labels(vertexCount);
        for (int q = 0; q < vertexCount; ++q) {
            labels[q] = isDirected ? ("in " + S(inDegree[q]) + "/out " + S(outDegree[q])) : ("deg " + S(degree[q]));
        }
        steps.push_back(dsDiagramStep(1,
            isDirected ? "Edge " + S(u) + " -> " + S(v) + ": vertex " + S(u) + " gets one more OUT-degree, vertex " + S(v) + " one more IN-degree."
                       : "Edge " + S(u) + " - " + S(v) + ": each end gets +1 degree (the degree of a vertex = number of edges touching it).",
            {{"edge", S(u) + (isDirected ? " -> " : " - ") + S(v)}, {"degrees", dsFormatVertexList(degree, vertexCount)}},
            dsDrawGraph(graph, vertexState, labels, chosenEdges, "counting degrees"),
            {"e" + S((int)i), "v" + S(u), "v" + S(v)}));
    }
    int degreeSum = 0;
    for (size_t i = 0; i < degree.size(); ++i) degreeSum += degree[i];
    vector<int> component(vertexCount, -1);
    int componentCount = 0;
    vector<vector<int>> undirectedAdj(vertexCount);
    for (size_t i = 0; i < graph.edges.size(); ++i) {
        undirectedAdj[graph.edges[i][0]].push_back(graph.edges[i][1]);
        undirectedAdj[graph.edges[i][1]].push_back(graph.edges[i][0]);
    }
    for (int i = 0; i < vertexCount; ++i) {
        if (component[i] < 0) {
            vector<int> dfsStack;
            dfsStack.push_back(i);
            component[i] = componentCount;
            while (!dfsStack.empty()) {
                int u = dfsStack.back();
                dfsStack.pop_back();
                for (size_t j = 0; j < undirectedAdj[u].size(); ++j) {
                    int w = undirectedAdj[u][j];
                    if (component[w] < 0) {
                        component[w] = componentCount;
                        dfsStack.push_back(w);
                    }
                }
            }
            ++componentCount;
        }
    }
    vector<string> labels(vertexCount);
    for (int q = 0; q < vertexCount; ++q) {
        labels[q] = isDirected ? ("in " + S(inDegree[q]) + "/out " + S(outDegree[q])) : ("deg " + S(degree[q]));
    }
    steps.push_back(dsDiagramStep(2,
        isDirected ? "Sum of in-degrees = sum of out-degrees = E = " + S(graph.edges.size()) +
                         " (every arrow has one tail and one head). The graph is " +
                         (componentCount == 1 ? "connected (ignoring direction)." : "in " + S(componentCount) + " separate pieces.")
                   : "Handshake lemma: the degrees add up to " + S(degreeSum) + " = 2 x E = 2 x " + S(graph.edges.size()) +
                         ", because every edge is counted at both ends. The graph has " + S(componentCount) +
                         " connected component(s). A complete graph on " + S(vertexCount) + " vertices would have " +
                         S(vertexCount * (vertexCount - 1) / 2) + " edges.",
        {{"sum of degrees", S(degreeSum)}, {"components", S(componentCount)}},
        dsDrawGraph(graph, vertexState, labels, chosenEdges, "summary"), {}));
    return steps;
}

static Steps simGraphUndirected(const Ints& input) {
    return simGraphFundamentalsHelper(false, input);
}

static Steps simGraphDirected(const Ints& input) {
    return simGraphFundamentalsHelper(true, input);
}

static Steps simAdjMatrixBuild(const Ints& input) {
    GraphData graph = dsParseGraph(input, 0, false, false, dsDefaultGraphEdges());
    int vertexCount = graph.numVertices;
    vector<vector<int>> matrix(vertexCount, vector<int>(vertexCount, 0));
    vector<int> vertexState(vertexCount, 0);
    vector<char> chosenEdges(graph.edges.size(), 0);
    Steps steps;
    steps.push_back(dsMatrixStep(0,
        "An adjacency matrix is a V x V table. Cell [row i][column j] = 1 means there is an edge from i to j. All zeros to begin with. Space: V x V = " +
            S(vertexCount * vertexCount) + " cells.",
        {{"V", S(vertexCount)}}, graph, vertexState, chosenEdges, matrix, -1, {}, "empty matrix"));
    for (size_t i = 0; i < graph.edges.size(); ++i) {
        int u = graph.edges[i][0];
        int v = graph.edges[i][1];
        matrix[u][v] = 1;
        matrix[v][u] = 1;
        vertexState[u] = 2;
        vertexState[v] = 2;
        chosenEdges[i] = 1;
        steps.push_back(dsMatrixStep(2,
            "Edge " + S(u) + " - " + S(v) + ": set A[" + S(u) + "][" + S(v) + "] = 1 and A[" + S(v) + "][" + S(u) +
                "] = 1. The edge is stored in two mirror cells.",
            {{"edge", S(u) + " - " + S(v)}}, graph, vertexState, chosenEdges, matrix,
            (vertexCount + 1) * (u + 1) + v + 1, {"e" + S((int)i)}, "matrix is symmetric"));
    }
    steps.push_back(dsMatrixStep(3,
        "Done. Is there an edge between u and v? One lookup A[u][v]: O(1). Degree of a vertex = sum of its row: O(V). The price: V x V memory even if there are very few edges.",
        {{"edges", S(graph.edges.size())}, {"cells used", S(vertexCount * vertexCount)}},
        graph, vertexState, chosenEdges, matrix, -1, {}, "adjacency matrix"));
    return steps;
}

static Steps simAdjMatrixNeighbours(const Ints& input) {
    GraphData graph = dsParseGraph(input, 1, false, false, dsDefaultGraphEdges());
    int vertexCount = graph.numVertices;
    long long rawVertex = input.empty() ? 3 : (long long)input[0];
    int queryVertex = (int)(abs(rawVertex) % vertexCount);
    vector<vector<int>> matrix(vertexCount, vector<int>(vertexCount, 0));
    for (size_t i = 0; i < graph.edges.size(); ++i) {
        matrix[graph.edges[i][0]][graph.edges[i][1]] = 1;
        matrix[graph.edges[i][1]][graph.edges[i][0]] = 1;
    }
    vector<int> vertexState(vertexCount, 0);
    vector<char> chosenEdges(graph.edges.size(), 0);
    Steps steps;
    string neighbourList;
    vertexState[queryVertex] = 1;
    steps.push_back(dsMatrixStep(0,
        "Find all neighbours of vertex " + S(queryVertex) + ". We must scan the whole row " + S(queryVertex) +
            " of the matrix: V = " + S(vertexCount) + " cells, even if the vertex has only 1 neighbour.",
        {{"u", S(queryVertex)}}, graph, vertexState, chosenEdges, matrix, -1, {"v" + S(queryVertex)}, "scan row " + S(queryVertex)));
    for (int j = 0; j < vertexCount; ++j) {
        bool hasEdge = matrix[queryVertex][j] == 1;
        if (hasEdge) {
            if (!neighbourList.empty()) neighbourList += ", ";
            neighbourList += S(j);
            vertexState[j] = 2;
        }
        steps.push_back(dsMatrixStep(2,
            "A[" + S(queryVertex) + "][" + S(j) + "] = " + S(matrix[queryVertex][j]) +
                (hasEdge ? ": vertex " + S(j) + " is a neighbour." : ": no edge."),
            {{"j", S(j)}, {"neighbours", neighbourList.empty() ? "(none)" : neighbourList}},
            graph, vertexState, chosenEdges, matrix,
            (vertexCount + 1) * (queryVertex + 1) + j + 1, {"v" + S(j)}, "scanning the row"));
    }
    steps.push_back(dsMatrixStep(3,
        "Neighbours of " + S(queryVertex) + ": " + (neighbourList.empty() ? string("none") : neighbourList) +
            ". Scanning a row is always O(V); an adjacency LIST would only look at the real neighbours.",
        {{"neighbours", neighbourList}}, graph, vertexState, chosenEdges, matrix, -1, {}, "done"));
    return steps;
}

static Steps simAdjListBuild(const Ints& input) {
    GraphData graph = dsParseGraph(input, 0, false, false, dsDefaultGraphEdges());
    int vertexCount = graph.numVertices;
    vector<Ints> chains(vertexCount);
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "An adjacency list keeps, for every vertex, a list of ONLY its neighbours. Space: V + 2E, much smaller than V x V for sparse graphs.",
        {{"V", S(vertexCount)}, {"E", S(graph.edges.size())}},
        dsDrawHashChains(vertexCount, chains, "one list per vertex"), {}));
    for (size_t i = 0; i < graph.edges.size(); ++i) {
        int u = graph.edges[i][0];
        int v = graph.edges[i][1];
        chains[u].push_back(v);
        chains[v].push_back(u);
        steps.push_back(dsDiagramStep(2,
            "Edge " + S(u) + " - " + S(v) + ": add " + S(v) + " to the list of " + S(u) + ", and " + S(u) + " to the list of " + S(v) + ".",
            {{"edge", S(u) + " - " + S(v)}},
            dsDrawHashChains(vertexCount, chains, "add both directions"),
            {"c" + S(u) + "_" + S(chains[u].size() - 1), "c" + S(v) + "_" + S(chains[v].size() - 1)}));
    }
    int totalNodes = 0;
    for (size_t i = 0; i < chains.size(); ++i) totalNodes += (int)chains[i].size();
    steps.push_back(dsDiagramStep(3,
        "Total list nodes = " + S(totalNodes) + " = 2E. Matrix would need " + S(vertexCount * vertexCount) +
            " cells, list needs " + S(vertexCount) + " heads + " + S(totalNodes) +
            " nodes. Listing the neighbours of a vertex costs only its degree.",
        {{"list nodes", S(totalNodes)}, {"matrix cells", S(vertexCount * vertexCount)}},
        dsDrawHashChains(vertexCount, chains, "adjacency list"), {}));
    return steps;
}

static Steps simAdjListNeighbours(const Ints& input) {
    GraphData graph = dsParseGraph(input, 1, false, false, dsDefaultGraphEdges());
    int vertexCount = graph.numVertices;
    long long rawVertex = input.empty() ? 3 : (long long)input[0];
    int queryVertex = (int)(abs(rawVertex) % vertexCount);
    vector<Ints> chains(vertexCount);
    for (size_t i = 0; i < graph.edges.size(); ++i) {
        chains[graph.edges[i][0]].push_back(graph.edges[i][1]);
        chains[graph.edges[i][1]].push_back(graph.edges[i][0]);
    }
    for (size_t i = 0; i < chains.size(); ++i) {
        sort(chains[i].begin(), chains[i].end());
    }
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "Neighbours of vertex " + S(queryVertex) + ": walk only ITS list.",
        {{"u", S(queryVertex)}},
        dsDrawHashChains(vertexCount, chains, "start at list " + S(queryVertex)), {"b" + S(queryVertex)}));
    string outList;
    for (size_t j = 0; j < chains[queryVertex].size(); ++j) {
        if (!outList.empty()) outList += ", ";
        outList += S(chains[queryVertex][j]);
        steps.push_back(dsDiagramStep(2,
            "Node " + S(j) + " holds " + S(chains[queryVertex][j]) + ": neighbour found.",
            {{"neighbours", outList}},
            dsDrawHashChains(vertexCount, chains, "walking the list"), {"c" + S(queryVertex) + "_" + S((int)j)}));
    }
    steps.push_back(dsDiagramStep(3,
        "Vertex " + S(queryVertex) + " has " + S(chains[queryVertex].size()) + " neighbour(s): " +
            (outList.empty() ? string("none") : outList) + ". Cost = degree(" + S(queryVertex) + ") = " +
            S(chains[queryVertex].size()) + ", not V. Checking one specific edge (u, v) however needs a list scan, O(degree), where a matrix needs O(1).",
        {{"degree", S(chains[queryVertex].size())}},
        dsDrawHashChains(vertexCount, chains, "done"), {}));
    return steps;
}

static void dsCompareDfsHelper(int vertex, const GraphData& graph, const vector<vector<pair<int, int>>>& adj,
                                 const vector<char>& chosenEdges, vector<char>& seen, vector<int>& vertexState,
                                 vector<string>& vertexLabels, int& visitCount, string& orderText, Steps& steps) {
    seen[vertex] = 1;
    vertexState[vertex] = 2;
    ++visitCount;
    vertexLabels[vertex] = S(visitCount);
    if (!orderText.empty()) orderText += ", ";
    orderText += S(vertex);
    steps.push_back(dsGraphStep(2, "DFS visits vertex " + S(vertex) + " and immediately goes deeper.",
        {{"DFS order", orderText}}, graph, vertexState, vertexLabels, chosenEdges, "DFS: deepest first", {"v" + S(vertex)}));
    for (size_t i = 0; i < adj[vertex].size(); ++i) {
        int nextVertex = adj[vertex][i].first;
        if (!seen[nextVertex]) {
            dsCompareDfsHelper(nextVertex, graph, adj, chosenEdges, seen, vertexState, vertexLabels, visitCount, orderText, steps);
        }
    }
}

static Steps simGraphTraversalCompare(const Ints& input) {
    GraphData graph = dsParseGraph(input, 1, false, false, dsDefaultGraphEdges());
    int vertexCount = graph.numVertices;
    int startVertex = dsGraphStartVertex(input, vertexCount);
    vector<vector<pair<int, int>>> adj = dsGraphAdj(graph);
    Steps steps;
    vector<char> chosenEdges(graph.edges.size(), 0);
    {
        vector<int> vertexState(vertexCount, 0);
        steps.push_back(dsGraphStep(0,
            "Two ways to visit every vertex from vertex " + S(startVertex) +
                ". BFS first (queue), then DFS (stack). The number on a vertex = the order in which it is visited.",
            {{"start", S(startVertex)}}, graph, vertexState, {}, chosenEdges, "graph", {"v" + S(startVertex)}));
    }
    for (int mode = 0; mode < 2; ++mode) {
        vector<int> vertexState(vertexCount, 0);
        vector<string> vertexLabels(vertexCount);
        vector<char> seen(vertexCount, 0);
        string orderText;
        int visitCount = 0;
        if (mode == 0) {
            deque<int> bfsQueue;
            bfsQueue.push_back(startVertex);
            seen[startVertex] = 1;
            while (!bfsQueue.empty()) {
                int u = bfsQueue.front();
                bfsQueue.pop_front();
                vertexState[u] = 2;
                ++visitCount;
                vertexLabels[u] = S(visitCount);
                if (!orderText.empty()) orderText += ", ";
                orderText += S(u);
                for (size_t i = 0; i < adj[u].size(); ++i) {
                    int w = adj[u][i].first;
                    if (!seen[w]) {
                        seen[w] = 1;
                        bfsQueue.push_back(w);
                    }
                }
                steps.push_back(dsGraphStep(1, "BFS visits vertex " + S(u) + " (queue: front first).",
                    {{"BFS order", orderText}}, graph, vertexState, vertexLabels, chosenEdges, "BFS: nearest first", {"v" + S(u)}));
            }
        } else {
            dsCompareDfsHelper(startVertex, graph, adj, chosenEdges, seen, vertexState, vertexLabels, visitCount, orderText, steps);
        }
    }
    steps.push_back(dsGraphStep(3,
        "Both visit the same vertices, only in a different ORDER. BFS finds the fewest-edges distance; DFS is natural for paths, cycles and components. Both cost O(V + E) with an adjacency list.",
        {}, graph, vector<int>(vertexCount, 0), {}, chosenEdges, "same vertices, different order", {}));
    return steps;
}

static void dsDfsPushWithStack(Steps& steps, Step step, const vector<string>& callStack) {
    step.stack = callStack;
    steps.push_back(step);
}

static void dsDfsRecursiveHelper(int u, const GraphData& graph, const vector<vector<pair<int, int>>>& adj,
                                 vector<int>& vertexState, vector<char>& chosenEdges, vector<string>& vertexLabels,
                                 vector<string>& callStack, string& orderText, int& visitCount, Steps& steps) {
    vertexState[u] = 1;
    ++visitCount;
    vertexLabels[u] = S(visitCount);
    if (!orderText.empty()) orderText += ", ";
    orderText += S(u);
    callStack.push_back("dfs(" + S(u) + ")");
    dsDfsPushWithStack(steps, dsGraphStep(1, "Visit " + S(u) + " (discovered #" + S(visitCount) + ") and mark it visited.",
        {{"order", orderText}}, graph, vertexState, vertexLabels, chosenEdges, "visit", {"v" + S(u)}), callStack);
    for (size_t i = 0; i < adj[u].size(); ++i) {
        int w = adj[u][i].first;
        int edgeIdx = adj[u][i].second;
        if (vertexState[w] == 0) {
            chosenEdges[edgeIdx] = 1;
            dsDfsPushWithStack(steps, dsGraphStep(3, "Neighbour " + S(w) + " is unvisited: go deeper.",
                {{"u", S(u)}, {"w", S(w)}}, graph, vertexState, vertexLabels, chosenEdges, "tree edge", {"e" + S(edgeIdx)}), callStack);
            dsDfsRecursiveHelper(w, graph, adj, vertexState, chosenEdges, vertexLabels, callStack, orderText, visitCount, steps);
            dsDfsPushWithStack(steps, dsGraphStep(3, "Back at " + S(u) + " after finishing " + S(w) + " (backtrack).",
                {{"u", S(u)}}, graph, vertexState, vertexLabels, chosenEdges, "backtrack", {"v" + S(u)}), callStack);
        } else {
            dsDfsPushWithStack(steps, dsGraphStep(3, "Neighbour " + S(w) + " was already visited: skip it (this is what stops cycles from looping).",
                {{"u", S(u)}, {"w", S(w)}}, graph, vertexState, vertexLabels, chosenEdges, "already visited", {"e" + S(edgeIdx)}), callStack);
        }
    }
    vertexState[u] = 2;
    callStack.pop_back();
}

static Steps simDfsRecursive(const Ints& input) {
    GraphData graph = dsParseGraph(input, 1, false, false, dsDefaultGraphEdges());
    int vertexCount = graph.numVertices;
    int startVertex = dsGraphStartVertex(input, vertexCount);
    vector<vector<pair<int, int>>> adj = dsGraphAdj(graph);
    vector<int> vertexState(vertexCount, 0);
    vector<char> chosenEdges(graph.edges.size(), 0);
    vector<string> vertexLabels(vertexCount);
    Steps steps;
    vector<string> callStack;
    string orderText;
    int visitCount = 0;
    steps.push_back(dsGraphStep(0,
        "Depth first search from vertex " + S(startVertex) +
            ": follow one path as far as it goes, then BACKTRACK. The call stack remembers the way back. Blue = on the current path, orange = finished.",
        {{"start", S(startVertex)}}, graph, vertexState, vertexLabels, chosenEdges, "start", {"v" + S(startVertex)}));
    dsDfsRecursiveHelper(startVertex, graph, adj, vertexState, chosenEdges, vertexLabels, callStack, orderText, visitCount, steps);
    int unvisitedCount = 0;
    for (int i = 0; i < vertexCount; ++i) {
        if (vertexState[i] == 0) ++unvisitedCount;
    }
    steps.push_back(dsGraphStep(4,
        "Finished. Visit order: " + orderText +
            (unvisitedCount ? ". " + S(unvisitedCount) + " vertex(es) are not reachable from " + S(startVertex) + " (another component)." : ".") +
            " O(V + E) time, O(V) stack depth in the worst case.",
        {{"order", orderText}}, graph, vertexState, vertexLabels, chosenEdges, "DFS done", {}));
    return steps;
}

static Step dsDfsIterSnap(int lineNum, const string& noteText, Vars varsList, const GraphData& graph,
                          const vector<int>& vertexState, const vector<string>& vertexLabels,
                          const vector<char>& chosenEdges, const vector<int>& explicitStack,
                          const string& caption, vector<string> highlightIds) {
    vector<string> tags = explicitStack.empty()
                              ? vector<string>()
                              : dsMakeTags(explicitStack.size(), {{(int)explicitStack.size() - 1, "top"}});
    Step step = dsArrayStep(lineNum, noteText, varsList, explicitStack, -1, tags, -1, -1, "stack");
    step.dg = dsDrawGraph(graph, vertexState, vertexLabels, chosenEdges, caption);
    step.hl = highlightIds;
    return step;
}

static Steps simDfsIterative(const Ints& input) {
    GraphData graph = dsParseGraph(input, 1, false, false, dsDefaultGraphEdges());
    int vertexCount = graph.numVertices;
    int startVertex = dsGraphStartVertex(input, vertexCount);
    vector<vector<pair<int, int>>> adj = dsGraphAdj(graph);
    vector<int> vertexState(vertexCount, 0);
    vector<int> explicitStack;
    explicitStack.push_back(startVertex);
    vector<string> vertexLabels(vertexCount);
    vector<char> chosenEdges(graph.edges.size(), 0);
    Steps steps;
    string orderText;
    int visitCount = 0;
    steps.push_back(dsDfsIterSnap(0,
        "Recursion hides a stack. Here the stack is explicit: push the start vertex " + S(startVertex) + ".",
        {{"start", S(startVertex)}}, graph, vertexState, vertexLabels, chosenEdges, explicitStack, "start", {"v" + S(startVertex)}));
    int guard = 0;
    while (!explicitStack.empty() && guard++ < 80) {
        int u = explicitStack.back();
        explicitStack.pop_back();
        if (vertexState[u] == 2) {
            steps.push_back(dsDfsIterSnap(3, "Pop " + S(u) + ": already visited, skip it.",
                {{"popped", S(u)}}, graph, vertexState, vertexLabels, chosenEdges, explicitStack, "skip", {"v" + S(u)}));
            continue;
        }
        vertexState[u] = 2;
        ++visitCount;
        vertexLabels[u] = S(visitCount);
        if (!orderText.empty()) orderText += ", ";
        orderText += S(u);
        steps.push_back(dsDfsIterSnap(3, "Pop " + S(u) + " from the TOP of the stack and visit it.",
            {{"order", orderText}}, graph, vertexState, vertexLabels, chosenEdges, explicitStack, "visit", {"v" + S(u)}));
        string pushedList;
        for (int i = (int)adj[u].size() - 1; i >= 0; --i) {
            int w = adj[u][i].first;
            if (vertexState[w] != 2) {
                explicitStack.push_back(w);
                if (!pushedList.empty()) pushedList += ", ";
                pushedList += S(w);
            }
        }
        if (!pushedList.empty()) {
            steps.push_back(dsDfsIterSnap(4, "Push the unvisited neighbours (" + pushedList + ") so the smallest ends up on top.",
                {{"order", orderText}}, graph, vertexState, vertexLabels, chosenEdges, explicitStack, "push neighbours", {"v" + S(u)}));
        }
    }
    steps.push_back(dsDfsIterSnap(5, "Stack empty. Visit order: " + orderText + ". The same order as the recursive version.",
        {{"order", orderText}}, graph, vertexState, vertexLabels, chosenEdges, explicitStack, "done", {}));
    return steps;
}

static Step dsBfsQueueSnap(int lineNum, const string& noteText, Vars varsList, const GraphData& graph,
                           const vector<int>& vertexState, const vector<string>& vertexLabels,
                           const vector<char>& chosenEdges, const deque<int>& bfsQueue,
                           const string& caption, vector<string> highlightIds) {
    Ints queueArray(bfsQueue.begin(), bfsQueue.end());
    Step step = dsArrayStep(lineNum, noteText, varsList, queueArray, -1, {}, -1, -1, "queue");
    step.dg = dsDrawGraph(graph, vertexState, vertexLabels, chosenEdges, caption);
    step.hl = highlightIds;
    return step;
}

static Steps simBfsTraversal(const Ints& input) {
    GraphData graph = dsParseGraph(input, 1, false, false, dsDefaultGraphEdges());
    int vertexCount = graph.numVertices;
    int startVertex = dsGraphStartVertex(input, vertexCount);
    vector<vector<pair<int, int>>> adj = dsGraphAdj(graph);
    vector<int> vertexState(vertexCount, 0);
    vector<int> distArr(vertexCount, -1);
    vector<string> vertexLabels(vertexCount);
    vector<char> chosenEdges(graph.edges.size(), 0);
    deque<int> bfsQueue;
    bfsQueue.push_back(startVertex);
    distArr[startVertex] = 0;
    vertexLabels[startVertex] = "d=0";
    vertexState[startVertex] = 1;
    Steps steps;
    string orderText;
    steps.push_back(dsBfsQueueSnap(0,
        "Breadth first search: visit the start, then all its neighbours, then THEIR neighbours: ring by ring. A QUEUE keeps the order. d = distance in edges from vertex " +
            S(startVertex) + ".",
        {{"start", S(startVertex)}}, graph, vertexState, vertexLabels, chosenEdges, bfsQueue, "start", {"v" + S(startVertex)}));
    while (!bfsQueue.empty()) {
        int u = bfsQueue.front();
        bfsQueue.pop_front();
        vertexState[u] = 2;
        if (!orderText.empty()) orderText += ", ";
        orderText += S(u);
        string addedText;
        for (size_t i = 0; i < adj[u].size(); ++i) {
            int w = adj[u][i].first;
            int edgeIdx = adj[u][i].second;
            if (distArr[w] < 0) {
                distArr[w] = distArr[u] + 1;
                vertexLabels[w] = "d=" + S(distArr[w]);
                vertexState[w] = 1;
                bfsQueue.push_back(w);
                chosenEdges[edgeIdx] = 1;
                if (!addedText.empty()) addedText += ", ";
                addedText += S(w);
            }
        }
        steps.push_back(dsBfsQueueSnap(4,
            "Dequeue " + S(u) + " (distance " + S(distArr[u]) + "). " +
                (addedText.empty() ? "No unvisited neighbours." : "Enqueue its unvisited neighbours " + addedText + " with distance " + S(distArr[u] + 1) + "."),
            {{"order", orderText}}, graph, vertexState, vertexLabels, chosenEdges, bfsQueue, "orange = done, blue = waiting in the queue", {"v" + S(u)}));
    }
    steps.push_back(dsBfsQueueSnap(5,
        "Queue empty. Visit order " + orderText + ". Because vertices come out in order of distance, d is the SHORTEST number of edges from " +
            S(startVertex) + ". O(V + E).",
        {{"order", orderText}}, graph, vertexState, vertexLabels, chosenEdges, bfsQueue, "BFS done", {}));
    return steps;
}

static Steps simBfsShortestPath(const Ints& input) {
    GraphData graph = dsParseGraph(input, 2, false, false, dsDefaultGraphEdges());
    int vertexCount = graph.numVertices;
    long long rawSource = input.empty() ? 0 : (long long)input[0];
    long long rawTarget = input.size() > 1 ? (long long)input[1] : 5;
    int sourceVertex = (int)(abs(rawSource) % vertexCount);
    int targetVertex = (int)(abs(rawTarget) % vertexCount);
    vector<vector<pair<int, int>>> adj = dsGraphAdj(graph);
    vector<int> vertexState(vertexCount, 0);
    vector<int> parentVertex(vertexCount, -1);
    vector<int> parentEdge(vertexCount, -1);
    vector<char> seen(vertexCount, 0);
    vector<char> chosenEdges(graph.edges.size(), 0);
    vector<string> vertexLabels(vertexCount);
    deque<int> bfsQueue;
    bfsQueue.push_back(sourceVertex);
    seen[sourceVertex] = 1;
    vertexState[sourceVertex] = 1;
    Steps steps;
    steps.push_back(dsGraphStep(0,
        "Shortest path (fewest edges) from " + S(sourceVertex) + " to " + S(targetVertex) +
            " in an unweighted graph: run BFS and remember who discovered each vertex (its parent).",
        {{"source", S(sourceVertex)}, {"target", S(targetVertex)}},
        graph, vertexState, vertexLabels, chosenEdges, "start", {"v" + S(sourceVertex), "v" + S(targetVertex)}));
    bool found = (sourceVertex == targetVertex);
    while (!bfsQueue.empty() && !found) {
        int u = bfsQueue.front();
        bfsQueue.pop_front();
        vertexState[u] = 2;
        for (size_t i = 0; i < adj[u].size(); ++i) {
            int w = adj[u][i].first;
            int edgeIdx = adj[u][i].second;
            if (!seen[w]) {
                seen[w] = 1;
                parentVertex[w] = u;
                parentEdge[w] = edgeIdx;
                vertexLabels[w] = "p=" + S(u);
                vertexState[w] = 1;
                bfsQueue.push_back(w);
                steps.push_back(dsGraphStep(0,
                    "From " + S(u) + " discover " + S(w) + ": parent[" + S(w) + "] = " + S(u) + ".",
                    {{"parent[" + S(w) + "]", S(u)}}, graph, vertexState, vertexLabels, chosenEdges, "discovering", {"e" + S(edgeIdx), "v" + S(w)}));
                if (w == targetVertex) {
                    found = true;
                    break;
                }
            }
        }
    }
    if (!found) {
        steps.push_back(dsGraphStep(3, "BFS ended without reaching " + S(targetVertex) + ": there is NO path.",
            {}, graph, vertexState, vertexLabels, chosenEdges, "no path", {}));
        return steps;
    }
    vector<int> path;
    for (int x = targetVertex; x >= 0; x = parentVertex[x]) {
        path.push_back(x);
        if (path.size() > 20) break;
    }
    reverse(path.begin(), path.end());
    string pathText;
    for (size_t i = 0; i < path.size(); ++i) {
        if (!pathText.empty()) pathText += " -> ";
        pathText += S(path[i]);
        if (i + 1 < path.size()) chosenEdges[parentEdge[path[i + 1]]] = 1;
        vertexState[path[i]] = 2;
    }
    steps.push_back(dsGraphStep(2,
        "Target reached. Walk back through the parents from " + S(targetVertex) + " to " + S(sourceVertex) +
            " and reverse: " + pathText + " (" + S(path.size() - 1) + " edge(s)). BFS guarantees no path has fewer edges.",
        {{"path", pathText}}, graph, vertexState, vertexLabels, chosenEdges, "shortest path", {}));
    return steps;
}

static vector<string> dsBuildDistLabels(const vector<int>& distArr, int vertexCount, int infVal) {
    vector<string> labels(vertexCount);
    for (int i = 0; i < vertexCount; ++i) {
        labels[i] = (distArr[i] == infVal) ? "inf" : S(distArr[i]);
    }
    return labels;
}

static Step dsDistArraySnap(int lineNum, const string& noteText, Vars varsList, const GraphData& graph,
                            const vector<int>& distArr, int infVal, const vector<int>& vertexState,
                            const vector<char>& chosenEdges, const string& caption, vector<string> highlightIds) {
    int vertexCount = graph.numVertices;
    Step step = dsArrayStep(lineNum, noteText, varsList, distArr, -1, {}, -1, -1, "dist");
    vector<string> tags(vertexCount);
    for (int i = 0; i < vertexCount; ++i) tags[i] = "v" + S(i);
    step.tags = tags;
    step.dg = dsDrawGraph(graph, vertexState, dsBuildDistLabels(distArr, vertexCount, infVal), chosenEdges, caption);
    step.hl = highlightIds;
    return step;
}

static Steps simDijkstra(const Ints& input) {
    Ints defaultWeighted = {0, 1, 7, 0, 2, 9, 0, 5, 14, 1, 2, 10, 1, 3, 15, 2, 3, 11, 2, 5, 2, 3, 4, 6, 4, 5, 9};
    GraphData graph = dsParseGraph(input, 1, true, false, defaultWeighted, 1, 99, 7);
    int vertexCount = graph.numVertices;
    int sourceVertex = dsGraphStartVertex(input, vertexCount);
    const int INF = INT_MIN;
    vector<int> distArr(vertexCount, INF);
    vector<int> vertexState(vertexCount, 0);
    vector<char> done(vertexCount, 0);
    vector<char> chosenEdges(graph.edges.size(), 0);
    distArr[sourceVertex] = 0;
    vector<int> prevEdge(vertexCount, -1);
    Steps steps;
    steps.push_back(dsDistArraySnap(0,
        "Dijkstra (non-negative weights): dist[v] = best known distance from vertex " + S(sourceVertex) +
            ". '?' means infinity (not reached yet). Always settle the CLOSEST unsettled vertex next.",
        {{"source", S(sourceVertex)}}, graph, distArr, INF, vertexState, chosenEdges, "start", {"v" + S(sourceVertex)}));
    for (int iter = 0; iter < vertexCount; ++iter) {
        int u = -1;
        for (int i = 0; i < vertexCount; ++i) {
            if (!done[i] && distArr[i] != INF && (u < 0 || distArr[i] < distArr[u])) u = i;
        }
        if (u < 0) break;
        done[u] = 1;
        vertexState[u] = 2;
        if (prevEdge[u] >= 0) chosenEdges[prevEdge[u]] = 1;
        steps.push_back(dsDistArraySnap(2,
            "Pick the unsettled vertex with the smallest dist: " + S(u) + " (distance " + S(distArr[u]) +
                "). Its distance is now FINAL (no cheaper route can appear later).",
            {{"u", S(u)}, {"dist[u]", S(distArr[u])}}, graph, distArr, INF, vertexState, chosenEdges, "settle " + S(u), {"v" + S(u)}));
        for (size_t i = 0; i < graph.edges.size(); ++i) {
            int a = graph.edges[i][0];
            int b = graph.edges[i][1];
            int w = graph.edges[i][2];
            int target = (a == u) ? b : ((b == u) ? a : -1);
            if (target < 0 || done[target]) continue;
            int newDist = distArr[u] + w;
            if (distArr[target] == INF || newDist < distArr[target]) {
                string oldText = (distArr[target] == INF) ? "inf" : S(distArr[target]);
                distArr[target] = newDist;
                prevEdge[target] = (int)i;
                steps.push_back(dsDistArraySnap(4,
                    "Relax edge " + S(u) + "-" + S(target) + " (weight " + S(w) + "): " + S(distArr[u]) + " + " + S(w) +
                        " = " + S(newDist) + " < " + oldText + ". Update dist[" + S(target) + "] = " + S(newDist) + ".",
                    {{"dist[" + S(target) + "]", S(newDist)}}, graph, distArr, INF, vertexState, chosenEdges,
                    "relax", {"e" + S((int)i), "v" + S(target)}));
            } else {
                steps.push_back(dsDistArraySnap(4,
                    "Edge " + S(u) + "-" + S(target) + " (weight " + S(w) + "): " + S(newDist) +
                        " is not better than " + S(distArr[target]) + ". No change.",
                    {}, graph, distArr, INF, vertexState, chosenEdges, "no improvement", {"e" + S((int)i)}));
            }
        }
    }
    steps.push_back(dsDistArraySnap(1,
        "Done. dist[] = shortest distance from " + S(sourceVertex) +
            " to every vertex (highlighted edges form the shortest-path tree). With a binary heap: O((V + E) log V).",
        {{"dist", dsFormatVertexList(distArr, vertexCount)}}, graph, distArr, INF, vertexState, chosenEdges, "shortest-path tree", {}));
    return steps;
}

static Steps simBellmanFord(const Ints& input) {
    Ints defaultEdges = {0, 1, 6, 0, 2, 7, 1, 2, 8, 1, 3, 5, 1, 4, -4, 2, 3, -3, 2, 4, 9, 3, 1, -2, 4, 3, 7, 4, 0, 2};
    GraphData graph = dsParseGraph(input, 1, true, true, defaultEdges, -20, 99, 6);
    int vertexCount = graph.numVertices;
    int sourceVertex = dsGraphStartVertex(input, vertexCount);
    const int INF = INT_MIN;
    vector<int> distArr(vertexCount, INF);
    vector<int> vertexState(vertexCount, 0);
    vector<char> chosenEdges(graph.edges.size(), 0);
    distArr[sourceVertex] = 0;
    vertexState[sourceVertex] = 1;
    Steps steps;
    steps.push_back(dsDistArraySnap(0,
        "Bellman-Ford handles NEGATIVE edge weights (Dijkstra cannot). Idea: relax EVERY edge, V - 1 = " + S(vertexCount - 1) +
            " times. After round k, all shortest paths using at most k edges are correct.",
        {{"source", S(sourceVertex)}}, graph, distArr, INF, vertexState, chosenEdges, "start", {"v" + S(sourceVertex)}));
    for (int round = 1; round < vertexCount; ++round) {
        bool anyChange = false;
        for (size_t i = 0; i < graph.edges.size(); ++i) {
            int u = graph.edges[i][0];
            int v = graph.edges[i][1];
            int w = graph.edges[i][2];
            if (distArr[u] == INF) continue;
            int newDist = distArr[u] + w;
            if (distArr[v] == INF || newDist < distArr[v]) {
                distArr[v] = newDist;
                anyChange = true;
                vertexState[v] = 2;
                steps.push_back(dsDistArraySnap(3,
                    "Round " + S(round) + ": edge " + S(u) + " -> " + S(v) + " (weight " + S(w) + ") gives " + S(newDist) +
                        ". Update dist[" + S(v) + "].",
                    {{"round", S(round)}, {"dist[" + S(v) + "]", S(newDist)}},
                    graph, distArr, INF, vertexState, chosenEdges, "relax in round " + S(round), {"e" + S((int)i), "v" + S(v)}));
            }
        }
        steps.push_back(dsDistArraySnap(1,
            "End of round " + S(round) + ". " + (anyChange ? "Some distances changed." : "Nothing changed: we can stop early."),
            {{"dist", dsFormatVertexList(distArr, vertexCount)}},
            graph, distArr, INF, vertexState, chosenEdges, "end of round " + S(round), {}));
        if (!anyChange) break;
    }
    bool hasNegativeCycle = false;
    for (size_t i = 0; i < graph.edges.size(); ++i) {
        int u = graph.edges[i][0];
        int v = graph.edges[i][1];
        int w = graph.edges[i][2];
        if (distArr[u] != INF && (distArr[v] == INF || distArr[u] + w < distArr[v])) {
            hasNegativeCycle = true;
            break;
        }
    }
    steps.push_back(dsDistArraySnap(4,
        hasNegativeCycle ? "Extra check: an edge can STILL be relaxed, so there is a negative cycle: shortest paths are not defined."
                         : "Extra check: no edge can still be relaxed, so there is no negative cycle and dist[] is final. Cost O(V x E): slower than Dijkstra but more general.",
        {{"negative cycle", hasNegativeCycle ? "yes" : "no"}},
        graph, distArr, INF, vertexState, chosenEdges, hasNegativeCycle ? "negative cycle" : "done", {}));
    return steps;
}

static vector<string> dsBuildFloydMatrixCells(int vertexCount, const vector<vector<int>>& distMatrix, int infVal) {
    vector<string> cells;
    cells.push_back("");
    for (int j = 0; j < vertexCount; ++j) cells.push_back(S(j));
    for (int i = 0; i < vertexCount; ++i) {
        cells.push_back(S(i));
        for (int j = 0; j < vertexCount; ++j) {
            cells.push_back(distMatrix[i][j] >= infVal ? "inf" : S(distMatrix[i][j]));
        }
    }
    return cells;
}

static Step dsFloydSnap(int lineNum, const string& noteText, Vars varsList, const GraphData& graph,
                        const vector<int>& vertexState, const vector<char>& chosenEdges,
                        const vector<vector<int>>& distMatrix, int infVal, int hiCell, const string& caption) {
    Step step = dsDiagramStep(lineNum, noteText, varsList, dsDrawGraph(graph, vertexState, {}, chosenEdges, caption), {});
    step.mat = dsBuildFloydMatrixCells(graph.numVertices, distMatrix, infVal);
    step.mc = graph.numVertices + 1;
    step.hi = hiCell;
    return step;
}

static Steps simFloydWarshall(const Ints& input) {
    Ints defaultEdges = {0, 1, 3, 0, 3, 7, 1, 0, 8, 1, 2, 2, 2, 0, 5, 2, 3, 1, 3, 0, 2};
    GraphData graph = dsParseGraph(input, 0, true, true, defaultEdges, -9, 99, 5);
    int vertexCount = graph.numVertices;
    const int INF = 1000000;
    vector<vector<int>> distMatrix(vertexCount, vector<int>(vertexCount, INF));
    for (int i = 0; i < vertexCount; ++i) distMatrix[i][i] = 0;
    for (size_t i = 0; i < graph.edges.size(); ++i) {
        int u = graph.edges[i][0];
        int v = graph.edges[i][1];
        distMatrix[u][v] = min(distMatrix[u][v], graph.edges[i][2]);
    }
    Steps steps;
    vector<int> vertexState(vertexCount, 0);
    vector<char> chosenEdges(graph.edges.size(), 0);
    steps.push_back(dsFloydSnap(0,
        "Floyd-Warshall finds the shortest distance between EVERY pair of vertices. Row i, column j = best known distance i -> j (inf = no route yet).",
        {{"V", S(vertexCount)}}, graph, vertexState, chosenEdges, distMatrix, INF, -1, "initial distances"));
    for (int k = 0; k < vertexCount; ++k) {
        int updates = 0;
        int lastCell = -1;
        string changesText;
        for (int i = 0; i < vertexCount; ++i) {
            for (int j = 0; j < vertexCount; ++j) {
                if (distMatrix[i][k] < INF && distMatrix[k][j] < INF && distMatrix[i][k] + distMatrix[k][j] < distMatrix[i][j]) {
                    if (!changesText.empty()) changesText += "; ";
                    string oldVal = (distMatrix[i][j] >= INF) ? "inf" : S(distMatrix[i][j]);
                    int newVal = distMatrix[i][k] + distMatrix[k][j];
                    changesText += S(i) + "->" + S(j) + ": " + oldVal + " becomes " + S(newVal);
                    distMatrix[i][j] = newVal;
                    ++updates;
                    lastCell = (vertexCount + 1) * (i + 1) + j + 1;
                }
            }
        }
        vertexState.assign(vertexCount, 0);
        vertexState[k] = 1;
        steps.push_back(dsFloydSnap(1,
            "Allow vertex " + S(k) + " as a stop-over (blue). " +
                (updates ? S(updates) + " pair(s) improve: " + changesText + "." : "No pair gets a shorter route through it."),
            {{"k", S(k)}, {"updates", S(updates)}}, graph, vertexState, chosenEdges, distMatrix, INF, lastCell,
            "intermediate vertex k = " + S(k)));
    }
    vertexState.assign(vertexCount, 2);
    steps.push_back(dsFloydSnap(4,
        "All " + S(vertexCount) + " rounds done: the table holds all-pairs shortest distances. Time O(V^3), space O(V^2). A negative number on the diagonal would reveal a negative cycle.",
        {}, graph, vertexState, chosenEdges, distMatrix, INF, -1, "all-pairs shortest paths"));
    return steps;
}

static int dsDsuFindRoot(vector<int>& parent, int x) {
    if (parent[x] == x) return x;
    parent[x] = dsDsuFindRoot(parent, parent[x]);
    return parent[x];
}

static vector<string> dsBuildDsuLabels(vector<int>& parent, int vertexCount) {
    vector<string> labels(vertexCount);
    for (int i = 0; i < vertexCount; ++i) {
        labels[i] = "set " + S(dsDsuFindRoot(parent, i));
    }
    return labels;
}

static Steps simMstKruskal(const Ints& input) {
    Ints defaultEdges = {0, 1, 4, 0, 2, 3, 1, 2, 1, 1, 3, 2, 2, 3, 4, 3, 4, 2, 4, 5, 6};
    GraphData graph = dsParseGraph(input, 0, true, false, defaultEdges, 1, 99, 7);
    int vertexCount = graph.numVertices;
    vector<int> edgeOrder(graph.edges.size());
    for (size_t i = 0; i < edgeOrder.size(); ++i) edgeOrder[i] = (int)i;
    // Insertion sort preserves tie order like std::stable_sort.
    for (size_t i = 1; i < edgeOrder.size(); ++i) {
        int keyIdx = edgeOrder[i];
        int keyWeight = graph.edges[keyIdx][2];
        int j = (int)i - 1;
        while (j >= 0 && graph.edges[edgeOrder[j]][2] > keyWeight) {
            edgeOrder[j + 1] = edgeOrder[j];
            --j;
        }
        edgeOrder[j + 1] = keyIdx;
    }
    vector<int> parent(vertexCount);
    for (int i = 0; i < vertexCount; ++i) parent[i] = i;
    vector<char> chosenEdges(graph.edges.size(), 0);
    vector<int> vertexState(vertexCount, 0);
    Steps steps;
    int totalWeight = 0;
    int takenCount = 0;
    steps.push_back(dsGraphStep(0,
        "A spanning tree connects all " + S(vertexCount) + " vertices with exactly V - 1 = " + S(vertexCount - 1) +
            " edges and no cycle. The MINIMUM one has the smallest total weight. Kruskal: look at the edges from cheapest to dearest. Labels show each vertex's component.",
        {{"edges sorted", S(graph.edges.size())}}, graph, vertexState, dsBuildDsuLabels(parent, vertexCount), chosenEdges, "start", {}));
    for (size_t k = 0; k < edgeOrder.size(); ++k) {
        int i = edgeOrder[k];
        int u = graph.edges[i][0];
        int v = graph.edges[i][1];
        int w = graph.edges[i][2];
        int rootU = dsDsuFindRoot(parent, u);
        int rootV = dsDsuFindRoot(parent, v);
        if (rootU != rootV) {
            parent[rootU] = rootV;
            chosenEdges[i] = 1;
            totalWeight += w;
            ++takenCount;
            vertexState[u] = 2;
            vertexState[v] = 2;
            steps.push_back(dsGraphStep(3,
                "Edge " + S(u) + "-" + S(v) + " (weight " + S(w) + "): the ends are in DIFFERENT components, so taking it cannot make a cycle. Take it and merge the components.",
                {{"edge", S(u) + "-" + S(v)}, {"total weight", S(totalWeight)}},
                graph, vertexState, dsBuildDsuLabels(parent, vertexCount), chosenEdges, "taken", {"e" + S(i)}));
        } else {
            steps.push_back(dsGraphStep(5,
                "Edge " + S(u) + "-" + S(v) + " (weight " + S(w) + "): both ends are already connected (same component). Taking it would form a CYCLE, so skip it.",
                {{"edge", S(u) + "-" + S(v)}, {"total weight", S(totalWeight)}},
                graph, vertexState, dsBuildDsuLabels(parent, vertexCount), chosenEdges, "rejected: cycle", {"e" + S(i)}));
        }
    }
    steps.push_back(dsGraphStep(1,
        takenCount == vertexCount - 1
            ? "Spanning tree complete: " + S(takenCount) + " edges, total weight " + S(totalWeight) + ". Time O(E log E) for the sort; union-find makes the cycle test almost O(1)."
            : "Only " + S(takenCount) + " edges could be taken: the graph is not connected, so this is a spanning FOREST.",
        {{"total weight", S(totalWeight)}},
        graph, vertexState, dsBuildDsuLabels(parent, vertexCount), chosenEdges, "minimum spanning tree", {}));
    return steps;
}

static Steps simMstPrim(const Ints& input) {
    Ints defaultEdges = {0, 1, 4, 0, 2, 3, 1, 2, 1, 1, 3, 2, 2, 3, 4, 3, 4, 2, 4, 5, 6};
    GraphData graph = dsParseGraph(input, 1, true, false, defaultEdges, 1, 99, 7);
    int vertexCount = graph.numVertices;
    int startVertex = dsGraphStartVertex(input, vertexCount);
    vector<int> vertexState(vertexCount, 0);
    vector<char> inTree(vertexCount, 0);
    vector<char> chosenEdges(graph.edges.size(), 0);
    inTree[startVertex] = 1;
    vertexState[startVertex] = 2;
    Steps steps;
    int totalWeight = 0;
    int takenCount = 0;
    vector<string> labels(vertexCount);
    steps.push_back(dsGraphStep(0,
        "Prim grows ONE tree from vertex " + S(startVertex) + ": repeatedly add the cheapest edge that reaches a new vertex. (Orange = in the tree.)",
        {{"start", S(startVertex)}}, graph, vertexState, labels, chosenEdges, "start", {"v" + S(startVertex)}));
    while (takenCount < vertexCount - 1) {
        int bestEdge = -1;
        string candidates;
        for (size_t i = 0; i < graph.edges.size(); ++i) {
            int u = graph.edges[i][0];
            int v = graph.edges[i][1];
            if (inTree[u] != inTree[v]) {
                if (!candidates.empty()) candidates += ", ";
                candidates += S(u) + "-" + S(v) + "(" + S(graph.edges[i][2]) + ")";
                if (bestEdge < 0 || graph.edges[i][2] < graph.edges[bestEdge][2]) bestEdge = (int)i;
            }
        }
        if (bestEdge < 0) break;
        int u = graph.edges[bestEdge][0];
        int v = graph.edges[bestEdge][1];
        int newVertex = inTree[u] ? v : u;
        inTree[newVertex] = 1;
        vertexState[newVertex] = 2;
        chosenEdges[bestEdge] = 1;
        totalWeight += graph.edges[bestEdge][2];
        ++takenCount;
        steps.push_back(dsGraphStep(2,
            "Edges leaving the tree: " + candidates + ". The cheapest is " + S(u) + "-" + S(v) + " (weight " +
                S(graph.edges[bestEdge][2]) + "): add vertex " + S(newVertex) + ".",
            {{"added edge", S(u) + "-" + S(v)}, {"total weight", S(totalWeight)}},
            graph, vertexState, labels, chosenEdges, "cheapest crossing edge", {"e" + S(bestEdge), "v" + S(newVertex)}));
    }
    steps.push_back(dsGraphStep(1,
        "Done: " + S(takenCount) + " edge(s), total weight " + S(totalWeight) +
            (takenCount == vertexCount - 1
                 ? ". Same total as Kruskal (the MST weight is unique even if several MSTs exist). With a heap: O(E log V)."
                 : ". Some vertices are unreachable."),
        {{"total weight", S(totalWeight)}}, graph, vertexState, labels, chosenEdges, "minimum spanning tree", {}));
    return steps;
}

void addDS6b(map<string, Sim>& m);
void addDS7(map<string, Sim>& m);

void addDS6(map<string, Sim>& m) {
    addDS6b(m);
    addDS7(m);
    const char* pairEdgesPrompt = "Edges as pairs: u v u v ... (vertices 0 to 7)";

    dsRegisterIntSim(m, "Fundamentals of Graph", pairEdgesPrompt, "0, 1, 0, 2, 1, 3, 2, 3, 3, 4, 4, 5",
                     {"G = (V, E)", "for each edge (u, v):", "    degree[u]++;  degree[v]++;", "sum of degrees = 2 * |E|"},
                     dsGraphRefs(),
                     {"LeetCode #997 (Find the Town Judge)", "LeetCode #1791 (Find Center of Star Graph)",
                      "LeetCode #547 (Number of Provinces)", "LeetCode #323 (Number of Connected Components)"},
                     simGraphUndirected);
    dsRegisterIntSim(m, "Fundamentals of Graph#1", pairEdgesPrompt, "0, 1, 0, 2, 1, 3, 2, 3, 3, 4, 4, 5",
                     {"G = (V, E)   (edges are arrows u -> v)", "for each edge u -> v:", "    out[u]++;  in[v]++;", "sum of in-degrees = sum of out-degrees = |E|"},
                     dsGraphRefs(),
                     {"LeetCode #997 (Find the Town Judge)", "LeetCode #207 (Course Schedule)",
                      "LeetCode #1557 (Minimum Number of Vertices to Reach All Nodes)"},
                     simGraphDirected);
    dsSetProgramNames("Fundamentals of Graph", {"Undirected graph: degree and handshake lemma", "Directed graph: in-degree and out-degree"});

    dsRegisterIntSim(m, "Adjacency Matrix", pairEdgesPrompt, "0, 1, 0, 2, 1, 3, 2, 3, 3, 4, 4, 5",
                     {"int A[V][V] = {0};", "for each edge (u, v):", "    A[u][v] = 1;", "    A[v][u] = 1;          // undirected: the matrix is symmetric"},
                     dsGraphRefs(),
                     {"LeetCode #547 (Number of Provinces: matrix input)", "LeetCode #997 (Find the Town Judge)",
                      "LeetCode #1971 (Find if Path Exists in Graph)"},
                     simAdjMatrixBuild);
    dsRegisterIntSim(m, "Adjacency Matrix#1", "Vertex u, then edges as pairs", "3, 0, 1, 0, 2, 1, 3, 2, 3, 3, 4, 4, 5",
                     {"neighbours of u:", "    for (j = 0; j < V; j++)", "        if (A[u][j] == 1)  report j;", "degree(u) = number of 1s in row u"},
                     dsGraphRefs(),
                     {"LeetCode #1971 (Find if Path Exists in Graph)", "LeetCode #1791 (Find Center of Star Graph)",
                      "LeetCode #997 (Find the Town Judge)"},
                     simAdjMatrixNeighbours);
    dsSetProgramNames("Adjacency Matrix", {"Build the matrix edge by edge", "Neighbours of a vertex (scan a row)"});

    dsRegisterIntSim(m, "Adjacency List", pairEdgesPrompt, "0, 1, 0, 2, 1, 3, 2, 3, 3, 4, 4, 5",
                     {"vector<int> adj[V];", "for each edge (u, v):", "    adj[u].push_back(v);", "    adj[v].push_back(u);       // undirected: store both directions"},
                     dsGraphRefs(),
                     {"LeetCode #133 (Clone Graph)", "LeetCode #207 (Course Schedule)",
                      "LeetCode #1971 (Find if Path Exists in Graph)", "LeetCode #797 (All Paths From Source to Target)"},
                     simAdjListBuild);
    dsRegisterIntSim(m, "Adjacency List#1", "Vertex u, then edges as pairs", "3, 0, 1, 0, 2, 1, 3, 2, 3, 3, 4, 4, 5",
                     {"neighbours of u:", "    for (Node *p = adj[u]; p != NULL; p = p->next)", "        report p->vertex;", "cost = degree(u)"},
                     dsGraphRefs(),
                     {"LeetCode #133 (Clone Graph)", "LeetCode #332 (Reconstruct Itinerary)", "LeetCode #743 (Network Delay Time)"},
                     simAdjListNeighbours);
    dsSetProgramNames("Adjacency List", {"Build the lists", "Neighbours of a vertex (walk one list)"});
}

void addDS6b(map<string, Sim>& m) {
    const char* startAndEdgesPrompt = "Start vertex, then edges as pairs u v";

    dsRegisterIntSim(m, "Graph Traversal", startAndEdgesPrompt, "0, 0, 1, 0, 2, 1, 3, 2, 4, 3, 5, 4, 5",
                     {"Traversal = visit every vertex reachable from the start exactly once.",
                      "BFS: use a QUEUE   -> level by level (nearest vertices first)",
                      "DFS: use a STACK / recursion  -> go as deep as possible, then backtrack",
                      "mark visited vertices so that cycles cannot loop forever"},
                     dsGraphRefs(),
                     {"LeetCode #200 (Number of Islands)", "LeetCode #733 (Flood Fill)",
                      "LeetCode #695 (Max Area of Island)", "LeetCode #1971 (Find if Path Exists in Graph)"},
                     simGraphTraversalCompare);

    vector<string> dfsPractice = {
        "LeetCode #200 (Number of Islands)", "LeetCode #695 (Max Area of Island)",
        "LeetCode #547 (Number of Provinces)", "LeetCode #417 (Pacific Atlantic Water Flow)"};
    dsRegisterIntSim(m, "Depth First Search (DFS)", startAndEdgesPrompt, "0, 0, 1, 0, 2, 1, 3, 2, 3, 3, 4, 4, 5",
                     {"void dfs(int u) {", "    visited[u] = true;   visit(u);", "    for (int w : adj[u])",
                      "        if (!visited[w])  dfs(w);          // go deeper", "}                                      // return = backtrack"},
                     dsGraphRefs(), dfsPractice, simDfsRecursive);
    dsRegisterIntSim(m, "Depth First Search (DFS)#1", startAndEdgesPrompt, "0, 0, 1, 0, 2, 1, 3, 2, 3, 3, 4, 4, 5",
                     {"stack.push(start);", "while (stack not empty) {", "    u = stack.pop();",
                      "    if (visited[u])  continue;   visited[u] = true;   visit(u);",
                      "    for (w in adj[u], largest first)  stack.push(w);    // so the smallest is popped first", "}"},
                     dsGraphRefs(), dfsPractice, simDfsIterative);
    dsSetProgramNames("Depth First Search (DFS)", {"Recursive DFS (call stack)", "Iterative DFS (explicit stack)"});

    vector<string> bfsPractice = {
        "LeetCode #994 (Rotting Oranges)", "LeetCode #127 (Word Ladder)",
        "LeetCode #1091 (Shortest Path in Binary Matrix)", "LeetCode #102 (Binary Tree Level Order Traversal)"};
    dsRegisterIntSim(m, "Breadth First Search (BFS)", startAndEdgesPrompt, "0, 0, 1, 0, 2, 1, 3, 2, 3, 3, 4, 4, 5",
                     {"queue.push(start);  dist[start] = 0;  visited[start] = true;", "while (queue not empty) {",
                      "    u = queue.pop();   visit(u);", "    for (int w : adj[u])",
                      "        if (!visited[w]) { visited[w] = true;  dist[w] = dist[u] + 1;  queue.push(w); }", "}"},
                     dsGraphRefs(), bfsPractice, simBfsTraversal);
    dsRegisterIntSim(m, "Breadth First Search (BFS)#1", "Source, target, then edges as pairs", "0, 5, 0, 1, 0, 2, 1, 3, 2, 3, 3, 4, 4, 5",
                     {"BFS from the source, remembering parent[w] = u for each discovery;", "when the target is reached:",
                      "    walk back  target -> parent -> parent ... -> source;", "    reverse it = a shortest path (fewest edges)"},
                     dsGraphRefs(), bfsPractice, simBfsShortestPath);
    dsSetProgramNames("Breadth First Search (BFS)", {"BFS order and distances (queue)", "Shortest path in an unweighted graph"});

    vector<string> shortestPathRefs = {
        "VisuAlgo: shortest paths|https://visualgo.net/en/sssp",
        "GeeksforGeeks: Dijkstra|https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/",
        "cp-algorithms: shortest paths|https://cp-algorithms.com/graph/dijkstra.html"};
    const char* weightedEdgesPrompt = "Source, then weighted edges as triples u v w";
    dsRegisterIntSim(m, "Shortest Path Algorithms", weightedEdgesPrompt,
                     "0, 0, 1, 7, 0, 2, 9, 0, 5, 14, 1, 2, 10, 1, 3, 15, 2, 3, 11, 2, 5, 2, 3, 4, 6, 4, 5, 9",
                     {"dist[all] = INF;  dist[src] = 0;", "repeat V times:",
                      "    u = unvisited vertex with the smallest dist;   mark u done;",
                      "    for each edge (u, w, weight):",
                      "        if (dist[u] + weight < dist[w])  dist[w] = dist[u] + weight;     // relax"},
                     shortestPathRefs,
                     {"LeetCode #743 (Network Delay Time)", "LeetCode #1514 (Path with Maximum Probability)",
                      "LeetCode #787 (Cheapest Flights Within K Stops)", "LeetCode #1631 (Path With Minimum Effort)"},
                     simDijkstra);
    dsRegisterIntSim(m, "Shortest Path Algorithms#1", "Source, then directed edges u v w (weights may be negative)",
                     "0, 0, 1, 6, 0, 2, 7, 1, 2, 8, 1, 3, 5, 1, 4, -4, 2, 3, -3, 2, 4, 9, 3, 1, -2, 4, 3, 7, 4, 0, 2",
                     {"dist[all] = INF;  dist[src] = 0;", "repeat V - 1 times:", "    for every edge (u, w, weight):",
                      "        if (dist[u] + weight < dist[w])  dist[w] = dist[u] + weight;",
                      "one more pass: if any edge still improves, there is a NEGATIVE CYCLE"},
                     shortestPathRefs,
                     {"LeetCode #787 (Cheapest Flights Within K Stops)", "LeetCode #743 (Network Delay Time)",
                      "LeetCode #1334 (Find the City With the Smallest Number of Neighbors)"},
                     simBellmanFord);
    dsRegisterIntSim(m, "Shortest Path Algorithms#2", "Directed edges u v w (up to 5 vertices)",
                     "0, 1, 3, 0, 3, 7, 1, 0, 8, 1, 2, 2, 2, 0, 5, 2, 3, 1, 3, 0, 2",
                     {"d[i][j] = weight of edge i->j, INF if none, 0 on the diagonal;",
                      "for (k = 0; k < V; k++)           // allow vertex k as an intermediate stop",
                      "    for (i = 0; i < V; i++)", "        for (j = 0; j < V; j++)",
                      "            if (d[i][k] + d[k][j] < d[i][j])  d[i][j] = d[i][k] + d[k][j];"},
                     shortestPathRefs,
                     {"LeetCode #1334 (Find the City With the Smallest Number of Neighbors at a Threshold Distance)",
                      "LeetCode #743 (Network Delay Time)", "LeetCode #399 (Evaluate Division)"},
                     simFloydWarshall);
    dsSetProgramNames("Shortest Path Algorithms", {"Dijkstra (non-negative weights)", "Bellman-Ford (negative weights allowed)", "Floyd-Warshall (all pairs)"});

    vector<string> mstRefs = {
        "VisuAlgo: minimum spanning tree|https://visualgo.net/en/mst",
        "GeeksforGeeks: Kruskal's algorithm|https://www.geeksforgeeks.org/kruskals-minimum-spanning-tree-algorithm-greedy-algo-2/",
        "cp-algorithms: MST|https://cp-algorithms.com/graph/mst_kruskal.html"};
    vector<string> mstPractice = {
        "LeetCode #1584 (Min Cost to Connect All Points)",
        "LeetCode #1135 (Connecting Cities With Minimum Cost)",
        "LeetCode #1489 (Find Critical and Pseudo-Critical Edges in MST)"};
    dsRegisterIntSim(m, "Minimum Spanning Tree (MST)", "Weighted edges as triples u v w (undirected)",
                     "0, 1, 4, 0, 2, 3, 1, 2, 1, 1, 3, 2, 2, 3, 4, 3, 4, 2, 4, 5, 6",
                     {"sort all edges by weight;", "for each edge (u, v, w) in that order:",
                      "    if (find(u) != find(v)) {       // different components: no cycle",
                      "        take the edge;  union(u, v);",
                      "    } else  skip the edge;          // it would form a cycle"},
                     mstRefs, mstPractice, simMstKruskal);
    dsRegisterIntSim(m, "Minimum Spanning Tree (MST)#1", "Start vertex, then weighted edges u v w",
                     "0, 0, 1, 4, 0, 2, 3, 1, 2, 1, 1, 3, 2, 2, 3, 4, 3, 4, 2, 4, 5, 6",
                     {"tree = { start };", "while (tree does not contain every vertex) {",
                      "    pick the CHEAPEST edge with exactly one end inside the tree;",
                      "    add that edge and its new vertex to the tree;", "}"},
                     mstRefs, mstPractice, simMstPrim);
    dsSetProgramNames("Minimum Spanning Tree (MST)", {"Kruskal (sort edges, avoid cycles)", "Prim (grow one tree)"});
}
