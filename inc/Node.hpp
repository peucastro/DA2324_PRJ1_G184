#ifndef NODE_H
#define NODE_H

#include <string>

class Node
{
public:
    Node();                                                                                                                                          // Default constructor
    Node(const unsigned char &type, const std::string &code, const std::string &reservoir, const std::string &municipality, const int &maxDelivery); // Reservoir constructor
    Node(const unsigned char &type, const std::string &code);                                                                                        // Station constructor
    Node(const unsigned char &type, const std::string &code, const std::string &municipality, const int &demand);                                     // Cities constructor
    Node(const std::string &code); //Simple constructor to find nodes

    // Methods for common attributes
    size_t getType() const;
    std::string getCode() const;

    // Methods for reservoir exclusive attributes
    std::string getReservoir() const;
    std::string getMunicipality() const;
    int getMaxDelivery() const;

    // Methods for city exclusive attributes
    int getDemand() const;
    int getPopulation() const;

    //  Operators overload
    bool operator==(const Node &other);

private:
    // Common attributes
    unsigned char type; // 0 -> Reservoir  ;  1 -> Station  ;  2 -> City
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
