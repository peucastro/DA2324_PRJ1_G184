#ifndef WATER_NETWORK_H
#define WATER_NETWORK_H

#include "../inc/Graph.hpp"
#include "../inc/Node.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stack>

class WaterNetwork
{
public:
    WaterNetwork();
    WaterNetwork(const std::string reservoirs_filename, const std::string stations_filename, const std::string cities_filename, const std::string pipes_filename);
    ~WaterNetwork();
    Graph<Node> *getNetworkGraph() const;
    double singleSinkMaxFlow(const std::string &city_code) const;                                                 // T2.1
    std::vector<std::pair<std::string, double>> multiSinkMaxFlow() const;                                         // T2.1
    std::vector<std::pair<std::string, double>> multiWaterNeeds() const;                                          // T2.2
    std::vector<std::pair<std::string, double>> evaluateReservoirImpact(const std::string &reservoir_code) const; // T3.1
    void evaluatePipelineImpact(const std::string &city_code) const; // T3.3

private:
    Graph<Node> *network;
};
void testAndVisit(std::queue<Vertex<Node> *> &q, Edge<Node> *e, Vertex<Node> *w, double residual);
bool findAugmentingPath(Graph<Node> *g, Vertex<Node> *s, Vertex<Node> *t);
double findMinResidualAlongPath(Vertex<Node> *s, Vertex<Node> *t);
void augmentFlowAlongPath(Vertex<Node> *s, Vertex<Node> *t, double f);
void edmondsKarp(Graph<Node> *g, Node source, Node target);

void resetGraph(Graph<Node> *g, const Node &s, const Node &t);
Node createSuperSource(Graph<Node> *g);
Node createSuperSink(Graph<Node> *g);
Graph<Node> *findConnectedComponent(Graph<Node> *g, const std::string &node_code);

#endif
