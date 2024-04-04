#ifndef WATER_NETWORK_H
#define WATER_NETWORK_H

#include "Utils.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cmath>

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

#endif
