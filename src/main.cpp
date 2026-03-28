#include <iostream>
#include <memory>

#include "ConferenceManager.h"
#include "Parser.h"

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
    cout << "Option: ";
}

int main(int argc, char* argv[]) {
    vector<Submission> submissions;
    vector<Reviewer> reviewers;
    Parameters params;
    bool dataLoaded = false;
    unique_ptr<ConferenceManager> manager;

    if (argc == 4 && string(argv[1]) == "-b") {
        string inputFile = argv[2];
        string outputFile = argv[3];

        parseFile(inputFile, submissions, reviewers, params);
        params.outputFileName = outputFile;

        if (submissions.empty() && reviewers.empty()) {
            cerr << "Error: Failed to load data from file " << inputFile << ".\n";
            return 1;
        }

        manager = make_unique<ConferenceManager>(submissions, reviewers, params);

        manager->generateAssignments();

        manager->riskAnalysis();

        return 0;
    }

    if (argc != 1 && !(argc == 4 && string(argv[1]) == "-b")) {
        cerr << "Usage:\n"
             << "  Interactive: " << argv[0] << "\n"
             << "  Batch:       " << argv[0] << " -b input.csv output.csv\n";
        return 1;
    }

    while (true) {
        showMenu();
        int opt;
        if (!(cin >> opt)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n\033[31mInvalid input. Please enter a number.\033[0m\n";
            continue;
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
                    cout << "\033[31mFailed to load data (see error above).\033[0m\n\n";
                    dataLoaded = false;
                } else {
                    manager = make_unique<ConferenceManager>(submissions, reviewers, params);
                    dataLoaded = true;
                    cout << "\033[32mFile loaded successfully.\033[0m\n\n";
                }
                break;
            }
            case 2:
                if (!dataLoaded) {
                    cout << "\033[31mLoad an input file first.\033[0m\n\n";
                    break;
                }
                manager->printSubmissions();
                cout << "\n";
                break;

            case 3:
                if (!dataLoaded) {
                    cout << "\033[31mLoad an input file first.\033[0m\n\n";
                    break;
                }
                manager->printReviewers();
                cout << "\n";
                break;

            case 4:
                if (!dataLoaded) {
                    cout << "\033[31mLoad an input file first.\033[0m\n\n";
                    break;
                }
                manager->printParameters();
                cout << "\n";
                break;

            case 5:
                if (!dataLoaded) {
                    cout << "\033[31mLoad an input file first.\033[0m\n\n";
                    break;
                }
                manager->generateAssignments();
                cout << "Assignments generated to file: " << params.outputFileName << "\n\n";
                break;

            case 6:
                if (!dataLoaded) {
                    cout << "\033[31mLoad an input file first.\033[0m\n\n";
                    break;
                }
                manager->riskAnalysis();
                cout << "Risk analysis appended to file: " << params.outputFileName << "\n\n";
                break;

            default:
                cout << "\n\033[31mInvalid option. Please choose a valid menu option.\033[0m\n\n";
        }
    }

    return 0;
}