# Inverted Search Engine (C++ Implementation)

[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)](https://isocpp.org/)
[![Build System](https://img.shields.io/badge/Build-CMake%20%7C%20Make-brightgreen.svg)]()
[![Testing](https://img.shields.io/badge/Testing-Catch2-red.svg)]()
![License](https://img.shields.io/badge/License-MIT-green)

A high-performance, multi-threaded **Inverted Search Engine** built in C++ as an optimized implementation of the **SIGMOD 2013 Programming Contest** task. Designed for real-time keyword matching over continuous document streams, this system implements custom metric indexing structures (**BK-Trees**, **Hashtables**) built entirely from scratch without using the C++ Standard Template Library (STL).

Developed as part of the **Software Development for Information Systems (K23a)** course (Winter Semester 2021-2022) at the **Department of Informatics & Telecommunications, National and Kapodistrian University of Athens (NKUA)**.

Contest website: [ACM SIGMOD Programming Contest 2013](https://sigmod.kaust.edu.sa/task-details.html)


## Overview

Standard search engines locate documents against dynamic search queries over a static database. In contrast, an **Inverted Search Engine** processes a continuous stream of incoming documents against a large, dynamic set of pre-registered queries (analogous to real-time event or tweet stream filtering).

This engine determines in real time which registered queries are satisfied by each arriving document. A document satisfies a query if it contains keywords matching **all** query terms within strictly defined similarity thresholds.

### Supported Keyword Matching Strategies:
1. **Exact Matching**: Constant-time matching ($O(1)$) via custom Hash Tables.
2. **Approximate Edit Distance Matching**: Levenshtein distance metric space matching evaluated over custom **BK-Trees**.
3. **Approximate Hamming Distance Matching**: Hamming metric matching evaluated across an **Array of BK-Trees**, partitioned by word length (lengths 4 to 31).




## Single-thread Implementation
The application logic is encapsulated inside the `Sigismund` core class, exposing the C-interface required by the contest benchmark driver:


### Primary Indices

- **`exactIndex` (Hashtable):** Direct dictionary lookups for exact match queries.
- **`editIndex` (BKTree):** Unified metric tree indexing words for Levenshtein distance queries.
- **`hammingIndex` (Array of BKTrees):** Array of 28 BK-Trees partitioned by word length for Hamming distance matching.
- **`queries` / `inactiveQueries` (Hashtable):** Tracks active query definitions and soft-deleted Query IDs for O(1) invalidation during result retrieval (`GetNextAvailRes`).

### Core Methods

- **`StartQuery`:** Tokenizes queries using `strtok_r`, inserts words into the respective indices, and updates postings lists (`Entry`).
- **`EndQuery`:** Registers the target Query ID in `inactiveQueries` for fast filtering during output extraction.
- **`MatchDocument`:** Deduplicates document words using a hash set, executes parallel multi-index range searches, and verifies complete query word coverage.

## Multi-threaded Implementation

To speed up processing using all CPU cores, parallel processing is driven by POSIX Threads (`pthreads`), mutexes, semaphores, and synchronization barriers managed through a custom **Thread Pool & Job Scheduler**.

The engine evaluates two distinct parallelization strategies:

* **Document-Level Parallelism:** Concurrently processes independent documents within a batch across multiple worker threads.
* **Word-Level Parallelism:** Concurrently searches the unique words of a single document across metric indices. This approach achieves near-linear scaling on large datasets (100MB+), cutting execution time down to **~25.7%** of the single-threaded baseline.

**Note:** For more detailed multi-threading implementation analysis and performance measurements, please refer to the [`project_report`](project_report.pdf) file.


## Unit Testing
**Internal State Testing Technique:** The compilation macro `#define private public` is selectively utilized in test targets (`MODE=TESTS`). This technique allows direct verification and testing of private internal data structures (e.g., BK-Tree nodes, HashTable buckets) without polluting production header files or introducing unnecessary public accessor methods.

For the testing of the structures, we used the library `catch.hpp`.

More information about the [Catch2](https://github.com/catchorg/Catch2).

## Compile & Execution

The Makefile provides the following build and execution targets:

- **`make tests MODE=TESTS`**: Compiles the `testsx` executable, which runs the unit test suite.
- **`make driver`**: Compiles the `testdriver` executable to run the competition driver with our library using **word-level multithreading**.
- **`make driver MT=DOCUMENT`**: Compiles the `testdriver` executable to run the competition driver with our library using **document-level multithreading**.
- **`make lib`**: Builds the shared library `libcore.so`.
- **`make clean`**: Removes the files generated by the Makefile.

### Compiler Optimizations

The Makefile enables `-O3` compiler optimizations, consistent with the reference implementation. No issues have been observed, and our implementation remains significantly faster than the reference implementation even without these optimizations.

## Environment 
* **Programming language:** C++
* **Build Tools:** CMake, Make
* **OS:** Ubuntu Linux 18.04, Ubuntu 20.04, PopOS 21.04, PopOS 21.10, Manjaro 21.1.6.2

## Authors

* **Sergios - Anestis Kefalidis** ([@SKefalidis](https://github.com/SKefalidis))
* **Eleftheria Lagkoni** ([@elagkoni](https://github.com/elagkoni)) 