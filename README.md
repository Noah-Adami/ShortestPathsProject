
# Shortest Paths

**Name:** Noah Adami  
**Course:** CSE 3318 – Data Structures and Algorithms  
**Language:** C++

## Project Overview

This project implements three shortest path algorithms: BFS, Bellman-Ford, and Dijkstra. The program reads graphs from text files, finds the shortest paths from a starting vertex, and compares the running times of the algorithms.

## How to Run

Open the project in Xcode and run `main.cpp`.

You can also compile it using Terminal:

```bash
g++ -std=c++17 main.cpp -o shortest_paths
./shortest_paths
```

Make sure the `inputs` folder is in the same working directory.

## Algorithms

- **BFS:** Finds shortest paths in unweighted graphs.
- **Bellman-Ford:** Works with negative weights and detects negative cycles.
- **Dijkstra:** Finds shortest paths in graphs with nonnegative weights.

## Input Files

Graph files are stored in the `inputs` folder.

Each file starts with the number of vertices and edges, followed by the starting vertex, ending vertex, and weight for each edge.

At the filename prompt, enter a file name like `weighted.txt` or `unweighted.txt`.

## Experiments

I tested the algorithms using graphs with 100, 250, 500, and 1,000 vertices. The program measures how long each algorithm takes to run in milliseconds.

The results and comparisons are included in the project report.

## Project Files

- `main.cpp` – Program source code
- `inputs/` – Graph test files
- `README.md` – Instructions
- `Report.pdf` – Report and runtime results
- `ShortestPathsProject.xcodeproj/` – Xcode project

## Conclusion

BFS works best for unweighted graphs, Bellman-Ford handles negative weights, and Dijkstra is useful for graphs with nonnegative weights. Their running times depend on the size and type of graph.
