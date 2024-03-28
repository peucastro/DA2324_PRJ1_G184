#ifndef APP_H
#define APP_H

#include "WaterNetwork.hpp"

class App
{
public:
    App(WaterNetwork &waternetwowk);
    std::string upperCase(const std::string &str);
    void mainMenu();
    void goBackMainMenu();
    void statisticsMenu();
    void maxFlowMenu();
    void waterNeedsMenu();
    void reservoirImpactMenu();
    void stationImpactMenu();
    void pipeImpactMenu();

private:
    WaterNetwork waternetwork;
};

#endif
