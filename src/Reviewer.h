#ifndef DA2026_PRJ1_REVIEWER_H
#define DA2026_PRJ1_REVIEWER_H

#include <string>
using namespace std;

/**
 * @brief Represents a reviewer available to evaluate conference submissions.
 */
struct Reviewer {
    int id;        ///< Unique identifier of the reviewer.
    string name;   ///< Full name of the reviewer.
    string email;  ///< Email address of the reviewer.
    int primary;   ///< Primary domain of expertise (used for matching with submissions).
    int secondary; ///< Secondary domain of expertise. Set to 0 if not applicable.
};

#endif //DA2026_PRJ1_REVIEWER_H