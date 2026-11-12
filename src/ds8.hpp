// Real-life scenario simulations (one extra program at the end of selected Data Structures topics).
#pragma once
#include "ds7.hpp"

using namespace std;

static vector<string> dsHistoryTags(const Ints& history) {
    if (history.empty()) return {};
    return dsMakeTags(history.size(), {{(int)history.size() - 1, "you are here"}});
}

static Steps simRealBrowserHistory(const Ints& input) {
    Ints commands = dsTakeInts(input, 0, 14, {1, 2, 3, 0, 4, 0, 0, 0, 0});
    Ints history;
    Steps steps;
    const int MAXH = 8;
    steps.push_back(dsArrayStep(0,
        "Your browser keeps the pages you visited on a STACK. The page on top is the one you are viewing. Opening a link pushes; the Back button pops.",
        {{"pages", "0"}}, history, -1, {}, -1, -1, "history"));
    for (size_t i = 0; i < commands.size(); ++i) {
        int cmd = commands[i];
        if (cmd > 0) {
            int page = cmd % 100;
            if ((int)history.size() == MAXH) {
                steps.push_back(dsArrayStep(0,
                    "History is full (" + S(MAXH) + " pages): real browsers drop the oldest page. Page " + S(page) + " is ignored here.",
                    {}, history, (int)history.size() - 1, dsHistoryTags(history), -1, -1, "history"));
                continue;
            }
            history.push_back(page);
            steps.push_back(dsArrayStep(0,
                "You click a link to page " + S(page) + ": push it. Now viewing page " + S(page) + ".",
                {{"viewing", "page " + S(page)}}, history, (int)history.size() - 1, dsHistoryTags(history), -1, -1, "history"));
        } else {
            if (history.size() <= 1) {
                steps.push_back(dsArrayStep(1,
                    history.empty() ? "Nothing to go back to." : "Only one page in history: the Back button is greyed out.",
                    {}, history, history.empty() ? -1 : 0, dsHistoryTags(history), -1, -1, "history"));
                continue;
            }
            int poppedPage = history.back();
            history.pop_back();
            steps.push_back(dsArrayStep(1,
                "You press Back: page " + S(poppedPage) + " is popped off the top. You see page " + S(history.back()) +
                    " again (last in, first out). Undo/Redo in an editor works exactly the same way.",
                {{"viewing", "page " + S(history.back())}}, history, (int)history.size() - 1, dsHistoryTags(history), -1, -1, "history"));
        }
    }
    steps.push_back(dsArrayStep(1,
        "Both actions touch only the top of the stack, so each one is O(1): that is why Back is instant even after hundreds of pages.",
        {{"pages in history", S(history.size())}}, history, -1, dsHistoryTags(history), -1, -1, "history"));
    return steps;
}

static Steps simRealPrinterQueue(const Ints& input) {
    Ints pages = dsTakeInts(input, 0, 6, {5, 2, 8, 1, 3});
    for (size_t i = 0; i < pages.size(); ++i) pages[i] = clamp(abs(pages[i]), 1, 9);
    int n = (int)pages.size();
    Ints queueArr;
    Steps steps;
    vector<string> pageTags;
    steps.push_back(dsArrayStep(0,
        "A shared office printer prints jobs in the order they arrive: a QUEUE. First come, first served. Cells show job numbers, the label shows pages.",
        {{"jobs", S(n)}}, queueArr, -1, {}, -1, -1, "print queue"));
    for (int i = 0; i < n; ++i) {
        queueArr.push_back(i + 1);
        pageTags.push_back(S(pages[i]) + " pg");
        steps.push_back(dsArrayStep(0,
            "Job " + S(i + 1) + " (" + S(pages[i]) + " pages) is sent to the printer: it joins at the REAR of the queue.",
            {{"waiting", S(queueArr.size())}}, queueArr, (int)queueArr.size() - 1, pageTags, -1, -1, "print queue"));
    }
    int clockTime = 0;
    double sumWait = 0;
    for (int i = 0; i < n; ++i) {
        int waitTime = clockTime;
        sumWait += waitTime;
        clockTime += pages[i];
        vector<string> remTags(pageTags.begin() + i, pageTags.end());
        Ints remQueue(queueArr.begin() + i, queueArr.end());
        steps.push_back(dsArrayStep(1,
            "Printer takes the FRONT job: job " + S(i + 1) + " (" + S(pages[i]) + " pages). It waited " + S(waitTime) +
                " page-time(s); it finishes at time " + S(clockTime) + ".",
            {{"printing", "job " + S(i + 1)}, {"waited", S(waitTime)}, {"time now", S(clockTime)}},
            remQueue, 0, remTags, -1, -1, "print queue"));
    }
    string avgText = to_string(sumWait / n).substr(0, 4);
    steps.push_back(dsArrayStep(1,
        "All jobs done at time " + S(clockTime) + ". Average waiting time = " + avgText +
            ". A queue is fair but a long job at the front delays everyone: real schedulers sometimes use a priority queue for that (next scenarios).",
        {{"average wait", avgText}}, Ints(), -1, {}, -1, -1, "print queue"));
    return steps;
}

static string dsDrawPlaylist(const vector<string>& songs, int curIdx, const string& caption,
                             const string& newSong = "", int slot = -1, int newNext = -2) {
    vector<string> labels(songs.size());
    labels[curIdx] = "playing";
    return dsDrawLinkedList(songs, dsSequentialLinks(songs.size()), labels, 0, caption, newSong, slot, newNext);
}

static Steps simRealPlaylist(const Ints& input) {
    Ints commands = dsTakeInts(input, 0, 8, {1, 1, 2, 1, 1, 1});
    vector<string> songs = {"A", "B", "C", "D"};
    int curIdx = 0;
    int nextNew = 0;
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "A music playlist is a linked list: each song points to the next one. 'playing' marks the current song.",
        {{"now playing", songs[curIdx]}}, dsDrawPlaylist(songs, curIdx, "playlist"), {"n0"}));
    for (size_t i = 0; i < commands.size(); ++i) {
        int cmd = commands[i];
        if (cmd == 2) {
            if (songs.size() >= 7) {
                steps.push_back(dsDiagramStep(1, "The playlist is full in this demo (7 songs).",
                    {}, dsDrawPlaylist(songs, curIdx, "full"), {}));
                continue;
            }
            string songName = string("N") + S(++nextNew);
            int slot = curIdx + 1;
            bool isLast = (slot >= (int)songs.size());
            steps.push_back(dsDiagramStep(1,
                "You tap 'Play next' on song " + songName + ". Step 1: point the new song at what comes after the current one.",
                {{"new song", songName}}, dsDrawPlaylist(songs, curIdx, "new song points forward", songName, slot, isLast ? -1 : slot), {"nw", "nl"}));
            songs.insert(songs.begin() + slot, songName);
            steps.push_back(dsDiagramStep(1,
                "Step 2: the current song now points to " + songName +
                    ". Only 2 pointers changed: no other song moved, so it is O(1) however long the playlist is.",
                {{"now playing", songs[curIdx]}}, dsDrawPlaylist(songs, curIdx, "inserted after the current song"),
                {"n" + S(slot), "l" + S(curIdx)}));
        } else {
            if (curIdx + 1 >= (int)songs.size()) {
                steps.push_back(dsDiagramStep(0, "End of the playlist (next is NULL): playback stops.",
                    {}, dsDrawPlaylist(songs, curIdx, "end of list"), {"n" + S(curIdx)}));
                continue;
            }
            ++curIdx;
            steps.push_back(dsDiagramStep(0, "Next: follow the pointer to song " + songs[curIdx] + ".",
                {{"now playing", songs[curIdx]}}, dsDrawPlaylist(songs, curIdx, "follow the next pointer"),
                {"n" + S(curIdx), "l" + S(curIdx - 1)}));
        }
    }
    steps.push_back(dsDiagramStep(0,
        "Linked lists make 'play next' and 'remove song' cheap because nothing has to shift (an array would move every later song). The price: no jumping straight to song number k.",
        {}, dsDrawPlaylist(songs, curIdx, "playlist"), {}));
    return steps;
}

static Steps simRealEmergencyTriage(const Ints& input) {
    Ints commands = dsTakeInts(input, 0, 14, {4, 2, 5, 1, 0, 3, 0, 0, 0});
    Ints heapArr(10, 0);
    int heapSize = 0;
    Steps steps;
    string treatedOrder;
    steps.push_back(dsHeapStep(0,
        "An emergency room treats the MOST URGENT patient first, not the one who arrived first. A min-heap keeps the lowest severity number at the top.",
        {}, heapArr, 0, -1, {}, {}, "waiting room (heap)"));
    for (size_t k = 0; k < commands.size(); ++k) {
        int cmd = commands[k];
        if (cmd != 0) {
            int severity = clamp(abs(cmd), 1, 9);
            if (heapSize == 10) {
                steps.push_back(dsHeapStep(0, "Waiting room full in this demo.", {}, heapArr, heapSize, -1, {}, {}, "full"));
                continue;
            }
            heapArr[heapSize++] = severity;
            int i = heapSize - 1;
            int moved = 0;
            while (i > 0 && heapArr[(i - 1) / 2] > heapArr[i]) {
                swap(heapArr[i], heapArr[(i - 1) / 2]);
                i = (i - 1) / 2;
                ++moved;
            }
            steps.push_back(dsHeapStep(0,
                "A patient with severity " + S(severity) + " arrives. " +
                    (moved ? "More urgent than the ones above, so the entry moves up " + S(moved) + " level(s)."
                           : "Not more urgent than its parent: it stays near the bottom.") +
                    " The most urgent is at the top.",
                {{"severity", S(severity)}, {"waiting", S(heapSize)}},
                heapArr, heapSize, i, dsMakeTags(heapSize, {{i, "new"}}), {"t" + S(i)}, "arrival"));
        } else {
            if (heapSize == 0) {
                steps.push_back(dsHeapStep(0, "No patients waiting.", {}, heapArr, 0, -1, {}, {}, "empty"));
                continue;
            }
            int rootVal = heapArr[0];
            if (!treatedOrder.empty()) treatedOrder += ", ";
            treatedOrder += S(rootVal);
            heapArr[0] = heapArr[heapSize - 1];
            --heapSize;
            int i = 0;
            while (true) {
                int l = 2 * i + 1;
                int r = l + 1;
                int bestIdx = i;
                if (l < heapSize && heapArr[l] < heapArr[bestIdx]) bestIdx = l;
                if (r < heapSize && heapArr[r] < heapArr[bestIdx]) bestIdx = r;
                if (bestIdx == i) break;
                swap(heapArr[i], heapArr[bestIdx]);
                i = bestIdx;
            }
            steps.push_back(dsHeapStep(1,
                "The doctor calls the patient with severity " + S(rootVal) +
                    " (the root). The heap repairs itself in O(log n). Treated so far: " + treatedOrder + ".",
                {{"treated", treatedOrder}, {"waiting", S(heapSize)}},
                heapArr, heapSize, 0, dsMakeTags(heapSize, {{0, "next"}}), {"t0"}, "after treatment"));
        }
    }
    steps.push_back(dsHeapStep(1,
        "Patients are served in order of urgency (ties by arrival here). The same idea schedules tasks in your phone's operating system and picks the next event in simulations.",
        {{"treated", treatedOrder}}, heapArr, heapSize, -1, {}, {}, "done"));
    return steps;
}

static Steps simRealFriendSuggestions(const Ints& input) {
    GraphData graph = dsParseGraph(input, 1, false, false, {0, 1, 0, 2, 1, 3, 2, 3, 2, 4, 3, 5, 4, 5, 4, 6});
    int vertexCount = graph.numVertices;
    long long rawSelf = input.empty() ? 0 : (long long)input[0];
    int meVertex = (int)(abs(rawSelf) % vertexCount);
    vector<vector<pair<int, int>>> adj = dsGraphAdj(graph);
    vector<int> vertexState(vertexCount, 0);
    vector<int> distArr(vertexCount, -1);
    vector<string> labels(vertexCount);
    vector<char> chosenEdges(graph.edges.size(), 0);
    Steps steps;
    vertexState[meVertex] = 1;
    labels[meVertex] = "you";
    distArr[meVertex] = 0;
    steps.push_back(dsDiagramStep(0,
        "A social network is a graph: people are vertices, friendships are edges. Suggest 'people you may know' = friends of friends = vertices exactly 2 steps away. BFS finds them level by level.",
        {{"you", S(meVertex)}}, dsDrawGraph(graph, vertexState, labels, chosenEdges, "you are the blue vertex"), {"v" + S(meVertex)}));
    string friendsText;
    for (size_t i = 0; i < adj[meVertex].size(); ++i) {
        int friendVertex = adj[meVertex][i].first;
        int edgeIdx = adj[meVertex][i].second;
        distArr[friendVertex] = 1;
        vertexState[friendVertex] = 2;
        labels[friendVertex] = "friend";
        chosenEdges[edgeIdx] = 1;
        if (!friendsText.empty()) friendsText += ", ";
        friendsText += S(friendVertex);
    }
    steps.push_back(dsDiagramStep(0,
        "Level 1 (BFS from you): your direct friends are " + (friendsText.empty() ? string("nobody") : friendsText) + ".",
        {{"friends", friendsText.empty() ? "(none)" : friendsText}},
        dsDrawGraph(graph, vertexState, labels, chosenEdges, "distance 1 = friends (orange)"), {"v" + S(meVertex)}));
    vector<int> mutualCount(vertexCount, 0);
    for (size_t i = 0; i < adj[meVertex].size(); ++i) {
        int f = adj[meVertex][i].first;
        for (size_t j = 0; j < adj[f].size(); ++j) {
            int fof = adj[f][j].first;
            if (distArr[fof] < 0) ++mutualCount[fof];
        }
    }
    string suggestionsText;
    for (int i = 0; i < vertexCount; ++i) {
        if (mutualCount[i] > 0 && distArr[i] < 0) {
            distArr[i] = 2;
            vertexState[i] = 1;
            labels[i] = "suggest";
            if (!suggestionsText.empty()) suggestionsText += ", ";
            suggestionsText += S(i) + " [" + S(mutualCount[i]) + " mutual]";
        }
    }
    for (size_t i = 0; i < graph.edges.size(); ++i) {
        int u = graph.edges[i][0];
        int v = graph.edges[i][1];
        if ((distArr[u] == 2 && distArr[v] == 1) || (distArr[v] == 2 && distArr[u] == 1)) chosenEdges[i] = 1;
    }
    steps.push_back(dsDiagramStep(1,
        "Level 2: friends of your friends who are not already your friends: " +
            (suggestionsText.empty() ? string("none") : suggestionsText) +
            ". The more MUTUAL friends, the better the suggestion: apps rank by this count.",
        {{"suggestions", suggestionsText.empty() ? "(none)" : suggestionsText}},
        dsDrawGraph(graph, vertexState, labels, chosenEdges, "distance 2 = suggestions"), {}));
    steps.push_back(dsDiagramStep(2,
        "Because BFS visits closer people first, it never wastes time on far-away strangers: it stops after level 2. The same idea gives 'six degrees of separation' and LinkedIn's '2nd-degree connection'.",
        {}, dsDrawGraph(graph, vertexState, labels, chosenEdges, "BFS stops at distance 2"), {}));
    return steps;
}

static vector<string> dsBuildGpsLabels(const vector<int>& distArr, int vertexCount, int srcVertex, int dstVertex, int infVal) {
    vector<string> labels(vertexCount);
    for (int i = 0; i < vertexCount; ++i) {
        labels[i] = (distArr[i] == infVal) ? "" : (S(distArr[i]) + " min");
    }
    labels[srcVertex] = "home 0";
    labels[dstVertex] = (distArr[dstVertex] == infVal ? "" : S(distArr[dstVertex]) + " min ") + "dest";
    return labels;
}

static Steps simRealGpsNavigation(const Ints& input) {
    Ints defaultRoads = {0, 1, 7, 0, 2, 9, 0, 5, 14, 1, 2, 10, 1, 3, 15, 2, 3, 11, 2, 5, 2, 3, 4, 6, 4, 5, 9};
    GraphData graph = dsParseGraph(input, 2, true, false, defaultRoads, 1, 99, 7);
    int vertexCount = graph.numVertices;
    long long rawSrc = input.empty() ? 0 : (long long)input[0];
    long long rawDst = input.size() > 1 ? (long long)input[1] : 4;
    int srcVertex = (int)(abs(rawSrc) % vertexCount);
    int dstVertex = (int)(abs(rawDst) % vertexCount);
    const int INF = INT_MAX;
    vector<int> distArr(vertexCount, INF);
    vector<int> vertexState(vertexCount, 0);
    vector<int> prevEdge(vertexCount, -1);
    vector<char> done(vertexCount, 0);
    vector<char> chosenEdges(graph.edges.size(), 0);
    distArr[srcVertex] = 0;
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "A GPS app sees the map as a graph: junctions are vertices, roads are edges, minutes are weights. You are at junction " +
            S(srcVertex) + ", going to " + S(dstVertex) + ". It needs the quickest route, not the shortest in kilometres.",
        {{"from", S(srcVertex)}, {"to", S(dstVertex)}},
        dsDrawGraph(graph, vertexState, dsBuildGpsLabels(distArr, vertexCount, srcVertex, dstVertex, INF), chosenEdges, "road map (minutes on roads)"),
        {"v" + S(srcVertex), "v" + S(dstVertex)}));
    for (int iter = 0; iter < vertexCount; ++iter) {
        int u = -1;
        for (int i = 0; i < vertexCount; ++i) {
            if (!done[i] && distArr[i] != INF && (u < 0 || distArr[i] < distArr[u])) u = i;
        }
        if (u < 0) break;
        done[u] = 1;
        vertexState[u] = 2;
        for (size_t i = 0; i < graph.edges.size(); ++i) {
            int a = graph.edges[i][0];
            int b = graph.edges[i][1];
            int w = graph.edges[i][2];
            int target = (a == u) ? b : ((b == u) ? a : -1);
            if (target < 0 || done[target]) continue;
            if (distArr[u] + w < distArr[target]) {
                distArr[target] = distArr[u] + w;
                prevEdge[target] = (int)i;
            }
        }
        steps.push_back(dsDiagramStep(2,
            "Explore the closest unexplored junction: " + S(u) + " (" + S(distArr[u]) +
                " min from home). Update the times to its neighbours.",
            {{"junction", S(u)}, {"minutes", S(distArr[u])}},
            dsDrawGraph(graph, vertexState, dsBuildGpsLabels(distArr, vertexCount, srcVertex, dstVertex, INF), chosenEdges, "exploring outwards"),
            {"v" + S(u)}));
        if (u == dstVertex) break;
    }
    if (distArr[dstVertex] == INF) {
        steps.push_back(dsDiagramStep(0, "No road connects these two places.",
            {}, dsDrawGraph(graph, vertexState, dsBuildGpsLabels(distArr, vertexCount, srcVertex, dstVertex, INF), chosenEdges, "unreachable"), {}));
        return steps;
    }
    vector<int> routePath;
    routePath.push_back(dstVertex);
    for (int x = dstVertex; x != srcVertex && prevEdge[x] >= 0 && routePath.size() < 20;) {
        int e = prevEdge[x];
        chosenEdges[e] = 1;
        x = (graph.edges[e][0] == x) ? graph.edges[e][1] : graph.edges[e][0];
        routePath.push_back(x);
    }
    reverse(routePath.begin(), routePath.end());
    string routeText;
    for (size_t i = 0; i < routePath.size(); ++i) {
        if (i) routeText += " -> ";
        routeText += S(routePath[i]);
    }
    steps.push_back(dsDiagramStep(3,
        "The destination is settled, so the GPS stops searching (no need to look at the whole country). Route: " +
            routeText + ". ETA " + S(distArr[dstVertex]) + " minutes. A traffic jam just changes a weight and the search is repeated.",
        {{"route", routeText}, {"ETA (min)", S(distArr[dstVertex])}},
        dsDrawGraph(graph, vertexState, dsBuildGpsLabels(distArr, vertexCount, srcVertex, dstVertex, INF), chosenEdges, "fastest route (double lines)"),
        {}));
    return steps;
}

static Steps simRealCableNetwork(const Ints& input) {
    Ints defaultCables = {0, 1, 4, 0, 2, 3, 1, 2, 1, 1, 3, 2, 2, 3, 4, 3, 4, 2, 4, 5, 6};
    GraphData graph = dsParseGraph(input, 0, true, false, defaultCables, 1, 99, 7);
    int vertexCount = graph.numVertices;
    vector<int> edgeOrder(graph.edges.size());
    for (size_t i = 0; i < edgeOrder.size(); ++i) edgeOrder[i] = (int)i;
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
    int totalCost = 0;
    int allRoutesCost = 0;
    for (size_t i = 0; i < graph.edges.size(); ++i) allRoutesCost += graph.edges[i][2];
    steps.push_back(dsDiagramStep(0,
        "A telecom company must connect " + S(vertexCount) + " towns with fibre. Every possible route has a cost. Goal: every town reachable from every other, at the lowest total cost. That is exactly a minimum spanning tree.",
        {{"towns", S(vertexCount)}, {"cost of ALL routes", S(allRoutesCost)}},
        dsDrawGraph(graph, vertexState, {}, chosenEdges, "possible cable routes (cost on each)"), {}));
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
            totalCost += w;
            vertexState[u] = 2;
            vertexState[v] = 2;
            steps.push_back(dsDiagramStep(2,
                "Cheapest remaining route: towns " + S(u) + " and " + S(v) + ", cost " + S(w) +
                    ". They are not connected yet: LAY it. Spent so far: " + S(totalCost) + ".",
                {{"laid", S(u) + "-" + S(v)}, {"spent", S(totalCost)}},
                dsDrawGraph(graph, vertexState, {}, chosenEdges, "double line = cable laid"), {"e" + S(i)}));
        } else {
            steps.push_back(dsDiagramStep(3,
                "Route " + S(u) + "-" + S(v) + " (cost " + S(w) + "): these towns can already reach each other through cables laid earlier. Skip it: it would be a waste.",
                {{"skipped", S(u) + "-" + S(v)}},
                dsDrawGraph(graph, vertexState, {}, chosenEdges, "redundant route skipped"), {"e" + S(i)}));
        }
    }
    steps.push_back(dsDiagramStep(1,
        "Network complete for " + S(totalCost) + " instead of " + S(allRoutesCost) + " for laying every route: a saving of " +
            S(allRoutesCost - totalCost) + ". Power grids, water pipes and road networks are planned the same way.",
        {{"total cost", S(totalCost)}, {"saved", S(allRoutesCost - totalCost)}},
        dsDrawGraph(graph, vertexState, {}, chosenEdges, "minimum cost network"), {}));
    return steps;
}

static int dsFolderDuHelper(int nodeIdx, TreeDiagram& tree, const vector<int>& ownSize, vector<string>& callStack, Steps& steps) {
    if (nodeIdx < 0) return 0;
    callStack.push_back("du(folder " + S(ownSize[nodeIdx]) + ")");
    int leftSize = dsFolderDuHelper(tree.ch[nodeIdx][0], tree, ownSize, callStack, steps);
    int rightSize = dsFolderDuHelper(tree.ch[nodeIdx][1], tree, ownSize, callStack, steps);
    int totalSize = ownSize[nodeIdx] + leftSize + rightSize;
    tree.lab[nodeIdx] = S(ownSize[nodeIdx]) + ":" + S(totalSize);
    tree.cls[nodeIdx] = "weld";
    Step step = dsDiagramStep(2,
        "Sub-folders are finished first (postorder): folder with own size " + S(ownSize[nodeIdx]) + " MB + " +
            S(leftSize) + " + " + S(rightSize) + " from its sub-folders = " + S(totalSize) + " MB.",
        {{"folder total", S(totalSize) + " MB"}},
        dsDrawTree(tree, true, "label = own:total (MB)"), {"t" + S(nodeIdx)});
    step.stack = callStack;
    steps.push_back(step);
    callStack.pop_back();
    return totalSize;
}

static Steps simRealFolderDu(const Ints& input) {
    Ints values = dsTakeInts(input, 0, 12, {5, 20, 10, 8, 30, -1, 12});
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i] != -1) values[i] = clamp(abs(values[i]), 0, 999);
    }
    TreeDiagram tree = dsBuildLevelTree(values);
    if (tree.root < 0) tree = dsBuildLevelTree({5, 20, 10});
    Steps steps;
    vector<string> callStack;
    vector<int> ownSize(tree.key.begin(), tree.key.end());
    steps.push_back(dsDiagramStep(0,
        "How big is a folder? The 'du' command adds the files inside it AND everything in its sub-folders. Each node is a folder; its label starts as its own file size (MB).",
        {}, dsDrawTree(tree, true, "own size of each folder"), {}));
    int grandTotal = dsFolderDuHelper(tree.root, tree, ownSize, callStack, steps);
    steps.push_back(dsDiagramStep(3,
        "The whole tree uses " + S(grandTotal) +
            " MB. A parent's size is only known after ALL its children are known, which is why this needs postorder. Deleting or copying a folder recursively uses the same order.",
        {{"total", S(grandTotal) + " MB"}}, dsDrawTree(tree, true, "total size"), {}));
    return steps;
}

static void dsCollectTrieSuggestions(const PrefixTriePool& trie, int nodeIdx, string wordSoFar, string& outText) {
    if (trie.isWordEnd[nodeIdx]) {
        if (!outText.empty()) outText += ", ";
        outText += wordSoFar;
    }
    for (int k = 0; k < 26; ++k) {
        if (trie.nextChild[nodeIdx][k] >= 0) {
            dsCollectTrieSuggestions(trie, trie.nextChild[nodeIdx][k], wordSoFar + (char)('a' + k), outText);
        }
    }
}

static Steps simRealSearchSuggestions(const string& input) {
    pair<string, string> parts = dsSplitPipe(input);
    vector<string> words = dsExtractWords(parts.first, 6, 6);
    vector<string> typedWords = dsExtractWords(parts.second, 1, 6);
    if (words.empty()) words = {"apple", "app", "apply", "apt", "bat", "bar"};
    string typed = typedWords.empty() ? "app" : typedWords[0];
    PrefixTriePool trie;
    for (size_t wi = 0; wi < words.size(); ++wi) {
        int cur = 0;
        for (size_t ci = 0; ci < words[wi].size(); ++ci) {
            char c = words[wi][ci];
            if (trie.nextChild[cur][c - 'a'] < 0) {
                int id = trie.createNode(c, cur);
                trie.nextChild[cur][c - 'a'] = id;
            }
            cur = trie.nextChild[cur][c - 'a'];
        }
        trie.isWordEnd[cur] = 1;
    }
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "A search box suggests words while you type. Behind it: a trie of known searches. Each key press moves one step down the trie.",
        {{"you type", typed}}, dsDrawPrefixTrie(trie, "search words as a trie"), {"t0"}));
    int cur = 0;
    string prefix;
    for (size_t i = 0; i < typed.size(); ++i) {
        char c = typed[i];
        prefix += c;
        int nextNode = trie.nextChild[cur][c - 'a'];
        if (nextNode < 0) {
            steps.push_back(dsDiagramStep(1,
                "After \"" + prefix + "\" there is no branch: no suggestions. The box stays empty.",
                {{"typed", prefix}}, dsDrawPrefixTrie(trie, "no match"), {"t" + S(cur)}));
            return steps;
        }
        cur = nextNode;
        string suggestions;
        dsCollectTrieSuggestions(trie, cur, prefix, suggestions);
        steps.push_back(dsDiagramStep(2,
            "You typed \"" + prefix + "\": suggestions = every word below this node: " + suggestions + ".",
            {{"typed", prefix}, {"suggestions", suggestions}},
            dsDrawPrefixTrie(trie, "suggestions narrow as you type"), {"t" + S(cur), "e" + S(cur)}));
    }
    steps.push_back(dsDiagramStep(2,
        "Each key press costs O(1) work, however many words are stored: that is how Google, YouTube and your phone keyboard feel instant.",
        {}, dsDrawPrefixTrie(trie, "done"), {}));
    return steps;
}

static Steps simRealDatabaseIndex(const Ints& input) {
    int lo = input.empty() ? 30 : abs(input[0]) % 1000;
    int hi = input.size() > 1 ? abs(input[1]) % 1000 : 60;
    if (hi < lo) swap(lo, hi);
    Ints keys = dsTakeInts(input, 2, 12, {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120});
    for (size_t i = 0; i < keys.size(); ++i) keys[i] = abs(keys[i]) % 1000;
    sort(keys.begin(), keys.end());
    keys.erase(unique(keys.begin(), keys.end()), keys.end());
    int n = (int)keys.size();

    Steps dummySteps;
    Steps steps;
    MultiwayTree tree;
    tree.maxKeys = 3;
    tree.isBPlus = true;
    tree.stepsOut = &dummySteps;
    for (size_t i = 0; i < keys.size(); ++i) tree.insertKey(keys[i]);

    int matches = 0;
    for (size_t i = 0; i < keys.size(); ++i) {
        if (keys[i] >= lo && keys[i] <= hi) ++matches;
    }
    Step tableStep = dsArrayStep(0,
        "A table of " + S(n) + " students. Query: roll numbers " + S(lo) + " to " + S(hi) +
            ". WITHOUT an index the database must read every row (the dimmed ones are wasted reads).",
        {{"rows read", S(n)}, {"matches", S(matches)}}, keys, -1, {}, -1, -1, "table rows");
    int firstIdx = -1;
    int lastIdx = -1;
    for (int i = 0; i < n; ++i) {
        if (keys[i] >= lo && keys[i] <= hi) {
            if (firstIdx < 0) firstIdx = i;
            lastIdx = i;
        }
    }
    tableStep.rl = firstIdx;
    tableStep.rh = lastIdx;
    steps.push_back(tableStep);

    int cur = tree.root;
    int reads = 0;
    steps.push_back(dsDiagramStep(1, "WITH a B+ tree index on the roll number: start at the root.",
        {{"node reads", "0"}},
        dsDrawTree(buildMultiwayDiagram(tree.nodes, tree.root), false, "index", {}, false, tree.getLeafLinks()),
        {"t" + S(tree.root)}));
    int guard = 0;
    while (!tree.nodes[cur].leaf && guard++ < 30) {
        ++reads;
        int childPos = MultiwayTree::countKeysBefore(tree.nodes[cur].keys, lo, true);
        steps.push_back(dsDiagramStep(2,
            "Read index node [" + formatMultiwayKeys(tree.nodes[cur].keys) + "] (disk read " + S(reads) + "): " +
                S(lo) + " belongs to child " + S(childPos) + ".",
            {{"node reads", S(reads)}},
            dsDrawTree(buildMultiwayDiagram(tree.nodes, tree.root), false, "descend", {}, false, tree.getLeafLinks()),
            {"t" + S(cur)}));
        cur = tree.nodes[cur].ch[childPos];
    }
    int leafCount = 0;
    while (cur >= 0 && leafCount < 30) {
        ++reads;
        ++leafCount;
        bool shouldStop = false;
        for (size_t i = 0; i < tree.nodes[cur].keys.size(); ++i) {
            if (tree.nodes[cur].keys[i] > hi) shouldStop = true;
        }
        steps.push_back(dsDiagramStep(2,
            "Read leaf [" + formatMultiwayKeys(tree.nodes[cur].keys) + "] (disk read " + S(reads) + "). " +
                (shouldStop ? "A key above " + S(hi) + " appears: stop." : "Follow the next arrow."),
            {{"node reads", S(reads)}},
            dsDrawTree(buildMultiwayDiagram(tree.nodes, tree.root), false, "walk the leaf chain", {}, false, tree.getLeafLinks()),
            {"t" + S(cur), "nx" + S(cur)}));
        if (shouldStop) break;
        cur = tree.nodes[cur].nextLeaf;
    }
    steps.push_back(dsDiagramStep(2,
        "With the index: " + S(reads) + " node read(s). Without it: " + S(n) +
            " row reads. On a table with a million rows the index needs about 3 to 4 reads instead of a million: this is why adding an index makes a slow query instant.",
        {{"with index", S(reads) + " reads"}, {"full scan", S(n) + " reads"}},
        dsDrawTree(buildMultiwayDiagram(tree.nodes, tree.root), false, "index vs full scan", {}, false, tree.getLeafLinks()),
        {}));
    return steps;
}

static string dsDrawGuessRangeBar(int lo, int hi, int guessVal, int secretVal, const string& caption) {
    Sk canvas;
    double x0 = 20;
    double width = 500;
    canvas.Rect("line", x0, 60, width, 24, "steel");
    canvas.Rect("range", x0 + (lo - 1) * width / 100.0, 60, (hi - lo + 1) * width / 100.0, 24, "weld");
    if (guessVal > 0) {
        double gx = x0 + (guessVal - 0.5) * width / 100.0;
        canvas.Arr(gx, 30, gx, 58, "guess");
        canvas.T(gx, 18, "guess " + S(guessVal));
    }
    double sx = x0 + (secretVal - 0.5) * width / 100.0;
    canvas.T(sx, 106, "secret", "middle");
    canvas.W("secret", {sx, 86, sx, 98});
    canvas.T(x0, 132, "1", "start");
    canvas.T(x0 + width, 132, "100", "end");
    canvas.T(280, 150, caption);
    return canvas.out(540, 164, {});
}

static Steps simRealGuessNumber(const Ints& input) {
    int secret = clamp(input.empty() ? 73 : abs(input[0]), 1, 100);
    int lo = 1;
    int hi = 100;
    int tries = 0;
    Steps steps;
    steps.push_back(dsDiagramStep(0,
        "You think of a number from 1 to 100; the computer finds it by asking only 'higher or lower?'. Orange = numbers still possible. The secret is " +
            S(secret) + ".",
        {{"possible numbers", "100"}}, dsDrawGuessRangeBar(lo, hi, 0, secret, "all 100 numbers possible"), {"range"}));
    while (lo <= hi && tries < 12) {
        int guess = (lo + hi) / 2;
        ++tries;
        if (guess == secret) {
            steps.push_back(dsDiagramStep(3,
                "Guess " + S(guess) + ": CORRECT after " + S(tries) + " guess(es).",
                {{"guesses", S(tries)}}, dsDrawGuessRangeBar(lo, hi, guess, secret, "found"), {"guess"}));
            return steps;
        }
        bool isTooLow = (guess < secret);
        steps.push_back(dsDiagramStep(4,
            "Guess " + S(guess) + " (middle of " + S(lo) + ".." + S(hi) + "): too " + (isTooLow ? "LOW" : "HIGH") +
                ". Throw away " + S(isTooLow ? guess - lo + 1 : hi - guess + 1) + " numbers at once.",
            {{"guesses", S(tries)}, {"left", S(isTooLow ? hi - guess : guess - lo)}},
            dsDrawGuessRangeBar(lo, hi, guess, secret, "halving the range"), {"guess", "range"}));
        if (isTooLow) lo = guess + 1;
        else hi = guess - 1;
    }
    return steps;
}

static string dsFormatOpDuration(double ops) {
    double sec = ops / 1e9;
    char buf[64];
    if (sec < 1e-6) snprintf(buf, sizeof(buf), "%.0f ns", sec * 1e9);
    else if (sec < 1e-3) snprintf(buf, sizeof(buf), "%.1f us", sec * 1e6);
    else if (sec < 1) snprintf(buf, sizeof(buf), "%.1f ms", sec * 1e3);
    else if (sec < 3600) snprintf(buf, sizeof(buf), "%.1f s", sec);
    else snprintf(buf, sizeof(buf), "%.1f hours", sec / 3600);
    return string(buf);
}

static Steps simRealScaleComplexity(const Ints& input) {
    long long n = clamp((long long)(input.empty() ? 1000000000 : input[0]), 1LL, 2000000000LL);
    Steps steps;
    int logSteps = (int)ceil(log2((double)n + 1));
    steps.push_back(mk(0,
        "A search engine or a bank looks for one record among n = " + S(n) +
            " records. A computer does about a billion simple steps per second. How long does each method take?",
        {{"n", S(n)}}));
    steps.push_back(mk(0,
        "LINEAR search checks the records one by one: up to " + S(n) + " comparisons = about " +
            dsFormatOpDuration((double)n) + " for ONE lookup.",
        {{"comparisons", S(n)}, {"time per lookup", dsFormatOpDuration((double)n)}}));
    steps.push_back(mk(1,
        "BINARY search on sorted data halves the range each time: only about " + S(logSteps) +
            " comparisons = about " + dsFormatOpDuration((double)logSteps) + ".",
        {{"comparisons", S(logSteps)}, {"time per lookup", dsFormatOpDuration((double)logSteps)}}));
    double million = 1e6;
    steps.push_back(mk(2,
        "Now imagine 1,000,000 users searching at once. Linear: " + dsFormatOpDuration((double)n * million) +
            " of work in total. Binary: " + dsFormatOpDuration((double)logSteps * million) +
            ". One design is impossible, the other is easy.",
        {{"linear, 1M lookups", dsFormatOpDuration((double)n * million)},
         {"binary, 1M lookups", dsFormatOpDuration((double)logSteps * million)}}));
    Step summaryStep = mk(2,
        "Doubling n adds just ONE step to binary search. That is the meaning of O(log n), and why databases keep data sorted and indexed.",
        {{"extra steps if n doubles", "1"}});
    summaryStep.mat = {"n", "1 thousand", "1 million", "1 billion",
                       "linear", "1000", "1000000", "1000000000",
                       "binary", "10", "20", "30"};
    summaryStep.mc = 4;
    steps.push_back(summaryStep);
    return steps;
}

static void dsAddRealLifeProg(const char* topicName, const string& label) {
    programNames()[topicName].push_back(label);
}

void addDS8(map<string, Sim>& m) {
    dsRegisterIntSim(m, "Stack#3", "Commands: a positive number = open that page, 0 = press Back", "1, 2, 3, 0, 4, 0, 0, 0, 0",
                     {"openPage(p):  history.push(p);", "pressBack():   history.pop();   current = history.top();"},
                     m.at("Stack").refs, m.at("Stack").practice, simRealBrowserHistory);
    dsAddRealLifeProg("Stack", "Real life: browser Back button");

    dsRegisterIntSim(m, "Queue#2", "Pages of each print job (up to 6 jobs, 1 to 9 pages)", "5, 2, 8, 1, 3",
                     {"submit(job):   queue.enqueue(job);",
                      "printer:  while (queue not empty) {  job = queue.dequeue();  print(job);  }"},
                     m.at("Queue").refs, m.at("Queue").practice, simRealPrinterQueue);
    dsAddRealLifeProg("Queue", "Real life: office printer queue");

    dsRegisterIntSim(m, "Linked List#3", "Commands: 1 = next song, 2 = 'play next' (insert a new song after the current one)",
                     "1, 1, 2, 1, 1, 1",
                     {"next():       current = current->next;",
                      "playNext(s):  s->next = current->next;   current->next = s;      // O(1) insert"},
                     m.at("Linked List").refs, m.at("Linked List").practice, simRealPlaylist);
    dsAddRealLifeProg("Linked List", "Real life: music playlist (next, play next)");

    dsRegisterIntSim(m, "Priority Queue using Binary Heap#2",
                     "Commands: 1 to 9 = a patient arrives with that severity (1 = most urgent), 0 = doctor treats the next patient",
                     "4, 2, 5, 1, 0, 3, 0, 0, 0",
                     {"arrive(severity):  heap.insert(severity);",
                      "treatNext():  patient = heap.extractMin();    // smallest number = most urgent"},
                     m.at("Priority Queue using Binary Heap").refs, m.at("Priority Queue using Binary Heap").practice,
                     simRealEmergencyTriage);
    dsAddRealLifeProg("Priority Queue using Binary Heap", "Real life: hospital emergency triage");

    dsRegisterIntSim(m, "Breadth First Search (BFS)#2", "You (a vertex), then friendships as pairs",
                     "0, 0, 1, 0, 2, 1, 3, 2, 3, 2, 4, 3, 5, 4, 5, 4, 6",
                     {"friends = neighbours(you);                       // distance 1",
                      "for each friend f:  for each neighbour s of f:",
                      "    if (s is not you and s is not a friend)  suggest(s);     // distance 2"},
                     m.at("Breadth First Search (BFS)").refs, m.at("Breadth First Search (BFS)").practice,
                     simRealFriendSuggestions);
    dsAddRealLifeProg("Breadth First Search (BFS)", "Real life: friend suggestions");

    dsRegisterIntSim(m, "Shortest Path Algorithms#3", "From, to, then roads as triples u v minutes",
                     "0, 4, 0, 1, 7, 0, 2, 9, 0, 5, 14, 1, 2, 10, 1, 3, 15, 2, 3, 11, 2, 5, 2, 3, 4, 6, 4, 5, 9",
                     {"junctions = vertices;  roads = edges;  travel time = weight;",
                      "run Dijkstra from your location until the destination is settled;",
                      "route = follow the parents back from the destination;",
                      "ETA = dist[destination]"},
                     m.at("Shortest Path Algorithms").refs, m.at("Shortest Path Algorithms").practice,
                     simRealGpsNavigation);
    dsAddRealLifeProg("Shortest Path Algorithms", "Real life: GPS navigation");

    dsRegisterIntSim(m, "Minimum Spanning Tree (MST)#2", "Possible cable routes as triples town town cost (lakh rupees)",
                     "0, 1, 4, 0, 2, 3, 1, 2, 1, 1, 3, 2, 2, 3, 4, 3, 4, 2, 4, 5, 6",
                     {"routes = all possible cables, cheapest first;", "for (each route):",
                      "    if (it joins two towns that are NOT yet connected)  lay it;",
                      "    else  skip it;      // would be a wasted, redundant cable"},
                     m.at("Minimum Spanning Tree (MST)").refs, m.at("Minimum Spanning Tree (MST)").practice,
                     simRealCableNetwork);
    dsAddRealLifeProg("Minimum Spanning Tree (MST)", "Real life: laying a cable network");

    dsRegisterIntSim(m, "Tree Traversals#4", "Folder tree in level order: own file size in MB of each folder, -1 = no folder",
                     "5, 20, 10, 8, 30, -1, 12",
                     {"du(folder):", "    total = size of the files directly in the folder;",
                      "    for (each sub-folder f)  total += du(f);        // children first: POSTORDER",
                      "    return total;"},
                     m.at("Tree Traversals").refs, m.at("Tree Traversals").practice, simRealFolderDu);
    dsAddRealLifeProg("Tree Traversals", "Real life: folder size (the du command)");

    dsRegisterTextSim(m, "Tries#3", "words | what you typed   e.g.  apple app apply apt bat | app",
                      "apple app apply apt bat bar | app",
                      {"for (each letter you type):", "    node = child(node, letter);",
                       "    suggestions = all words below node;      // list narrows with every key press"},
                      m.at("Tries").refs, m.at("Tries").practice, simRealSearchSuggestions);
    dsAddRealLifeProg("Tries", "Real life: search-box suggestions");

    dsRegisterIntSim(m, "B+ Tree#2", "Roll number range low, high, then up to 12 roll numbers in the table",
                     "30, 60, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120",
                     {"SELECT * FROM students WHERE roll BETWEEN low AND high;",
                      "without an index:  read EVERY row of the table;",
                      "with a B+ tree index:  descend to the first matching leaf, then follow the leaf chain"},
                     m.at("B+ Tree").refs, m.at("B+ Tree").practice, simRealDatabaseIndex);
    dsAddRealLifeProg("B+ Tree", "Real life: database index vs full table scan");

    dsRegisterIntSim(m, "Interpolation Search#2", "The secret number (1 to 100) the computer must guess", "73",
                     {"lo = 1;  hi = 100;", "while (true) {", "    guess = (lo + hi) / 2;",
                      "    if (guess == secret)  break;",
                      "    if (guess < secret)  lo = guess + 1;   else  hi = guess - 1;", "}"},
                     m.at("Interpolation Search").refs, m.at("Interpolation Search").practice, simRealGuessNumber);
    dsAddRealLifeProg("Interpolation Search", "Real life: guess-the-number game (binary search)");

    dsRegisterIntSim(m, "Time Complexity Analysis#3", "Number of items n to search (1 to 2,000,000,000)", "1000000000",
                     {"linear search:   up to n comparisons          O(n)",
                      "binary search:   about log2(n) comparisons   O(log n)",
                      "time = comparisons / 1,000,000,000 per second"},
                     m.at("Time Complexity Analysis").refs, m.at("Time Complexity Analysis").practice,
                     simRealScaleComplexity);
    dsAddRealLifeProg("Time Complexity Analysis", "Real life: why search is instant at scale");
}
