#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <utility>
#include <random>
#include <Eigen/Dense>
#include <cmath>
#include <iostream>
#include <chrono>

class Graph {
public:
    std::unordered_map<int, std::vector<int>> adjList;
    void addEdge(int u, int v) {
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }
    void removeEdge(int node1, int node2) {
    // Remove node2 from node1's adjacency list
        if (adjList.find(node1) != adjList.end()) {
            auto& neighbors = adjList[node1];
            neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), node2), neighbors.end());
        }

        // Remove node1 from node2's adjacency list
        if (adjList.find(node2) != adjList.end()) {
            auto& neighbors = adjList[node2];
            neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), node1), neighbors.end());
        }
    }

    // Function to remove a node from the graph
    void removeNode(int node) {
        // First, remove the node from all its neighbors
        if (adjList.find(node) != adjList.end()) {
            for (int neighbor : adjList[node]) {
                // Remove the node from each neighbor's vector of neighbors
                auto& neighborList = adjList[neighbor];
                neighborList.erase(std::remove(neighborList.begin(), neighborList.end(), node), neighborList.end());
            }
            // Now remove the node itself from the adjacency list
            adjList.erase(node);
        }
    }

    // Get number of edges in the graph
    int getNumEdges() const {
        int edgeCount = 0;
        for (const auto& pair : adjList) {
            edgeCount += pair.second.size();
        }
        // Since the graph is undirected, each edge is counted twice, so divide by 2
        return edgeCount / 2;
    }

    // Get number of nodes
    int getNumNodes() const {
        return adjList.size();
    }

    // Get neighbors of a node
    std::unordered_set<int> getNeighbors(int node) const {
    auto it = adjList.find(node);
    if (it != adjList.end()) {
        return std::unordered_set<int>(it->second.begin(), it->second.end());  // Convert vector to set
        }
        return {};  // Empty set if node has no neighbors
    }

    // Perform DFS to check connectivity
    bool dfs(int node, std::unordered_set<int>& visited) const {
        visited.insert(node);
        for (int neighbor : adjList.at(node)) {
            if (visited.find(neighbor) == visited.end()) {
                dfs(neighbor, visited);
            }
        }
        return true;
    }

    // Check if the graph is connected
    bool isConnected() const {
        if (adjList.empty()) return true; // An empty graph is trivially "connected"

        std::unordered_set<int> visited;
        // Start DFS from the first node (arbitrary choice)
        dfs(adjList.begin()->first, visited);

        // If visited nodes count equals the number of nodes in the graph, it's connected
        return visited.size() == adjList.size();
    }

    // Function to read a graph from a file
    void readFromFile(const std::string& filename) {
        std::ifstream infile(filename);
        if (!infile) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        std::string line;
        int node1, node2;

        while (std::getline(infile, line)) {
            std::stringstream ss(line);
            ss >> node1 >> node2;
            if (ss) {  // Ensure the input was valid
                addEdge(node1, node2);
            }
        }
    }

    std::vector<std::pair<int, int>> getPathEdges(const std::vector<int>& path) {
        std::vector<std::pair<int, int>> edges;
        if (path.size() < 2) {
            return edges; // A single node or empty path has no edges
        }

        for (size_t i = 0; i < path.size() - 1; ++i) {
            edges.emplace_back(path[i], path[i + 1]);
        }

        return edges;
    }

    std::pair<std::vector<std::pair<int, int>>, int> randomEdgeWalk(int startNode, int targetNode, int maxLength) {
        std::vector<std::pair<int, int>> pathEdges;  // To store edges of the path
        std::vector<int> path;
        std::unordered_map<int, bool> visited;  // To track visited nodes
        int walkLength = 0;
        visited[startNode] = true;  // Mark start node as visited
        // Use a random engine for shuffling the neighbors
        std::srand(std::time(nullptr));  
        std::default_random_engine rng(std::rand()); 
        int currentNode = startNode;
        for (int step = 0; step < maxLength; ++step) {
            if (currentNode == targetNode) {
                break;
            }
            if (adjList[currentNode].empty()) {
                break;
            }

            std::uniform_int_distribution<int> dist(0, adjList[currentNode].size() - 1);
            currentNode = adjList[currentNode][dist(rng)];
            path.push_back(currentNode);
            walkLength++;
        }

        pathEdges = getPathEdges(path);
        return {pathEdges, walkLength};  // Return the path (edges) and total length of the walk
    }

    std::pair<std::vector<std::pair<int, int>>, std::vector<int>> collectAllPaths(int targetNode, int maxLength, int epsilon) {
        std::vector<std::pair<int, int>> pathEdgesAll; // To store all paths' edges
        std::vector<int> pathLengthAll; // To store the total path length
        for (int i=1; i<epsilon; ++i){
            for (const auto& pair : adjList) {
            int node1 = pair.first;
            for (int node2 : pair.second) {
                auto path1 = randomEdgeWalk(node1, targetNode, maxLength);
                auto path2 = randomEdgeWalk(node2, targetNode, maxLength);
                if (!path1.first.empty()) {
                    pathEdgesAll.insert(pathEdgesAll.end(), path1.first.begin(), path1.first.end());
                    pathLengthAll.push_back(path1.second);
                }
                if (!path2.first.empty()) {
                    pathEdgesAll.insert(pathEdgesAll.end(), path2.first.begin(), path2.first.end());
                    pathLengthAll.push_back(path2.second);
                }
            }
        }
    }
        return {pathEdgesAll, pathLengthAll};
    }

    // Update the function signature to avoid passing the graph object twice.
    std::pair<std::vector<std::pair<int, int>>, std::vector<double>> removeEdgeResistanceSum(Graph& g, int targetNode, const std::vector<std::pair<int, int>>& pathEdgesAll, const std::vector<int>& pathLengthAll) {
        std::vector<std::pair<int, int>> removeEdge;
        std::vector<double> removeEdgeResistance;
        // Store edges to remove separately before processing
        std::vector<std::pair<int, int>> edgesToRemove;
        for (const auto& pair : g.adjList) {
            int node1 = pair.first;
            for (int node2 : pair.second) {
                edgesToRemove.push_back({node1, node2});
            }
        }
        // Process edges separately after iterating over the adjacency list
        for (const auto& edge : edgesToRemove) {
            int node1 = edge.first;
            int node2 = edge.second;
            double weights = 0;
            // Remove the edge temporarily
            g.removeEdge(node1, node2);
            // Ensure the graph is still connected (excluding paths involving the target node)
            if (g.isConnected() && (node1 != targetNode && node2 != targetNode)) {
                // Calculate the resistance sum for the path lengths
                for (size_t i = 0; i < pathLengthAll.size(); ++i) {
                    if (i >= pathEdgesAll.size()) {
                        break;  // Prevent out-of-bounds access
                    }

                    int pathLength = pathLengthAll[i];
                    for (int j = 0; j < pathLength; ++j) {
                        if (j >= pathEdgesAll.size()) {
                            break;  // Prevent out-of-bounds access
                        }
                        const auto& edge = pathEdgesAll[j];
                        // Calculate weights here
                        if (edge.first == node1 || edge.second == node2) {
                            // std::cout << "Processing edge (" << node1 << ", " << node2 << ") with path length " << pathLength << std::endl;
                            weights += 1.0 / (pathLength);  // Adjust this formula as needed
                            weights += 1.0 / (pathLength);  // Adjust this formula as needed
                        }
                    }
                }

                // Store the edge and its resistance if we found valid weights
                if (weights > 0) {
                    removeEdge.emplace_back(node1, node2);
                    removeEdgeResistance.push_back(weights);
                }
            }

            // Re-add the edge to restore the graph to its original state
            g.addEdge(node1, node2);
        }
        return {removeEdge, removeEdgeResistance};
    }
};

int max_random_walk_length(Graph& g, int target, double gamma) {
    // Get the number of nodes and edges in the graph
    int n = g.getNumNodes();
    int m = g.getNumEdges();

    // Get the degree of each node, excluding the target node
    std::unordered_map<int, int> degrees;
    for (const auto& node : g.adjList) {
        degrees[node.first] = node.second.size();
    }

    // Remove the target node's degree
    degrees.erase(target);

    // Calculate the norm of the degree vector
    double d_norm = 0.0;
    for (const auto& entry : degrees) {
        d_norm += std::pow(entry.second, 2);
    }
    d_norm = std::sqrt(d_norm);
    // Create the adjacency matrix A
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(n, n);
    for (const auto& node : g.adjList) {
        int u = node.first;
        for (int v : node.second) {
            A(u - 1, v - 1) = 1.0;
            A(v - 1, u - 1) = 1.0;
        }
    }

    // Create a reduced adjacency matrix A_reduced (without the target node)
    Eigen::MatrixXd A_reduced = Eigen::MatrixXd::Zero(n - 1, n - 1);
    // Map old indices to new indices after removing the target node
    std::unordered_map<int, int> nodeMapping;
    int newIndex = 0;
    // Build the reduced adjacency matrix and create the node mapping
    for (int u = 0; u < n; ++u) {
        if (u == target) continue; // Skip the target node
        nodeMapping[u] = newIndex;
        newIndex++;
        for (int v : g.getNeighbors(u)) {
            if (v == target) continue; // Skip the target node
            A_reduced(nodeMapping[u], nodeMapping[v]) = 1.0;
            A_reduced(nodeMapping[v], nodeMapping[u]) = 1.0; // Since the graph is undirected
        }
    }

    // Compute the spectral radius of the reduced adjacency matrix
    Eigen::VectorXcd eigenvalues = A_reduced.eigenvalues();
    double spectral_radius = eigenvalues.cwiseAbs().maxCoeff();

    if (spectral_radius <= 1) {
        std::cerr << "Error: Spectral radius is less than or equal to 1, invalid for max_length calculation!" << std::endl;
        return -1; // or some other invalid value
    }

    // Compute the maximum random walk length
    int max_length = static_cast<int>((std::log(m * gamma / std::sqrt(n - 1)) * d_norm) / std::log(spectral_radius));
    return max_length;
}

void optimizedRandomNodeSelection(int n, int t, int targetNode, std::unordered_set<int>& excludedNodes) {
    // Step 1: Generate all the nodes
    std::vector<int> nodes;
    for (int i = 0; i < n; ++i) {
        if (i != targetNode) {  // Exclude target node
            nodes.push_back(i);
        }
    }

    // Step 2: Shuffle the nodes randomly
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(nodes.begin(), nodes.end(), gen);

    // Step 3: Select the first t nodes from the shuffled list
    excludedNodes.insert(nodes.begin(), nodes.begin() + t);
}

std::vector<std::pair<int, int>> FASTICM(Graph& g, int numberOfEdge, int targetNode, int maxLength, double alpha, int phi) {
    double beta = alpha / 2;
    double epsilon = (alpha / 2)/ phi;
    int n = g.getNumNodes();
    int rho = static_cast<int>(0.01 * log(n) / pow(epsilon, 2));
    int t = static_cast<int>(0.95 * phi * sqrt(n * log(n)) / beta);
    // Consider the number of node we need to delete
    if (t > n) {
        t = 0;
    } else {
        t = n - t;
    }
    
    auto start_read = std::chrono::high_resolution_clock::now();
    // Randomly remove some vertices except for the target node
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, n - 1);
    std::unordered_set<int> selectedNodes = {targetNode};
    // Randomly select t-1 other nodes
    while (selectedNodes.size() < t) {
        selectedNodes.insert(dis(gen));
    }
    // Remove selected nodes and their edges
    for (int node : selectedNodes) {
        if (node == targetNode) continue;
        for (int neighbor : g.getNeighbors(node)) {
            g.removeEdge(node, neighbor); // Remove edges
        }
        g.removeNode(node); // Remove node
    }
    // Initialization Need to place here
    auto allPaths = g.collectAllPaths(targetNode, maxLength, rho);
    const auto& pathEdgesAll = allPaths.first;
    const auto& pathLengthAll = allPaths.second;

    auto stop_read = std::chrono::high_resolution_clock::now();
    auto duration_read = std::chrono::duration_cast<std::chrono::milliseconds>(stop_read - start_read);

    std::cout << "Reading running time: " << duration_read.count() / 1000.0 << " seconds" << std::endl;

    std::vector<std::pair<int, int>> P;
    for (int i = 0; i < numberOfEdge; ++i) {
        // Delete edge
        auto allResistance = g.removeEdgeResistanceSum(g, targetNode, pathEdgesAll, pathLengthAll);
        const auto& pathEdges = allResistance.first;
        const auto& pathEdgesResistance = allResistance.second;
        // Select max resistance
        std::pair<int, int> getMaxEdge;
        double maxResistance = -std::numeric_limits<double>::infinity();
        for (size_t i = 0; i < pathEdges.size(); ++i) {
            const auto& edge = pathEdges[i];  // Get the edge (pair of nodes)
            double resistance = pathEdgesResistance[i];  // Get the corresponding resistance
            // Check if this resistance is greater than the current maximum
            if (resistance > maxResistance) {
                maxResistance = resistance;
                getMaxEdge = edge;  // Update the edge with the maximum resistance
            }
        }
        // Remove the edge with the max resistance
        g.removeEdge(getMaxEdge.first, getMaxEdge.second);
        P.push_back(getMaxEdge);
    }
    return P; // Return the list of edges with max resistance
}

int main(int argc, char* argv[]) {

    double gamma = 0.95;
    double alpha = 0.005;
    int phi = 1; //dimensions
    int numberOfEdges = 1;

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> <target_node>" << std::endl;
        return 1; // Exit with error code if no filename or target node is provided
    }
    // Get the filename and target node from the command line argument
    std::string filename = argv[1];
    int targetNode = std::stoi(argv[2]);
    // Seed the random number generator
    std::srand(std::time(nullptr));
    // Read the graph from the file
    Graph g;
    g.readFromFile(filename);
    int max_length = max_random_walk_length(g, targetNode, gamma);
    // Output the graph details
    std::vector<std::pair<int, int>> result = FASTICM(g, numberOfEdges, targetNode, max_length, alpha, phi);
    std::cout << "Edges with max resistance:" << std::endl;
    for (const auto& edge : result) {
        std::cout << "(" << edge.first << ", " << edge.second << ")" << std::endl;
    }

    return 0;
}
