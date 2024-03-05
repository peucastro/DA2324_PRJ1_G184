#include "../inc/Node.hpp"

Node::Node() {}

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
