#ifndef DA2026_PRJ1_CONFERENCEMANAGER_H
#define DA2026_PRJ1_CONFERENCEMANAGER_H

#include <unordered_map>
#include "Submission.h"
#include "Reviewer.h"
#include "Parser.h"
#include "../data-structures/Graph.h"

class ConferenceManager {
public:
    ConferenceManager(const vector<Submission>& submissions, const vector<Reviewer>& reviewers, const Parameters& params);
    void buildFlowGraph();
    double edmondsKarp(int source, int sink);
    void generateAssignments();
    void riskAnalysis();

private:
    Graph<int> graph;
    unordered_map<int, Submission> submissions;
    unordered_map<int, Reviewer> reviewers;
    unordered_map<int, int> submissionNodeIds;
    unordered_map<int, int> reviewerNodeIds;
    unordered_map<int, int> reviewerIdFromNodeId;
    Parameters params;
    // Helpers for Node ID
    int sourceId();
    int sinkId();
    int submissionNodeId(int submissionId);
    int reviewerNodeId(int reviewerId);
    // Helpers for Edmonds-Karp
    bool bfs(int source, int sink, unordered_map<int, Edge<int>*>& parent);
    double findBottleneck(int source, int sink, unordered_map<int, Edge<int>*>& parent);
    void augmentFlow(int source, int sink, double bottleneck, unordered_map<int, Edge<int>*>& parent);
    void addResidualEdge(int from, int to, double capacity);
};

#endif //DA2026_PRJ1_CONFERENCEMANAGER_H