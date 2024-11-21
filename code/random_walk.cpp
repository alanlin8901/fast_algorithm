#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cstring>

void readMtxToCSR(const std::string& filename, std::vector<int>& row_ptr, std::vector<int>& col_idx, std::vector<int>& dirct_vertex, std::vector<int>& dirct_edge) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file " << filename << std::endl;
        return;
    }

    std::vector<std::pair<int, int>> edges;
    int max_node = -1;
    int u, v;
    while (file >> u >> v) {
        if (u > v)
            edges.emplace_back(v - 1, u - 1);
        else
            edges.emplace_back(u - 1, v - 1);

        max_node = std::max(max_node, std::max(u - 1, v - 1));
        if (u != v) {
            if (u > v)
                edges.emplace_back(u - 1, v - 1);
            else
                edges.emplace_back(v - 1, u - 1);
        }
    }

    file.close();
    row_ptr.resize(max_node + 1, 0);
    for (const auto& edge : edges) {
        row_ptr[edge.first + 1]++;
    }
    for (int i = 1; i < row_ptr.size(); ++i) {
        row_ptr[i] += row_ptr[i - 1];
    }
    col_idx.resize(edges.size());
    std::vector<int> current_row_count(max_node, 0);

    for (const auto& edge : edges) {
        int row = edge.first;
        int dest = edge.second;
        int index = row_ptr[row] + current_row_count[row];
        col_idx[index] = dest;
        current_row_count[row]++;
    }
    file.open(filename);
    edges.clear();
    max_node = -1;
    while (file >> u >> v) {
        if (u > v)
            edges.emplace_back(v - 1, u - 1);
        else
            edges.emplace_back(u - 1, v - 1);

        max_node = std::max(max_node, std::max(u - 1, v - 1));
    }

    file.close();
    dirct_vertex.resize(max_node + 1, 0);
    dirct_edge.resize(edges.size());
    for (const auto& edge : edges) {
        dirct_vertex[edge.first + 1]++;
    }
    for (int i = 1; i < dirct_vertex.size(); ++i) {
        dirct_vertex[i] += dirct_vertex[i - 1];
    }
    current_row_count.assign(max_node, 0);

    for (const auto& edge : edges) {
        int row = edge.first;
        int dest = edge.second;
        int index = dirct_vertex[row] + current_row_count[row];
        dirct_edge[index] = dest;
        current_row_count[row]++;
    }

    return;
}

float random_walk(const std::vector<int>& row_ptr, const std::vector<int>& col_idx, int source, int target, int steps) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0, 1.0);

    int current_node = source;
    int current_step = 0;

    while (current_step < steps) {
        int start = row_ptr[current_node];
        int end;
        if (current_node >= row_ptr.size() - 1)
            end = col_idx.size();
        else
            end = row_ptr[current_node + 1];

        if (start == end) {
            return 0;
        }
        int next_node = col_idx[start + (int)((end - start) * dis(gen))];
        current_node = next_node;
        current_step++;
        if (current_node == target) {
            return 1 / (float)current_step;
        }
    }
    return 0;
}

std::mutex edge_scores_mutex;

void calculateEdgeScore(const std::vector<int>& row_ptr, const std::vector<int>& col_idx, const std::vector<int>& dirct_vertex, const std::vector<int>& dirct_edge, std::vector<float>& edge_scores, int target, int rho, int steps) {
    int target_index = target - 1;
    std::vector<std::thread> threads;

    // Reserve space to prevent reallocation
    threads.reserve(dirct_vertex.size());

    for (int i = 0; i < dirct_vertex.size(); ++i) {
        threads.emplace_back([&, i]() {
            int source_1 = i;
            int start = dirct_vertex[i];
            int end = (i == dirct_vertex.size() - 1) ? dirct_edge.size() : dirct_vertex[i + 1];
            if (source_1 == target_index) {
                for (int j = start; j < end; ++j) {
                    int source_2 = dirct_edge[j];
                    float score_sum = 0;
                    for (int count = 0; count < rho; ++count) {
                        float v1_score = random_walk(row_ptr, col_idx, source_2, target_index, steps);
                        if (v1_score != 0) {
                            score_sum += v1_score;
                        }
                    }
                    std::lock_guard<std::mutex> lock(edge_scores_mutex);
                    edge_scores[j] = score_sum / (float)rho;
                }
                return;
            }

            for (int j = start; j < end; ++j) {
                int source_2 = dirct_edge[j];
                float score_sum = 0;

                for (int count = 0; count < rho; ++count) {
                    float v1_score = random_walk(row_ptr, col_idx, source_1, target_index, steps);

                    if (source_2 != target_index) {
                        float v2_score = random_walk(row_ptr, col_idx, source_2, target_index, steps);
                        if (v1_score != 0 && v2_score != 0) {
                            score_sum += (v1_score + v2_score);
                        }
                    } else {
                        if (v1_score != 0) {
                            score_sum += v1_score;
                        }
                    }
                }

                std::lock_guard<std::mutex> lock(edge_scores_mutex);
                edge_scores[j] = score_sum / (float)rho;
            }
        });
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}


int main(int argc, char* argv[]) {
    std::string filename;
    int target = 1;
    int rho = 1000;
    int steps = 1000;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-t") == 0 || std::strcmp(argv[i], "--target") == 0) {
            if (i + 1 < argc) {
                target = std::stoi(argv[++i]);
            }
        } else {
            filename = argv[i];
        }
    }
    if (filename.empty()) {
        std::cout << "Please enter the path to the .mtx file: ";
        std::cin >> filename;
    }
    std::vector<int> row_ptr;
    std::vector<int> col_idx;
    std::vector<int> dirct_vertex;
    std::vector<int> dirct_edge;
    readMtxToCSR(filename, row_ptr, col_idx, dirct_vertex, dirct_edge);
    std::vector<float> edge_scores(dirct_edge.size(), 0);
    calculateEdgeScore(row_ptr, col_idx, dirct_vertex, dirct_edge, edge_scores, target, rho, steps);
    std::vector<std::tuple<int, int, float>> edges_with_scores;
    for (int i = 0; i < dirct_edge.size(); ++i) {
        int source_vertex = -1;
        for (int j = 0; j < dirct_vertex.size() - 1; ++j) {
            if (i >= dirct_vertex[j] && i < dirct_vertex[j + 1]) {
                source_vertex = j;
                break;
            }
        }
        int target_vertex = dirct_edge[i];
        edges_with_scores.emplace_back(source_vertex, target_vertex, edge_scores[i]);
    }
    std::sort(edges_with_scores.begin(), edges_with_scores.end(), [](const std::tuple<int, int, float>& a, const std::tuple<int, int, float>& b) {
        return std::get<2>(a) > std::get<2>(b);
    });
    std::string formatted_output;
    for (int i = 0; i < 10 && i < edges_with_scores.size(); ++i) {
        int source_vertex = std::get<0>(edges_with_scores[i]);
        int target_vertex = std::get<1>(edges_with_scores[i]);
        if (!formatted_output.empty()) {
            formatted_output += ", ";
        }
        formatted_output += "(" + std::to_string(source_vertex + 1) + ", " + std::to_string(target_vertex + 1) + ")";
    }
    std::cout << formatted_output << std::endl;

    return 0;
}
