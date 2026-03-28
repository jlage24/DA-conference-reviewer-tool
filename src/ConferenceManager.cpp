#include "ConferenceManager.h"

ConferenceManager::ConferenceManager(const vector<Submission> &submissions, const vector<Reviewer> &reviewers, const Parameters &params) : params(params) {
    for (const auto& s : submissions) {
        this->submissions[s.id] = s;
    }
    for (const auto& r : reviewers) {
        this->reviewers[r.id] = r;
    }
    int index = 2;
    for (const auto& s : submissions) {
        submissionNodeIds[s.id] = index++;
    }
    for (const auto& r : reviewers) {
        reviewerNodeIds[r.id] = index;
        reviewerIdFromNodeId[index] = r.id;
        index++;
    }
}

int ConferenceManager::sourceId() {
    return 0;
}

int ConferenceManager::sinkId() {
    return 1;
}

int ConferenceManager::submissionNodeId(int submissionId) {
    return submissionNodeIds.at(submissionId);
}

int ConferenceManager::reviewerNodeId(int reviewerId) {
    return reviewerNodeIds.at(reviewerId);
}

void ConferenceManager::addResidualEdge(int from, int to, double capacity) {
    auto v1 = graph.findVertex(from);
    auto v2 = graph.findVertex(to);
    auto e1 = v1->addEdge(v2, capacity);
    auto e2 = v2->addEdge(v1, 0);
    e1->setReverse(e2);
    e2->setReverse(e1);
}

void ConferenceManager::buildFlowGraph() {
    while (!graph.getVertexSet().empty()) {
        graph.removeVertex(graph.getVertexSet()[0]->getInfo());
    }
    graph.addVertex(sourceId());
    graph.addVertex(sinkId());
    // Submissions by growing order of ID
    vector<int> subIds;
    for (const auto& pair : submissions) subIds.push_back(pair.first);
    sort(subIds.begin(), subIds.end());
    for (int id : subIds) {
        graph.addVertex(submissionNodeId(id));
        addResidualEdge(sourceId(), submissionNodeId(id), params.minReviewsPerSubmission);
    }
    // Reviewers by growing order of ID
    vector<int> revIds;
    for (const auto& pair : reviewers) revIds.push_back(pair.first);
    sort(revIds.begin(), revIds.end());
    for (int id : revIds) {
        graph.addVertex(reviewerNodeId(id));
        addResidualEdge(reviewerNodeId(id), sinkId(), params.maxReviewsPerReviewer);
    }
    for (int sid : subIds) {
        for (int rid : revIds) {
            const Submission& sub = submissions[sid];
            const Reviewer& rev = reviewers[rid];
            int matchDomain = 0;
            if (sub.primary == rev.primary) matchDomain = sub.primary;
            else if (params.generateAssignments >= 2 && sub.secondary != 0 && sub.secondary == rev.primary) matchDomain = sub.secondary;
            if (matchDomain != 0) {
                matchDomains[{sid, rid}] = matchDomain;
                addResidualEdge(submissionNodeId(sid), reviewerNodeId(rid), 1);
            }
        }
    }
}

bool ConferenceManager::bfs(int source, int sink, unordered_map<int, Edge<int>*>& parent) {
    for (auto v : graph.getVertexSet()) v->setVisited(false);
    queue<int> q;
    q.push(source);
    graph.findVertex(source)->setVisited(true);
    while (!q.empty()) {
        int curr = q.front(); q.pop();
        Vertex<int>* v = graph.findVertex(curr);
        for (auto e : v->getAdj()) {
            int next = e->getDest()->getInfo();
            double residual = e->getWeight() - e->getFlow();
            Vertex<int>* nv = graph.findVertex(next);
            if (residual > 0 && !nv->isVisited()) {
                nv->setVisited(true);
                parent[next] = e;
                q.push(next);
            }
        }
    }
    return graph.findVertex(sink)->isVisited();
}

double ConferenceManager::findBottleneck(int source, int sink, unordered_map<int, Edge<int>*>& parent) {
    double bottleneck = INF;
    int curr = sink;
    while (curr != source) {
        Edge<int>* e = parent[curr];
        double residual = e->getWeight() - e->getFlow();
        bottleneck = min(bottleneck, residual);
        curr = e->getOrig()->getInfo();
    }
    return bottleneck;
}

void ConferenceManager::augmentFlow(int source, int sink, double bottleneck, unordered_map<int, Edge<int>*>& parent) {
    int curr = sink;
    while (curr != source) {
        Edge<int>* e = parent[curr];
        e->setFlow(e->getFlow() + bottleneck);
        e->getReverse()->setFlow(e->getReverse()->getFlow() - bottleneck);
        curr = e->getOrig()->getInfo();
    }
}

double ConferenceManager::edmondsKarp(int source, int sink) {
    for (auto v : graph.getVertexSet()) {
        for (auto e : v->getAdj()) {
            e->setFlow(0);
        }
    }
    double totalFlow = 0;
    unordered_map<int, Edge<int>*> parent;
    while (bfs(source, sink, parent)) {
        double bottleneck = findBottleneck(source, sink, parent);
        augmentFlow(source, sink, bottleneck, parent);
        totalFlow += bottleneck;
        parent.clear();
    }
    return totalFlow;
}

void ConferenceManager::generateAssignments() {
    buildFlowGraph();
    double maxFlow = edmondsKarp(sourceId(), sinkId());
    double expected = params.minReviewsPerSubmission * submissions.size();
    if (params.generateAssignments == 0) return;
    ofstream out(params.outputFileName);
    vector<pair<int,int>> assignments;
    int total = 0;
    for (const auto& sub : submissions) {
        Vertex<int>* v = graph.findVertex(submissionNodeId(sub.first));
        for (auto e : v->getAdj()) {
            if (e->getDest()->getInfo() == sourceId() || e->getDest()->getInfo() == sinkId()) continue;
            if (e->getFlow() > 0.5) {
                int revId = reviewerIdFromNodeId[e->getDest()->getInfo()];
                assignments.emplace_back(sub.first, revId);
                total++;
            }
        }
    }
    sort(assignments.begin(), assignments.end());
    out << "#SubmissionId,ReviewerId,Match\n";
    for (const auto& a : assignments) {
        out << a.first << ", " << a.second << ", " << matchDomains[{a.first, a.second}] << "\n";
    }
    sort(assignments.begin(), assignments.end(), [](const pair<int,int>& a, const pair<int,int>& b) {
        if (a.second != b.second) return a.second < b.second;
        return a.first < b.first;
    });
    out << "#ReviewerId,SubmissionId,Match\n";
    for (const auto& a : assignments) {
        out << a.second << ", " << a.first << ", " << matchDomains[{a.first, a.second}] << "\n";
    }
    out << "#Total: " << total << "\n";
    if (maxFlow < expected) {
        vector<pair<int,int>> missing;
        for (const auto& sub : submissions) {
            Vertex<int>* v = graph.findVertex(submissionNodeId(sub.first));
            int assigned = 0;
            for (auto e : v->getAdj()) {
                if (e->getDest()->getInfo() == sourceId() || e->getDest()->getInfo() == sinkId()) continue;
                if (e->getFlow() > 0.5) assigned++;
            }
            int miss = params.minReviewsPerSubmission - assigned;
            if (miss > 0) missing.emplace_back(sub.first, miss);
        }
        sort(missing.begin(), missing.end());
        out << "#SubmissionId,Domain,MissingReviews\n";
        for (const auto& m : missing) {
            out << m.first << ", " << submissions[m.first].primary << ", " << m.second << "\n";
        }
    }
}

void ConferenceManager::riskAnalysis() {
    if (params.riskAnalysis == 0) return;
    vector<int> riskyReviewers;
    for (const auto& rev : reviewers) {
        Vertex<int>* v = graph.findVertex(reviewerNodeId(rev.first));
        for (auto e : v->getAdj()) {
            if (e->getDest()->getInfo() == sinkId()) {
                double original = e->getWeight();
                e->setWeight(0);
                double maxFlow = edmondsKarp(sourceId(), sinkId());
                double expected = params.minReviewsPerSubmission * submissions.size();
                if (maxFlow < expected) {
                    riskyReviewers.push_back(rev.first);
                }
                e->setWeight(original);
            }
        }
    }
    sort(riskyReviewers.begin(), riskyReviewers.end());
    ofstream out(params.outputFileName, ios::app);
    out << "#Risk Analysis: " << params.riskAnalysis << "\n";
    for (int i = 0; i < (int)riskyReviewers.size(); i++) {
        out << riskyReviewers[i];
        if (i + 1 < (int)riskyReviewers.size()) out << ", ";
    }
    out << "\n";
}

void ConferenceManager::printSubmissions() const {
    cout << "\n=== Submissions ===\n";
    for (const auto& [id, s] : submissions) {
        cout << id << " | " << s.title << " | " << s.primary << "\n";
    }
}

void ConferenceManager::printReviewers() const {
    cout << "\n=== Reviewers ===\n";
    for (const auto& [id, r] : reviewers) {
        cout << id << " | " << r.name << " | " << r.primary << "\n";
    }
}

void ConferenceManager::printParameters() const {
    cout << "\n=== Parameters ===\n";
    cout << "MinReviewsPerSubmission: " << params.minReviewsPerSubmission << "\n";
    cout << "MaxReviewsPerReviewer:   " << params.maxReviewsPerReviewer << "\n";
    cout << "PrimaryReviewerExpertise: " << params.primaryReviewerExpertise << "\n";
    cout << "SecondaryReviewerExpertise: " << params.secondaryReviewerExpertise << "\n";
    cout << "PrimarySubmissionDomain: " << params.primarySubmissionDomain << "\n";
    cout << "SecondarySubmissionDomain: " << params.secondarySubmissionDomain << "\n";
    cout << "GenerateAssignments:     " << params.generateAssignments << "\n";
    cout << "RiskAnalysis:            " << params.riskAnalysis << "\n";
    cout << "OutputFileName:          " << params.outputFileName << "\n";
}