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
         << "[3] todo..." << endl
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
        cout << "todo...\n";
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
            cout << p.first << ',' << p.second << endl;
        cout << endl
             << "You can also check this information in the output folder (maxFLow.csv)." << endl
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
