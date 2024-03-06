#include "../inc/WaterNetwork.hpp"
#include "../inc/Node.hpp"
#include <iostream>
using namespace std;

// g++ -o parsingDataTest parsingDataTest.cpp ../src/Node.cpp ../src/WaterNetwork.cpp


void testCities(WaterNetwork *wn)
{
    cout << "Cities:\n";
    for (const Vertex<Node> *v : wn->getNetworkGraph()->getVertexSet())
    {
        if (v->getInfo().getType() == 2)
            cout << "Code: " << v->getInfo().getCode() << ", Demand: " << v->getInfo().getDemand() << '\n';
    }
    cout << '\n';
}


void testReservoirs(WaterNetwork *wn)
{
    cout << "Reservoirs:\n";
    for (const Vertex<Node> *v : wn->getNetworkGraph()->getVertexSet())
    {
        if (v->getInfo().getType() == 0)
            cout << v->getInfo().getReservoir() << '\n';
    }
    cout << '\n';
}

void testStations(WaterNetwork *wn)
{
    cout << "Stations:\n";
    for (const Vertex<Node> *v : wn->getNetworkGraph()->getVertexSet())
    {
        if (v->getInfo().getType() == 1)
            cout << v->getInfo().getCode() << '\n';
    }
    cout << '\n';
}

int main()
{
    WaterNetwork wn("Reservoirs_Madeira.csv", "Stations_Madeira.csv", "Cities_Madeira.csv", "Pipes_Madeira.csv");

    try
    {
        testReservoirs(&wn);
        testStations(&wn);
        testCities(&wn);

        cout << "Compiled successfully!\n";
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
}