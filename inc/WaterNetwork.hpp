#ifndef WATER_NETWORK_H
#define WATER_NETWORK_H

#include "../inc/Graph.hpp"
#include "../inc/Node.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stack>

/**
 * @brief Class representing a water supply network.\n
 *
 * This class provides functionality to model and analyze a water supply network, including methods
 * for calculating maximum flow, evaluating network metrics, and managing network components.
 */
class WaterNetwork
{
public:
    /**
     * @brief Default constructor for the WaterNetwork class.
     */
    WaterNetwork();

    /**
     * @brief Constructor for the WaterNetwork class.\n
     * This constructor initializes a water network graph by parsing data from provided files.
     *
     * @param reservoirs_filename The filename of the CSV file containing reservoir data.
     * @param stations_filename The filename of the CSV file containing station data.
     * @param cities_filename The filename of the CSV file containing city data.
     * @param pipes_filename The filename of the CSV file containing pipe data.
     */
    WaterNetwork(const std::string reservoirs_filename, const std::string stations_filename, const std::string cities_filename, const std::string pipes_filename);

    /**
     * @brief Destructor for the WaterNetwork class.
     */
    ~WaterNetwork();

    /**
     * @brief Getter for the network graph.
     * @return A pointer to the network graph.
     */
    Graph<Node> *getNetworkGraph() const;

    double singleSinkMaxFlow(const std::string &city_code) const;                                                 // T2.1
    std::vector<std::pair<std::string, double>> multiSinkMaxFlow() const;                                         // T2.1
    std::vector<std::pair<std::string, double>> multiWaterNeeds(Graph<Node> *g, const bool &flag) const;          // T2.2
    std::vector<std::pair<std::string, double>> calculateMetrics(Graph<Node> *g) const;                           // T2.3
    std::vector<std::pair<std::string, double>> evaluateReservoirImpact(const std::string &reservoir_code) const; // T3.1
    void evaluateAllReservoirImpact() const;                                                                      // T3.1
    void evaluateAllPumpingStationImpact() const;                                                                 // T3.2
    void evaluatePipelineImpact(const std::string &city_code) const;                                              // T3.3

private:
    Graph<Node> *network; //**< The water supply network Graph */
};

/**
 * @brief Helper function to test and visit vertices during BFS traversal.\n
 *
 * This function checks if the vertex 'w' is not visited and there is residual capacity.\n
 * If so, it marks 'w' as visited, sets the path through which it was reached, and enqueues it.
 *
 * @param q The queue of vertices to visit.
 * @param e The edge connecting the current vertex to 'w'.
 * @param w The vertex to visit.
 * @param residual The residual capacity of the edge connecting the current vertex to 'w'.
 *
 * @complexity O(1)
 */
void testAndVisit(std::queue<Vertex<Node> *> &q, Edge<Node> *e, Vertex<Node> *w, double residual);

/**
 * @brief Function to find an augmenting path in the graph using BFS.\n
 *
 * This function marks all vertices as not visited, marks the source vertex as visited, and performs BFS traversal to find an augmenting path from the source to the target vertex.\n
 * It returns true if a path to the target vertex is found, and false otherwise.
 *
 * @param g The graph to search for augmenting paths.
 * @param s The source vertex.
 * @param t The target vertex.
 * @return True if an augmenting path is found, false otherwise.
 *
 * @complexity O(V + E)
 */
bool findAugmentingPath(Graph<Node> *g, Vertex<Node> *s, Vertex<Node> *t);

/**
 * @brief Function to find the minimum residual capacity along an augmenting path.\n
 *
 * This function traverses the augmenting path from the target vertex to the source vertex to find the
 * minimum residual capacity.\n
 * It considers the capacity of edges and the demand of cities.
 *
 * @param s The source vertex.
 * @param t The target vertex.
 * @return The minimum residual capacity along the augmenting path.
 *
 * @complexity O(V)
 */
double findMinResidualAlongPath(Vertex<Node> *s, Vertex<Node> *t);

/**
 * @brief Function to augment flow along an augmenting path.\n
 *
 * This function traverses the augmenting path from the target vertex to the source vertex and updates
 * the flow values of edges and current flow values of vertices accordingly.
 *
 * @param s The source vertex.
 * @param t The target vertex.
 * @param f The flow value to augment along the path.
 *
 * @complexity O(V)
 */
void augmentFlowAlongPath(Vertex<Node> *s, Vertex<Node> *t, double f);

/**
 * @brief Function implementing the Edmonds-Karp algorithm for maximum flow.\n
 *
 * This function finds the maximum flow in the graph from a source vertex to a target vertex using the Edmonds-Karp algorithm.\n
 * It repeatedly finds augmenting paths using BFS and augments flow along the paths until no more augmenting paths exist.
 *
 * @param g The graph representing the water network.
 * @param source The source node.
 * @param target The target node.
 *
 * @complexity O(V * E^2)
 */
void edmondsKarp(Graph<Node> *g, Node source, Node target);

/**
 * @brief Function to reset the state of the graph.\n
 *
 * This function resets the state of the graph by marking all vertices as not visited, clearing paths, current flow, and used delivery.\n
 * It also resets the flow of all edges to zero. Additionally, it removes the super source and super sink nodes from the graph.
 *
 * @param g The graph to reset.
 * @param s The super source node to remove.
 * @param t The super sink node to remove.
 *
 * @complexity O(V + E)
 */
void resetGraph(Graph<Node> *g, const Node &s, const Node &t);

/**
 * @brief Function to create a super source node in the graph.\n
 *
 * This function creates a super source node in the graph with maximum delivery capacity.\n
 * It adds edges from the super source to all reservoir nodes in the graph except itself.
 *
 * @param g The graph to add the super source node.
 * @return The super source node created.
 *
 * @complexity O(V)
 */
Node createSuperSource(Graph<Node> *g);

/**
 * @brief Function to create a super sink node in the graph.\n
 *
 * This function creates a super sink node in the graph with maximum demand capacity.\n
 * It adds edges from all city nodes in the graph except itself to the super sink.
 *
 * @param g The graph to add the super sink node.
 * @return The super sink node created.
 *
 * @complexity O(V)
 */
Node createSuperSink(Graph<Node> *g);

Graph<Node> *findConnectedComponent(Graph<Node> *g, const std::string &node_code);

#endif
