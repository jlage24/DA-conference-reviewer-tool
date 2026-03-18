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
