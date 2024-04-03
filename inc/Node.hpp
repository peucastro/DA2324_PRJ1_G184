#ifndef NODE_H
#define NODE_H

#include <string>

/**
 * @class Node
 * @brief Represents a node in a network.
 */
class Node
{
public:
    /** @brief Default constructor. */
    Node();

    /**
     * @brief Constructor for a reservoir node.
     * @param type The type of the node.
     * @param code The code associated with the node.
     * @param reservoir The name of the reservoir.
     * @param municipality The name of the municipality.
     * @param maxDelivery The maximum delivery of the reservoir.
     */
    Node(const unsigned char &type, const std::string &code, const std::string &reservoir, const std::string &municipality, const int &maxDelivery);

    /**
     * @brief Constructor for a station node.
     * @param type The type of the node.
     * @param code The code associated with the node.
     */
    Node(const unsigned char &type, const std::string &code);

    /**
     * @brief Constructor for a city node.
     * @param type The type of the node.
     * @param code The code associated with the node.
     * @param municipality The name of the municipality.
     * @param demand The demand of the city.
     */
    Node(const unsigned char &type, const std::string &code, const std::string &municipality, const int &demand);

    /**
     * @brief Constructor to find nodes.
     * @param code The code associated with the node.
     */
    Node(const std::string &code);

    // Methods for common attributes
    /**
     * @brief Get the type of the node.
     * @return The type of the node.
     */
    size_t getType() const;

    /**
     * @brief Get the code associated with the node.
     * @return The code associated with the node.
     */
    std::string getCode() const;

    // Methods for reservoir exclusive attributes
    /**
     * @brief Get the name of the reservoir.
     * @return The name of the reservoir.
     */
    std::string getReservoir() const;

    /**
     * @brief Get the name of the municipality.
     * @return The name of the municipality.
     */
    std::string getMunicipality() const;

    /**
     * @brief Get the maximum delivery of the reservoir.
     * @return The maximum delivery of the reservoir.
     */
    int getMaxDelivery() const;

    // Methods for city exclusive attributes
    /**
     * @brief Get the demand of the city.
     * @return The demand of the city.
     */
    int getDemand() const;

    /**
     * @brief Get the population of the city.
     * @return The population of the city.
     */
    int getPopulation() const;

    // Operators overload
    /**
     * @brief Overloaded equality operator.
     * @param other The other node to compare.
     * @return True if the nodes codes are equal, false otherwise.
     */
    bool operator==(const Node &other);

private:
    // Common attributes
    unsigned char type; //**< Represents the type of the node (0 -> Reservoir  ;  1 -> Station  ;  2 -> City) */
    std::string code;   //**< The node code */

    // Reservoir exclusive attributes
    std::string reservoir;    //**< The reservoir name */
    std::string municipality; //*< The reservoir municipality */
    int maxDelivery;          //*< The reservoir maximum delivery amount */

    // City exclusive attributes
    int demand;     //*< The city demant amount */
    int population; //*< The city population */
};

#endif
