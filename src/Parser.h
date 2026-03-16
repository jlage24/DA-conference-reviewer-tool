#ifndef DA2026_PRJ1_PARSER_H
#define DA2026_PRJ1_PARSER_H

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
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
    size_t start = result.find_first_not_of(" \t");
    if (start == string::npos) return ""; // the string is entirely whitespace
    size_t end = result.find_last_not_of(" \t");
    return result.substr(start, end - start + 1);
}

inline void parseFile(const string& filename, vector<Submission>& submissions, vector<Reviewer>& reviewers, Parameters& params) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open the file " << filename << "." << endl;
        return;
    }
    string line;
    Section currentSection = NONE;
    while (getline(file, line)) {
        // Strip out any trailing carriage returns before going forward
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        if (line == "#Submissions") { currentSection = SUBMISSIONS; continue; }
        if (line == "#Reviewers") { currentSection = REVIEWERS; continue; }
        if (line == "#Parameters") { currentSection = PARAMETERS; continue; }
        if (line == "#Control") { currentSection = CONTROL; continue; }
        // Strip inline comments, ignoring characters after the '#' until the end of the line
        size_t hashPos = line.find('#');
        if (hashPos != string::npos) {
            line = line.substr(0, hashPos);
        }
        line = trim(line);
        if (line.empty()) continue;
        stringstream ss(line);
        string token;
        try {
            switch (currentSection) {
                case SUBMISSIONS: {
                    Submission s;
                    getline(ss, token, ','); s.id = stoi(trim(token));
                    getline(ss, token, ','); s.title = trim(token);
                    getline(ss, token, ','); s.authors = trim(token);
                    getline(ss, token, ','); s.email = trim(token);
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
                    if (key == "MinReviewsPerSubmission") params.minReviewsPerSubmission = stoi(value);
                    else if (key == "MaxReviewsPerReviewer") params.maxReviewsPerReviewer = stoi(value);
                    else if (key == "PrimaryReviewerExpertise") params.primaryReviewerExpertise = stoi(value);
                    else if (key == "SecondaryReviewerExpertise") params.secondaryReviewerExpertise = stoi(value);
                    else if (key == "PrimarySubmissionDomain") params.primarySubmissionDomain = stoi(value);
                    else if (key == "SecondarySubmissionDomain") params.secondarySubmissionDomain = stoi(value);
                    break;
                }
                case CONTROL: {
                    string key, value;
                    getline(ss, key, ','); key = trim(key);
                    getline(ss, value);    value = trim(value);
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
        } catch (const invalid_argument& e) {
            // Files will be rejected if inconsistent values are found, outputting an error message
            cout << "Error: Inconsistent value detected. Expected a number, but found invalid characters in line: \n" << line << endl;
            submissions.clear();
            reviewers.clear();
            return;
        } catch (const out_of_range& e) {
            // Files will be rejected if numerical values exceed the maximum limits of the data type
            cout << "Error: Number out of range in line: \n" << line << endl;
            submissions.clear();
            reviewers.clear();
            return;
        }
    }
}

#endif //DA2026_PRJ1_PARSER_H