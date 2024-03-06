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

    ifstream pipes_file(pipes_filename);
    if (!pipes_file.is_open())
        throw runtime_error("Pipes file does not exist!");

    getline(pipes_file, line);
    while (getline(pipes_file, line))
    {
        istringstream iss(line);
        string source_str, target_str, cap_str, dir_str;
        getline(getline(getline(getline(iss, source_str, ','), target_str, ','), cap_str, ','), dir_str, '\r');

        double cap = std::stod(cap_str);

        Node source_node(source_str);
        Node target_node(target_str);

        Vertex<Node>* source_v = network->findVertex(source_node);
        Vertex<Node>* target_v = network->findVertex(target_node);

        if(dir_str == "1"){
            source_v->addEdge(target_v, cap);
        }
        else{
            source_v->addEdge(target_v, cap);
            target_v->addEdge(source_v, cap);
        }
    }
}

Graph<Node> *WaterNetwork::getNetworkGraph() const
{
    return network;
}