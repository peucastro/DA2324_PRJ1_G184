#include "../inc/Node.hpp"

Node::Node() {}

Node::Node(const unsigned char &type, const std::string &code, const std::string &reservoir, const std::string &municipality, const int &maxDelivery) : type(0), code(code), reservoir(reservoir), municipality(municipality), maxDelivery(maxDelivery), usedDelivery(0) {}

Node::Node(const unsigned char &type, const std::string &code) : type(1), code(code) {}

Node::Node(const unsigned char &type, const std::string &code, const std::string &municipality, const int &demand) : type(2), code(code), municipality(municipality), demand(demand), currentFlow(0) {}

Node::Node(const std::string &code) : code(code) {}

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

int Node::getUsedDelivery() const
{
    return usedDelivery;
}
void Node::setUsedDelivery(const int &new_delivery)
{
    this->usedDelivery = new_delivery;
}

int Node::getDemand() const
{
    return demand;
}

int Node::getPopulation() const
{
    return population;
}

int Node::getCurrentFlow() const
{
    return currentFlow;
}

void Node::setCurrentFlow(const int &new_flow)
{
    this->currentFlow = new_flow;
}

bool Node::operator==(const Node &other)
{
    return this->code == other.code;
}
