#ifndef WATER_NETWORK_H
#define WATER_NETWORK_H

#include "Utils.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cmath>

/**
 * @brief Class representing a water supply network.
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
     * @brief Constructor for the WaterNetwork class.
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

    /**
     * @brief Determine the maximum amount of water that can reach a specific city. (T2.1)
     *
     * This function calculates the maximum flow of water that can reach a specified city in the water supply network.
     * It uses the Edmonds-Karp algorithm to find the optimal flow through the network, with water reservoirs set as sources,
     * delivery sites as sinks, and pumping stations as intermediate nodes. The flow reaching the specified city is then computed
     * based on the flow of incoming edges to the city.
     *
     * @param city_code The code of the city for which the maximum flow is to be determined.
     * @return The maximum flow of water that can reach the specified city.
     *
     * @complexity O(V * E^2), where V is the number of vertices and E is the number of edges in the graph.
     */
    double singleSinkMaxFlow(const std::string &city_code) const;

    /**
     * @brief Determine the maximum amount of water that can reach each city in the water supply network. (T2.1)
     *
     * This function calculates the maximum flow of water that can reach each city in the water supply network.
     * It uses the Edmonds-Karp algorithm to find the optimal flow through the network, with water reservoirs set as sources,
     * delivery sites as sinks, and pumping stations as intermediate nodes. The function returns a vector of pairs containing
     * each city's code and the corresponding maximum flow of water reaching it. Additionally, if the flag parameter is set to true,
     * the function writes the results to a CSV file named "MaxFlow.csv" in the "out" directory.
     *
     * @param g A pointer to the graph representing the water supply network.
     * @param flag A boolean flag indicating whether to write the results to a CSV file (true) or not (false).
     * @return A vector of pairs, where each pair consists of a city code and the maximum flow of water reaching that city.
     *
     * @complexity O(V * E^2), where V is the number of vertices and E is the number of edges in the graph.
     */
    std::vector<std::pair<std::string, double>> multiSinkMaxFlow(Graph<Node> *g, const bool &flag) const;

    /**
     * @brief Determine the water deficit or excess for each city in the water supply network. (T2.2)
     *
     * This function calculates the water deficit or excess for each city in the water supply network.
     * It uses the Edmonds-Karp algorithm to find the optimal flow through the network, with water reservoirs set as sources, delivery sites as sinks, and pumping stations as intermediate nodes.
     * The function returns a vector of pairs containing each city's code and the corresponding water deficit or excess.
     * If the flag parameter is set to true, the function also writes the results to a CSV file named "WaterNeeds.csv" in the "out" directory.
     *
     * @param g A pointer to the graph representing the water supply network.
     * @param flag A boolean flag indicating whether to write the results to a CSV file (true) or not (false).
     * @return A vector of pairs, where each pair consists of a city code and the corresponding water deficit or excess.
     *
     * @complexity O(V * E^2), where V is the number of vertices and E is the number of edges in the graph.
     */
    std::vector<std::pair<std::string, double>> multiWaterNeeds(Graph<Node> *g, const bool &flag) const;

    std::vector<std::pair<std::string, double>> calculateMetrics(Graph<Node> *g) const; // T2.3

    /**
     * @brief Evaluate the impact of removing a reservoir from the water supply network. (T3.1)
     *
     * This function evaluates the impact of removing a reservoir specified by its code from the water supply network.
     * It calculates the change in water flow to each city in the network before and after removing the reservoir.
     * The function first checks if the provided reservoir code is valid and corresponds to a reservoir node in the network.
     * Then, it retrieves the previous maximum flow of water to each city from a CSV file named "MaxFlow.csv" if it exists,
     * otherwise, it calculates the maximum flow using the multiSinkMaxFlow function. After that, it constructs a subgraph
     * containing only the connected component of the network that includes the specified reservoir. Next, it calculates
     * the current maximum flow of water to each city in this subgraph. Finally, it prints the impact of removing the reservoir
     * by comparing the previous and current flows to each city and displaying the difference, if any, in water flow.
     *
     * @param reservoir_code The code of the reservoir to be removed from the network.
     *
     * @throw std::runtime_error if the provided reservoir code is not valid or does not correspond to a reservoir node in the network.
     *
     * @complexity O(V^2 + VE + ElogV), where V is the number of vertices and E is the number of edges in the graph.
     * This complexity arises from the operations performed to retrieve previous flow data, construct the subgraph, and calculate
     * the current flow to each city.
     */
    void evaluateReservoirImpact(const std::string &reservoir_code) const;

    /**
     * @brief Evaluate the impact of removing all reservoirs from the water supply network, one ate a time. (T3.1)
     *
     * This function evaluates the impact of removing all reservoirs from the water supply network.
     * It calculates the change in water flow to each city in the network before and after removing each reservoir.
     * The function retrieves the previous maximum flow of water to each city from a CSV file named "MaxFlow.csv" if it exists,
     * otherwise, it calculates the maximum flow using the multiWaterNeeds function. Then, for each reservoir node in the network,
     * it constructs a subgraph containing only the connected component of the network that includes the reservoir, calculates
     * the current maximum flow of water to each city in this subgraph, and prints the impact of removing the reservoir
     * by comparing the previous and current flows to each city and displaying the difference, if any, in water flow.
     *
     * @throw std::runtime_error if the "MaxFlow.csv" file could not be opened or if an error occurs during the evaluation process.
     *
     * @complexity O(V^3 + V^2E + VElogV), where V is the number of vertices and E is the number of edges in the graph.
     * This complexity arises from the operations performed to retrieve previous flow data, construct subgraphs for each reservoir,
     * calculate current flows, and print the impact for each reservoir.
     */
    void evaluateAllReservoirImpact() const;

    /**
     * @brief Evaluate the impact of removing all pumping stations from the water supply network, one at a time. (T3.1)
     *
     * This function evaluates the impact of removing all pumping stations from the water supply network.
     * It calculates the change in water flow to each city in the network before and after removing each pumping station.
     * The function retrieves the previous maximum flow of water to each city from a CSV file named "MaxFlow.csv" if it exists,
     * otherwise, it calculates the maximum flow using the multiSinkMaxFlow function. Then, for each pumping station node in the network,
     * it constructs a subgraph containing only the connected component of the network that excludes the pumping station, calculates
     * the current maximum flow of water to each city in this subgraph, and prints the impact of removing the pumping station
     * by comparing the previous and current flows to each city and displaying the difference, if any, in water flow.
     *
     * @throw std::runtime_error if the "MaxFlow.csv" file could not be opened or if an error occurs during the evaluation process.
     *
     * @complexity O(V^3 + V^2E + VElogV), where V is the number of vertices and E is the number of edges in the graph.
     * This complexity arises from the operations performed to retrieve previous flow data, construct subgraphs for each pumping station,
     * calculate current flows, and print the impact for each pumping station.
     */
    void evaluateAllPumpingStationImpact() const;
    void evaluatePipelineImpact(const std::string &city_code) const; // T3.3

private:
    Graph<Node> *network; //**< The water supply network Graph */
};

#endif
