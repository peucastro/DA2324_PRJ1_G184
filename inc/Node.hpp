#ifndef NODE_H
#define NODE_H

#include <string>

class Node
{
public:
    Node();

    // Getters for common attributes
    size_t getType() const;
    std::string getCode() const;

    // Getters for reservoir exclusive attributes
    std::string getReservoir() const;
    std::string getMunicipality() const;
    int getMaxDelivery() const;

    // Getters for city exclusive attributes
    int getDemand() const;
    int getPopulation() const;

private:
    // Common attributes
    size_t type; // 0 -> Reservoir  ;  1 -> Station  ;  2 -> City
    std::string code;

    // Reservoir exclusive attributes
    std::string reservoir;
    std::string municipality;
    int maxDelivery;

    // City exclusive attributes
    int demand;
    int population;
};

#endif
