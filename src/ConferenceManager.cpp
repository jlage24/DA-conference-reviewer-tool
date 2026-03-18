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
        reviewerNodeIds[r.id] = index++;
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

void ConferenceManager::buildFlowGraph() {
    graph.addVertex(sourceId());
    graph.addVertex(sinkId());
    for (const auto& pair : submissions) {
        graph.addVertex(submissionNodeId(pair.first));
        graph.addEdge(sourceId(), submissionNodeId(pair.first), params.minReviewsPerSubmission);
    }
    for (const auto& pair : reviewers) {
        graph.addVertex(reviewerNodeId(pair.first));
        graph.addEdge(reviewerNodeId(pair.first), sinkId(), params.maxReviewsPerReviewer);
    }
    for (const auto& sub : submissions) {
        for (const auto& rev : reviewers) {
            if (sub.second.primary == rev.second.primary) {
                graph.addEdge(submissionNodeId(sub.first), reviewerNodeId(rev.first), 1);
            }
        }
    }
}

bool ConferenceManager::bfs(int source, int sink, unordered_map<int, Edge<int> *> &parent) {
    for (auto v : graph.getVertexSet()) {
        v->setVisited(false);
    }
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

double ConferenceManager::findBottleneck(int source, int sink, unordered_map<int, Edge<int> *> &parent) {
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
