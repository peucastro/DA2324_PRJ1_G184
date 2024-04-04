#include "../inc/WaterNetwork.hpp"
using namespace std;

WaterNetwork::WaterNetwork() {}

WaterNetwork::WaterNetwork(const string reservoirs_filename, const string stations_filename, const string cities_filename, const string pipes_filename)
{
    network = new Graph<Node>();
    string line;

    // Parse reservoirs file
    ifstream reservoirs_file(reservoirs_filename);
    if (!reservoirs_file.is_open())
        throw runtime_error("Reservoirs file does not exist!");

    getline(reservoirs_file, line);
    while (getline(reservoirs_file, line))
    {
        istringstream iss(line);
        string reservoir_str, municipality_str, id_str, code_str, maxDelivery_str;
        getline(getline(getline(getline(getline(iss, reservoir_str, ','), municipality_str, ','), id_str, ','), code_str, ','), maxDelivery_str, '\r');

        Node new_reservoir(0, code_str, reservoir_str, municipality_str, stoi(maxDelivery_str));

        network->addVertex(new_reservoir);
    }

    // Parse stations file
    ifstream stations_file(stations_filename);
    if (!stations_file.is_open())
        throw runtime_error("Stations file does not exist!");

    getline(stations_file, line);
    while (getline(stations_file, line))
    {
        istringstream iss(line);
        string id_str, code_str;
        getline(getline(iss, id_str, ','), code_str, '\r');

        Node new_station(1, code_str);

        network->addVertex(new_station);
    }

    // Parse cities file
    ifstream cities_file(cities_filename);
    if (!cities_file.is_open())
        throw runtime_error("Cities file does not exist!");

    getline(cities_file, line);
    while (getline(cities_file, line))
    {
        istringstream iss(line);
        string municipality_str, id_str, code_str, demand_str, population_str;
        getline(getline(getline(getline(getline(iss, municipality_str, ','), id_str, ','), code_str, ','), demand_str, ','), population_str, '\r');

        Node new_city(2, code_str, municipality_str, stoi(demand_str));

        network->addVertex(new_city);
    }

    // Parse pipes file
    ifstream pipes_file(pipes_filename);
    if (!pipes_file.is_open())
        throw runtime_error("Pipes file does not exist!");

    getline(pipes_file, line);
    while (getline(pipes_file, line))
    {
        istringstream iss(line);
        string source_str, target_str, cap_str, dir_str;
        getline(getline(getline(getline(iss, source_str, ','), target_str, ','), cap_str, ','), dir_str, '\r');

        double cap = stod(cap_str);

        Node source_node(source_str);
        Node target_node(target_str);

        Vertex<Node> *source_v = network->findVertex(source_node);
        Vertex<Node> *target_v = network->findVertex(target_node);

        if (dir_str == "1")
        {
            source_v->addEdge(target_v, cap);
        }
        else
        {
            source_v->addEdge(target_v, cap);
            target_v->addEdge(source_v, cap);
        }
    }

    reservoirs_file.close();
    stations_file.close();
    cities_file.close();
    pipes_file.close();
}

WaterNetwork::~WaterNetwork()
{
    delete network;
}

Graph<Node> *WaterNetwork::getNetworkGraph() const
{
    return network;
}

double WaterNetwork::singleSinkMaxFlow(const string &city_code) const
{
    Node s = createSuperSource(network);
    Node t = createSuperSink(network);
    edmondsKarp(network, s, t);

    Node city(city_code);
    double flow = 0.0;
    Vertex<Node> *sink_vertex = network->findVertex(city);
    for (Edge<Node> *e : sink_vertex->getIncoming())
        flow += e->getFlow();

    resetGraph(network, s, t);
    return flow;
}

vector<pair<string, double>> WaterNetwork::multiSinkMaxFlow() const
{
    vector<pair<string, double>> res;
    double flow;

    Node s = createSuperSource(network);
    Node t = createSuperSink(network);
    edmondsKarp(network, s, t);

    filesystem::path outputPath("../out");
    if (!filesystem::exists(outputPath))
        filesystem::create_directories(outputPath);
    ofstream out("../out/MaxFlow.csv");
    out.clear();
    out << "CityCode,Flow\r";

    for (Vertex<Node> *v : network->getVertexSet())
    {
        if (v->getInfo().getType() == 2 && v->getInfo().getCode() != "superSink")
        {
            res.push_back(make_pair(v->getInfo().getCode(), v->getCurrentFlow()));
            out << v->getInfo().getCode() << ',' << v->getCurrentFlow() << '\r';
        }
    }

    resetGraph(network, s, t);
    out.close();
    return res;
}

vector<pair<string, double>> WaterNetwork::multiWaterNeeds(Graph<Node> *g, const bool &flag) const
{
    vector<pair<string, double>> res;

    Node s = createSuperSource(g);
    Node t = createSuperSink(g);
    edmondsKarp(g, s, t);

    ofstream out;
    if (flag)
    {
        filesystem::path outputPath("../out");
        if (!filesystem::exists(outputPath))
            filesystem::create_directories(outputPath);
        out.open("../out/WaterNeeds.csv");
        out << "CityCode,WaterDeficit\r";
    }

    for (Vertex<Node> *v : g->getVertexSet())
    {
        if (v->getInfo().getType() == 2 && v->getInfo().getCode() != "superSink")
        {
            res.push_back(make_pair(v->getInfo().getCode(), abs(v->getCurrentFlow() - v->getInfo().getDemand())));
            out << v->getInfo().getCode() << ',' << abs(v->getCurrentFlow() - v->getInfo().getDemand()) << '\r';
        }
    }

    out.close();
    resetGraph(g, s, t);
    return res;
}
vector<pair<string, double>> WaterNetwork::calculateMetrics(Graph<Node> *g) const
{
    vector<pair<string, double>> res;

    Node s = createSuperSource(g);
    Node t = createSuperSink(g);
    edmondsKarp(g, s, t);

    double dif;
    double maxDif = 0;
    double sumDif = 0;
    double var = 0;
    double count = 0;
    double variance = 0;

    for (Vertex<Node> *v : g->getVertexSet())
    {
        for (Edge<Node> *e : v->getAdj())
        {
            dif = e->getWeight() - e->getFlow();
            if (dif > maxDif)
                maxDif = dif;
            sumDif += dif;
            count++;
        }
    }

    double avg = sumDif / count;

    for (Vertex<Node> *v : g->getVertexSet())
    {
        for (Edge<Node> *e : v->getAdj())
        {
            dif = e->getWeight() - e->getFlow();
            var += pow(dif - avg, 2);
        }
    }

    variance = (var / count);

    res.push_back(make_pair("Maximum Difference: ", maxDif));
    res.push_back(make_pair("Average Difference: ", avg));
    res.push_back(make_pair("Variance of Difference: ", variance));

    return res;
}

vector<pair<string, double>> WaterNetwork::evaluateReservoirImpact(const string &reservoir_code) const
{
    vector<pair<string, double>> res;
    Node reservoir(reservoir_code);
    Vertex<Node> *reservoir_vertex = network->findVertex(reservoir);

    if (reservoir_vertex == nullptr || reservoir_vertex->getInfo().getType() != 0)
        throw runtime_error("Please inform a valid reservoir code.");
    reservoir = reservoir_vertex->getInfo();

    vector<pair<string, double>> previousDeficit;
    if (filesystem::exists("../out/WaterNeeds.csv"))
    {
        ifstream waterneeds_file("../out/WaterNeeds.csv");
        if (!waterneeds_file.is_open())
            throw runtime_error("WaterNeeds.csv file could not be opened!");

        string line;
        waterneeds_file >> line;
        while (waterneeds_file >> line)
        {
            istringstream iss(line);
            string cityCode, waterDeficit_str;
            getline(getline(iss, cityCode, ','), waterDeficit_str, '\r');
            double waterDeficit = stod(waterDeficit_str);

            previousDeficit.push_back(make_pair(cityCode, waterDeficit));
        }
        waterneeds_file.close();
    }
    else
        previousDeficit = multiWaterNeeds(network, true);

    reservoir_vertex->setUsedDelivery(reservoir_vertex->getInfo().getMaxDelivery());
    vector<pair<string, double>> currentDeficit = multiWaterNeeds(network, false);

    for (size_t i = 0; i < previousDeficit.size(); i++)
    {
        double diff = previousDeficit[i].second - currentDeficit[i].second;
        res.push_back(make_pair(previousDeficit[i].first, diff));
        cout << "CITY: " << previousDeficit[i].first << "  ;  PREV DEFICIT: " << previousDeficit[i].second << "  ;  CURR DEFICIT: " << currentDeficit[i].second << '\n';
    }

    reservoir_vertex->setUsedDelivery(0);
    return res;
}

void WaterNetwork::evaluateAllReservoirImpact() const
{
    vector<pair<string, double>> previousDeficit;
    if (filesystem::exists("../out/WaterNeeds.csv"))
    {
        ifstream waterneeds_file("../out/WaterNeeds.csv");
        if (!waterneeds_file.is_open())
            throw runtime_error("WaterNeeds.csv file could not be opened!");

        string line;
        waterneeds_file >> line;
        while (waterneeds_file >> line)
        {
            istringstream iss(line);
            string cityCode, waterDeficit_str;
            getline(getline(iss, cityCode, ','), waterDeficit_str, '\r');
            double waterDeficit = stod(waterDeficit_str);

            previousDeficit.push_back(make_pair(cityCode, waterDeficit));
        }
        waterneeds_file.close();
    }
    else
        previousDeficit = multiWaterNeeds(network, true);

    vector<pair<string, double>> currentDeficit;
    for (Vertex<Node> *v : network->getVertexSet())
    {
        if (v->getInfo().getType() == 0)
        {
            v->setUsedDelivery(v->getInfo().getMaxDelivery());
            Graph<Node> *subgraph = findConnectedComponent(network, v->getInfo().getCode());
            currentDeficit = multiWaterNeeds(subgraph, false);

            cout << "Impact of removing the reservoir " << v->getInfo().getCode() << ':' << endl
                 << setw(10) << left << "Cities" << setw(15) << left << "| Impacted?" << setw(15) << left << "| Impact" << endl
                 << "-------------------------------------------------------------------------------" << endl;
            for (pair<string, double> &p : previousDeficit)
            {
                string currCity = p.first;
                vector<pair<string, double>>::iterator it = find_if(currentDeficit.begin(), currentDeficit.end(),
                                                                    [currCity](const pair<string, double> &pairInCurrentDeficit)
                                                                    { return pairInCurrentDeficit.first == currCity; });

                if (it == currentDeficit.end() || p.second - it->second == 0)
                {
                    cout << setw(15) << currCity << setw(15) << "no" << setw(15) << "--" << '\n';
                }
                else
                {
                    cout << setw(15) << currCity << setw(15) << "yes" << setw(15) << p.second - it->second << '\n';
                }
            }

            cout << endl;
            v->setUsedDelivery(0);
        }
    }
}

void WaterNetwork::evaluateAllPumpingStationImpact() const
{
    vector<pair<string, double>> previousDeficit;
    if (filesystem::exists("../out/WaterNeeds.csv"))
    {
        ifstream waterneeds_file("../out/WaterNeeds.csv");
        if (!waterneeds_file.is_open())
            throw runtime_error("WaterNeeds.csv file could not be opened!");

        string line;
        waterneeds_file >> line;
        while (waterneeds_file >> line)
        {
            istringstream iss(line);
            string cityCode, waterDeficit_str;
            getline(getline(iss, cityCode, ','), waterDeficit_str, '\r');
            double waterDeficit = stod(waterDeficit_str);

            previousDeficit.push_back(make_pair(cityCode, waterDeficit));
        }
        waterneeds_file.close();
    }
    else
        previousDeficit = multiWaterNeeds(network, true);

    vector<pair<string, double>> currentDeficit;
    for (Vertex<Node> *v : network->getVertexSet())
    {
        if (v->getInfo().getType() == 1)
        {
            v->setUsedDelivery(v->getInfo().getMaxDelivery());
            Graph<Node> *subgraph = findConnectedComponent(network, v->getInfo().getCode());
            /*for (Vertex<Node> *v : subgraph->getVertexSet())
            {
                cout << v->getInfo().getCode() << endl;
            }*/
            currentDeficit = multiWaterNeeds(subgraph, false);
            /*for (const pair<string, double> &p : currentDeficit)
            {
                cout << setw(18) << left << p.first;
                cout << p.second << endl;
            }*/

            cout << "Impact of removing the pumping station " << v->getInfo().getCode() << ':' << endl
                 << setw(10) << left << "Cities" << setw(15) << left << "| Impacted?" << setw(15) << left << "| Impact" << endl
                 << "-------------------------------------------------------------------------------" << endl;
            for (pair<string, double> &p : previousDeficit)
            {
                string currCity = p.first;
                vector<pair<string, double>>::iterator it = find_if(currentDeficit.begin(), currentDeficit.end(),
                                                                    [currCity](const pair<string, double> &pairInCurrentDeficit)
                                                                    { return pairInCurrentDeficit.first == currCity; });

                if (it == currentDeficit.end() || p.second - it->second == 0)
                {
                    cout << setw(15) << currCity << setw(15) << "no" << setw(15) << "--" << '\n';
                }
                else
                {
                    cout << setw(15) << currCity << setw(15) << "yes" << setw(15) << p.second - it->second << '\n';
                }
            }

            cout << endl;
            v->setVisited(false);
        }
    }
}

void WaterNetwork::evaluatePipelineImpact(const std::string &city_code) const
{
    double previousCityDeficit;
    vector<pair<string, double>> previousDeficit;
    Node city(city_code);
    Vertex<Node>* city_vertex = network->findVertex(city);

    if (filesystem::exists("../out/WaterNeeds.csv"))
    {
        ifstream waterneeds_file("../out/WaterNeeds.csv");
        if (!waterneeds_file.is_open())
            throw runtime_error("WaterNeeds.csv file could not be opened!");

        string line;
        waterneeds_file >> line;
        while (waterneeds_file >> line)
        {
            istringstream iss(line);
            string cityCode, waterDeficit_str;
            getline(getline(iss, cityCode, ','), waterDeficit_str, '\r');
            double waterDeficit = stod(waterDeficit_str);
            if(cityCode == city_code) previousCityDeficit = waterDeficit;
            previousDeficit.push_back(make_pair(cityCode, waterDeficit));
        }
        waterneeds_file.close();
    }
    else
        previousDeficit = multiWaterNeeds(network, true);
    

    vector<pair<string, double>> currentDeficit;
    cout << "Pipelines that, if removed, will impact the city " << city_code << ':' << endl
                 << setw(10) << left << "Pipes" << setw(15) << left << "| Impact" << endl
                 << "-------------------------------------------------------------------------------" << endl;

    for (Vertex<Node> *v : network->getVertexSet())
    {
        for(Edge<Node> *pipe : v->getAdj()){

            double originalPipe = pipe->getWeight();
            pipe->setWeight(0);

            Graph<Node> *subgraph = findConnectedComponent(network, pipe->getOrig()->getInfo().getCode());
            /*for (Vertex<Node> *v : subgraph->getVertexSet())
            {
                cout << v->getInfo().getCode() << endl;
            }*/
            currentDeficit = multiWaterNeeds(subgraph, false);
            /*for (const pair<string, double> &p : currentDeficit)
            {
                cout << setw(18) << left << p.first;
                cout << p.second << endl;
            }*/

            for (pair<string, double> &p : previousDeficit)
            {
                string currCity = p.first;
                if(currCity != city_code) continue;
                vector<pair<string, double>>::iterator it = find_if(currentDeficit.begin(), currentDeficit.end(),
                                                                    [currCity](const pair<string, double> &pairInCurrentDeficit)
                                                                    { return pairInCurrentDeficit.first == currCity; });

                if (it == currentDeficit.end() || p.second - it->second == 0)
                {
                    continue;
                    cout << setw(15) << currCity << setw(15) << "no" << setw(15) << "--" << '\n';
                }
                else
                {
                    cout << pipe->getOrig()->getInfo().getCode() << " -> "  << setw(15) << pipe->getDest()->getInfo().getCode() << setw(15) << "yes" << setw(15) << p.second - it->second << '\n';
                }
            }
            v->setVisited(false);
        }
    }
}
