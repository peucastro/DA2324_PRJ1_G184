#ifndef WATER_NETWORK_H
#define WATER_NETWORK_H

#include "../inc/Graph.hpp"
#include "../inc/Node.hpp"
#include <string>
#include <fstream>
#include <sstream>

class WaterNetwork
{
public:
    WaterNetwork();
    WaterNetwork(const std::string reservoirs_filename, const std::string stations_filename, const std::string cities_filename, const std::string pipes_filename);

    Graph<Node> *getNetworkGraph() const;

private:
    Graph<Node> *network;
};

#endif
