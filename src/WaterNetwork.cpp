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

Node createSuperSource(Graph<Node> *g)
{
    Node superNode(0, "superSource", "superSource", "superSource", numeric_limits<int>::max());
    g->addVertex(superNode);
    Vertex<Node> *superSource = g->findVertex(superNode);

    for (Vertex<Node> *v : g->getVertexSet())
        if (v->getInfo().getType() == 0 && v->getInfo().getCode() != "superSource")
            superSource->addEdge(v, INF);

    return superNode;
}

double WaterNetwork::singleSinkMaxFlow(const string &city_code) const
{
    Node s = createSuperSource(network);
    Node t(city_code);
    edmondsKarp(network, s, t);

    double flow = 0;
    Vertex<Node> *sink_vertex = network->findVertex(t);
    for (Edge<Node> *e : sink_vertex->getIncoming())
        flow += e->getFlow();

    for (Vertex<Node> *v : network->getVertexSet())
    {
        v->setVisited(false);
        v->setPath(nullptr);

        for (Edge<Node> *e : v->getAdj())
            e->setFlow(0);
        for (Edge<Node> *e : v->getIncoming())
            e->setFlow(0);
    }
    network->removeVertex(s);

    return flow;
}

Node createSuperSink(Graph<Node> *g)
{
    Node superNode(2, "superSink", "superSink", numeric_limits<int>::max());
    g->addVertex(superNode);
    Vertex<Node> *superSink = g->findVertex(superNode);

    for (Vertex<Node> *v : g->getVertexSet())
        if (v->getInfo().getType() == 2 && v->getInfo().getCode() != "superSink")
            v->addEdge(superSink, INF);

    return superNode;
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
        flow = 0.0;
        if (v->getInfo().getType() == 2 && v->getInfo().getCode() != "superSink")
        {
            for (Edge<Node> *e : v->getIncoming())
                flow += e->getFlow();
            res.push_back(make_pair(v->getInfo().getCode(), flow));
            out << v->getInfo().getCode() << ',' << flow << '\r';
        }
    }

    for (Vertex<Node> *v : network->getVertexSet())
    {
        v->setVisited(false);
        v->setPath(nullptr);

        for (Edge<Node> *e : v->getAdj())
            e->setFlow(0);
        for (Edge<Node> *e : v->getIncoming())
            e->setFlow(0);
    }
    network->removeVertex(s);
    network->removeVertex(t);

    out.close();
    return res;
}
