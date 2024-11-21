
# A Fast Algorithm for Moderating Critical Nodes via Edge Removal

## Description

This program calculates the effective resistance between pairs of nodes in a graph based on the Laplacian matrix and its pseudo-inverse. It employs fast algorithms to identify critical edges for removal to minimize the information centrality of specified target nodes. The computation leverages the Eigen library for efficient matrix operations and is designed for undirected, unweighted graphs. The output includes the sum of effective resistances, runtime performance, and the specific edges to remove.

---

## Prerequisites

- **Python(3.9.20)**: Used for additional analysis and visualization.
- **C++ compiler**: (e.g., `g++`) for compiling the C++ code.
- **Eigen library**: A C++ template library for linear algebra.

---

## Installation

### Step 1: Install Eigen
1. Clone the Eigen library repository:
   ```bash
   cd include
   git clone https://gitlab.com/libeigen/eigen.git
   cd ..
   ```
2. Include the Eigen directory (e.g., `include/eigen/`) in your project or specify it during compilation.

### Step 2: Compile the Code
You can compile the program manually or using the provided Makefile.

#### Manual Compilation:
```bash
g++ -std=c++17 -I include/eigen/ code/main.cpp code/graph_utils.cpp -o output/main -O3
```

#### Compilation with Makefile:
```bash
make main
```

### Step 3: Install Python Dependencies
Install the Python dependencies listed in the `requirements.txt` file:
```bash
pip install -r python/requirements.txt
```

### Step 4: Test Using Jupyter Notebook
Run the Jupyter notebook to validate and analyze results:
```bash
jupyter notebook python/Test_information_centrality.ipynb
```

---

## Usage

### Run the Program
Execute the compiled binary with the following syntax:
```bash
./output/main <input_file> -t "<target_nodes>" [options]
```

#### Example:
```bash
./output/main data/karate.mtx -t "12, 15" -k 3
```

### Command-Line Options:
| Option                   | Description                                                                                           |
|--------------------------|-------------------------------------------------------------------------------------------------------|
| `-t "<nodes>"`           | Specify target nodes whose information centrality is to be reduced (comma-separated).                |
| `-d <dimension>`         | Specify the graph's dimension (default is `1`).                                                      |
| `-k <num_edges>`         | Number of edges to remove per target node (default is `3`).                                          |
| `--disable-optimum`      | Disable the OPTIMUM algorithm.                                                                       |
| `--disable-exactsm`      | Disable the EXACTSM algorithm.                                                                       |
| `--disable-approxisc`    | Disable the APPROXISC algorithm.                                                                     |
| `--disable-fasticm`      | Disable the FASTICM algorithm.                                                                       |

#### Examples:
1. Target multiple nodes:
   ```bash
   ./output/main data/polbooks.mtx -t "12, 23, 1"
   ```
2. Adjust graph dimension:
   ```bash
   ./output/main data/polbooks.mtx -t "12, 23" -d 2
   ```
3. Limit edges removed per node:
   ```bash
   ./output/main data/polbooks.mtx -t "12, 23" -k 1
   ```
4. Disable specific algorithms:
   ```bash
   ./output/main data/polbooks.mtx -t "12, 23" --disable-optimum --disable-exactsm
   ```

---

## Output

Results are stored in the `result/` folder.

#### Example Output in terminal when running `main`:
```text
Target nodes: [12, 15]
OPTIMUM running time: 10.213 seconds
OPTIMUM edges to remove: [(12, 0), (15, 2), (6, 3)]
EXACTSM running time: 0.097 seconds
EXACTSM edges to remove: [(12, 0), (15, 2), (6, 3)]
```

---

## Comparing Information Centrality in Python

The Jupyter notebook `python/Test_information_centrality.ipynb` allows you to analyze the graph before and after edge removal. Modify the following parameters in the notebook:
```python
# Modify here
data_name = "karate"
```

#### Output Example:
```text
Information of target nodes when remove edges from ../result/karateFASTICM_result.txt
Before remove edges of information centrality average of target nodes: 1.48
After remove edges of information centrality average of target nodes: 1.23

Information of target nodes when remove edges from ../result/karateAPPROXISC_result.txt
Before remove edges of information centrality average of target nodes: 1.48
After remove edges of information centrality average of target nodes: 1.30

Information of target nodes when remove edges from ../result/karateEXACTSM_result.txt
Before remove edges of information centrality average of target nodes: 1.48
After remove edges of information centrality average of target nodes: 0.82

Information of target nodes when remove edges from ../result/karateOPTIMUM_result.txt
Before remove edges of information centrality average of target nodes: 1.48
After remove edges of information centrality average of target nodes: 0.82
```

---

## Notes

1. The program supports **undirected, unweighted graphs**.
2. Ensure that the Eigen library is installed and properly included during compilation.
3. Python scripts rely on `requirements.txt`. Install dependencies before running Python-based analysis.

---
