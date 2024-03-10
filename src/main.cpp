#include "../inc/Graph.hpp"
#include "../inc/Node.hpp"
#include "../inc/WaterNetwork.hpp"
#include "../inc/App.hpp"

int main()
{
    try
    {
        WaterNetwork waternetwork("../test/Reservoirs_Madeira.csv", "../test/Stations_Madeira.csv", "../test/Cities_Madeira.csv", "../test/Pipes_Madeira.csv");
        App app(waternetwork);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}
