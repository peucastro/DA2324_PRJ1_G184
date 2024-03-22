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

void testAndVisit(queue<Vertex<Node> *> &q, Edge<Node> *e, Vertex<Node> *w, double residual)
{
    // Check if the vertex 'w' is not visited and there is residual capacity
    if (!w->isVisited() && residual > 0)
    {
        // If 'w' is a reservoir, check if it's already saturated
        if (w->getInfo().getType() == 0 && w->getInfo().getMaxDelivery() == w->getUsedDelivery())
            return;

        // If 'w' is a city, check if the demand is already satisfied
        else if (w->getInfo().getType() == 2 && (w->getInfo().getDemand() == w->getCurrentFlow() || w->getCurrentFlow() > w->getInfo().getDemand()))
            return;

        // Mark 'w' as visited, set the path through which it was reached, and enqueue it
        w->setVisited(true);
        w->setPath(e);
        q.push(w);
    }
}

bool findAugmentingPath(Graph<Node> *g, Vertex<Node> *s, Vertex<Node> *t)
{
    // Mark all vertices as not visited
    for (Vertex<Node> *v : g->getVertexSet())
    {
        // If 'v' is a reservoir, check if it's already saturated
        if (v->getInfo().getType() == 0 && v->getInfo().getMaxDelivery() == v->getUsedDelivery())
            v->setVisited(true);

        // If 'v' is a city, check if the demand is already satisfied
        if (v->getInfo().getType() == 2 && (v->getInfo().getDemand() == v->getCurrentFlow() || v->getCurrentFlow() > v->getInfo().getDemand()))
            v->setVisited(true);

        else
            v->setVisited(false);
    }

    // Mark the source vertex as visited and enqueue it
    s->setVisited(true);
    queue<Vertex<Node> *> q;
    q.push(s);

    // BFS to find an augmenting path
    while (!q.empty() && !t->isVisited())
    {
        Vertex<Node> *v = q.front();
        q.pop();

        // Process outgoing edges
        for (Edge<Node> *e : v->getAdj())
            testAndVisit(q, e, e->getDest(), e->getWeight() - e->getFlow());
        // Process incoming edges
        for (Edge<Node> *e : v->getIncoming())
            testAndVisit(q, e, e->getOrig(), e->getFlow());
    }
    // Return true if a path to the target is found, false otherwise
    return t->isVisited();
}

// Function to find the minimum residual capacity along the augmenting path
double findMinResidualAlongPath(Vertex<Node> *s, Vertex<Node> *t)
{
    double f = INF;
    // Traverse the augmenting path to find the minimum residual capacity
    for (Vertex<Node> *v = t; v != s;)
    {
        Edge<Node> *e = v->getPath();
        if (e->getDest() == v)
        {
            f = min(f, e->getWeight() - e->getFlow());
            // If the destination vertex is a city (and not superSink), consider its demand
            if (e->getDest()->getInfo().getType() == 2 && e->getDest()->getInfo().getCode() != "superSink")
                f = min(f, (double)(e->getDest()->getInfo().getDemand() - e->getDest()->getCurrentFlow()));
            v = e->getOrig();
        }
        else
        {
            f = min(f, e->getFlow());
            // If the origin vertex is a reservoir (and not superSource), consider it's the residual capacity
            if (e->getOrig()->getInfo().getType() == 0 && e->getOrig()->getInfo().getCode() != "superSource")
                f = min(f, (double)(e->getOrig()->getInfo().getMaxDelivery() - e->getOrig()->getUsedDelivery()));
            v = e->getDest();
        }
    }
    // Return the minimum residual capacity
    return f;
}

// Function to augment flow along the augmenting path with the given flow value
void augmentFlowAlongPath(Vertex<Node> *s, Vertex<Node> *t, double f)
{
    // Traverse the augmenting path and update the flow values accordingly
    for (Vertex<Node> *v = t; v != s;)
    {
        Edge<Node> *e = v->getPath();
        double flow = e->getFlow();
        if (e->getDest() == v)
        {
            // Update flow and current flow for destination vertex
            e->setFlow(flow + f);
            if (v->getInfo().getType() == 2) // City
                v->setCurrentFlow(v->getCurrentFlow() + f);
            else if (v->getInfo().getType() == 0) // Reservoir
                v->setUsedDelivery(v->getUsedDelivery() + f);
            v = e->getOrig(); // Move to the origin of the edge
        }
        else
        {
            // Update flow and current flow for origin vertex
            e->setFlow(flow - f);
            if (v->getInfo().getType() == 2) // City
                v->setCurrentFlow(v->getCurrentFlow() - f);
            else if (v->getInfo().getType() == 0) // Reservoir
                v->setUsedDelivery(v->getUsedDelivery() - f);
            v = e->getDest(); // Move to the destination of the edge
        }
    }
}

// Main function implementing the Edmonds-Karp algorithm
void edmondsKarp(Graph<Node> *g, Node source, Node target)
{
    // Find source and target vertices in the graph
    Vertex<Node> *s = g->findVertex(source);
    Vertex<Node> *t = g->findVertex(target);

    // Validate source and target vertices
    if (s == nullptr || t == nullptr || s == t)
        throw logic_error("Invalid source and/or target vertex");

    // Initialize flow on all edges to 0
    for (Vertex<Node> *v : g->getVertexSet())
        for (Edge<Node> *e : v->getAdj())
            e->setFlow(0);

    // While there is an augmenting path, augment the flow along the path
    while (findAugmentingPath(g, s, t))
    {
        double f = findMinResidualAlongPath(s, t);
        augmentFlowAlongPath(s, t, f);
    }
}

Node createSuperSource(Graph<Node> *g)
{
    Node superNode(0, "superSource", "superSource", "superSource", numeric_limits<int>::max());
    g->addVertex(superNode);
    Vertex<Node> *superSource = g->findVertex(superNode);

    for (Vertex<Node> *v : g->getVertexSet())
        if (v->getInfo().getType() == 0 && v->getInfo().getCode() != "superSource")
            superSource->addEdge(v, v->getInfo().getMaxDelivery());

    return superNode;
}

Node createSuperSink(Graph<Node> *g)
{
    Node superNode(2, "superSink", "superSink", numeric_limits<int>::max());
    g->addVertex(superNode);
    Vertex<Node> *superSink = g->findVertex(superNode);

    for (Vertex<Node> *v : g->getVertexSet())
        if (v->getInfo().getType() == 2 && v->getInfo().getCode() != "superSink")
            v->addEdge(superSink, v->getInfo().getDemand());

    return superNode;
}

void resetGraph(Graph<Node> *g, const Node &s, const Node &t)
{
    for (Vertex<Node> *v : g->getVertexSet())
    {
        v->setVisited(false);
        v->setPath(nullptr);
        v->setCurrentFlow(0);
        v->setUsedDelivery(0);

        for (Edge<Node> *e : v->getAdj())
            e->setFlow(0);
        for (Edge<Node> *e : v->getIncoming())
            e->setFlow(0);
    }
    g->removeVertex(s);
    g->removeVertex(t);
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

vector<pair<string, double>> WaterNetwork::multiWaterNeeds() const
{
    vector<pair<string, double>> res;

    Node s = createSuperSource(network);
    Node t = createSuperSink(network);
    edmondsKarp(network, s, t);

    filesystem::path outputPath("../out");
    if (!filesystem::exists(outputPath))
        filesystem::create_directories(outputPath);
    ofstream out("../out/WaterNeeds.csv");
    out.clear();
    out << "CityCode,WaterDeficit\r";

    for (Vertex<Node> *v : network->getVertexSet())
    {
        if (v->getInfo().getType() == 2 && v->getInfo().getCode() != "superSink")
        {
            res.push_back(make_pair(v->getInfo().getCode(), abs(v->getCurrentFlow() - v->getInfo().getDemand())));
            out << v->getInfo().getCode() << ',' << abs(v->getCurrentFlow() - v->getInfo().getDemand()) << '\r';
        }
    }

    resetGraph(network, s, t);
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
        previousDeficit = multiWaterNeeds();

    reservoir_vertex->setUsedDelivery(reservoir_vertex->getInfo().getMaxDelivery());
    vector<pair<string, double>> currentDeficit = multiWaterNeeds();

    for (size_t i = 0; i < previousDeficit.size(); i++)
    {
        double diff = previousDeficit[i].second - currentDeficit[i].second;
        res.push_back(make_pair(previousDeficit[i].first, diff));
        cout << "CITY: " << previousDeficit[i].first << "  ;  PREV DEFICIT: " << previousDeficit[i].second << "  ;  CURR DEFICIT: " << currentDeficit[i].second << '\n';
    }

    reservoir_vertex->setUsedDelivery(0);
    return res;
}
void WaterNetwork::evaluatePipelineImpact(const std::string &city_code) const
{

    Node city(city_code);
    Vertex<Node> *city_vertex = network->findVertex(city);

    if (city_vertex == nullptr || city_vertex->getInfo().getType() != 2)
        throw runtime_error("Please inform a valid city code.");

    vector<pair<string, double>> previousDeficit;
    previousDeficit = multiWaterNeeds();
    double previousCityDeficit;

    for (size_t i = 0; i < previousDeficit.size(); i++)
    {
        if (previousDeficit[i].first == city_code)
        {
            previousCityDeficit = previousDeficit[i].second;
        }
    }
    for (Vertex<Node> *node : network->getVertexSet())
    {
        for (Edge<Node> *pipe : node->getAdj())
        {
            double pipeWeight = pipe->getWeight();
            pipe->setWeight(0);
            vector<pair<string, double>> currentDeficit = multiWaterNeeds();
            double currentCityDeficit;

            for (size_t i = 0; i < currentDeficit.size(); i++)
            {
                if (currentDeficit[i].first == city_code)
                {
                    currentCityDeficit = currentDeficit[i].second;
                    if (currentCityDeficit > previousCityDeficit)
                    {
                        cout << "PIPE: " << pipe->getOrig()->getInfo().getCode() << " -> " << pipe->getDest()->getInfo().getCode() << "  ;  PREV DEFICIT: " << previousCityDeficit << "  ;  CURR DEFICIT: " << currentCityDeficit << '\n';
                    }
                }
            }
            pipe->setWeight(pipeWeight);
        }
    }
}
