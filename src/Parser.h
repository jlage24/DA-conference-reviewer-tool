#ifndef DA2026_PRJ1_PARSER_H
#define DA2026_PRJ1_PARSER_H

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "Submission.h"
#include "Reviewer.h"

using namespace std;

enum Section {
    NONE,
    SUBMISSIONS,
    REVIEWERS,
    PARAMETERS,
    CONTROL
};

struct Parameters {
    int minReviewsPerSubmission;
    int maxReviewsPerReviewer;
    int primaryReviewerExpertise;
    int secondaryReviewerExpertise;
    int primarySubmissionDomain;
    int secondarySubmissionDomain;
    int generateAssignments;
    int riskAnalysis;
    string outputFileName = "output.csv";
};

inline string trim(const string& s) {
    string result = s;
    result.erase(remove(result.begin(), result.end(), '\r'), result.end());
    size_t start = result.find_first_not_of(" ");
    if (start == string::npos) return "";
    return result.substr(start);
}

inline void parseFile(const string& filename, vector<Submission>& submissions, vector<Reviewer>& reviewers, Parameters& params) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "There was an error when trying to open the file." << endl;
        return;
    }
    string line;
    Section currentSection = NONE;
    while (getline(file, line)) {
        if (line == "#Submissions") {
            currentSection = SUBMISSIONS;
        } else if (line == "#Reviewers") {
            currentSection = REVIEWERS;
        } else if (line == "#Parameters") {
            currentSection = PARAMETERS;
        } else if (line == "#Control") {
            currentSection = CONTROL;
        } else if (line[0] == '#' || line.empty()) {
            continue;
        } else {
            stringstream ss(line);
            string token;
            switch (currentSection) {
                case SUBMISSIONS: {
                    Submission s;
                    getline(ss, token, ','); s.id      = stoi(trim(token));
                    getline(ss, token, ','); s.title   = trim(token);
                    getline(ss, token, ','); s.authors = trim(token);
                    getline(ss, token, ','); s.email   = trim(token);
                    getline(ss, token, ','); s.primary = stoi(trim(token));
                    getline(ss, token, ','); token = trim(token);
                    s.secondary = token.empty() ? 0 : stoi(token);
                    submissions.push_back(s);
                    break;
                }
                case REVIEWERS: {
                    Reviewer r;
                    getline(ss, token, ','); r.id = stoi(trim(token));
                    getline(ss, token, ','); r.name = trim(token);
                    getline(ss, token, ','); r.email = trim(token);
                    getline(ss, token, ','); r.primary = stoi(trim(token));
                    getline(ss, token, ','); token = trim(token);
                    r.secondary = token.empty() ? 0 : stoi(token);
                    reviewers.push_back(r);
                    break;
                }
                case PARAMETERS: {
                    string key, value;
                    getline(ss, key, ','); key = trim(key);
                    getline(ss, value); value = trim(value);
                    if      (key == "MinReviewsPerSubmission") params.minReviewsPerSubmission   = stoi(value);
                    else if (key == "MaxReviewsPerReviewer") params.maxReviewsPerReviewer     = stoi(value);
                    else if (key == "PrimaryReviewerExpertise") params.primaryReviewerExpertise  = stoi(value);
                    else if (key == "SecondaryReviewerExpertise") params.secondaryReviewerExpertise = stoi(value);
                    else if (key == "PrimarySubmissionDomain") params.primarySubmissionDomain   = stoi(value);
                    else if (key == "SecondarySubmissionDomain") params.secondarySubmissionDomain = stoi(value);
                    break;
                }
                case CONTROL: {
                    string key, value;
                    getline(ss, key, ','); key = trim(key);
                    getline(ss, value); value = trim(value);
                    if (key == "GenerateAssignments") params.generateAssignments = stoi(value);
                    else if (key == "RiskAnalysis") params.riskAnalysis = stoi(value);
                    else if (key == "OutputFileName") {
                        value.erase(remove(value.begin(), value.end(), '"'), value.end());
                        params.outputFileName = value;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}

#endif //DA2026_PRJ1_PARSER_H