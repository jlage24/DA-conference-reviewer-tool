#ifndef DA2026_PRJ1_SUBMISSION_H
#define DA2026_PRJ1_SUBMISSION_H

#include <string>
using namespace std;

/**
 * @brief Represents a paper submission to the conference.
 */
struct Submission {
    int id;        ///< Unique identifier of the submission.
    string title;  ///< Title of the submitted paper.
    string authors;///< Authors of the submitted paper.
    string email;  ///< Contact email of the submitting author.
    int primary;   ///< Primary domain of the submission (used for matching with reviewers).
    int secondary; ///< Secondary domain of the submission. Set to 0 if not applicable.
};

#endif //DA2026_PRJ1_SUBMISSION_H