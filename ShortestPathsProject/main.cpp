#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <string>
#include <chrono>
#include <iomanip>
#include <random>
using namespace std;

struct Edge {
    int from;
    int to;
    long long weight;
};
struct Neighbor {
    int vertex;
    long long weight;
};

void printPath(const vector<int>& parent, int source, int destination) {
    vector<int> path;
    int current = destination;

    for (size_t step = 0; step < parent.size(); step++) {
        if (current < 0 ||
            current >= static_cast<int>(parent.size())) {
            break;
        }

        path.push_back(current); // the first element gets added, our destination, remember we're workink backwards, we keep getting more and more elements until were done.
        
        if (current == source) { // ie, if we're done with the pathing, like we reached the begining.
            // print backward through the vector to show the path from source to destination.

            for (size_t i = path.size(); i > 0; i--) {
                cout << path[i - 1];

                if (i > 1) {
                    cout << " -> ";
                }
            }
            return;
        }
// if we arent at the end, current gets replaced by the parent of the current we were just at
        current = parent[current];
    }

    cout << "Invalid parent chain";
}

void bellmanFord(int vertices, const vector<Edge>& edgeList, int source, bool                   showResults = true, double* measuredTime = nullptr) {
    if (source < 0 || source >= vertices) {
        cerr << "Invalid source vertex." << endl;
        return; // lets make sure youre even valid
    }

    const long long INF = numeric_limits<long long>::max(); // infinity

    // INF means we have not found a path to this vertex.
    auto startTime = chrono::steady_clock::now(); // the clock starts.
    vector<long long> distance(vertices, INF);
    vector<int> parent(vertices, -1); //

    distance[source] = 0;

    // calculate a path's distance
    auto calculateDistance = [&](long long current,
                                 long long weight,
                                 long long& result) -> bool {
        if ((weight > 0 && current >= INF - weight) ||
            (weight < 0 && current < numeric_limits<long long>::min() - weight)) {
            cerr << "Path distance exceeds the supported range." << endl;
            return false;
        }

        result = current + weight;
        return true;
    };

    // shortest path without cycles at most V - 1 edges
    for (int pass = 0; pass < vertices - 1; pass++) {
        bool changed = false; // at the begining of each pass, we assume no distances will improve. if we find a shorter path durring this pass we will make it true

        for (const Edge& edge : edgeList) {
            // skip edges whose starting vertex is infinity
            if (distance[edge.from] == INF) {
                continue; // skip the rest of the loop but start over
                //suppose we have a route 2 -> 3 weight 4, but we have not discovered a route to 2 yet. so 2 is infinity, if the distance of 2, edge.from, is infinity, go to the next edge
            }
            
            long long candidate;
            if (!calculateDistance(distance[edge.from],edge.weight, candidate)) {
                return;
                // if calculating this distance is unsafe, exit bellmanford
                
            }
            // relax the edge. keep the shorter path.
            if (candidate < distance[edge.to]) {
                distance[edge.to] = candidate;
                parent[edge.to] = edge.from;
                changed = true;
            }
        }

        // no improvements means the distances are finished.
        if (!changed) {
            break;
        }
    }

    // one additional pass checks for a reachable negative cycle.
    for (const Edge& edge : edgeList) {
        if (distance[edge.from] == INF) {
            continue;
        }

        long long candidate;
        if (!calculateDistance(distance[edge.from], edge.weight, candidate)) {
            return;
        }

        if (candidate < distance[edge.to]) {
            cout << "\nA negative cycle is reachable from vertex " << source << "." << endl;
            cout << "Shortest distances are not defined for vertices "
                 << "affected by that cycle." << endl;
            return;
        }
    }
    auto endTime = chrono::steady_clock::now();
    double runtimeMs = chrono::duration<double, milli>(endTime - startTime).count();
    
    if (measuredTime != nullptr){
        *measuredTime = runtimeMs;
    }
    if (!showResults){
        return;
    }
    
    cout << "\nBellman-Ford starting from vertex " << source << endl;
    cout << fixed << setprecision(6) << "Runtime: " << runtimeMs << " ms" << endl;
    cout << "Vertex\tDistance\tPath" << endl;

    for (int vertex = 0; vertex < vertices; vertex++) {
        cout << vertex << "\t";

        if (distance[vertex] == INF) {
            cout << "Unreachable\tUnreachable";
        } else {
            cout << distance[vertex] << "\t\t";
            printPath(parent, source, vertex);
        }

        cout << endl;
    }
}

void dijkstra(int vertices, const vector<vector<Neighbor>>& adjacency, int source,
              bool showResults = true, double* measuredTime = nullptr) {

    if (source < 0 || source >= vertices) {
        cerr << "Invalid source vertex." << endl;
        return;
    }

    // Dijkstra cannot handle negative edge weights.
    for (const auto& neighbors : adjacency) {
        for (const Neighbor& neighbor : neighbors) {
            if (neighbor.weight < 0) {
                cerr << "Dijkstra requires nonnegative weights." << endl;
                return;
            }
        }
    }

    const long long INF = numeric_limits<long long>::max();

    // initialize distances, parents, and visited status.
    auto startTime = chrono::steady_clock::now();
    vector<long long> distance(vertices, INF);
    vector<int> parent(vertices, -1);
    vector<bool> visited(vertices, false);

    // the source has a distance of 0 from itself.
    distance[source] = 0;

    for (int step = 0; step < vertices; step++) {

        // find the closest reachable, unvisited vertex.
        int current = -1;
        long long smallestDistance = INF;

        for (int vertex = 0; vertex < vertices; vertex++) {
            
            // select a vertex only if it has not been visited and its distance is smaller than our best candidate.
            
            if (!visited[vertex] &&
                distance[vertex] < smallestDistance) {

                smallestDistance = distance[vertex];
                current = vertex;
            }
        }

        // No reachable, unvisited vertices remain.
        if (current == -1) {
            break;
        }

        // this vertex's shortest distance is finalized.
        visited[current] = true;

        // try to improve distances to its neighbors.
        for (const Neighbor& neighbor : adjacency[current]) {
            int next = neighbor.vertex;

            if (visited[next]) {
                continue;
            }
            //skips distances that are already finalized

            // prevent overflow. INF is reserved for unreachable.
            if (neighbor.weight >= INF - distance[current]) {
                cerr << "Path distance exceeds the supported range."
                     << endl;
                return;
            }
            //calculate a possible shorter path
            long long candidate =
                distance[current] + neighbor.weight;
            
            // Update the shortest distance and parent if the new route is better than the existing one.
            if (candidate < distance[next]) {
                distance[next] = candidate;
                parent[next] = current;
            }
        }

        // the loop repeats with the next closest vertex.
    }
    //stop the timer and calculate runtime.
    auto endTime = chrono::steady_clock::now(); //
    double runtimeMs = chrono::duration<double, milli>(endTime - startTime).count();
    //store the runtime if the caller requested it
    if (measuredTime != nullptr){
        *measuredTime = runtimeMs;
    }
    if (!showResults){
        return;
    }
    
    cout << "\nDijkstra starting from vertex " << source << endl;
    cout << fixed << setprecision(6) << "Runtime: " << runtimeMs << " ms" << endl;
    cout << "Vertex\tDistance\tPath" << endl;

    for (int vertex = 0; vertex < vertices; vertex++) {
        cout << vertex << "\t";

        if (distance[vertex] == INF) {
            cout << "Unreachable\tUnreachable";
        } else {
            cout << distance[vertex] << "\t\t";
            printPath(parent, source, vertex);
        }

        cout << endl;
    }
}

void bfs(int vertices,
         const vector<vector<Neighbor>>& adjacency,
         int source, bool showResults = true, double* measuredTime = nullptr) {

    if (source < 0 || source >= vertices) {
        cerr << "Invalid source vertex." << endl;
        return;
    }

    for (const auto& neighbors : adjacency) {
        for (const Neighbor& neighbor : neighbors) {
            if (neighbor.weight != 1) {
                cout << "\nBFS skipped: all weights must be 1."
                     << endl;
                return;
            }
        }
    }
    auto startTime = chrono::steady_clock::now();
    
    vector<int> distance(vertices, -1);
    vector<int> parent(vertices, -1);

    vector<int> pending;
    size_t front = 0;

    distance[source] = 0;
    pending.push_back(source);

    while (front < pending.size()) {
        int current = pending[front];
        front++;

        for (const Neighbor& neighbor : adjacency[current]) {
            int next = neighbor.vertex;

            if (distance[next] == -1) {
                distance[next] = distance[current] + 1;
                parent[next] = current;
                pending.push_back(next);
            }
        }
    }
    auto endTime = chrono::steady_clock::now();
    double runtimeMs = chrono::duration<double, milli>(endTime - startTime).count();
    
    if (measuredTime != nullptr){
        *measuredTime = runtimeMs;
    }
    if (!showResults){
        return;
    }
    
    cout << "\nBFS starting from vertex " << source << endl;
    cout << fixed << setprecision(6) << "Runtime: " << runtimeMs << " ms" << endl;
    cout << "Vertex\tDistance\tPath" << endl;
    

    for (int vertex = 0; vertex < vertices; vertex++) {
        cout << vertex << "\t";

        if (distance[vertex] == -1) {
            cout << "Unreachable\tUnreachable";
        } else {
            cout << distance[vertex] << "\t\t";
            printPath(parent, source, vertex);
        }

        cout << endl;
    }
}
void generateUnweightedGraph(int vertices) {
    if (vertices < 5) {
        cerr << "Use at least 5 vertices." << endl;
        return;
    }

    vector<Edge> generatedEdges;

    // a fixed seed makes the generated input reproducible.
    mt19937 generator(42);
    uniform_int_distribution<int> pickVertex(0, vertices - 1);

    for (int from = 0; from < vertices; from++) {
        vector<int> targets;

        // connect vertices in a cycle so all are reachable.
        targets.push_back((from + 1) % vertices);

        // give each vertex four distinct outgoing edges.
        while (targets.size() < 4) {
            int to = pickVertex(generator);
            bool duplicate = false;

            for (int existing : targets) {
                if (existing == to) {
                    duplicate = true;
                    break;
                }
            }

            if (to != from && !duplicate) {
                targets.push_back(to);
            }
        }

        for (int to : targets) {
            generatedEdges.push_back({from, to, 1});
        }
    }

    // shuffle edge order so edges are not grouped by vertex.
    for (size_t i = generatedEdges.size(); i > 1; i--) {
        uniform_int_distribution<size_t> pickIndex(0, i - 1);
        size_t j = pickIndex(generator);

        Edge temporary = generatedEdges[i - 1];
        generatedEdges[i - 1] = generatedEdges[j];
        generatedEdges[j] = temporary;
    }

    string filename =
        "inputs/unweighted_" + to_string(vertices) + ".txt";

    ofstream outputFile(filename);

    if (!outputFile) {
        cerr << "Could not create " << filename << endl;
        return;
    }

    outputFile << vertices << " " << generatedEdges.size() << "\n";

    for (const Edge& edge : generatedEdges) {
        outputFile << edge.from << " "
                   << edge.to << " "
                   << edge.weight << "\n";
    }

    outputFile.close();

    if (!outputFile) {
        cerr << "Error saving " << filename << endl;
        return;
    }

    cout << "Created " << filename << endl;
    
}

double medianTime(vector<double> times){
    if (times.empty()){
        return -1;
    }
    for (size_t i = 1; i < times.size(); i++){
        double current = times[i];
        size_t j = i;
        while (j > 0 && times[j-1] > current){
            times[j] = times[j-1];
            j--;
        }
        times[j]= current;
    }
    size_t middle = times.size()/2;
    if (times.size() % 2 == 0){
        return (times[middle - 1] + times[middle])/2.0;
    }
    return times[middle];
}

void compareRunTimes(int vertices, const vector<Edge>& edgeList,
    const vector<vector<Neighbor>>& adjacency,
                     int source){
    if (source < 0 || source >= vertices){
        cout << "invalid source vertex.\n";
        return;
    }
    string names[3] = {"BFS", "Bellman-Ford", "Dijkstra"};
    string status[3] = {"OK", "OK", "OK"};
    vector<double> times[3];
    
    for (const Edge& edge : edgeList)
    {
        if(edge.weight != 1){
            status[0] = "Skipped: weights must be 1";
        }
        if (edge.weight < 0){
            status[2] = "Skipped: negative weights";
        }
    }
    for (int trial = 0; trial < 7; trial++){
        for (int position = 0; position < 3; position++){
            int algorithm = (position + trial) % 3;
            if (status[algorithm] != "OK"){
                continue;
            }
            double measured = -1;
            if (algorithm == 0){
                bfs(vertices, adjacency, source, false, &measured);
            }
            else if (algorithm == 1){
                bellmanFord(vertices, edgeList, source, false, &measured);
            }
            else{
                dijkstra(vertices, adjacency, source, false, &measured);
            }
            if (measured < 0){
                status[algorithm] = "Failed: see error above";
                times[algorithm].clear();
                continue;
            }
            if (trial > 0){
                times[algorithm].push_back(measured);
            }
        }
    }
    cout << "\nRuntime comparison\n";
    cout << "vertices: " << vertices
         << "| Edges: " << edgeList.size()
         << "| Source: " << source << "\n";
    cout << "median of 7 runs; one warm-up excluded.\n\n";
    cout << left << setw(18) << "Algorithm" << "Median time (ms)\n";
    for (int algorithm = 0; algorithm < 3; algorithm++){
        cout << left << setw(18) << names[algorithm];
        if(status[algorithm] == "OK"){
            cout << fixed << setprecision(6)
            << medianTime(times[algorithm]);
        }
        else{
            cout << status[algorithm];
        }
        cout << "\n";
    }
    
}

void runExperiments(){
    vector<int> sizes = {100, 250, 500, 1000};
    for (int size : sizes) {
        generateUnweightedGraph(size);
        string filename = "inputs/unweighted_" + to_string(size) + ".txt";
        ifstream inputFile(filename);
        if (!inputFile){
            cerr << "Cannot open " << filename << endl;
            return;
        }
        int vertices;
        int edges;
        if(!(inputFile >> vertices >> edges) || vertices != size || edges < 0){
            cerr << "Invalid generated graph header." << endl;
            return;
        }
        vector<Edge> originalEdges;
        for (int i = 0; i < edges; i++){
            Edge edge;
            if(!(inputFile >> edge.from >> edge.to >> edge.weight)){
                cerr << "Invalid generated edge data." << endl;
                return;
            }
            originalEdges.push_back(edge);
        }
        inputFile.close();
        for (int type = 0; type < 2; type++){
            vector<Edge> testEdges = originalEdges;
            if (type == 1) {
                mt19937 weightGenerator(123);
                uniform_int_distribution<int> pickWeight (1, 20);
                
                for(Edge& edge : testEdges){
                    edge.weight = pickWeight(weightGenerator);
                }
                string weightedFilename = "inputs/weighted_" + to_string(size) + ".txt";
                ofstream outputFile(weightedFilename);
                if(!outputFile){
                    cerr << "Cannot create " << weightedFilename << endl;
                    return;
                }
                outputFile << vertices << " " << edges << "\n";
                for (const Edge& edge : testEdges) {
                    outputFile << edge.from << " "<< edge.to<< " "<< edge.weight<< "\n";
                }
                outputFile.close();
                if (!outputFile){
                    cerr << "Error saving weighted graph." << endl;
                    return;
                }
            }
            vector <vector<Neighbor>> adjacency(vertices);
            for(const Edge& edge : testEdges){
                adjacency[edge.from].push_back({
                    edge.to, edge.weight});
            }
            if (type == 0){
                cout << "\n=== UNWEIGHTED GRPAH ===\n";
            }
            else{
                cout << "\n=== WEIGHTED GRAPH ===\n";
            }
            compareRunTimes(vertices, testEdges, adjacency, 0);
        }
    }
    cout << "\nAll performance experiments finished.\n";
}

int main() {
    generateUnweightedGraph(250);
    generateUnweightedGraph(500);
    generateUnweightedGraph(1000);
    string filename;

    cout << "Enter graph filename (example: weighted.txt): ";
    getline(cin, filename);
    if (filename == "experiments"){
        runExperiments();
        return 0;
    }
    ifstream inputFile("inputs/" + filename);

    if (!inputFile) {
        cerr << "Error opening inputs/" << filename << endl;
        return 1;
    }

    int vertices;
    int edges;

    if (!(inputFile >> vertices >> edges) ||
        vertices <= 0 || edges < 0) {
        cerr << "Invalid graph header." << endl;
        return 1;
    }

    vector<Edge> edgeList;

    for (int i = 0; i < edges; i++) {
        Edge edge;

        if (!(inputFile >> edge.from >> edge.to >> edge.weight)) {
            cerr << "Invalid edge data." << endl;
            return 1;
        }

        if (edge.from < 0 || edge.from >= vertices ||
            edge.to < 0 || edge.to >= vertices) {
            cerr << "Invalid vertex number." << endl;
            return 1;
        }

        edgeList.push_back(edge);
    }

    vector<vector<Neighbor>> adjacency(vertices);

    for (const Edge& edge : edgeList) {
        adjacency[edge.from].push_back({edge.to, edge.weight});
    }

    cout << "\nVertices: " << vertices << endl;
    cout << "Edges: " << edges << endl;

    while (true) {
        cout << "\n--- Shortest Paths Menu ---\n";
        cout << "1. BFS (all weights must be 1)\n";
        cout << "2. Bellman-Ford\n";
        cout << "3. Dijkstra (nonnegative weights)\n";
        cout << "4. Run all three\n";
        cout << "5. Compare runtimes automatically\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";

        int choice;

        if (!(cin >> choice)) {
            if (cin.eof()) {
                break;
            }

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a number from 0 to 5.\n";
            continue;
        }

        if (choice == 0) {
            break;
        }

        if (choice < 1 || choice > 5) {
            cout << "Please choose a number from 0 to 5.\n";
            continue;
        }

        cout << "Enter source vertex (0 to "
             << vertices - 1 << "): ";

        int source;

        if (!(cin >> source)) {
            if (cin.eof()) {
                break;
            }

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a valid vertex number.\n";
            continue;
        }

        if (source < 0 || source >= vertices) {
            cout << "Source vertex is outside the graph.\n";
            continue;
        }

        if (choice == 1 || choice == 4) {
            bfs(vertices, adjacency, source);
        }

        if (choice == 2 || choice == 4) {
            bellmanFord(vertices, edgeList, source);
        }

        if (choice == 3 || choice == 4) {
            dijkstra(vertices, adjacency, source);
        }
        if (choice == 5){
            compareRunTimes(vertices, edgeList, adjacency, source);
        }
    }

    cout << "Program finished." << endl;
    return 0;
}
