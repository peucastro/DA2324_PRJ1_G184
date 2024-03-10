#ifndef APP_H
#define APP_H

#include "WaterNetwork.hpp"

class App
{
public:
    App(WaterNetwork &waternetwowk);

    void mainMenu();
    void goBackMainMenu();
    void statisticsMenu();
    void maxFlowMenu();

private:
    WaterNetwork waternetwork;
};

#endif
