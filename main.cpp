#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <list>
#include <chrono>

using namespace std;

// ==========================================
// 1. LRU CACHE (O(1) Memory Shield)
// ==========================================
class LRUCache {
    int capacity;
    list<pair<string, vector<string>>> dll; // Doubly Linked List
    unordered_map<string, decltype(dll.begin())> cacheMap; // Hash Map to list nodes

public:
    LRUCache(int cap) : capacity(cap) {}

    bool get(const string& key, vector<string>& result) {
        if (cacheMap.find(key) == cacheMap.end()) return false;
        
        // Move accessed item to the front (Most Recently Used)
        dll.splice(dll.begin(), dll, cacheMap[key]);
        result = cacheMap[key]->second;
        return true;
    }

    void put(const string& key, const vector<string>& value) {
        if (cacheMap.find(key) != cacheMap.end()) {
            dll.splice(dll.begin(), dll, cacheMap[key]);
            cacheMap[key]->second = value;
            return;
        }
        if (dll.size() == capacity) {
            // Evict Least Recently Used (Tail of list)
            cacheMap.erase(dll.back().first);
            dll.pop_back();
        }
        dll.push_front({key, value});
        cacheMap[key] = dll.begin();
    }
};

// ==========================================
// 2. TRIE NODE & TRIE (O(L) Prefix Search)
// ==========================================
class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
    bool isEndOfWord;
    TrieNode() : isEndOfWord(false) {}
};

class Trie {
    TrieNode* root;

    void dfs(TrieNode* node, string currentWord, vector<string>& result) {
        if (node->isEndOfWord) result.push_back(currentWord);
        for (auto& pair : node->children) {
            dfs(pair.second, currentWord + pair.first, result);
        }
    }

public:
    Trie() { root = new TrieNode(); }

    void insert(const string& word) {
        TrieNode* curr = root;
        for (char c : word) {
            if (curr->children.find(c) == curr->children.end()) {
                curr->children[c] = new TrieNode();
            }
            curr = curr->children[c];
        }
        curr->isEndOfWord = true;
    }

    vector<string> getWordsWithPrefix(const string& prefix) {
        TrieNode* curr = root;
        for (char c : prefix) {
            if (curr->children.find(c) == curr->children.end()) return {}; // Prefix not found
            curr = curr->children[c];
        }
        vector<string> result;
        dfs(curr, prefix, result); // Gather all words under this prefix
        return result;
    }
};

// ==========================================
// 3. AUTOCOMPLETE ENGINE (The Coordinator)
// ==========================================
class AutocompleteEngine {
    Trie trie;
    LRUCache cache;
    unordered_map<string, long long> globalFrequency; // Note: long long prevents integer overflow

public:
    AutocompleteEngine(int cacheSize) : cache(cacheSize) {}

    void insertWord(const string& word, long long frequency) {
        trie.insert(word);
        globalFrequency[word] = frequency;
    }

    pair<vector<string>, bool> search(const string& prefix) {
        vector<string> cachedResult;
        
        // Step 1: Check LRU Cache
        if (cache.get(prefix, cachedResult)) {
            return {cachedResult, true}; // Cache Hit
        }

        // Step 2: Traverse Trie on Cache Miss
        vector<string> words = trie.getWordsWithPrefix(prefix);
        
        // Step 3: Rank using Max-Heap
        auto comp = [&](const string& a, const string& b) {
            return globalFrequency[a] < globalFrequency[b];
        };
        priority_queue<string, vector<string>, decltype(comp)> maxHeap(comp);

        for (const string& w : words) {
            maxHeap.push(w);
        }

        // Step 4: Extract Top 5
        vector<string> topK;
        int k = 5;
        while (!maxHeap.empty() && k > 0) {
            topK.push_back(maxHeap.top());
            maxHeap.pop();
            k--;
        }

        // Step 5: Save to Cache and Return
        cache.put(prefix, topK);
        return {topK, false};
    }
};

// ==========================================
// 4. MAIN & WINDOWS REAL-TIME UI
// ==========================================
#include <conio.h> // Windows native console I/O for _getch()

void loadFromCSV(AutocompleteEngine& engine, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << ". Check if the file is in the same folder.\n";
        return;
    }

    string line, word, countStr;
    long long frequency;
    int wordsLoaded = 0;

    cout << "Loading dataset into RAM (This may take a moment)...\n";
    auto start = chrono::high_resolution_clock::now();

    getline(file, line); // Skip header

    while (getline(file, line)) {
        stringstream ss(line);
        if (getline(ss, word, ',') && getline(ss, countStr, ',')) {
            try {
                frequency = stoll(countStr);
                engine.insertWord(word, frequency);
                wordsLoaded++;
            } catch (...) {
                // Skip malformed entries safely
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "[SYSTEM] Successfully loaded " << wordsLoaded << " words in " << duration.count() << " ms.\n";
}

int main() {
    AutocompleteEngine engine(50); // Expanded cache capacity for rapid interactive typing
    loadFromCSV(engine, "unigram_freq.csv");

    string prefix = "";
    
    while (true) {
        // Clear screen instantly on every keystroke to render fresh suggestions
        system("cls"); 
        
        cout << "==================================================\n";
        cout << "   REAL-TIME SEARCH SEARCH ENGINE INITIALIZED\n";
        cout << "==================================================\n";
        cout << " Instructions: Start typing naturally. \n";
        cout << " Press [Backspace] to delete | Press [Esc] to exit.\n";
        cout << "==================================================\n";
        cout << "\n Search Bar >> " << prefix << "_\n";
        cout << "--------------------------------------------------\n";

        // Only query the pipeline if the user has typed something
        if (!prefix.empty()) {
            auto start = chrono::high_resolution_clock::now();
            auto result = engine.search(prefix);
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

            if (result.first.empty()) {
                cout << "  [No matches found]\n";
            } else {
                for (size_t i = 0; i < result.first.size(); ++i) {
                    cout << "  \t-> " << result.first[i] << "\n";
                }
            }
            
            cout << "--------------------------------------------------\n";
            if (result.second) {
                cout << " [STATUS: CACHE HIT]  -> 0 microseconds (O(1) Memory Shield)\n";
            } else {
                cout << " [STATUS: CACHE MISS] -> " << duration.count() << " microseconds (Trie + Heap)\n";
            }
        } else {
            cout << "  (Type characters to see ranked autocomplete lists)\n";
            cout << "--------------------------------------------------\n";
        }

        // Intercept raw hardware keystrokes before they hit standard OS buffers
        char ch = _getch();

        if (ch == 27) { 
            // Escape key condition to terminate program clean
            break; 
        } 
        else if (ch == 8) { 
            // Backspace condition to safely shrink prefix
            if (!prefix.empty()) {
                prefix.pop_back();
            }
        } 
        else if (ch >= 32 && ch <= 126) {
            // Strictly enforce standard printable ASCII spectrum (ignoring system function keys)
            // Automatically downcase input to match database formatting
            prefix += tolower(ch);
        }
    }

    system("cls");
    cout << "Search engine system terminated safely.\n";
    return 0;
}