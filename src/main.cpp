#include <iostream>
#include <memory>
#include "Parser.h"
#include "ConferenceManager.h"

using namespace std;

void showMenu() {
    cout << "\n==== Conference Reviewer Tool ====\n";
    cout << "1 - Load input file\n";
    cout << "2 - List submissions\n";
    cout << "3 - List reviewers\n";
    cout << "4 - Show parameters\n";
    cout << "5 - Generate assignments\n";
    cout << "6 - Run risk analysis\n";
    cout << "0 - Exit\n";
    cout << "Opção: ";
}

int main() {
    vector<Submission> submissions;
    vector<Reviewer> reviewers;
    Parameters params;
    bool dataLoaded = false;
    unique_ptr<ConferenceManager> manager;

    while (true) {
        showMenu();
        int opt;
        if (!(cin >> opt)) {
            cout << "Invalid input. Exiting.\n";
            break;
        }

        if (opt == 0) {
            cout << "Bye!\n\n";
            break;
        }

        switch (opt) {
            case 1: {
                cout << "Input file path: ";
                string filename;
                cin >> filename;

                submissions.clear();
                reviewers.clear();
                params = Parameters();

                parseFile(filename, submissions, reviewers, params);
                if (submissions.empty() && reviewers.empty()) {
                    cout << "Failed to load data (see error above).\n";
                    dataLoaded = false;
                } else {
                    manager = make_unique<ConferenceManager>(submissions, reviewers, params);
                    dataLoaded = true;
                    cout << "File loaded successfully.\n";
                }
                break;
            }
            case 2:
                if (!dataLoaded) {
                    cout << "Load an input file first.\n";
                    break;
                }
                manager->printSubmissions();
                break;

            case 3:
                if (!dataLoaded) {
                    cout << "Load an input file first.\n";
                    break;
                }
                manager->printReviewers();
                break;

            case 4:
                if (!dataLoaded) {
                    cout << "Load an input file first.\n";
                    break;
                }
                manager->printParameters();
                break;

            case 5:
                if (!dataLoaded) {
                    cout << "Load an input file first.\n";
                    break;
                }
                manager->generateAssignments();
                cout << "Assignments generated to file: " << params.outputFileName << "\n";
                break;

            case 6:
                if (!dataLoaded) {
                    cout << "Load an input file first.\n";
                    break;
                }
                manager->riskAnalysis();
                cout << "Risk analysis appended to file: " << params.outputFileName << "\n";
                break;

            default:
                cout << "Invalid option. Please choose a valid menu option.\n";
        }
    }

    return 0;
}
