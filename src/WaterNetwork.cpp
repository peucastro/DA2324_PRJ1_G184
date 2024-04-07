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

vector<pair<string, double>> WaterNetwork::multiSinkMaxFlow(Graph<Node> *g, const bool &flag) const
{
    vector<pair<string, double>> res;
    double flow;

    Node s = createSuperSource(g);
    Node t = createSuperSink(g);
    edmondsKarp(g, s, t);

    ofstream out;
    if (flag)
    {
        filesystem::path outputPath("../out");
        if (!filesystem::exists(outputPath))
            filesystem::create_directories(outputPath);
        out.open("../out/MaxFlow.csv");
        out << "CityCode,Flow\r";
    }

    for (Vertex<Node> *v : g->getVertexSet())
    {
        if (v->getInfo().getType() == 2 && v->getInfo().getCode() != "superSink")
        {
            res.push_back(make_pair(v->getInfo().getCode(), v->getCurrentFlow()));
            out << v->getInfo().getCode() << ',' << v->getCurrentFlow() << '\r';
        }
    }

    resetGraph(g, s, t);
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

bool comparePipes(const pair<Edge<Node> *, double> &a, const pair<Edge<Node> *, double> &b)
{
    return a.second > b.second;
}

vector<pair<string, double>> WaterNetwork::calculateMetrics(Graph<Node> *g) const
{
    vector<pair<string, double>> res;
    vector<pair<Edge<Node> *, double>> pipes;

    Node s = createSuperSource(g);
    Node t = createSuperSink(g);
    edmondsKarp(g, s, t);

    double dif;
    double maxDifBef = 0;
    double sumDif = 0;
    double var = 0;
    double count = 0;
    double variancebef = 0;

    for (Vertex<Node> *v : g->getVertexSet())
    {
        for (Edge<Node> *e : v->getAdj())
        {
            dif = e->getWeight() - e->getFlow();
            if (dif > maxDifBef)
                maxDifBef = dif;
            sumDif += dif;
            count++;
            pipes.push_back(make_pair(e, dif));
        }
    }

    double avgbef = sumDif / count;

    for (Vertex<Node> *v : g->getVertexSet())
    {
        for (Edge<Node> *e : v->getAdj())
        {
            dif = e->getWeight() - e->getFlow();
            var += pow(dif - avgbef, 2);
        }
    }
    variancebef = (var / count);

    sort(pipes.begin(), pipes.end(), comparePipes);
    double max = 0;
    double min = pipes.size() - 1;
    Edge<Node> *badPipe = pipes[max].first;
    Edge<Node> *goodPipe = pipes[min].first;

    while ((max != min) && (max < pipes.size()) && (min >= 0))
    {
        Edge<Node> *badPipe = pipes[max].first;
        Edge<Node> *goodPipe = pipes[min].first;
        double badCap = badPipe->getWeight() - pipes[max].second;
        double goodFlow = goodPipe->getWeight();
        while ((pipes[max].second >= 0) && (pipes[max].second <= badPipe->getWeight()) && (pipes[min].second >= 0) && (pipes[min].second <= goodPipe->getWeight()))
        {
            pipes[max].second -= 1;
            pipes[min].second += 1;
        }
        max++;
        min--;
        if (max == min)
            break;
    }

    double maxDifAft = 0;
    double varianceaft = 0;
    var = 0;
    count = 0;
    sumDif = 0;

    for (auto p : pipes)
    {
        Edge<Node> *e = p.first;
        dif = e->getWeight() - p.second;
        if (dif > maxDifAft)
            maxDifAft = dif;
        sumDif += dif;
        count++;
    }
    double avgaft = sumDif / count;
    for (auto p : pipes)
    {
        Edge<Node> *e = p.first;
        dif = e->getWeight() - p.second;
        var += pow(dif - avgbef, 2);
    }
    varianceaft = (var / count);

    res.push_back(make_pair("Maximum difference before balance: ", maxDifBef));
    res.push_back(make_pair("Average difference before balance: ", avgbef));
    res.push_back(make_pair("Variance of difference before balance: ", variancebef));
    res.push_back(make_pair("Maximum difference after balance: ", maxDifAft));
    res.push_back(make_pair("Average difference after balance: ", avgaft));
    res.push_back(make_pair("Variance of difference after balance: ", varianceaft));

    resetGraph(g, s, t);
    return res;
}

void WaterNetwork::evaluateReservoirImpact(const string &reservoir_code) const
{
    Node reservoir(reservoir_code);
    Vertex<Node> *reservoir_vertex = network->findVertex(reservoir);

    if (reservoir_vertex == nullptr || reservoir_vertex->getInfo().getType() != 0)
        throw runtime_error("Please inform a valid reservoir code.");
    reservoir = reservoir_vertex->getInfo();

    vector<pair<string, double>> previousFlow = multiSinkMaxFlow(network, true);

    Graph<Node> *subgraph = findConnectedComponent(network, reservoir_code);
    Vertex<Node> *subgraph_vertex = subgraph->findVertex(reservoir);
    subgraph_vertex->setUsedDelivery(subgraph_vertex->getInfo().getMaxDelivery());
    vector<pair<string, double>> currentFlow = multiSinkMaxFlow(subgraph, false);

    cout << "Impact of removing the reservoir " << reservoir_code << ":\n";
    cout << "-------------------------------------------------------------------------------" << endl;
    cout << setw(10) << left << "Cities" << setw(15) << left << "| Old Flow" << setw(15) << left << "| New Flow" << setw(15) << left << "| Difference "<< endl;
    for (size_t i = 0; i < currentFlow.size(); i++)
    {
        string currCity = currentFlow[i].first;
        vector<pair<string, double>>::iterator it = find_if(previousFlow.begin(), previousFlow.end(),
                                                            [currCity](const pair<string, double> &p)
                                                            { return p.first == currCity; });
        if (it != previousFlow.end())
        {
            double diff = it->second - currentFlow[i].second;
            if (diff != 0)
                cout << std::setw(15) << it->first << setw(15) << it->second << setw(15)  << currentFlow[i].second << setw(15) << diff << endl;
        }
    }

    reservoir_vertex->setUsedDelivery(0);
}

void WaterNetwork::evaluateAllReservoirImpact() const
{
    vector<pair<string, double>> previousFlow = multiWaterNeeds(network, true);

    vector<pair<string, double>> currentFlow;
    for (Vertex<Node> *v : network->getVertexSet())
    {
        if (v->getInfo().getType() == 0)
        {
            Graph<Node> *subgraph = findConnectedComponent(network, v->getInfo().getCode());
            Vertex<Node> *subgraph_vertex = subgraph->findVertex(v->getInfo());
            subgraph_vertex->setUsedDelivery(subgraph_vertex->getInfo().getMaxDelivery());
            currentFlow = multiSinkMaxFlow(subgraph, false);

            cout << "Impact of removing the reservoir " << v->getInfo().getCode() << ":\n";
            cout << "-------------------------------------------------------------------------------" << endl;
            cout << setw(10) << left << "Cities" << setw(15) << left << "| Old Flow" << setw(15) << left << "| New Flow" << setw(15) << left << "| Difference "<< endl;
            for (size_t i = 0; i < currentFlow.size(); i++)
            {
                string currCity = currentFlow[i].first;
                vector<pair<string, double>>::iterator it = find_if(previousFlow.begin(), previousFlow.end(),
                                                                    [currCity](const pair<string, double> &p)
                                                                    { return p.first == currCity; });
                                if (it != previousFlow.end())
                {
                    double diff = it->second - currentFlow[i].second;
                    if (diff != 0)
                        cout << std::setw(15) << it->first <<  setw(15) << it->second << setw(15) << currentFlow[i].second << setw(15) << diff << endl;
                }
            }

            cout << endl;
            subgraph_vertex->setUsedDelivery(0);
        }
    }
}

void WaterNetwork::evaluateAllPumpingStationImpact() const
{
    int CityDeficit;
    vector<pair<string, double>> previousFlow = multiSinkMaxFlow(network, true);
    vector<string> removable;
    vector<pair<string, double>> currentFlow;
    for (Vertex<Node> *v : network->getVertexSet())
    {
        if (v->getInfo().getType() == 1)
        {
            Graph<Node> *subgraph = findConnectedComponent(network, v->getInfo().getCode());
            Node ps(v->getInfo().getCode());
            subgraph->removeVertex(ps);
            currentFlow = multiSinkMaxFlow(subgraph, false);

            cout << "Impact of removing the pumping station " << v->getInfo().getCode() << ":\n";
            cout << "-------------------------------------------------------------------------------" << endl;
            cout << setw(10) << left << "Cities" << setw(15) << left << "| Old Flow" << setw(15) << left << "| New Flow" << setw(15) << left << "| Difference "<< endl;
            for (size_t i = 0; i < currentFlow.size(); i++)
            {
                string currCity = currentFlow[i].first;
                vector<pair<string, double>>::iterator it = find_if(previousFlow.begin(), previousFlow.end(),
                                                                    [currCity](const pair<string, double> &p)
                                                                    { return p.first == currCity; });
                if (it != previousFlow.end())
                {
                    double diff = it->second - currentFlow[i].second;
                    if (diff != 0) {
                        cout << std::setw(15) << it->first << setw(15) << it->second << setw(15)
                             << currentFlow[i].second << setw(15) << diff << endl;
                        CityDeficit++;
                    }
                }
            }
            if (!CityDeficit) removable.push_back(v->getInfo().getCode());
            CityDeficit = 0;
            cout << endl;
        }
    }

    if (!removable.empty()){
        cout << "So, it is possible to temporarily remove a pumping station!" << endl;
        cout << "Pumping Stations that could be removed:\n";
        for (auto element : removable){
            cout << element << endl;
        }
    }   else cout << "Unfortunately, it is not possible temporarily remove a pumping station without affecting the delivery capacity of the system.\n";
}

void WaterNetwork::evaluatePipelineImpact(const std::string &city_code) const
{
    double previousCityDeficit;
    vector<pair<string, double>> previousDeficit = multiWaterNeeds(network, true);
    Node city(city_code);
    Vertex<Node> *city_vertex = network->findVertex(city);

    vector<pair<string, double>> currentDeficit;
    cout << "Pipelines that, if removed, will impact the city " << city_code << ':' << endl
         << setw(10) << left << "Pipes" << setw(15) << left << "| Impact" << endl
         << "-------------------------------------------------------------------------------" << endl;

    for (Vertex<Node> *v : network->getVertexSet())
    {
        for (Edge<Node> *pipe : v->getAdj())
        {

            double originalPipe = pipe->getWeight();
            pipe->setWeight(0);

            Graph<Node> *subgraph = findConnectedComponent(network, pipe->getOrig()->getInfo().getCode());
            currentDeficit = multiWaterNeeds(subgraph, false);

            for (pair<string, double> &p : previousDeficit)
            {
                string currCity = p.first;
                if (currCity != city_code)
                    continue;
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
                    cout << pipe->getOrig()->getInfo().getCode() << " -> " << setw(15) << pipe->getDest()->getInfo().getCode() << setw(15) << "yes" << setw(15) << p.second - it->second << '\n';
                }
            }
            v->setVisited(false);
            pipe->setWeight(originalPipe);
        }
    }
}
