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
