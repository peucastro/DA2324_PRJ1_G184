#include "../inc/Node.hpp"

Node::Node() {}

Node::Node(const unsigned char &type, const std::string &code, const std::string &reservoir, const std::string &municipality, const int &maxDelivery) : type(0), code(code), reservoir(reservoir), municipality(municipality), maxDelivery(maxDelivery) {}

Node::Node(const unsigned char &type, const std::string &code) : type(1), code(code) {}

size_t Node::getType() const
{
    return type;
}

std::string Node::getCode() const
{
    return code;
}

std::string Node::getReservoir() const
{
    return reservoir;
}

std::string Node::getMunicipality() const
{
    return municipality;
}

int Node::getMaxDelivery() const
{
    return maxDelivery;
}

int Node::getDemand() const
{
    return demand;
}

int Node::getPopulation() const
{
    return population;
}

bool Node::operator==(const Node &other)
{
    return this->code == other.code;
}
