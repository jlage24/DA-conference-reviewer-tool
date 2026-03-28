#ifndef DA2026_PRJ1_CONFERENCEMANAGER_H
#define DA2026_PRJ1_CONFERENCEMANAGER_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <fstream>
#include <algorithm>
#include "Submission.h"
#include "Reviewer.h"
#include "Parser.h"
#include "../data-structures/Graph.h"

/**
 * @brief Manages the conference paper review assignment process.
 *
 * This class encapsulates the logic for building a flow network,
 * running the Edmonds-Karp max-flow algorithm, generating reviewer
 * assignments for submissions, and performing risk analysis on the
 * resulting assignment.
 */
class ConferenceManager {
public:
    /**
     * @brief Constructs a ConferenceManager with the given submissions, reviewers, and parameters.
     *
     * Initialises internal maps for submissions and reviewers, and assigns
     * unique graph node IDs to each submission and reviewer.
     *
     * @param submissions Vector of submissions to be reviewed.
     * @param reviewers   Vector of available reviewers.
     * @param params      Configuration parameters (e.g. min/max reviews).
     */
    ConferenceManager(const vector<Submission>& submissions, const vector<Reviewer>& reviewers, const Parameters& params);

    /**
     * @brief Builds the flow graph and generates reviewer assignments.
     *
     * Runs the Edmonds-Karp algorithm on the flow network. If the maximum
     * flow meets the expected total reviews, outputs the assignments to a file.
     * Otherwise, outputs which submissions are missing reviewers.
     */
    void generateAssignments();

    /**
     * @brief Performs a risk analysis on the current reviewer assignments.
     *
     * For each reviewer, temporarily removes their capacity edge and checks
     * whether the maximum flow is still achievable. Reviewers whose removal
     * causes an infeasible assignment are considered critical and reported.
     */
    void riskAnalysis();

    /**
     * @brief Prints all submissions to standard output.
     */
    void printSubmissions() const;

    /**
     * @brief Prints all reviewers to standard output.
     */
    void printReviewers() const;

    /**
     * @brief Prints the current configuration parameters to standard output.
     */
    void printParameters() const;

private:
    /// The flow network graph, using integer node IDs as vertex identifiers.
    Graph<int> graph;

    /// Map from submission ID to Submission data.
    unordered_map<int, Submission> submissions;

    /// Map from reviewer ID to Reviewer data.
    unordered_map<int, Reviewer> reviewers;

    /// Map from submission ID to its corresponding graph node ID.
    unordered_map<int, int> submissionNodeIds;

    /// Map from reviewer ID to its corresponding graph node ID.
    unordered_map<int, int> reviewerNodeIds;

    /// Reverse map from graph node ID to reviewer ID.
    unordered_map<int, int> reviewerIdFromNodeId;

    /// Configuration parameters for the assignment process.
    Parameters params;

    /**
     * @brief Builds the residual flow graph from scratch.
     *
     * Adds source, sink, submission, and reviewer nodes, and connects
     * them with appropriate capacity edges based on domain matching
     * and review constraints.
     */
    void buildFlowGraph();

    /**
     * @brief Runs the Edmonds-Karp max-flow algorithm between source and sink.
     *
     * Resets all edge flows, then repeatedly finds augmenting paths via BFS
     * and augments flow along them until no more paths exist.
     *
     * @param source Node ID of the source vertex.
     * @param sink   Node ID of the sink vertex.
     * @return The total maximum flow value.
     */
    double edmondsKarp(int source, int sink);

    // --- Node ID Helpers ---

    /// @brief Returns the node ID of the source vertex (always 0).
    int sourceId();

    /// @brief Returns the node ID of the sink vertex (always 1).
    int sinkId();

    /**
     * @brief Returns the graph node ID for a given submission.
     * @param submissionId The submission's domain ID.
     * @return Corresponding graph node ID.
     */
    int submissionNodeId(int submissionId);

    /**
     * @brief Returns the graph node ID for a given reviewer.
     * @param reviewerId The reviewer's domain ID.
     * @return Corresponding graph node ID.
     */
    int reviewerNodeId(int reviewerId);

    // --- Edmonds-Karp Helpers ---

    /**
     * @brief Performs a BFS to find an augmenting path from source to sink.
     *
     * @param source Node ID of the source vertex.
     * @param sink   Node ID of the sink vertex.
     * @param parent Map populated with the edge used to reach each visited node.
     * @return True if the sink was reached (i.e. an augmenting path exists).
     */
    bool bfs(int source, int sink, unordered_map<int, Edge<int>*>& parent);

    /**
     * @brief Finds the bottleneck (minimum residual capacity) along a path.
     *
     * Traces back from sink to source using the parent map.
     *
     * @param source Node ID of the source vertex.
     * @param sink   Node ID of the sink vertex.
     * @param parent Map of edges forming the augmenting path.
     * @return The bottleneck capacity of the path.
     */
    double findBottleneck(int source, int sink, unordered_map<int, Edge<int>*>& parent);

    /**
     * @brief Augments flow along the path defined by the parent map.
     *
     * Increases flow on forward edges and decreases flow on reverse edges
     * by the given bottleneck amount.
     *
     * @param source     Node ID of the source vertex.
     * @param sink       Node ID of the sink vertex.
     * @param bottleneck The amount of flow to augment.
     * @param parent     Map of edges forming the augmenting path.
     */
    void augmentFlow(int source, int sink, double bottleneck, unordered_map<int, Edge<int>*>& parent);

    /**
     * @brief Adds a forward edge and its corresponding reverse residual edge.
     *
     * The forward edge has the given capacity; the reverse edge starts with
     * capacity 0. Both edges are linked via their reverse pointers.
     *
     * @param from     Source node ID.
     * @param to       Destination node ID.
     * @param capacity Capacity of the forward edge.
     */
    void addResidualEdge(int from, int to, double capacity);
};

#endif //DA2026_PRJ1_CONFERENCEMANAGER_H