cmake_minimum_required(VERSION 3.26)
project(DA2026_PRJ1)

set(CMAKE_CXX_STANDARD 20)

include_directories(data_structures)

add_executable(DA2026_PRJ1 src/main.cpp
        data-structures/MutablePriorityQueue.h
        data-structures/Graph.h
        src/Reviewer.h
        src/Submission.h
        src/Parser.h
        src/ConferenceManager.h)
