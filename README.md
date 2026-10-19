# Inverted Search Engine (C++ Implementation)

[![CMake Build](https://github.com/elagkoni/inverted-search-engine/actions/workflows/cmake.yml/badge.svg)](https://github.com/elagkoni/inverted-search-engine/actions)
[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)](https://isocpp.org/)
[![Testing](https://img.shields.io/badge/Testing-Catch2-red.svg)]()
![License](https://img.shields.io/badge/License-MIT-green)

A high-performance, multi-threaded **Inverted Search Engine** built in C++ as an optimized implementation of the **SIGMOD 2013 Programming Contest** task. Designed for real-time keyword matching over continuous document streams, this system uses custom-built structures (**BK-Trees**, **Hashtables**) to quickly index and search text.

Developed as part of the **Software Development for Information Systems (K23a)** course (Winter Semester 2021-2022) at the **Department of Informatics & Telecommunications, National and Kapodistrian University of Athens (NKUA)**.

Contest website: [ACM SIGMOD Programming Contest 2013](https://sigmod.kaust.edu.sa/task-details.html)


## Overview

Standard search engines locate documents against dynamic search queries over a static database. In contrast, an **Inverted Search Engine** processes a continuous stream of incoming documents against a large, dynamic set of pre-registered queries (analogous to real-time event or tweet stream filtering).

This engine determines in real time which registered queries are satisfied by each arriving document. A document satisfies a query if it contains keywords matching **all** query terms within strictly defined similarity thresholds.

### Supported Keyword Matching Strategies:
1. **Exact Matching**: Constant-time matching ( $O(1)$ ) via custom **Hashtables**.
2. **Approximate Edit Distance Matching**: Approximate word matching (Levenshtein distance) using custom **BK-Trees**.
3. **Approximate Hamming Distance Matching**: Hamming distance matching handled by an **Array of BK-Trees**, grouped by word length (lengths 4 to 31).

## Custom Data Structures 
- **BK-Tree:** Implements `insert`, `lookup`, and a specialized `multiLookup` method that searches within a distance range `[minDist, maxDist]` in a single tree traversal.
- **Hashtable:** A custom hash table using the `djb2` algorithm and dynamic resizing for fast $O(1)$ lookups.
- **List:** Custom linked list for quick $O(1)$ insertions of search results.

## Single-thread Implementation
The `Sigismund` class handles the main logic and provides the C-interface needed by the contest test driver:

### Search Indices

- `exactIndex` **(Hashtable):** Direct dictionary lookups for exact match queries.
- `editIndex` **(BKTree):** Tree structure that searches for words with similar spelling or small typos.
- `hammingIndex` **(Array of BKTrees):** Array of 28 BK-Trees partitioned by word length for Hamming distance matching.
- `queries` / `inactiveQueries` **(Hashtable):** Stores active and deleted queries for fast ( $O(1)$ ) lookup and filtering.

### Core Methods

- `StartQuery`: Splits incoming queries into words and adds them to the search indices.
- `EndQuery`: Marks a query as inactive so its results are automatically skipped.
- `MatchDocument`: Searches document words across all indices (in parallel) to find matching queries.

## Multi-threaded Implementation

To utilize all CPU cores, multi-threading is implemented using `pthreads` with a custom **Thread Pool & Job Scheduler**.

The engine supports two parallel strategies:

- **Document-Level Parallelism:** Processes multiple documents simultaneously across worker threads.
- **Word-Level Parallelism:** Processes the unique words of a single document in parallel. On large datasets (100MB+), this approach reduces execution time to **~25.7%** of the single-threaded baseline (~4x speedup).

**Note:** For more detailed multi-threading implementation analysis and performance measurements, please refer to the [`project_report`](project_report.pdf) file.

## Unit Testing 

Unit testing is implemented using the [Catch2](https://github.com/catchorg/Catch2) framework, with automated testing fully integrated via **GitHub Actions**.

### Internal State Testing
Unit tests (`MODE=TESTS`) directly inspect internal structures (BK-Tree nodes, Hashtable buckets) to verify core logic without polluting production headers with unnecessary public accessors.

## Compile & Execution

### Option 1: CMake Build

1. **Create build directory and configure:** `mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release ..`
2. **Compile project:** `make`
3. **Run main application:** `./sigismund`
4. **Run unit tests:** `ctest --output-on-failure`

**Note:** Passing `-DCMAKE_BUILD_TYPE=Release` enables full compiler optimizations (`-O3`).

### Option 2: Makefile Build

* **Run Unit Tests:** `make tests MODE=TESTS && ./testsx`
* **Build & Run Competition Driver (Word-Level Multithreading - Default):** `make driver && ./testdriver`
* **Build & Run Competition Driver (Document-Level Multithreading):** `make driver MT=DOCUMENT && ./testdriver`
* **Build Shared Library:** `make lib`
* **Clean Build Files:** `make clean`

## Environment 
* **Programming language:** C++
* **Build Tools:** CMake, Make
* **OS:** Ubuntu Linux 18.04, Ubuntu 20.04, PopOS 21.04, PopOS 21.10, Manjaro 21.1.6.2

## Authors

* **Sergios - Anestis Kefalidis** ([@SKefalidis](https://github.com/SKefalidis))
* **Eleftheria Lagkoni** ([@elagkoni](https://github.com/elagkoni)) 