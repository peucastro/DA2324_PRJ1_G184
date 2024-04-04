#include "../inc/Utils.hpp"
using namespace std;

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

void dfsVisitFindCC(Vertex<Node> *v, vector<Vertex<Node> *> &res)
{
    v->setVisited(true);
    res.push_back(v);
    for (Edge<Node> *e : v->getAdj())
    {
        Vertex<Node> *w = e->getDest();
        if (!w->isVisited())
        {
            dfsVisitFindCC(w, res);
        }
    }

    for (Edge<Node> *e : v->getIncoming())
    {
        Vertex<Node> *w = e->getOrig();

        if (!w->isVisited())
        {
            w->setVisited(true);
            if (w->getInfo().getType() == 1 || w->getInfo().getType() == 2)
            {
                dfsVisitFindCC(w, res);
            }
            else
            {
                w->setVisited(true);
                res.push_back(w);
            }
        }
    }
}

Graph<Node> *findConnectedComponent(Graph<Node> *g, const string &node_code)
{

    vector<Vertex<Node> *> res;
    Graph<Node> *subgraph = new Graph<Node>();

    Node node(node_code);
    Vertex<Node> *node_vertex = g->findVertex(node);
    if (node_vertex == nullptr)
        throw runtime_error("Please inform a valid code.");

    for (Vertex<Node> *v : g->getVertexSet())
        v->setVisited(false);

    dfsVisitFindCC(node_vertex, res);
    for (Vertex<Node> *v : res)
    {
        if (v->getInfo().getCode() != node_code)
        {
            subgraph->addVertex(v->getInfo());
            for (Edge<Node> *e : v->getAdj())
            {
                subgraph->addEdge(v->getInfo(), e->getDest()->getInfo(), e->getWeight());
            }
            for (Edge<Node> *e : v->getIncoming())
            {
                subgraph->addEdge(e->getOrig()->getInfo(), v->getInfo(), e->getWeight());
            }
        }
    }

    return subgraph;
}
