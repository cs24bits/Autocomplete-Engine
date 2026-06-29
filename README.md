# Low-Latency Autocomplete Engine

This project is a high-performance, real-time autocomplete engine built in C++. It was designed to provide instant, frequency-ranked search suggestions for large-scale datasets, with a focus on minimizing memory overhead and achieving sub-millisecond query latencies.

### Core Architecture
To handle large-scale word data efficiently, I implemented the following data structures:
- **n-ary Trie:** Used to store the word dictionary. This ensures that prefix searches are independent of the total dictionary size, providing O(L) time complexity where L is the prefix length.
- **LRU Cache:** Implemented using a **Doubly Linked List** and a **Hash Map**. This allows for O(1) lookup time for recent queries, bypassing the Trie entirely for repetitive searches.
- **Max-Heap:** Used for real-time ranking. When a prefix matches multiple entries, a `std::priority_queue` isolates the Top-5 results based on their frequency weights.

### Performance Optimizations
- **64-bit Integer Handling:** The engine uses `long long` for frequency tracking to accommodate real-world datasets (like the Kaggle unigram corpus) where word weights can exceed 23 billion.
- **I/O Efficiency:** Bypassed standard buffered input streams to capture raw hardware keystrokes. This allows the search engine to trigger and re-render suggestions instantly as the user types, eliminating input lag.

### How to Run
1. Ensure you have a C++ compiler (g++ recommended) installed.
2. Compile the source code:
   `g++ main.cpp -o autocomplete`
3. Run the executable:
   `./autocomplete`

### Dataset
The engine is currently configured to work with `unigram_freq.csv`. Ensure this file is present in the root directory before running the application to load the frequency weights into the Trie.
