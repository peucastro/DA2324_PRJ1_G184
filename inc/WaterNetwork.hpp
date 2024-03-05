#ifndef WATER_NETWORK_H
#define WATER_NETWORK_H

#include "../inc/Graph.hpp"
#include "../inc/Node.hpp"
#include <string>

class WaterSupplyNetwork
{
public:
    WaterSupplyNetwork(const std::string reservoir_filename, const std::string stations_filename, const std::string cities_filename, const std::string pipes_filename);

private:
    Graph<Node> *network;
};

#endif
