#include "../inc/WaterNetwork.hpp"
using namespace std;

WaterNetwork::WaterNetwork() {}

WaterNetwork::WaterNetwork(const std::string reservoirs_filename, const std::string stations_filename, const std::string cities_filename, const std::string pipes_filename)
{
    network = new Graph<Node>();
    string line;

    // Parse reservoirs file
    ifstream reservoirs_file(reservoirs_filename);
    if (!reservoirs_file.is_open())
        throw runtime_error("Reservoirs file does not exist!");

    getline(reservoirs_file, line);
    while (getline(reservoirs_file, line))
    {
        istringstream iss(line);
        string reservoir_str, municipality_str, id_str, code_str, maxDelivery_str;
        getline(getline(getline(getline(getline(iss, reservoir_str, ','), municipality_str, ','), id_str, ','), code_str, ','), maxDelivery_str, '\r');

        Node new_reservoir(0, code_str, reservoir_str, municipality_str, stoi(maxDelivery_str));

        network->addVertex(new_reservoir);
    }

    // Parse stations file
    ifstream stations_file(stations_filename);
    if (!stations_file.is_open())
        throw runtime_error("Stations file does not exist!");

    getline(stations_file, line);
    while (getline(stations_file, line))
    {
        istringstream iss(line);
        string id_str, code_str;
        getline(getline(iss, id_str, ','), code_str, '\r');

        Node new_station(1, code_str);

        network->addVertex(new_station);
    }

    // Parse cities file
    ifstream cities_file(cities_filename);
    if(!cities_file.is_open())
        throw runtime_error("Cities file does not exist!");
    getline(cities_file, line);
    while (getline(cities_file, line))
    {
        istringstream iss(line);
        string municipality_str, id_str, code_str, demand_str, population_str;
        getline(getline(getline(getline(getline(iss, municipality_str, ','), id_str, ','), code_str, ','), demand_str, ','),population_str, '\r');

        Node new_city(2, code_str, municipality_str, stoi(demand_str));

        network->addVertex(new_city);
    }
}

Graph<Node> *WaterNetwork::getNetworkGraph() const
{
    return network;
}
