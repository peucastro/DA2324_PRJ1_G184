#include "../inc/Graph.hpp"
#include "../inc/Node.hpp"
#include "../inc/WaterNetwork.hpp"
#include "../inc/App.hpp"

int main()
{
    try
    {
        WaterNetwork waternetwork("../data/Reservoir.csv", "../data/Stations.csv", "../data/Cities.csv", "../data/Pipes.csv");
        App app(waternetwork);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}
