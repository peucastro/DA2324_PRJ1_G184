#ifndef UTILS_H
#define UTILS_H

#include "Graph.hpp"
#include "Node.hpp"

/**
 * @brief Helper function to test and visit vertices during BFS traversal.
 *
 * This function checks if the vertex 'w' is not visited and there is residual capacity.
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
 * @brief Function to find an augmenting path in the graph using BFS.
 *
 * This function marks all vertices as not visited, marks the source vertex as visited, and performs BFS traversal to find an augmenting path from the source to the target vertex.
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
 * @brief Function to find the minimum residual capacity along an augmenting path.
 *
 * This function traverses the augmenting path from the target vertex to the source vertex to find the
 * minimum residual capacity.
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
 * @brief Function to augment flow along an augmenting path.
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
 * @brief Function implementing the Edmonds-Karp algorithm for maximum flow.
 *
 * This function finds the maximum flow in the graph from a source vertex to a target vertex using the Edmonds-Karp algorithm.
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
 * @brief Function to reset the state of the graph.
 *
 * This function resets the state of the graph by marking all vertices as not visited, clearing paths, current flow, and used delivery.
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
 * @brief Function to create a super source node in the graph.
 *
 * This function creates a super source node in the graph with maximum delivery capacity.
 * It adds edges from the super source to all reservoir nodes in the graph except itself.
 *
 * @param g The graph to add the super source node.
 * @return The super source node created.
 *
 * @complexity O(V)
 */
Node createSuperSource(Graph<Node> *g);

/**
 * @brief Function to create a super sink node in the graph.
 *
 * This function creates a super sink node in the graph with maximum demand capacity.
 * It adds edges from all city nodes in the graph except itself to the super sink.
 *
 * @param g The graph to add the super sink node.
 * @return The super sink node created.
 *
 * @complexity O(V)
 */
Node createSuperSink(Graph<Node> *g);

/**
 * @brief Depth-first search visit function to find connected components.
 *
 * This function performs a depth-first search (DFS) traversal starting from a given vertex to find all
 * connected components in the graph. It marks visited vertices and adds them to the result vector.
 *
 * During the traversal, if the adjacent vertex 'w' is either a Pumping Station (type 1) or a city (type 2),
 * the DFS traversal continues recursively from 'w' to explore further connected vertices.
 * Otherwise, if 'w' is a Reservoir (type 0), it is marked as visited and added to the result vector.
 *
 * @param v The vertex to start the DFS traversal from.
 * @param res The vector to store the visited vertices representing the connected component.
 *
 * @complexity O(V + E), where V is the number of vertices and E is the number of edges in the graph.
 */
void dfsVisitFindCC(Vertex<Node> *v, std::vector<Vertex<Node> *> &res);

/**
 * @brief Find connected component containing a specific node.
 *
 * This function finds the connected component in the graph that contains the specified node.
 * It performs a depth-first search (DFS) traversal from the specified node to identify the connected component.
 *
 * The function constructs a subgraph representing the connected component by adding edges from
 * the original graph based on the connections of vertices within the connected component.
 *
 * @param g The graph to search for the connected component.
 * @param node_code The code of the node indicating the connected component to find.
 * @return A pointer to the subgraph representing the connected component containing the specified node.
 *
 * @throws std::runtime_error if the specified node code is not found in the graph.
 *
 * @complexity O(V + E), where V is the number of vertices and E is the number of edges in the graph.
 */
Graph<Node> *findConnectedComponent(Graph<Node> *g, const std::string &node_code);


#endif
