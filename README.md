# DA2026 — Project 1: Scientific Conference Organization Tool

A command-line tool to assign paper submissions to reviewers for a scientific conference, built around a Max-Flow formulation using the Edmonds-Karp algorithm.

## What it does

Given a list of submissions and reviewers (with domain expertise), the tool figures out which reviewer covers which paper — respecting a minimum number of reviews per submission and a maximum workload per reviewer. If a full assignment isn't possible, it tells you what's missing and why.

## Building
```bash
cmake -B build
cmake --build build
```

The executable will be at `build/DA2026_PRJ1` (or whatever your CMake target is named).

## Usage

**Interactive mode** (menu-driven):
```bash
./DA2026_PRJ1
```

**Batch mode:**
```bash
./DA2026_PRJ1 -b input.csv output.csv
```

In batch mode, the output file argument overrides whatever `OutputFileName` is set to in the CSV.

Error messages go to stderr in both modes.

## Input format

A `.csv` file split into four sections: `#Submissions`, `#Reviewers`, `#Parameters`, and `#Control`. Anything after `#` on a data line is treated as a comment and ignored.

Example:
```
#Submissions
31, "The Eternal Wheel of Reincarnation", Ralph Kholer, kholer@gmail.com, 3, 4

#Reviewers
1, Jaqueline N. Chame, jchame@yahoo.com, 3,

#Parameters
MinReviewsPerSubmission, 2
MaxReviewsPerReviewer, 4

#Control
GenerateAssignments, 1
RiskAnalysis, 1
OutputFileName, "output.csv"
```

`GenerateAssignments` values: `0` = run but don't report, `1` = primary domains only.  
`RiskAnalysis` values: `0` = skip, `1` = check each reviewer individually.

## Output format

Successful assignment:
```
#SubmissionId,ReviewerId,Match
31, 1, 3
#ReviewerId,SubmissionId,Match
1, 31, 3
#Total: 1
```

Failed assignment:
```
#SubmissionId,Domain,MissingReviews
31, 3, 2
```

Risk analysis:
```
#Risk Analysis: 1
1, 2
```

## Project structure
```
src/                  — source and header files
data-structures/      — Graph.h and MutablePriorityQueue.h (provided, unmodified*)
docs/                 — Doxygen HTML output
```

*`setWeight()` was added to `Edge` in `Graph.h` to support temporarily disabling reviewer edges during risk analysis.

## Documentation

Generated with Doxygen. To rebuild:
```bash
doxygen Doxyfile
```
Then open `docs/html/index.html`.