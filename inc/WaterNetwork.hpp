#ifndef WATER_NETWORK_H
#define WATER_NETWORK_H

#include "../inc/Graph.hpp"
#include "../inc/Node.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

class WaterNetwork
{
public:
    WaterNetwork();
    WaterNetwork(const std::string reservoirs_filename, const std::string stations_filename, const std::string cities_filename, const std::string pipes_filename);
    ~WaterNetwork();
    Graph<Node> *getNetworkGraph() const;
    double singleSinkMaxFlow(const std::string &city_code) const;
    std::vector<std::pair<std::string, double>> multiSinkMaxFlow() const;

private:
    Graph<Node> *network;
};

Node createSuperSource(Graph<Node> *g);
Node createSuperSink(Graph<Node> *g);

#endif
