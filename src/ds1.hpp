// Data Structures (Semester 3), part 1: Module 1 (linear structures) and searching.
#pragma once
#include <climits>
#include "simulations9.hpp"

using namespace std;

// ==================== Shared Helpers for Data Structures Lab ====================

static Ints dsTakeInts(const Ints& input, size_t startIndex, size_t maxCount, Ints defaultValues) {
    Ints result;
    for (size_t i = startIndex; i < input.size() && result.size() < maxCount; ++i) {
        result.push_back(input[i]);
    }
    return result.empty() ? defaultValues : result;
}

static vector<string> dsMakeTags(size_t totalSize, vector<pair<int, string>> entries) {
    vector<string> tags(totalSize);
    for (size_t i = 0; i < entries.size(); ++i) {
        int pos = entries[i].first;
        const string& label = entries[i].second;
        if (pos >= 0 && pos < (int)totalSize) {
            if (!tags[pos].empty()) tags[pos] += "/";
            tags[pos] += label;
        }
    }
    return tags;
}

static Step dsArrayStep(int lineNum, const string& noteText, Vars varsList, const Ints& arrayData,
                        int highlightIdx, vector<string> tags = {}, int rangeLo = -1, int rangeHi = -1,
                        const char* arrayName = "a") {
    Step step = mkArr(lineNum, noteText, varsList, arrayData, highlightIdx);
    step.tags = tags;
    step.rl = rangeLo;
    step.rh = rangeHi;
    step.nm = arrayName;
    return step;
}

static vector<int> dsSequentialLinks(int count) {
    vector<int> links(count);
    for (int i = 0; i < count; ++i) {
        links[i] = (i + 1 < count) ? (i + 1) : -1;
    }
    return links;
}

static string dsFormatInt(long long value) {
    if (value == INT_MIN) return "-inf";
    return to_string(value);
}

static double dsListNodeX(int index) {
    const double startX = 56.0;
    const double stepX = 64.0;  // nodeWidth (40) + gap (24)
    return startX + index * stepX;
}

// Drawing of a singly linked list. nextLinks[i]: i+1 forward, i-1 backward, -1 NULL, -2 not set.
static string dsDrawLinkedList(const vector<string>& values, const vector<int>& nextLinks,
                               const vector<string>& pointerLabels, int headIdx, const string& caption,
                               const string& newValue = "", int newSlot = -1, int newNext = -2) {
    Sk canvas;
    int count = (int)values.size();
    const double boxW = 40.0;
    const double boxH = 34.0;
    const double baseY = 110.0;

    for (int i = 0; i < count; ++i) {
        string nodeId = "n" + S(i);
        double x = dsListNodeX(i);
        canvas.Rect(nodeId.c_str(), x, baseY, boxW, boxH, "steel2");
        canvas.T(x + boxW / 2, baseY + boxH / 2, values[i]);
    }

    for (int i = 0; i < count; ++i) {
        int target = (i < (int)nextLinks.size()) ? nextLinks[i] : -2;
        string linkId = "l" + S(i);
        double x = dsListNodeX(i);
        if (target == i + 1 && i + 1 < count) {
            canvas.Arr(x + boxW, baseY + boxH * 0.35, dsListNodeX(i + 1), baseY + boxH * 0.35, linkId.c_str());
        } else if (target == i - 1 && i > 0) {
            canvas.Arr(x, baseY + boxH * 0.72, dsListNodeX(i - 1) + boxW, baseY + boxH * 0.72, linkId.c_str());
        } else if (target == -1) {
            if (i == count - 1) {
                canvas.Arr(x + boxW, baseY + boxH / 2, x + boxW + 14, baseY + boxH / 2, linkId.c_str());
                canvas.T(x + boxW + 18, baseY + boxH / 2, "NULL", "start");
            } else if (i == 0) {
                canvas.Arr(dsListNodeX(0), baseY + boxH / 2, dsListNodeX(0) - 14, baseY + boxH / 2, linkId.c_str());
                canvas.T(dsListNodeX(0) - 18, baseY + boxH / 2, "NULL", "end");
            } else {
                canvas.Arr(x + boxW / 2, baseY + boxH, x + boxW / 2, baseY + boxH + 10, linkId.c_str());
                canvas.T(x + boxW / 2, baseY + boxH + 22, "NULL");
            }
        }
    }

    for (int i = 0; i < count && i < (int)pointerLabels.size(); ++i) {
        if (!pointerLabels[i].empty()) {
            canvas.T(dsListNodeX(i) + boxW / 2, baseY + boxH + 18, pointerLabels[i]);
        }
    }

    if (headIdx >= 0 && headIdx < count) {
        double hx = dsListNodeX(headIdx) + boxW / 2;
        canvas.T(hx, baseY - 30, "head");
        canvas.Arr(hx, baseY - 22, hx, baseY - 2, "head");
    }

    if (!newValue.empty()) {
        int slot = (newSlot >= 0) ? min(newSlot, 7) : min(count, 7);
        double newX = dsListNodeX(slot);
        double newY = baseY - 80;
        canvas.Rect("nw", newX, newY, boxW, boxH, "weld");
        canvas.T(newX + boxW / 2, newY + boxH / 2, newValue);
        canvas.T(newX - 6, newY + boxH / 2, "new", "end");
        if (newNext >= 0 && newNext < count) {
            canvas.Arr(newX + boxW / 2, newY + boxH, dsListNodeX(newNext) + 8, baseY - 2, "nl");
        } else if (newNext == -1) {
            canvas.Arr(newX + boxW, newY + boxH / 2, newX + boxW + 14, newY + boxH / 2, "nl");
            canvas.T(newX + boxW + 18, newY + boxH / 2, "NULL", "start");
        }
    }

    double width = max(300.0, dsListNodeX(max(count, newSlot + 1)) + 74);
    canvas.T(width / 2, 186, caption);
    return canvas.out((int)width, 204, {});
}

static Step dsDiagramStep(int lineNum, const string& noteText, Vars varsList,
                          const string& diagramJson, vector<string> highlightIds) {
    Step step = mk(lineNum, noteText, varsList);
    step.dg = diagramJson;
    step.hl = highlightIds;
    return step;
}

static vector<string> dsToStringList(const Ints& arr) {
    vector<string> result;
    for (size_t i = 0; i < arr.size(); ++i) {
        result.push_back(S(arr[i]));
    }
    return result;
}

static void dsRegisterIntSim(map<string, Sim>& simMap, const string& key, const char* label,
                             const char* defaultInput, vector<string> code, vector<string> refs,
                             vector<string> practice, Steps (*runner)(const Ints&)) {
    simMap[key] = Sim{label, defaultInput, code, refs, practice, runner, nullptr, "", {}};
}

static void dsRegisterTextSim(map<string, Sim>& simMap, const string& key, const char* label,
                              const char* defaultInput, vector<string> code, vector<string> refs,
                              vector<string> practice, Steps (*runner)(const string&)) {
    simMap[key] = Sim{label, defaultInput, code, refs, practice, nullptr, runner, "", {}};
}

static void dsSetProgramNames(const char* topic, vector<string> names) {
    programNames()[topic] = names;
}

// ==================== Module 1 Simulations ====================

static Steps simOverviewArrayInsert(const Ints& input) {
    int newValue = input.empty() ? 5 : input[0];
    Ints items = dsTakeInts(input, 1, 6, {20, 30, 40, 50});
    int count = (int)items.size();
    Ints arr = items;
    arr.push_back(INT_MIN);

    Steps steps;
    int moves = 0;
    steps.push_back(dsArrayStep(0, "The array holds " + S(count) + " items and has one spare cell at the end. To add " + S(newValue) + " at the FRONT, every item must first move one place right.",
                                {{"n", S(count)}, {"x", S(newValue)}}, arr, -1));
    for (int i = count; i > 0; --i) {
        arr[i] = arr[i - 1];
        ++moves;
        steps.push_back(dsArrayStep(1, "a[" + S(i) + "] = a[" + S(i - 1) + "] copies " + S(arr[i]) + " one place right. Moves so far: " + S(moves) + ".",
                                    {{"i", S(i)}, {"moves", S(moves)}}, arr, i, dsMakeTags(arr.size(), {{i, "i"}})));
    }
    arr[0] = newValue;
    steps.push_back(dsArrayStep(2, "Cell 0 is free now: a[0] = " + S(newValue) + ".", {{"moves", S(moves)}}, arr, 0));
    steps.push_back(dsArrayStep(3, "n becomes " + S(count + 1) + ". Cost: " + S(moves) + " moves. It grows with n, so inserting at the front of an array is O(n).",
                                {{"n", S(count + 1)}, {"moves", S(moves)}}, arr, -1));
    return steps;
}

static Steps simOverviewLinkedListInsert(const Ints& input) {
    int newValue = input.empty() ? 5 : input[0];
    Ints items = dsTakeInts(input, 1, 6, {20, 30, 40, 50});
    int count = (int)items.size();
    vector<string> valStrings = dsToStringList(items);

    Steps steps;
    steps.push_back(dsDiagramStep(0, "Create a new node holding " + S(newValue) + ". It is not part of the list yet.",
                                  {{"x", S(newValue)}},
                                  dsDrawLinkedList(valStrings, dsSequentialLinks(count), {}, 0,
                                                   "linked list: each node stores a value and the address of the next node",
                                                   S(newValue), 0, -2),
                                  {"nw"}));
    steps.push_back(dsDiagramStep(1, "n->next = head: the new node now points at the old first node. One pointer written.",
                                  {{"n->next", "old head"}},
                                  dsDrawLinkedList(valStrings, dsSequentialLinks(count), {}, 0,
                                                   "new node points to the old first node",
                                                   S(newValue), 0, 0),
                                  {"nl"}));
    vector<string> updatedVals = valStrings;
    updatedVals.insert(updatedVals.begin(), S(newValue));
    steps.push_back(dsDiagramStep(2, "head = n: head moves to the new node. Two pointer writes in total, however long the list is: O(1).",
                                  {{"head", "n"}},
                                  dsDrawLinkedList(updatedVals, dsSequentialLinks(count + 1), {}, 0,
                                                   "inserted at the front without moving any other node"),
                                  {"n0", "l0", "head"}));
    return steps;
}

static Steps simOverviewStackQueue(const Ints& input) {
    Ints items = dsTakeInts(input, 0, 5, {10, 20, 30, 40});
    int count = (int)items.size();
    Ints stackArr(count, INT_MIN);
    Steps steps;
    string poppedOut;
    int top = -1;

    for (size_t i = 0; i < items.size(); ++i) {
        int val = items[i];
        stackArr[++top] = val;
        steps.push_back(dsArrayStep(0, "STACK push(" + S(val) + "): goes on the top, top = " + S(top) + ".",
                                    {{"top", S(top)}}, stackArr, top, dsMakeTags(count, {{top, "top"}}), 0, top, "stack"));
    }
    while (top >= 0) {
        int val = stackArr[top];
        if (!poppedOut.empty()) poppedOut += ", ";
        poppedOut += S(val);
        --top;
        steps.push_back(dsArrayStep(1, "STACK pop() returns " + S(val) + ", the LAST item pushed (last in, first out).",
                                    {{"popped", poppedOut}}, stackArr, top >= 0 ? top : -1,
                                    dsMakeTags(count, {{top, "top"}}), 0, top, "stack"));
    }

    string stackOrder = poppedOut;
    string servedOut;
    int front = 0;
    int rear = -1;
    Ints queueArr(count, INT_MIN);

    for (size_t i = 0; i < items.size(); ++i) {
        int val = items[i];
        queueArr[++rear] = val;
        steps.push_back(dsArrayStep(2, "QUEUE enqueue(" + S(val) + "): joins at the rear, rear = " + S(rear) + ".",
                                    {{"front", S(front)}, {"rear", S(rear)}}, queueArr, rear,
                                    dsMakeTags(count, {{front, "front"}, {rear, "rear"}}), front, rear, "queue"));
    }
    while (front <= rear) {
        int val = queueArr[front];
        if (!servedOut.empty()) servedOut += ", ";
        servedOut += S(val);
        ++front;
        steps.push_back(dsArrayStep(3, "QUEUE dequeue() returns " + S(val) + ", the FIRST item added (first in, first out).",
                                    {{"served", servedOut}}, queueArr, front <= rear ? front : -1,
                                    dsMakeTags(count, {{front, "front"}, {rear, "rear"}}), front, rear, "queue"));
    }
    steps.push_back(dsArrayStep(3, "Same items, same order in. The stack gave back " + stackOrder + " (reversed); the queue gave back " + servedOut + " (same order).",
                                {{"stack out", stackOrder}, {"queue out", servedOut}}, queueArr, -1, {}, front, rear, "queue"));
    return steps;
}

static Steps simArrayInsert(const Ints& input) {
    Ints items = dsTakeInts(input, 2, 7, {10, 20, 30, 40, 50});
    int count = (int)items.size();
    int pos = clamp(input.empty() ? 2 : input[0], 0, count);
    int newValue = (input.size() > 1) ? input[1] : 99;
    Ints arr = items;
    arr.push_back(INT_MIN);

    Steps steps;
    steps.push_back(dsArrayStep(0, "Insert " + S(newValue) + " at index " + S(pos) + ". Every item from index " + S(pos) + " onward must move one place right, starting from the LAST one (otherwise values are overwritten).",
                                {{"n", S(count)}, {"pos", S(pos)}, {"x", S(newValue)}}, arr, pos,
                                dsMakeTags(arr.size(), {{pos, "pos"}})));
    for (int i = count; i > pos; --i) {
        arr[i] = arr[i - 1];
        steps.push_back(dsArrayStep(1, "a[" + S(i) + "] = a[" + S(i - 1) + "]: " + S(arr[i]) + " moves right.",
                                    {{"i", S(i)}}, arr, i, dsMakeTags(arr.size(), {{i, "i"}, {pos, "pos"}})));
    }
    if (pos == count) {
        steps.push_back(dsArrayStep(0, "pos equals n, so nothing has to move: the new item goes in the spare cell.",
                                    {{"pos", S(pos)}}, arr, pos));
    }
    arr[pos] = newValue;
    steps.push_back(dsArrayStep(2, "a[" + S(pos) + "] = " + S(newValue) + ". The gap is filled.", {{"pos", S(pos)}}, arr, pos));
    steps.push_back(dsArrayStep(3, "n becomes " + S(count + 1) + ". " + S(count - pos) + " items moved: worst case O(n) (insert at 0), best case O(1) (insert at the end).",
                                {{"n", S(count + 1)}}, arr, -1));
    return steps;
}

static Steps simArrayDelete(const Ints& input) {
    Ints arr = dsTakeInts(input, 1, 8, {10, 20, 30, 40, 50});
    int count = (int)arr.size();
    int pos = clamp(input.empty() ? 1 : input[0], 0, count - 1);
    int removedVal = arr[pos];

    Steps steps;
    steps.push_back(dsArrayStep(0, "Delete the item at index " + S(pos) + ", which is " + S(removedVal) + ". Remember it, then close the gap by moving later items LEFT.",
                                {{"pos", S(pos)}, {"x", S(removedVal)}}, arr, pos, dsMakeTags(count, {{pos, "pos"}})));
    for (int i = pos; i < count - 1; ++i) {
        arr[i] = arr[i + 1];
        steps.push_back(dsArrayStep(2, "a[" + S(i) + "] = a[" + S(i + 1) + "]: " + S(arr[i]) + " moves left.",
                                    {{"i", S(i)}}, arr, i, dsMakeTags(count, {{i, "i"}, {pos, "pos"}})));
    }
    arr[count - 1] = INT_MIN;
    steps.push_back(dsArrayStep(3, "n-- : the last cell is no longer part of the array. " + S(count - 1 - pos) + " items moved: O(n) worst case.",
                                {{"n", S(count - 1)}}, arr, -1, {}, 0, count - 2));
    return steps;
}

static Steps simArrayTopTwo(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 8, {12, 35, 1, 10, 34, 1});
    if (arr.size() < 2) arr = {12, 35};
    int count = (int)arr.size();
    int max1 = arr[0];
    int max2 = INT_MIN;

    Steps steps;
    steps.push_back(dsArrayStep(0, "One pass is enough. max1 = a[0] = " + S(max1) + ", max2 = -inf.",
                                {{"max1", dsFormatInt(max1)}, {"max2", dsFormatInt(max2)}}, arr, 0));
    for (int i = 1; i < count; ++i) {
        if (arr[i] > max1) {
            max2 = max1;
            max1 = arr[i];
            steps.push_back(dsArrayStep(2, S(arr[i]) + " > max1, so the old max1 becomes max2 and " + S(arr[i]) + " is the new max1.",
                                        {{"max1", dsFormatInt(max1)}, {"max2", dsFormatInt(max2)}}, arr, i, dsMakeTags(count, {{i, "i"}})));
        } else if (arr[i] > max2 && arr[i] != max1) {
            max2 = arr[i];
            steps.push_back(dsArrayStep(3, S(arr[i]) + " is not above max1 but beats max2 (and differs from max1), so max2 = " + S(max2) + ".",
                                        {{"max1", dsFormatInt(max1)}, {"max2", dsFormatInt(max2)}}, arr, i, dsMakeTags(count, {{i, "i"}})));
        } else {
            steps.push_back(dsArrayStep(3, S(arr[i]) + " changes nothing (not above max1, and not a new second largest).",
                                        {{"max1", dsFormatInt(max1)}, {"max2", dsFormatInt(max2)}}, arr, i, dsMakeTags(count, {{i, "i"}})));
        }
    }
    string summary = (max2 == INT_MIN) ? "Every value is the same, so there is no second largest."
                                       : "Largest = " + S(max1) + ", second largest = " + S(max2) + ". One pass, O(n) time, O(1) space.";
    steps.push_back(dsArrayStep(4, summary, {{"max1", dsFormatInt(max1)}, {"max2", dsFormatInt(max2)}}, arr, -1));
    return steps;
}

static vector<string> makeSinglePointerLabel(int totalSize, int index, const string& name) {
    vector<string> labels(totalSize);
    if (index >= 0 && index < totalSize) labels[index] = name;
    return labels;
}

static Steps simLinkedListSearch(const Ints& input) {
    int key = input.empty() ? 40 : input[0];
    Ints arr = dsTakeInts(input, 1, 7, {10, 20, 30, 40, 50});
    int count = (int)arr.size();
    vector<string> valStrings = dsToStringList(arr);
    vector<int> links = dsSequentialLinks(count);
    Steps steps;
    bool wasFound = false;

    steps.push_back(dsDiagramStep(0, "cur starts at the head, the first node. Looking for key = " + S(key) + ".",
                                  {{"key", S(key)}},
                                  dsDrawLinkedList(valStrings, links, makeSinglePointerLabel(count, 0, "cur"), 0,
                                                   "search = follow the next pointers one node at a time"),
                                  {"n0"}));
    for (int i = 0; i < count; ++i) {
        steps.push_back(dsDiagramStep(1, "cur is not NULL, so enter the loop.",
                                      {{"cur->data", S(arr[i])}},
                                      dsDrawLinkedList(valStrings, links, makeSinglePointerLabel(count, i, "cur"), 0,
                                                       "visiting node " + S(i)),
                                      {"n" + S(i)}));
        bool matched = (arr[i] == key);
        steps.push_back(dsDiagramStep(2, "Is cur->data == key ? " + S(arr[i]) + " == " + S(key) + " is " + (matched ? "true: return this node." : "false."),
                                      {{"cur->data", S(arr[i])}, {"key", S(key)}},
                                      dsDrawLinkedList(valStrings, links, makeSinglePointerLabel(count, i, "cur"), 0,
                                                       matched ? "found at node " + S(i) : "no match here"),
                                      {"n" + S(i)}));
        if (matched) {
            wasFound = true;
            break;
        }
        steps.push_back(dsDiagramStep(3, "cur = cur->next moves to the next node" + string(i + 1 < count ? "." : ": that is NULL, the end of the list."),
                                      {{"cur", i + 1 < count ? "node " + S(i + 1) : "NULL"}},
                                      dsDrawLinkedList(valStrings, links, makeSinglePointerLabel(count, i + 1, "cur"), 0,
                                                       "follow the pointer"),
                                      {"l" + S(i)}));
    }
    if (!wasFound) {
        steps.push_back(dsDiagramStep(5, "cur is NULL: every node was checked. Return NULL. Search in a linked list is O(n); there is no jump to the middle.",
                                      {{"result", "NULL"}},
                                      dsDrawLinkedList(valStrings, links, {}, 0, "not found"),
                                      {}));
    }
    return steps;
}

static Steps simLinkedListInsert(const Ints& input) {
    Ints arr = dsTakeInts(input, 2, 6, {10, 20, 30, 40});
    int count = (int)arr.size();
    int pos = clamp(input.empty() ? 2 : input[0], 0, count);
    int newValue = (input.size() > 1) ? input[1] : 99;
    vector<string> valStrings = dsToStringList(arr);
    vector<int> links = dsSequentialLinks(count);
    Steps steps;

    steps.push_back(dsDiagramStep(0, "Create a new node holding " + S(newValue) + ". It will become index " + S(pos) + ". Not linked yet.",
                                  {{"x", S(newValue)}, {"pos", S(pos)}},
                                  dsDrawLinkedList(valStrings, links, {}, 0, "insert " + S(newValue) + " at position " + S(pos), S(newValue), pos, -2),
                                  {"nw"}));

    vector<string> finalVals = valStrings;
    finalVals.insert(finalVals.begin() + pos, S(newValue));

    if (pos == 0) {
        steps.push_back(dsDiagramStep(1, "pos == 0: n->next = head, then head = n. No walking needed.",
                                      {{"pos", "0"}},
                                      dsDrawLinkedList(valStrings, links, {}, 0, "front insert", S(newValue), 0, count > 0 ? 0 : -1),
                                      {"nl"}));
        steps.push_back(dsDiagramStep(1, "head now points to the new node.",
                                      {{"head", "n"}},
                                      dsDrawLinkedList(finalVals, dsSequentialLinks(count + 1), {}, 0, "done"),
                                      {"n0", "head"}));
        return steps;
    }

    steps.push_back(dsDiagramStep(2, "p starts at the head. We must stop at the node BEFORE the insert position (index " + S(pos - 1) + ").",
                                  {{"p", "node 0"}},
                                  dsDrawLinkedList(valStrings, links, dsMakeTags(count, {{0, "p"}}), 0, "walk to index " + S(pos - 1), S(newValue), pos, -2),
                                  {"n0"}));
    for (int i = 0; i < pos - 1; ++i) {
        steps.push_back(dsDiagramStep(4, "p = p->next moves to node " + S(i + 1) + ".",
                                      {{"i", S(i)}, {"p", "node " + S(i + 1)}},
                                      dsDrawLinkedList(valStrings, links, dsMakeTags(count, {{i + 1, "p"}}), 0, "p moves right", S(newValue), pos, -2),
                                      {"n" + S(i + 1)}));
    }
    steps.push_back(dsDiagramStep(5, "n->next = p->next: the new node points to what p pointed to. Do this FIRST, or the rest of the list is lost.",
                                  {{"n->next", pos < count ? "node " + S(pos) : "NULL"}},
                                  dsDrawLinkedList(valStrings, links, dsMakeTags(count, {{pos - 1, "p"}}), 0, "link the new node forward first", S(newValue), pos, pos < count ? pos : -1),
                                  {"nl"}));
    steps.push_back(dsDiagramStep(6, "p->next = n: the previous node now points to the new node. The list has " + S(count + 1) + " nodes.",
                                  {{"p->next", "n"}},
                                  dsDrawLinkedList(finalVals, dsSequentialLinks(count + 1), dsMakeTags(count + 1, {{pos, "new"}}), 0, "done: only pointers changed, no shifting"),
                                  {"n" + S(pos), "l" + S(pos - 1)}));
    return steps;
}

static vector<string> makeReversePointerLabels(int count, int prevIdx, int curIdx, int nextIdx) {
    vector<string> labels(count);
    if (prevIdx >= 0 && prevIdx < count) labels[prevIdx] += "prev";
    if (curIdx >= 0 && curIdx < count) {
        if (!labels[curIdx].empty()) labels[curIdx] += "/";
        labels[curIdx] += "cur";
    }
    if (nextIdx >= 0 && nextIdx < count) {
        if (!labels[nextIdx].empty()) labels[nextIdx] += "/";
        labels[nextIdx] += "next";
    }
    return labels;
}

static Steps simLinkedListReverse(const Ints& input) {
    Ints arr = dsTakeInts(input, 0, 7, {10, 20, 30, 40});
    int count = (int)arr.size();
    vector<string> valStrings = dsToStringList(arr);
    vector<int> nextLinks = dsSequentialLinks(count);
    Steps steps;

    steps.push_back(dsDiagramStep(0, "prev = NULL, cur = head. We will turn every arrow around, one node at a time.",
                                  {{"prev", "NULL"}, {"cur", "node 0"}},
                                  dsDrawLinkedList(valStrings, nextLinks, makeReversePointerLabels(count, -1, 0, -1), 0, "before reversing"),
                                  {"n0"}));
    for (int curIdx = 0; curIdx < count; ++curIdx) {
        steps.push_back(dsDiagramStep(2, "next = cur->next saves the rest of the list before we break the link.",
                                      {{"cur", "node " + S(curIdx)}, {"next", curIdx + 1 < count ? "node " + S(curIdx + 1) : "NULL"}},
                                      dsDrawLinkedList(valStrings, nextLinks, makeReversePointerLabels(count, curIdx - 1, curIdx, curIdx + 1),
                                                       curIdx == 0 ? 0 : -1, "save next"),
                                      {"n" + S(curIdx)}));
        nextLinks[curIdx] = (curIdx == 0) ? -1 : (curIdx - 1);
        steps.push_back(dsDiagramStep(3, "cur->next = prev: the arrow of node " + S(curIdx) + " now points " + (curIdx == 0 ? "to NULL." : "backwards."),
                                      {{"cur->next", curIdx == 0 ? "NULL" : "node " + S(curIdx - 1)}},
                                      dsDrawLinkedList(valStrings, nextLinks, makeReversePointerLabels(count, curIdx - 1, curIdx, curIdx + 1),
                                                       -1, "arrow turned around"),
                                      {"l" + S(curIdx)}));
        steps.push_back(dsDiagramStep(4, "prev = cur; cur = next: both pointers step one node to the right.",
                                      {{"prev", "node " + S(curIdx)}, {"cur", curIdx + 1 < count ? "node " + S(curIdx + 1) : "NULL"}},
                                      dsDrawLinkedList(valStrings, nextLinks, makeReversePointerLabels(count, curIdx, curIdx + 1, -1),
                                                       -1, "move prev and cur"),
                                      {"n" + S(curIdx)}));
    }
    steps.push_back(dsDiagramStep(6, "cur is NULL, so stop. head = prev: the old last node is the new head. One pass, O(n) time, O(1) extra space.",
                                  {{"head", "node " + S(count - 1)}},
                                  dsDrawLinkedList(valStrings, nextLinks, {}, count - 1, "reversed in place"),
                                  {"head", "n" + S(count - 1)}));
    return steps;
}

static Steps simStackPushPop(const Ints& input) {
    Ints commands = dsTakeInts(input, 0, 14, {5, 8, 3, 0, 0, 7, 0, 0, 0});
    const int MAX_CAP = 6;
    Ints stackArr(MAX_CAP, INT_MIN);
    int top = -1;
    Steps steps;

    steps.push_back(dsArrayStep(0, "Empty stack of capacity " + S(MAX_CAP) + ". top = -1 means no item. A positive number = push, 0 = pop.",
                                {{"top", "-1"}}, stackArr, -1, {}, 0, -1, "stack"));
    for (size_t i = 0; i < commands.size(); ++i) {
        int cmd = commands[i];
        if (cmd != 0) {
            steps.push_back(dsArrayStep(1, "push(" + S(cmd) + "): is top == MAX - 1 ? " + S(top) + " == " + S(MAX_CAP - 1) + " is " + (top == MAX_CAP - 1 ? "true." : "false."),
                                        {{"top", S(top)}}, stackArr, top, dsMakeTags(MAX_CAP, {{top, "top"}}), 0, top, "stack"));
            if (top == MAX_CAP - 1) {
                steps.push_back(dsArrayStep(1, "OVERFLOW: the stack is full, " + S(cmd) + " is rejected.",
                                            {{"top", S(top)}}, stackArr, top, dsMakeTags(MAX_CAP, {{top, "top"}}), 0, top, "stack"));
                continue;
            }
            stackArr[++top] = cmd;
            steps.push_back(dsArrayStep(2, "a[++top] = " + S(cmd) + ": top is now " + S(top) + ".",
                                        {{"top", S(top)}}, stackArr, top, dsMakeTags(MAX_CAP, {{top, "top"}}), 0, top, "stack"));
        } else {
            steps.push_back(dsArrayStep(4, "pop(): is top == -1 ? " + S(top) + " == -1 is " + (top == -1 ? "true." : "false."),
                                        {{"top", S(top)}}, stackArr, top, dsMakeTags(MAX_CAP, {{top, "top"}}), 0, top, "stack"));
            if (top == -1) {
                steps.push_back(dsArrayStep(4, "UNDERFLOW: nothing to pop.", {{"top", "-1"}}, stackArr, -1, {}, 0, -1, "stack"));
                continue;
            }
            int poppedVal = stackArr[top--];
            steps.push_back(dsArrayStep(5, "x = a[top--] returns " + S(poppedVal) + ". The cell keeps its old value (dimmed) but is no longer inside the stack.",
                                        {{"x", S(poppedVal)}, {"top", S(top)}}, stackArr, -1, dsMakeTags(MAX_CAP, {{top, "top"}}), 0, top, "stack"));
        }
    }
    steps.push_back(dsArrayStep(0, "Every command is done. Size = top + 1 = " + S(top + 1) + ". push and pop are both O(1).",
                                {{"size", S(top + 1)}}, stackArr, -1, dsMakeTags(MAX_CAP, {{top, "top"}}), 0, top, "stack"));
    return steps;
}

static Step makeCharStackStep(int lineNum, const string& noteText, Vars varsList, const Ints& stackArr, int highlightIdx) {
    vector<string> tags;
    if (!stackArr.empty()) {
        tags = dsMakeTags(stackArr.size(), {{(int)stackArr.size() - 1, "top"}});
    }
    Step step = dsArrayStep(lineNum, noteText, varsList, stackArr, highlightIdx, tags, -1, -1, "stack");
    step.chr = true;
    return step;
}

static Steps simStackBrackets(const string& input) {
    string brackets;
    for (size_t i = 0; i < input.size(); ++i) {
        char ch = input[i];
        if (string("()[]{}").find(ch) != string::npos && brackets.size() < 14) {
            brackets += ch;
        }
    }
    if (brackets.empty()) brackets = "{[()]}";

    Steps steps;
    Ints charStack;
    steps.push_back(makeCharStackStep(0, "Check the string \"" + brackets + "\" one character at a time. Opening brackets wait on the stack; a closing bracket must match the top.",
                                      {{"s", brackets}}, charStack, -1));
    for (size_t i = 0; i < brackets.size(); ++i) {
        char ch = brackets[i];
        bool isOpen = (ch == '(' || ch == '[' || ch == '{');
        Vars vars = {{"c", string(1, ch)}, {"index", S((int)i)}};
        if (isOpen) {
            charStack.push_back(ch);
            steps.push_back(makeCharStackStep(1, "'" + string(1, ch) + "' is an opening bracket: push it.", vars, charStack, (int)charStack.size() - 1));
            continue;
        }
        char needed = (ch == ')') ? '(' : (ch == ']' ? '[' : '{');
        if (charStack.empty()) {
            steps.push_back(makeCharStackStep(2, "'" + string(1, ch) + "' needs a '" + string(1, needed) + "' but the stack is empty. NOT balanced.", vars, charStack, -1));
            return steps;
        }
        if (charStack.back() != needed) {
            steps.push_back(makeCharStackStep(2, "'" + string(1, ch) + "' needs '" + string(1, needed) + "' on top but the top is '" + string(1, (char)charStack.back()) + "'. NOT balanced.",
                                              vars, charStack, (int)charStack.size() - 1));
            return steps;
        }
        charStack.pop_back();
        steps.push_back(makeCharStackStep(3, "'" + string(1, ch) + "' matches the top '" + string(1, needed) + "': pop it.", vars, charStack, -1));
    }
    steps.push_back(makeCharStackStep(4, charStack.empty() ? "The stack is empty: every bracket was matched. BALANCED."
                                                           : "The string ended but " + S((int)charStack.size()) + " opening bracket(s) are still waiting. NOT balanced.",
                                      {{"stack size", S((int)charStack.size())}}, charStack, -1));
    return steps;
}

static vector<string> makeTopTags(const Ints& stackArr) {
    if (stackArr.empty()) return {};
    return dsMakeTags(stackArr.size(), {{(int)stackArr.size() - 1, "top"}});
}

static Steps simStackPostfix(const string& input) {
    vector<string> tokens;
    istringstream stream(input);
    string word;
    while (stream >> word && tokens.size() < 12) {
        bool isOp = (word.size() == 1 && string("+-*/").find(word[0]) != string::npos);
        bool isNum = (!word.empty() && word.size() < 8);
        size_t start = (word[0] == '-' && word.size() > 1) ? 1 : 0;
        for (size_t k = start; k < word.size(); ++k) {
            if (!isdigit((unsigned char)word[k])) isNum = false;
        }
        if (isOp || isNum) tokens.push_back(word);
    }
    if (tokens.empty()) tokens = {"2", "3", "1", "*", "+", "9", "-"};

    Steps steps;
    Ints valStack;
    steps.push_back(dsArrayStep(0, "Postfix needs no brackets: numbers wait on the stack, an operator uses the top two.",
                                {{"tokens", S((int)tokens.size())}}, valStack, -1, {}, -1, -1, "stack"));
    for (size_t i = 0; i < tokens.size(); ++i) {
        const string& tok = tokens[i];
        bool isOp = (tok.size() == 1 && string("+-*/").find(tok[0]) != string::npos);
        if (!isOp) {
            valStack.push_back(clamp(atoi(tok.c_str()), -1000000000, 1000000000));
            steps.push_back(dsArrayStep(1, tok + " is a number: push it.", {{"token", tok}}, valStack,
                                        (int)valStack.size() - 1, makeTopTags(valStack), -1, -1, "stack"));
            continue;
        }
        if (valStack.size() < 2) {
            steps.push_back(dsArrayStep(2, "'" + tok + "' needs two numbers but the stack has " + S((int)valStack.size()) + ". The expression is invalid.",
                                        {{"token", tok}}, valStack, -1, makeTopTags(valStack), -1, -1, "stack"));
            return steps;
        }
        long long rightOperand = valStack.back();
        valStack.pop_back();
        long long leftOperand = valStack.back();
        valStack.pop_back();
        if (tok == "/" && rightOperand == 0) {
            steps.push_back(dsArrayStep(2, "Division by zero: stop.", {{"a", S((int)leftOperand)}, {"b", "0"}},
                                        valStack, -1, makeTopTags(valStack), -1, -1, "stack"));
            return steps;
        }
        long long res = 0;
        if (tok == "+") res = leftOperand + rightOperand;
        else if (tok == "-") res = leftOperand - rightOperand;
        else if (tok == "*") res = leftOperand * rightOperand;
        else res = leftOperand / rightOperand;
        res = clamp(res, -1000000000LL, 1000000000LL);
        valStack.push_back((int)res);
        steps.push_back(dsArrayStep(3, "b = " + S((int)rightOperand) + " (popped first), a = " + S((int)leftOperand) + ". Compute a " + tok + " b = " + to_string(res) + " and push it back.",
                                    {{"a", S((int)leftOperand)}, {"b", S((int)rightOperand)}, {"a " + tok + " b", to_string(res)}},
                                    valStack, (int)valStack.size() - 1, makeTopTags(valStack), -1, -1, "stack"));
    }
    steps.push_back(dsArrayStep(4, valStack.size() == 1 ? "One value is left: the answer is " + S(valStack.back()) + "."
                                                        : "The stack should end with exactly one value, but it has " + S((int)valStack.size()) + ". The expression is invalid.",
                                {{"stack size", S((int)valStack.size())}}, valStack, valStack.empty() ? -1 : 0, makeTopTags(valStack), -1, -1, "stack"));
    return steps;
}

static Steps simQueueLinear(const Ints& input) {
    Ints commands = dsTakeInts(input, 0, 14, {5, 8, 3, 0, 0, 7, 9, 4, 6});
    const int MAX_CAP = 6;
    Ints queueArr(MAX_CAP, INT_MIN);
    int front = 0;
    int rear = -1;
    Steps steps;

    steps.push_back(dsArrayStep(0, "Empty queue of capacity " + S(MAX_CAP) + ". Items join at the REAR and leave from the FRONT.",
                                {{"front", "0"}, {"rear", "-1"}}, queueArr, -1, {}, 0, -1, "queue"));
    for (size_t i = 0; i < commands.size(); ++i) {
        int cmd = commands[i];
        Vars vars = {{"front", S(front)}, {"rear", S(rear)}};
        vector<string> queueTags = dsMakeTags(MAX_CAP, {{front, "front"}, {rear, "rear"}});
        if (cmd != 0) {
            if (rear == MAX_CAP - 1) {
                steps.push_back(dsArrayStep(1, "enqueue(" + S(cmd) + "): rear == MAX - 1, so the queue is FULL" +
                                            (front > 0 ? " even though " + S(front) + " cell(s) at the left are free: a linear queue wastes them." : ".") + " " + S(cmd) + " is rejected.",
                                            vars, queueArr, rear, queueTags, front, rear, "queue"));
                continue;
            }
            queueArr[++rear] = cmd;
            steps.push_back(dsArrayStep(2, "a[++rear] = " + S(cmd) + ": rear is now " + S(rear) + ".",
                                        {{"front", S(front)}, {"rear", S(rear)}}, queueArr, rear,
                                        dsMakeTags(MAX_CAP, {{front, "front"}, {rear, "rear"}}), front, rear, "queue"));
        } else {
            if (front > rear) {
                steps.push_back(dsArrayStep(4, "dequeue(): front > rear, so the queue is EMPTY.",
                                            vars, queueArr, -1, queueTags, front, rear, "queue"));
                continue;
            }
            int dequeued = queueArr[front++];
            steps.push_back(dsArrayStep(5, "x = a[front++] returns " + S(dequeued) + " (the oldest item). front moves to " + S(front) + ".",
                                        {{"x", S(dequeued)}, {"front", S(front)}, {"rear", S(rear)}}, queueArr, -1,
                                        dsMakeTags(MAX_CAP, {{front, "front"}, {rear, "rear"}}), front, rear, "queue"));
        }
    }
    steps.push_back(dsArrayStep(0, "Done. Items waiting: " + S(rear - front + 1) + ". Both operations are O(1), but the freed cells on the left can never be reused.",
                                {{"size", S(rear - front + 1)}}, queueArr, -1,
                                dsMakeTags(MAX_CAP, {{front, "front"}, {rear, "rear"}}), front, rear, "queue"));
    return steps;
}

static vector<string> makeCircularQueueTags(int maxCap, int front, int rear, int itemCount) {
    if (itemCount > 0) return dsMakeTags(maxCap, {{front, "front"}, {rear, "rear"}});
    return dsMakeTags(maxCap, {{front, "front"}});
}

static Steps simQueueCircular(const Ints& input) {
    Ints commands = dsTakeInts(input, 0, 14, {5, 8, 3, 0, 0, 7, 9, 4, 6});
    const int MAX_CAP = 6;
    Ints queueArr(MAX_CAP, INT_MIN);
    int front = 0;
    int rear = MAX_CAP - 1;
    int itemCount = 0;
    Steps steps;

    steps.push_back(dsArrayStep(0, "Circular queue of capacity " + S(MAX_CAP) + ". After the last cell the index wraps to 0, so freed cells are reused.",
                                {{"front", "0"}, {"count", "0"}}, queueArr, -1,
                                makeCircularQueueTags(MAX_CAP, front, rear, itemCount), -1, -1, "queue"));
    for (size_t i = 0; i < commands.size(); ++i) {
        int cmd = commands[i];
        if (cmd != 0) {
            if (itemCount == MAX_CAP) {
                steps.push_back(dsArrayStep(1, "enqueue(" + S(cmd) + "): count == MAX, the queue is truly FULL. Rejected.",
                                            {{"count", S(itemCount)}}, queueArr, -1,
                                            makeCircularQueueTags(MAX_CAP, front, rear, itemCount), -1, -1, "queue"));
                continue;
            }
            rear = (rear + 1) % MAX_CAP;
            queueArr[rear] = cmd;
            ++itemCount;
            steps.push_back(dsArrayStep(3, "rear = (rear + 1) % " + S(MAX_CAP) + " = " + S(rear) + (rear == 0 && itemCount > 1 ? " (wrapped around)" : "") + ". Store " + S(cmd) + ".",
                                        {{"front", S(front)}, {"rear", S(rear)}, {"count", S(itemCount)}}, queueArr, rear,
                                        makeCircularQueueTags(MAX_CAP, front, rear, itemCount), -1, -1, "queue"));
        } else {
            if (itemCount == 0) {
                steps.push_back(dsArrayStep(5, "dequeue(): count == 0, the queue is EMPTY.",
                                            {{"count", "0"}}, queueArr, -1,
                                            makeCircularQueueTags(MAX_CAP, front, rear, itemCount), -1, -1, "queue"));
                continue;
            }
            int dequeued = queueArr[front];
            queueArr[front] = INT_MIN;
            front = (front + 1) % MAX_CAP;
            --itemCount;
            steps.push_back(dsArrayStep(7, "x = " + S(dequeued) + " leaves. front = (front + 1) % " + S(MAX_CAP) + " = " + S(front) + ".",
                                        {{"x", S(dequeued)}, {"front", S(front)}, {"count", S(itemCount)}}, queueArr, -1,
                                        makeCircularQueueTags(MAX_CAP, front, rear, itemCount), -1, -1, "queue"));
        }
    }
    steps.push_back(dsArrayStep(0, "Done. " + S(itemCount) + " item(s) waiting. The same 6 cells are reused again and again.",
                                {{"count", S(itemCount)}}, queueArr, -1,
                                makeCircularQueueTags(MAX_CAP, front, rear, itemCount), -1, -1, "queue"));
    return steps;
}

static Steps simBinarySearch(const Ints& input) {
    int key = input.empty() ? 57 : input[0];
    Ints arr = dsTakeInts(input, 1, 10, {4, 8, 15, 16, 23, 42, 57, 61, 88});
    sort(arr.begin(), arr.end());
    int count = (int)arr.size();
    int lo = 0;
    int hi = count - 1;
    Steps steps;
    int comparisons = 0;

    steps.push_back(dsArrayStep(0, "Binary search needs a SORTED array (yours is sorted automatically). lo and hi mark the part that can still hold key = " + S(key) + ".",
                                {{"key", S(key)}, {"lo", "0"}, {"hi", S(hi)}}, arr, -1, dsMakeTags(count, {{lo, "L"}, {hi, "R"}}), lo, hi));
    while (lo <= hi) {
        steps.push_back(dsArrayStep(1, "lo <= hi (" + S(lo) + " <= " + S(hi) + "), so " + S(hi - lo + 1) + " candidate(s) remain.",
                                    {{"lo", S(lo)}, {"hi", S(hi)}}, arr, -1, dsMakeTags(count, {{lo, "L"}, {hi, "R"}}), lo, hi));
        int mid = lo + (hi - lo) / 2;
        ++comparisons;
        steps.push_back(dsArrayStep(2, "mid = " + S(lo) + " + (" + S(hi) + " - " + S(lo) + ") / 2 = " + S(mid) + ". Written this way it cannot overflow.",
                                    {{"lo", S(lo)}, {"mid", S(mid)}, {"hi", S(hi)}}, arr, mid, dsMakeTags(count, {{lo, "L"}, {mid, "M"}, {hi, "R"}}), lo, hi));
        if (arr[mid] == key) {
            steps.push_back(dsArrayStep(3, "a[" + S(mid) + "] = " + S(arr[mid]) + " equals key. FOUND at index " + S(mid) + " after " + S(comparisons) + " comparison(s).",
                                        {{"result", S(mid)}}, arr, mid, dsMakeTags(count, {{mid, "found"}}), lo, hi));
            return steps;
        }
        if (arr[mid] < key) {
            steps.push_back(dsArrayStep(4, "a[" + S(mid) + "] = " + S(arr[mid]) + " < key: the key is to the RIGHT. lo = mid + 1 = " + S(mid + 1) + ". Half of the array is gone.",
                                        {{"lo", S(mid + 1)}, {"hi", S(hi)}}, arr, mid, dsMakeTags(count, {{mid, "M"}}), lo, hi));
            lo = mid + 1;
        } else {
            steps.push_back(dsArrayStep(5, "a[" + S(mid) + "] = " + S(arr[mid]) + " > key: the key is to the LEFT. hi = mid - 1 = " + S(mid - 1) + ". Half of the array is gone.",
                                        {{"lo", S(lo)}, {"hi", S(mid - 1)}}, arr, mid, dsMakeTags(count, {{mid, "M"}}), lo, hi));
            hi = mid - 1;
        }
    }
    steps.push_back(dsArrayStep(7, "lo > hi: no candidates left. Return -1 (not found) after " + S(comparisons) + " comparison(s). At most about log2(n) comparisons are ever needed.",
                                {{"lo", S(lo)}, {"hi", S(hi)}, {"result", "-1"}}, arr, -1, {}, 0, -1));
    return steps;
}

static Steps simInterpolationSearch(const Ints& input) {
    int key = input.empty() ? 70 : input[0];
    Ints arr = dsTakeInts(input, 1, 10, {10, 20, 30, 40, 50, 60, 70, 80, 90});
    sort(arr.begin(), arr.end());
    int count = (int)arr.size();
    int lo = 0;
    int hi = count - 1;
    Steps steps;
    int probes = 0;

    steps.push_back(dsArrayStep(0, "Interpolation search guesses where the key should be, like looking for 'Zebra' near the END of a dictionary. It works best when values are spread evenly.",
                                {{"key", S(key)}, {"lo", "0"}, {"hi", S(hi)}}, arr, -1, dsMakeTags(count, {{lo, "L"}, {hi, "R"}}), lo, hi));
    while (lo <= hi && key >= arr[lo] && key <= arr[hi]) {
        steps.push_back(dsArrayStep(1, "key " + S(key) + " lies between a[lo] = " + S(arr[lo]) + " and a[hi] = " + S(arr[hi]) + ", so it could be inside.",
                                    {{"lo", S(lo)}, {"hi", S(hi)}}, arr, -1, dsMakeTags(count, {{lo, "L"}, {hi, "R"}}), lo, hi));
        long long span = (long long)arr[hi] - arr[lo];
        int pos = lo;
        ++probes;
        if (span == 0) {
            steps.push_back(dsArrayStep(2, "a[hi] == a[lo], so the formula would divide by zero. Use pos = lo.",
                                        {{"pos", S(pos)}}, arr, pos, dsMakeTags(count, {{lo, "L"}, {pos, "P"}, {hi, "R"}}), lo, hi));
        } else {
            pos = (int)(lo + ((long long)key - arr[lo]) * (hi - lo) / span);
            pos = clamp(pos, lo, hi);
            steps.push_back(dsArrayStep(3, "pos = " + S(lo) + " + (" + S(key) + " - " + S(arr[lo]) + ") * (" + S(hi) + " - " + S(lo) + ") / (" + S(arr[hi]) + " - " + S(arr[lo]) + ") = " + S(pos) + ". The probe jumps straight to the likely place.",
                                        {{"lo", S(lo)}, {"pos", S(pos)}, {"hi", S(hi)}}, arr, pos, dsMakeTags(count, {{lo, "L"}, {pos, "P"}, {hi, "R"}}), lo, hi));
        }
        if (arr[pos] == key) {
            steps.push_back(dsArrayStep(4, "a[" + S(pos) + "] = " + S(arr[pos]) + " equals key. FOUND at index " + S(pos) + " after " + S(probes) + " probe(s).",
                                        {{"result", S(pos)}}, arr, pos, dsMakeTags(count, {{pos, "found"}}), lo, hi));
            return steps;
        }
        if (arr[pos] < key) {
            steps.push_back(dsArrayStep(5, "a[" + S(pos) + "] = " + S(arr[pos]) + " < key: lo = pos + 1 = " + S(pos + 1) + ".",
                                        {{"lo", S(pos + 1)}}, arr, pos, dsMakeTags(count, {{pos, "P"}}), lo, hi));
            lo = pos + 1;
        } else {
            steps.push_back(dsArrayStep(6, "a[" + S(pos) + "] = " + S(arr[pos]) + " > key: hi = pos - 1 = " + S(pos - 1) + ".",
                                        {{"hi", S(pos - 1)}}, arr, pos, dsMakeTags(count, {{pos, "P"}}), lo, hi));
            hi = pos - 1;
        }
    }
    steps.push_back(dsArrayStep(8, "The key is outside the remaining range (or lo > hi): NOT found after " + S(probes) + " probe(s). Average O(log log n) for evenly spread data, worst case O(n).",
                                {{"result", "-1"}}, arr, -1, {}, 0, -1));
    return steps;
}

void addDS2(map<string, Sim>& m);
void addDS8(map<string, Sim>& m);

void addDS1(map<string, Sim>& m) {
    addDS2(m);
    const string OVERVIEW_TOPIC = "Overview of Linear Data Structures";

    vector<string> refOverview = {
        "VisuAlgo: linked list, stack and queue|https://visualgo.net/en/list",
        "GeeksforGeeks: Data Structures|https://www.geeksforgeeks.org/data-structures/",
        "GeeksforGeeks: Linear data structures|https://www.geeksforgeeks.org/linear-data-structures/"};
    dsRegisterIntSim(m, OVERVIEW_TOPIC, "First the new value x, then up to 6 items already stored", "5, 20, 30, 40, 50",
                     {"for (int i = n; i > 0; i--)", "    a[i] = a[i - 1];", "a[0] = x;", "n++;"},
                     refOverview, {"LeetCode #283 (Move Zeroes)", "LeetCode #189 (Rotate Array)", "LeetCode #88 (Merge Sorted Array)"},
                     simOverviewArrayInsert);
    dsRegisterIntSim(m, OVERVIEW_TOPIC + "#1", "First the new value x, then up to 6 items already in the list", "5, 20, 30, 40, 50",
                     {"Node *n = new Node(x);", "n->next = head;", "head = n;"},
                     refOverview, {"LeetCode #707 (Design Linked List)", "LeetCode #206 (Reverse Linked List)", "LeetCode #21 (Merge Two Sorted Lists)"},
                     simOverviewLinkedListInsert);
    dsRegisterIntSim(m, OVERVIEW_TOPIC + "#2", "Up to 5 items (they are added in this order)", "10, 20, 30, 40",
                     {"push(x)   // stack: a[++top] = x", "pop()     // stack: return a[top--]", "enqueue(x) // queue: a[++rear] = x", "dequeue()  // queue: return a[front++]"},
                     refOverview, {"LeetCode #232 (Implement Queue using Stacks)", "LeetCode #225 (Implement Stack using Queues)", "LeetCode #20 (Valid Parentheses)"},
                     simOverviewStackQueue);
    dsSetProgramNames(OVERVIEW_TOPIC.c_str(), {
        "Array: insert at the front (items shift)",
        "Linked list: insert at the front (pointers change)",
        "Stack (LIFO) and queue (FIFO) with the same items"});

    vector<string> refArray = {
        "GeeksforGeeks: Array data structure|https://www.geeksforgeeks.org/array-data-structure/",
        "VisuAlgo: array operations|https://visualgo.net/en/array",
        "cppreference: std::array and arrays|https://en.cppreference.com/w/cpp/container/array"};
    dsRegisterIntSim(m, "Array", "Position, value, then up to 7 items", "2, 99, 10, 20, 30, 40, 50",
                     {"for (int i = n; i > pos; i--)", "    a[i] = a[i - 1];", "a[pos] = x;", "n++;"},
                     refArray, {"LeetCode #35 (Search Insert Position)", "LeetCode #88 (Merge Sorted Array)", "LeetCode #1 (Two Sum)"},
                     simArrayInsert);
    dsRegisterIntSim(m, "Array#1", "Position, then up to 8 items", "1, 10, 20, 30, 40, 50",
                     {"int x = a[pos];", "for (int i = pos; i < n - 1; i++)", "    a[i] = a[i + 1];", "n--;"},
                     refArray, {"LeetCode #27 (Remove Element)", "LeetCode #26 (Remove Duplicates from Sorted Array)", "LeetCode #283 (Move Zeroes)"},
                     simArrayDelete);
    dsRegisterIntSim(m, "Array#2", "Enter 2 to 8 numbers", "12, 35, 1, 10, 34, 1",
                     {"int max1 = a[0], max2 = -INF;", "for (int i = 1; i < n; i++) {", "    if (a[i] > max1) { max2 = max1; max1 = a[i]; }", "    else if (a[i] > max2 && a[i] != max1) max2 = a[i];", "}"},
                     refArray, {"LeetCode #414 (Third Maximum Number)", "LeetCode #628 (Maximum Product of Three Numbers)", "LeetCode #1464 (Maximum Product of Two Elements)"},
                     simArrayTopTwo);
    dsSetProgramNames("Array", {
        "Insert at a position (shift right)",
        "Delete at a position (shift left)",
        "Largest and second largest in one pass"});

    vector<string> refLinkedList = {
        "GeeksforGeeks: Linked list|https://www.geeksforgeeks.org/data-structures/linked-list/",
        "VisuAlgo: linked list|https://visualgo.net/en/list",
        "cppreference: std::forward_list|https://en.cppreference.com/w/cpp/container/forward_list"};
    dsRegisterIntSim(m, "Linked List", "The key to find, then up to 7 values", "40, 10, 20, 30, 40, 50",
                     {"Node *cur = head;", "while (cur != NULL) {", "    if (cur->data == key)  return cur;", "    cur = cur->next;", "}", "return NULL;"},
                     refLinkedList, {"LeetCode #141 (Linked List Cycle)", "LeetCode #876 (Middle of the Linked List)", "LeetCode #203 (Remove Linked List Elements)"},
                     simLinkedListSearch);
    dsRegisterIntSim(m, "Linked List#1", "Position, value, then up to 6 values", "2, 99, 10, 20, 30, 40",
                     {"Node *n = new Node(x);", "if (pos == 0) { n->next = head; head = n; return; }", "Node *p = head;", "for (int i = 0; i < pos - 1; i++)", "    p = p->next;", "n->next = p->next;", "p->next = n;"},
                     refLinkedList, {"LeetCode #707 (Design Linked List)", "LeetCode #21 (Merge Two Sorted Lists)", "LeetCode #19 (Remove Nth Node From End of List)"},
                     simLinkedListInsert);
    dsRegisterIntSim(m, "Linked List#2", "Enter 2 to 7 values", "10, 20, 30, 40",
                     {"Node *prev = NULL, *cur = head;", "while (cur != NULL) {", "    Node *next = cur->next;", "    cur->next = prev;", "    prev = cur;  cur = next;", "}", "head = prev;"},
                     refLinkedList, {"LeetCode #206 (Reverse Linked List)", "LeetCode #92 (Reverse Linked List II)", "LeetCode #25 (Reverse Nodes in k-Group)"},
                     simLinkedListReverse);
    dsSetProgramNames("Linked List", {
        "Search for a key (traverse)",
        "Insert at a position",
        "Reverse the list (prev, cur, next)"});

    vector<string> refStack = {
        "GeeksforGeeks: Stack|https://www.geeksforgeeks.org/stack-data-structure/",
        "VisuAlgo: stack and queue|https://visualgo.net/en/list",
        "cppreference: std::stack|https://en.cppreference.com/w/cpp/container/stack"};
    dsRegisterIntSim(m, "Stack", "Commands: a positive number pushes it, 0 pops (capacity 6)", "5, 8, 3, 0, 0, 7, 0, 0, 0",
                     {"push(x):", "    if (top == MAX - 1)  return;   // overflow", "    a[++top] = x;", "pop():", "    if (top == -1)  return;        // underflow", "    x = a[top--];"},
                     refStack, {"LeetCode #155 (Min Stack)", "LeetCode #232 (Implement Queue using Stacks)", "LeetCode #739 (Daily Temperatures)"},
                     simStackPushPop);
    dsRegisterTextSim(m, "Stack#1", "Type brackets such as ( ) [ ] { }", "{[()]}(",
                      {"for each character c in s:", "    if c is an opening bracket:  push(c)", "    else if stack is empty or top does not match c:  return false", "    else  pop()", "return stack is empty"},
                      refStack, {"LeetCode #20 (Valid Parentheses)", "LeetCode #32 (Longest Valid Parentheses)", "LeetCode #22 (Generate Parentheses)"},
                      simStackBrackets);
    dsRegisterTextSim(m, "Stack#2", "Postfix expression, e.g. 2 3 1 * + 9 -", "2 3 1 * + 9 -",
                      {"for each token t:", "    if t is a number:  push(t)", "    else {  b = pop();  a = pop();", "            push(a op b);  }", "answer = pop()"},
                      refStack, {"LeetCode #150 (Evaluate Reverse Polish Notation)", "LeetCode #224 (Basic Calculator)", "LeetCode #227 (Basic Calculator II)"},
                      simStackPostfix);
    dsSetProgramNames("Stack", {
        "Push and pop with overflow / underflow",
        "Balanced brackets (needs a stack)",
        "Evaluate a postfix expression"});

    vector<string> refQueue = {
        "GeeksforGeeks: Queue|https://www.geeksforgeeks.org/queue-data-structure/",
        "VisuAlgo: stack and queue|https://visualgo.net/en/list",
        "cppreference: std::queue|https://en.cppreference.com/w/cpp/container/queue"};
    dsRegisterIntSim(m, "Queue", "Commands: a positive number enqueues it, 0 dequeues (capacity 6)", "5, 8, 3, 0, 0, 7, 9, 4, 6",
                     {"enqueue(x):", "    if (rear == MAX - 1)  return;   // full", "    a[++rear] = x;", "dequeue():", "    if (front > rear)  return;      // empty", "    x = a[front++];"},
                     refQueue, {"LeetCode #232 (Implement Queue using Stacks)", "LeetCode #933 (Number of Recent Calls)", "LeetCode #346 (Moving Average from Data Stream)"},
                     simQueueLinear);
    dsRegisterIntSim(m, "Queue#1", "Commands: a positive number enqueues it, 0 dequeues (capacity 6)", "5, 8, 3, 0, 0, 7, 9, 4, 6",
                     {"enqueue(x):", "    if (count == MAX)  return;            // full", "    rear = (rear + 1) % MAX;", "    a[rear] = x;  count++;", "dequeue():", "    if (count == 0)  return;              // empty", "    x = a[front];", "    front = (front + 1) % MAX;  count--;"},
                     refQueue, {"LeetCode #622 (Design Circular Queue)", "LeetCode #641 (Design Circular Deque)", "LeetCode #239 (Sliding Window Maximum)"},
                     simQueueCircular);
    dsSetProgramNames("Queue", {
        "Linear queue (front and rear)",
        "Circular queue (wrap-around)"});

    vector<string> refSearch = {
        "GeeksforGeeks: Binary search|https://www.geeksforgeeks.org/binary-search/",
        "GeeksforGeeks: Interpolation search|https://www.geeksforgeeks.org/interpolation-search/",
        "VisuAlgo: sorting and searching|https://visualgo.net/en/sorting"};
    dsRegisterIntSim(m, "Interpolation Search", "The key, then up to 10 numbers (sorted for you)", "57, 4, 8, 15, 16, 23, 42, 57, 61, 88",
                     {"int lo = 0, hi = n - 1;", "while (lo <= hi) {", "    int mid = lo + (hi - lo) / 2;", "    if (a[mid] == key)  return mid;", "    if (a[mid] < key)  lo = mid + 1;", "    else               hi = mid - 1;", "}", "return -1;"},
                     refSearch, {"LeetCode #704 (Binary Search)", "LeetCode #35 (Search Insert Position)", "LeetCode #34 (First and Last Position in Sorted Array)", "LeetCode #33 (Search in Rotated Sorted Array)"},
                     simBinarySearch);
    dsRegisterIntSim(m, "Interpolation Search#1", "The key, then up to 10 numbers (sorted for you)", "70, 10, 20, 30, 40, 50, 60, 70, 80, 90",
                     {"int lo = 0, hi = n - 1;", "while (lo <= hi && key >= a[lo] && key <= a[hi]) {", "    if (a[hi] == a[lo])  pos = lo;", "    else  pos = lo + (long)(key - a[lo]) * (hi - lo) / (a[hi] - a[lo]);", "    if (a[pos] == key)  return pos;", "    if (a[pos] < key)  lo = pos + 1;", "    else               hi = pos - 1;", "}", "return -1;"},
                     refSearch, {"LeetCode #704 (Binary Search: try the probe formula instead of mid)", "LeetCode #74 (Search a 2D Matrix)", "LeetCode #278 (First Bad Version)"},
                     simInterpolationSearch);
    dsSetProgramNames("Interpolation Search", {
        "Binary search (left, mid, right)",
        "Interpolation search (probe formula)"});

    addDS8(m);
}
