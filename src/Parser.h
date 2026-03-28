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

/**
 * @brief Represents the current section being parsed in the input file.
 *
 * Used to guide the parser's behavior as it reads through the CSV file,
 * switching context whenever a section header (e.g. \#Submissions) is encountered.
 */
enum Section {
    NONE,        ///< No section has been identified yet.
    SUBMISSIONS, ///< Currently parsing the submissions section.
    REVIEWERS,   ///< Currently parsing the reviewers section.
    PARAMETERS,  ///< Currently parsing the parameters section.
    CONTROL      ///< Currently parsing the control section.
};

/**
 * @brief Holds the configuration parameters for the assignment process.
 *
 * These values are read from the \#Parameters and \#Control sections
 * of the input file and control the behavior of the flow algorithm
 * and output generation.
 */
struct Parameters {
    int minReviewsPerSubmission;   ///< Minimum number of reviewers required per submission.
    int maxReviewsPerReviewer;     ///< Maximum number of submissions a reviewer can be assigned.
    int primaryReviewerExpertise;  ///< Weight/flag for primary reviewer expertise matching.
    int secondaryReviewerExpertise;///< Weight/flag for secondary reviewer expertise matching.
    int primarySubmissionDomain;   ///< Weight/flag for primary submission domain matching.
    int secondarySubmissionDomain; ///< Weight/flag for secondary submission domain matching.
    int generateAssignments;       ///< Flag indicating whether to generate assignments (1) or not (0).
    int riskAnalysis;              ///< Flag indicating whether to perform risk analysis (1) or not (0).
    string outputFileName = "output.csv"; ///< Name of the output file. Defaults to "output.csv".
};

/**
 * @brief Trims leading/trailing whitespace and carriage return characters from a string.
 *
 * @param s The input string to trim.
 * @return A new string with whitespace and '\\r' characters removed from both ends.
 */
inline string trim(const string& s) {
    string result = s;
    result.erase(remove(result.begin(), result.end(), '\r'), result.end());
    size_t start = result.find_first_not_of(" \t");
    if (start == string::npos) return "";
    size_t end = result.find_last_not_of(" \t");
    return result.substr(start, end - start + 1);
}

/**
 * @brief Parses a CSV input file and populates submissions, reviewers, and parameters.
 *
 * Reads the file line by line, switching parsing context based on section headers
 * (\#Submissions, \#Reviewers, \#Parameters, \#Control). Inline comments (text after
 * '\#' on a data line) are stripped before parsing. Empty lines are skipped.
 *
 * If an inconsistent value or out-of-range number is encountered, an error message
 * is printed to standard output, and both the submissions and reviewers vectors are
 * cleared before returning.
 *
 * @param filename    Path to the CSV input file.
 * @param submissions Vector to be populated with parsed Submission objects.
 * @param reviewers   Vector to be populated with parsed Reviewer objects.
 * @param params      Parameters struct to be populated with parsed configuration values.
 */
inline void parseFile(const string& filename, vector<Submission>& submissions, vector<Reviewer>& reviewers, Parameters& params) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open the file " << filename << "." << endl;
        return;
    }
    string line;
    Section currentSection = NONE;
    while (getline(file, line)) {
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        if (line == "#Submissions") { currentSection = SUBMISSIONS; continue; }
        if (line == "#Reviewers")   { currentSection = REVIEWERS;   continue; }
        if (line == "#Parameters")  { currentSection = PARAMETERS;  continue; }
        if (line == "#Control")     { currentSection = CONTROL;     continue; }
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
                    getline(ss, token, ',');
                    token.erase(remove(token.begin(), token.end(), '"'), token.end());
                    s.title = trim(token);
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
            cout << "Error: Inconsistent value detected. Expected a number, but found invalid characters in line: \n" << line << endl;
            submissions.clear();
            reviewers.clear();
            return;
        } catch (const out_of_range& e) {
            cout << "Error: Number out of range in line: \n" << line << endl;
            submissions.clear();
            reviewers.clear();
            return;
        }
    }
}

#endif //DA2026_PRJ1_PARSER_H