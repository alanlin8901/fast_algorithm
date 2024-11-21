#ifndef GRAPH_UTILS_H
#define GRAPH_UTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <set>
#include <algorithm>
#include <random>
#include <map>
#include <iterator>
#include <chrono>
#include <queue>
#include <Eigen/SVD>
#include <cstdio>
#include <memory>

using namespace std;
using namespace Eigen;
using namespace chrono;

typedef pair<int, int> Edge;
typedef vector<Edge> EdgeList;

SparseMatrix<double> createLaplacianMatrix(int num_nodes, const EdgeList& edges);
MatrixXd computePseudoinverse(const SparseMatrix<double>& L);
vector<int> selectRandomNodes(int max_node, int count);
double calculateEffectiveResistanceSum(const MatrixXd& L_dagger, int num_nodes);
vector<Edge> removeSpecificEdges(vector<Edge>& edges, const vector<Edge>& edges_to_remove);
bool isConnected(int n, EdgeList& edges);
void printTargetNodesAndEdges(const std::string& inputFilename, const std::string& algorithmName, const vector<int>& T, const vector<pair<int, vector<Edge>>>& all_P);
EdgeList EXACTSM(string filename, int targetnode, int k);
EdgeList OPTIMUM(string filename, int targetnode, int k);
int max_random_walk_length(string filename, int target, double gamma);
EdgeList APPROXISC(string filename, int k, int targetnode, int maxLength, double epsilon);
EdgeList FASTICM(string filename, int k, int targetnode, int maxLength, double alpha, int phi);
EdgeList processEdgesWithScores(string filename, int k, int target, int maxLength, double epsilon);

#endif // GRAPH_UTILS_H//
