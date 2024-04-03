#include "../inc/App.hpp"
using namespace std;

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

App::App(WaterNetwork &waternetwork)
{
    this->waternetwork = waternetwork;
    clearScreen();
    mainMenu();
}

void App::mainMenu()
{
    unsigned int option;
    cout << "=================================================================================================" << endl
         << "Welcome to the Portuguese continental water supply network system!" << endl
         << "Please enter your option:" << endl
         << endl
         << "[1] Check the water supply network statistics" << endl
         << "[2] See the maximum amount of water that can reach each or a specific city." << endl
         << "[3] Verify if the network configuration meets the water needs of its customer" << endl
         << "[4] Evaluate the network resiliency if a reservoir gets out of commission." << endl
         << "[5] Evaluate the network resiliency if a pumping station is in periodic maintenance." << endl
         << "[6] Evaluate the city resiliency if a pipeline gets ruptured" << endl
         << "[7] See some statistics about the pipes balance" << endl
         << "[0] Exit" << endl
         << "=================================================================================================" << endl
         << "-> ";
    cin >> option;
    clearScreen();

    switch (option)
    {
    case 1:
    {
        clearScreen();
        statisticsMenu();
        break;
    }
    case 2:
    {
        clearScreen();
        maxFlowMenu();
        break;
    }
    case 3:
    {
        clearScreen();
        waterNeedsMenu();
        break;
    }
    case 4:
    {
        clearScreen();
        reservoirImpactMenu();
        break;
    }
    case 5:
    {
        clearScreen();
        stationImpactMenu();
        break;
    }
    case 6:
    {
        clearScreen();
        pipeImpactMenu();
        break;
    }
    case 7:
    {
        clearScreen();
        balanceMenu();
        break;
    }
    case 0:
    {
        exit(EXIT_SUCCESS);
    }
    default:
    {
        clearScreen();
        cout << "Invalid option! Please try again:" << endl
             << endl;
        mainMenu();
    }
    }
}

void App::goBackMainMenu()
{
    unsigned int option;
    cout << "[0] Go back to the main menu." << endl
         << endl;

    while (true)
    {
        cout << "-> ", cin >> option;
        if (option == 0)
        {
            clearScreen();
            mainMenu();
            break;
        }
        else
        {
            cout << "Invalid option, please try again:" << endl
                 << endl;
        }
    }
}

void App::statisticsMenu()
{
    vector<Node> reservoirs;
    vector<Node> stations;
    vector<Node> cities;
    vector<Edge<Node> *> pipes;

    for (Vertex<Node> *v : waternetwork.getNetworkGraph()->getVertexSet())
    {
        if (v->getInfo().getType() == 0)
            reservoirs.push_back(v->getInfo());
        else if (v->getInfo().getType() == 1)
            stations.push_back(v->getInfo());
        else
            cities.push_back(v->getInfo());

        for (Edge<Node> *e : v->getAdj())
            pipes.push_back(e);
    }

    cout << "=================================================================================================" << endl
         << reservoirs.size() << " reservoirs:" << endl;
    for (const Node &r : reservoirs)
        cout << '[' << r.getCode() << "] " << r.getReservoir() << endl;

    cout << "=================================================================================================" << endl
         << stations.size() << " stations:" << endl;
    for (const Node &s : stations)
        cout << s.getCode() << endl;

    cout << "=================================================================================================" << endl
         << cities.size() << " cities:" << endl;
    for (const Node &c : cities)
        cout << '[' << c.getCode() << "] " << c.getMunicipality() << endl;

    cout << "=================================================================================================" << endl
         << pipes.size() << " pipes:" << endl;
    for (const Edge<Node> *p : pipes)
        cout << p->getOrig()->getInfo().getCode() << " --- " << p->getWeight() << "l ---> " << p->getDest()->getInfo().getCode() << endl;
    cout << "=================================================================================================" << endl;

    goBackMainMenu();
}

void App::maxFlowMenu()
{
    unsigned int sink_type;
    cout << "=================================================================================================" << endl
         << "Select an option:" << endl
         << "[1] See the maximum amount of water that can reach each city." << endl
         << "[2] See the maximum amount of water that can reach a specific city." << endl
         << "[0] Go back to the main menu." << endl
         << "=================================================================================================" << endl
         << "-> ";
    cin >> sink_type;

    switch (sink_type)
    {
    case 1:
    {
        clearScreen();
        vector<pair<string, double>> pairs;
        double totalFlow = 0.0;

        try
        {
            pairs = waternetwork.multiSinkMaxFlow();
        }
        catch (const std::exception &e)
        {
            clearScreen();
            std::cerr << e.what() << '\n';
            mainMenu();
        }

        cout << "=================================================================================================" << endl
             << "Maximum amount of water that can reach each city:" << endl;
        for (const pair<string, double> &p : pairs)
        {
            cout << p.first << ',' << p.second << endl;
            totalFlow += p.second;
        }
        cout << endl
             << "Toal flow: " << totalFlow << endl
             << endl
             << "You can also check this information in the output folder (MaxFLow.csv)." << endl
             << "=================================================================================================" << endl;
        goBackMainMenu();
        break;
    }
    case 2:
    {
        clearScreen();
        string city_code;
        cout << "Inform the selected city code:" << endl
             << "-> ";
        cin >> city_code;

        city_code = upperCase(city_code);

        Node city_node(city_code);
        Vertex<Node> *city_vertex = waternetwork.getNetworkGraph()->findVertex(city_node);
        double flow;

        try
        {
            flow = waternetwork.singleSinkMaxFlow(city_code);
        }
        catch (const std::exception &e)
        {
            clearScreen();
            std::cerr << e.what() << '\n';
            mainMenu();
        }

        cout << endl
             << "=================================================================================================" << endl
             << "The maximum amount of water that can reach " << city_vertex->getInfo().getMunicipality() << " is " << flow << endl
             << "=================================================================================================" << endl;
        goBackMainMenu();
        break;
    }
    case 0:
    {
        clearScreen();
        mainMenu();
        break;
    }
    default:
    {
        clearScreen();
        cout << "Invalid option! Please try again:" << endl
             << endl;
        maxFlowMenu();
    }
    }
}

void App::waterNeedsMenu()
{
    unsigned int option_type;
    cout << "=================================================================================================" << endl
         << "Select an option:" << endl
         << "[1] Check if the network meet the water needs of each city." << endl
         << "[2] Check if the network meet the water needs of a specific city." << endl
         << "[0] Go back to the main menu." << endl
         << "=================================================================================================" << endl
         << "-> ";
    cin >> option_type;

    switch (option_type)
    {
    case 1:
    {
        clearScreen();
        vector<pair<string, double>> pairs;
        double totalFlow = 0.0;

        try
        {
            pairs = waternetwork.multiWaterNeeds(waternetwork.getNetworkGraph(), true);
        }
        catch (const std::exception &e)
        {
            clearScreen();
            std::cerr << e.what() << '\n';
            mainMenu();
        }
        cout << "=================================================================================================" << endl;
        cout << setw(6) << left << "Cities " << setw(25) << left << "| Enough water supplied?"
             << "| Residual demand:" << endl;
        cout << "----------------------------------------------------" << endl;
        for (const pair<string, double> &p : pairs)
        {
            cout << setw(18) << left << p.first;

            if (p.second != 0)
            {
                cout << setw(20) << left << "no";
            }
            else
            {
                cout << setw(20) << left << "yes";
            }

            if (p.second == 0)
            {
                cout << "--" << endl;
            }
            else
                cout << p.second << endl;
            totalFlow += p.second;
        }
        cout << endl
             << "Total amount of water flow in deficit: " << totalFlow << endl
             << endl
             << "You can also check this information in the output folder (WaterNeeds.csv)." << endl
             << "=================================================================================================" << endl;
        goBackMainMenu();
        break;
    }
    case 2:
    {
        clearScreen();
        string city_code;
        cout << "Inform the selected city code:" << endl
             << "-> ";
        cin >> city_code;

        city_code = upperCase(city_code);

        Node city_node(city_code);
        Vertex<Node> *city_vertex = waternetwork.getNetworkGraph()->findVertex(city_node);

        double flow;
        try
        {
            flow = waternetwork.singleSinkMaxFlow(city_code);
        }
        catch (const std::exception &e)
        {
            clearScreen();
            std::cerr << e.what() << '\n';
            mainMenu();
        }

        int waterDefict = abs(static_cast<int>(flow) - city_vertex->getInfo().getDemand());

        cout << endl
             << "=================================================================================================" << endl
             << "The city " << city_vertex->getInfo().getMunicipality();
        if (waterDefict != 0)
            cout << " can't";
        else
            cout << " can";
        cout << " be supplied by the desired water rate level. " << endl;

        if (waterDefict != 0)
            cout << "The water flow in deficit is " << waterDefict << endl;
        cout << "=================================================================================================" << endl;
        goBackMainMenu();
        break;
    }
    case 0:
    {
        clearScreen();
        mainMenu();
        break;
    }
    default:
    {
        clearScreen();
        cout << "Invalid option! Please try again:" << endl
             << endl;
        waterNeedsMenu();
    }
    }
}

void App::reservoirImpactMenu()
{
    string reservoir_code;
    vector<pair<string, double>> pairs;
    double totalImpact;
    cout << "Please inform the selected reservoir code (or type 0 if you want to check the impact of each reservoir):" << endl
         << "-> ";
    cin >> reservoir_code;

    reservoir_code = upperCase(reservoir_code);

    if (reservoir_code[0] != '0')
    {
        try
        {
            pairs = waternetwork.evaluateReservoirImpact(reservoir_code);
        }
        catch (const std::exception &e)
        {
            clearScreen();
            std::cerr << e.what() << '\n';
            mainMenu();
        }

        cout << "=================================================================================================" << endl
             << setw(10) << left << "Cities" << setw(15) << left << "| Impacted?" << setw(15) << left << "| Impact" << endl
             << "-------------------------------------------------------------------------------" << endl;

        for (const pair<string, double> &p : pairs)
        {
            cout << setw(15) << left << p.first;

            if (p.second == 0)
                cout << setw(15) << "no" << setw(15) << "--" << '\n';
            else
                cout << setw(15) << "yes" << setw(15) << p.second << '\n';

            totalImpact += p.second;
        }

        cout << endl
             << "Total impact amount: " << totalImpact << endl
             << endl
             << "=================================================================================================" << endl;
    }
    else
    {
        cout << endl
             << "=================================================================================================" << endl;
        waternetwork.evaluateAllReservoirImpact();
        cout << endl
             << "=================================================================================================" << endl;
    }

    goBackMainMenu();
}

void App::pipeImpactMenu()
{
    string city_code;
    vector<pair<string, double>> pairs;
    double totalImpact;
    cout << "Please inform the selected city code:" << endl
         << "-> ";
    cin >> city_code;
    city_code = upperCase(city_code);

    try
    {
        waternetwork.evaluatePipelineImpact(city_code);
    }
    catch (const std::exception &e)
    {
        clearScreen();
        std::cerr << e.what() << '\n';
        mainMenu();
    }

    goBackMainMenu();
}

void App::stationImpactMenu(){

    string answer;
    cout << "Can any pumping station be temporarily taken out of service without affecting the delivery capacity to all the cities?" << endl
         << "The answer is yes (or no)! XX could be temporarily removed.\n"
         << "================================================================================================= \n" <<
            "Do you want to check the impact of temporarily removing each pumping station? [Y/N] \n"
            << "-> ";
    cin >> answer;
    answer = upperCase(answer);

    if (answer == "Y"){

        cout << endl
             << "================================================================================================= \n" <<
             "Let's check the impact of temporarily removing each pumping station:" << endl;
        waternetwork.evaluateAllPumpingStationImpact();
        cout << endl
             << "=================================================================================================" << endl;
    }

    goBackMainMenu();
}
void App::balanceMenu(){
    cout << "Let's see some statistics about the difference between the capacity of pipes and flow that passes trough them:" << endl;
    cout << "===============================================================================================================" << endl;
    vector<pair<string, double>> pairs = waternetwork.calculateMetrics(waternetwork.getNetworkGraph());
    for(auto pair : pairs){
        cout << pair.first << pair.second << endl;
    }

    cout << "===============================================================================================================" << endl;
    goBackMainMenu();
}

std::string App::upperCase(const std::string &str)
{
    string s;
    for (char c : str)
    {
        s += toupper(c);
    }
    return s;
}
