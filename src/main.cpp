#include <iostream>
#include "Parser.h"

using namespace std;

int main() {
    vector<Submission> submissions;
    vector<Reviewer> reviewers;
    Parameters params;
    parseFile("../data/input/dataset5.csv", submissions, reviewers, params);
    cout << "=== Submissions ===" << endl;
    for (auto& s : submissions) {
        cout << s.id << " | " << s.title << " | " << s.primary << endl;
    }
    cout << "=== Reviewers ===" << endl;
    for (auto& r : reviewers) {
        cout << r.id << " | " << r.name << " | " << r.primary << endl;
    }
    return 0;
}