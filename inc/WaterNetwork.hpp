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

    /**
     * @brief Calculate metrics to assess the balance of water flow in a given graph. (T2.3)
     *
     * This function calculates various metrics to evaluate the balance of water flow in a provided graph.
     * It assesses the difference between the weight of each edge (representing pipeline capacity) and its flow,
     * both before and after attempting to balance the flow across the network.
     *
     * The function first creates a super source and a super sink node in the graph and uses the Edmonds-Karp algorithm
     * to find the maximum flow in the network. Then, it iterates over all edges in the graph to compute the differences
     * between the edge weights and their corresponding flow values. These differences are used to calculate the maximum,
     * average, and variance of the differences before attempting to balance the flow.
     *
     * Next, the function attempts to balance the flow across the network by adjusting the flow in each pipeline.
     * It identifies the pipeline with the maximum difference and the pipeline with the minimum difference and iteratively
     * transfers flow from the maximum to the minimum until the differences are minimized.
     *
     * After balancing the flow, the function recalculates the maximum, average, and variance of the differences.
     *
     * Finally, the function stores the calculated metrics, including the maximum difference, average difference,
     * and variance of differences before and after balancing, in a vector of pairs and returns it.
     *
     * @param g Pointer to the graph for which metrics are to be calculated.
     * @return A vector of pairs containing the calculated metrics.
     */
    std::vector<std::pair<std::string, double>> calculateMetrics(Graph<Node> *g) const;

    /**
     * @brief Evaluate the impact of removing a reservoir from the water supply network. (T3.1)
     *
     * This function calculates the impact of removing a reservoir specified by its code from the water supply network.
     * It first validates the provided reservoir code to ensure it corresponds to a reservoir node in the network.
     * Then, it determines the change in water flow to each city in the network before and after removing the reservoir.
     * The function achieves this by calculating the maximum flow of water to each city using the multiSinkMaxFlow function.
     *
     * After validating the reservoir code and calculating the initial flow, the function constructs a subgraph
     * consisting only of the connected component of the network that includes the specified reservoir.
     * It then recalculates the maximum flow of water to each city in this subgraph.
     *
     * Finally, the function prints the impact of removing the reservoir by comparing the previous and current flows
     * to each city and displaying the difference, if any, in water flow.
     *
     * @param reservoir_code The code of the reservoir to be removed from the network.
     *
     * @throw std::runtime_error if the provided reservoir code is not valid or does not correspond to a reservoir node in the network.
     *
     * @complexity The time complexity of this function is O(V * E^2), where V is the number of vertices and E is the number of edges in the graph.
     * This complexity arises from the operations performed to retrieve previous flow data, construct the subgraph, and calculate the current flow to each city.
     */
    void evaluateReservoirImpact(const std::string &reservoir_code) const;

    /**
     * @brief Evaluate the impact of removing all reservoirs from the water supply network, one at a time. (T3.1)
     *
     * This function assesses the consequences of removing each reservoir from the water supply network individually.
     * It calculates the change in water flow to each city in the network before and after removing each reservoir.
     *
     * To begin, the function computes the initial water flow to each city using the multiWaterNeeds function.
     * Then, for each reservoir node in the network, it constructs a subgraph that contains only the connected component
     * of the network including the reservoir. Subsequently, it determines the current maximum flow of water to each city
     * within this subgraph. Finally, the function prints the impact of removing the reservoir by comparing the previous
     * and current flows to each city and displaying any differences in water flow.
     *
     * @complexity The time complexity of this function is O(V * E^2 + V^2), where V is the number of vertices and E is the number of edges in the graph.
     * This complexity arises from the operations performed to retrieve previous flow data, construct subgraphs for each reservoir,
     * calculate current flows, and print the impact for each reservoir.
     */
    void evaluateAllReservoirImpact() const;

    /**
     * @brief Evaluate the impact of removing all pumping stations from the water supply network, one at a time. (T3.1)
     *
     * This function assesses the consequences of removing each pumping station from the water supply network individually.
     * It calculates the change in water flow to each city in the network before and after removing each pumping station.
     *
     * To begin, the function computes the initial water flow to each city using the multiSinkMaxFlow function.
     * Then, for each pumping station node in the network, it constructs a subgraph that contains only the connected component
     * of the network excluding the pumping station. Subsequently, it determines the current maximum flow of water to each city
     * within this subgraph. Finally, the function prints the impact of removing the pumping station by comparing the previous
     * and current flows to each city and displaying any differences in water flow.
     *
     * @complexity The time complexity of this function is O(V * E^2 + V^2), where V is the number of vertices and E is the number of edges in the graph.
     * This complexity arises from the operations performed to retrieve previous flow data, construct subgraphs for each pumping station,
     * calculate current flows, and print the impact for each pumping station.
     */
    void evaluateAllPumpingStationImpact() const;

    /**
     * @brief Evaluate the impact of removing pipelines on the water supply to a specific city. (T3.3)
     *
     * This function assesses the impact of removing pipelines on the water supply to a specific city within the network.
     * It calculates the change in water deficit for the specified city before and after removing each pipeline connected to it.
     *
     * To begin, the function computes the initial water deficit for the city using the multiWaterNeeds function.
     * Then, for each pipeline connected to the specified city, it temporarily removes the pipeline from the network,
     * constructs a subgraph containing only the connected component of the network that includes the origin node of the pipeline,
     * and calculates the current water deficit for the city in this subgraph.
     *
     * If there is a change in water deficit for the city after removing the pipeline, it indicates that removing the pipeline impacts
     * the water supply to the city, and the function prints the pipeline's origin and destination codes along with the magnitude of the impact.
     * If there is no change in water deficit, it means that removing the pipeline does not affect the water supply to the city, and
     * the function indicates this as well.
     *
     * @param city_code The code of the city for which the impact of removing pipelines is to be evaluated.
     *
     * @complexity The time complexity of this function depends on the size of the network and the number of pipelines connected to the specified city.
     * For each pipeline, it constructs a subgraph, which has a complexity of O(V + E), where V is the number of vertices and E is the number of edges.
     * Thus, the overall time complexity is O(V * E + V * E^2), where V is the number of vertices and E is the number of edges in the graph.
     */
    void evaluatePipelineImpact(const std::string &city_code) const;

private:
    Graph<Node> *network; //**< The water supply network Graph */
};

#endif
