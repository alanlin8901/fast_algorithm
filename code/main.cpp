#include "graph_utils.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <string>

using namespace std;
using namespace chrono;

// Helper function to split a string by a delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream tokenStream(str);
    std::string token;
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Helper function to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";  // No non-space characters
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Helper function to print vector elements
void printVector(const std::vector<int>& vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i < vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]";
}

int main(int argc, char* argv[]) {
    
    std::string filename;
    std::vector<int> targetnodes;
    int diameter = 1;
    int w = 20;
    int k = 3; // need to remove edge in each nodes
    //targetnodes = selectRandomNodes(w, 10);  // Select w random target nodes

    // Flags for calculations
    bool runOPTIMUM = true;
    bool runEXACTSM = true;
    bool runAPPROXISC = true;
    bool runFASTICM = true;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-t") == 0 || std::strcmp(argv[i], "--targetnodes") == 0) {
            if (i + 1 < argc) {
                std::string targetnodesStr = argv[i + 1];
                // Split by commas and process each token
                std::vector<std::string> parts = split(targetnodesStr, ',');
                for (const std::string& part : parts) {
                    int value = std::stoi(trim(part));
                    targetnodes.push_back(value);
                }
                i++;
            } else {
                std::cerr << "No values provided for targetnodes.\n";
                return 1;
            }
        } else if (std::strcmp(argv[i], "-d") == 0 || std::strcmp(argv[i], "--diameter") == 0) {
            if (i + 1 < argc) {
                try {
                    diameter = std::stoi(argv[i + 1]);
                    i++; // Skip the next argument
                } catch (const std::invalid_argument&) {
                    std::cerr << "Invalid diameter value: " << argv[i + 1] << "\n";
                    return 1;
                } catch (const std::out_of_range&) {
                    std::cerr << "Diameter value out of range: " << argv[i + 1] << "\n";
                    return 1;
                }
            } else {
                std::cerr << "No value provided for diameter.\n";
                return 1;
            }
        } else if (std::strcmp(argv[i], "-k") == 0) {
            if (i + 1 < argc) {
                try {
                    k = std::stoi(argv[i + 1]);
                    i++; // Skip the next argument
                } catch (const std::invalid_argument&) {
                    std::cerr << "Invalid k: " << argv[i + 1] << "\n";
                    return 1;
                } catch (const std::out_of_range&) {
                    std::cerr << "k is out of range: " << argv[i + 1] << "\n";
                    return 1;
                }
            } else {
                std::cerr << "No value provided for k.\n";
                return 1;
            }
        } else if (std::strcmp(argv[i], "--disable-optimum") == 0) {
            runOPTIMUM = false;
        } else if (std::strcmp(argv[i], "--disable-exactsm") == 0) {
            runEXACTSM = false;
        } else if (std::strcmp(argv[i], "--disable-approxisc") == 0) {
            runAPPROXISC = false;
        } else if (std::strcmp(argv[i], "--disable-fasticm") == 0) {
            runFASTICM = false;
        } else {
            filename = argv[i];
        }
    }
    if (filename.empty()) {
        std::cout << "Please enter the path to the .mtx file: ";
        std::cin >> filename;
    }
    std::cout << "Filename: " << filename << std::endl;
    cout << "Target nodes: [";
    for (size_t i = 0; i < targetnodes.size(); ++i) {
        cout << targetnodes[i];
        if (i < targetnodes.size() - 1) {
            cout << ", ";
        }
    }
    cout << "]" << endl;
    std::cout << std::endl;

    vector<pair<int, vector<Edge>>> all_P;
    
    //OPTIMUM//
    if (runOPTIMUM) {
        cout << "OPTIMUM" << endl;
        auto start_optimum = high_resolution_clock::now();
        for (int target: targetnodes) {
            vector<Edge> P_optimum = OPTIMUM(filename, target, k);
            all_P.push_back(make_pair(target, P_optimum));
        }
        auto stop_optimum = high_resolution_clock::now();
        auto duration_optimum = duration_cast<milliseconds>(stop_optimum - start_optimum);
        cout << "OPTIMUM running time: " << duration_optimum.count() / 1000.0 << " seconds" << endl;
        printTargetNodesAndEdges(filename, "OPTIMUM", targetnodes, all_P);
        all_P.clear();
    }
    
    //EXACTSM//
    if (runEXACTSM) {
        cout << "EXACTSM" << endl;
        auto start_exactsm = high_resolution_clock::now();
        for (int target: targetnodes) {
            vector<Edge> P_exactsm = EXACTSM(filename, target, k);
            all_P.push_back(make_pair(target, P_exactsm));
        }
        auto stop_exactsm = high_resolution_clock::now();
        auto duration_exactsm = duration_cast<milliseconds>(stop_exactsm - start_exactsm);
        cout << "EXACTSM running time: " << duration_exactsm.count() / 1000.0 << " seconds" << endl;
        printTargetNodesAndEdges(filename, "EXACTSM", targetnodes, all_P);
        all_P.clear();
    }

    //FASTICM//
    if (runFASTICM) {
        cout << "FASTICM" << endl;
        double gamma = 0.95;
        double alpha = 0.05;
        int phi = diameter;
        auto start_fasticm = high_resolution_clock::now();
        for (int target: targetnodes) {
            int max_length = max_random_walk_length(filename, target, gamma);
            vector<Edge> P_fasticm = FASTICM(filename, k, target, max_length, alpha, phi);
            all_P.push_back(make_pair(target, P_fasticm));
        }
        auto stop_fasticm = high_resolution_clock::now();
        auto duration_fasticm = duration_cast<milliseconds>(stop_fasticm - start_fasticm);
        cout << "FASTICM running time: " << duration_fasticm.count() / 1000.0 << " seconds" << endl;
        printTargetNodesAndEdges(filename, "FASTICM", targetnodes, all_P);
        all_P.clear();
    }
    
    //APPROXISC//
    if (runAPPROXISC) {
        cout << "APPROXISC" << endl;
        double gamma = 0.95;
        double epsilon = 0.005;
        auto start_approxisc = high_resolution_clock::now();
        for (int target: targetnodes) {
            int max_length = max_random_walk_length(filename, target, gamma);
            vector<Edge> P_approxisc = processEdgesWithScores(filename, k, target, max_length, epsilon);
            all_P.push_back(make_pair(target, P_approxisc));
        }
        auto stop_approxisc = high_resolution_clock::now();
        auto duration_fasticm = duration_cast<milliseconds>(stop_approxisc - start_approxisc);
        cout << "APPROXISC running time: " << duration_fasticm.count() / 1000.0 << " seconds" << endl;
        printTargetNodesAndEdges(filename, "APPROXISC", targetnodes, all_P);
        all_P.clear();
    }
    return 0;
}
