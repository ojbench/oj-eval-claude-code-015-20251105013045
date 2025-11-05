#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <cstring>

using namespace std;

// Structure to store key-value pair
struct Entry {
    char key[65];  // 64 bytes + null terminator
    int value;
};

class FileStorage {
private:
    string filename;
    unordered_map<string, set<int>> cache;  // In-memory cache for fast access
    bool cache_dirty;

public:
    FileStorage(const string& fname) : filename(fname), cache_dirty(false) {
        load_cache();
    }

    ~FileStorage() {
        if (cache_dirty) {
            save_cache();
        }
    }

    void insert(const string& key, int value) {
        cache[key].insert(value);
        cache_dirty = true;
    }

    void remove(const string& key, int value) {
        auto it = cache.find(key);
        if (it != cache.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                cache.erase(it);
            }
            cache_dirty = true;
        }
    }

    vector<int> find(const string& key) {
        auto it = cache.find(key);
        if (it == cache.end()) {
            return {};
        }
        return vector<int>(it->second.begin(), it->second.end());
    }

private:
    void load_cache() {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            return;
        }

        Entry entry;
        while (file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            cache[entry.key].insert(entry.value);
        }
        file.close();
    }

    void save_cache() {
        ofstream file(filename, ios::binary | ios::trunc);

        for (const auto& pair : cache) {
            for (int value : pair.second) {
                Entry entry;
                strncpy(entry.key, pair.first.c_str(), 64);
                entry.key[64] = '\0';
                entry.value = value;
                file.write(reinterpret_cast<const char*>(&entry), sizeof(Entry));
            }
        }
        file.close();
        cache_dirty = false;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    FileStorage storage("data.db");

    int n;
    cin >> n;
    cin.ignore();  // Ignore newline after n

    for (int i = 0; i < n; i++) {
        string command;
        cin >> command;

        if (command == "insert") {
            string key;
            int value;
            cin >> key >> value;
            storage.insert(key, value);
        } else if (command == "delete") {
            string key;
            int value;
            cin >> key >> value;
            storage.remove(key, value);
        } else if (command == "find") {
            string key;
            cin >> key;
            vector<int> values = storage.find(key);

            if (values.empty()) {
                cout << "null\n";
            } else {
                for (size_t j = 0; j < values.size(); j++) {
                    if (j > 0) cout << " ";
                    cout << values[j];
                }
                cout << "\n";
            }
        }
    }

    return 0;
}