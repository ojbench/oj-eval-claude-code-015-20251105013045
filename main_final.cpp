#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

// Structure to store key-value pair
struct Entry {
    char key[65];  // 64 bytes + null terminator
    int value;

    bool operator<(const Entry& other) const {
        int key_cmp = strcmp(key, other.key);
        if (key_cmp != 0) return key_cmp < 0;
        return value < other.value;
    }

    bool operator==(const Entry& other) const {
        return strcmp(key, other.key) == 0 && value == other.value;
    }
};

class FileStorage {
private:
    string filename;

public:
    FileStorage(const string& fname) : filename(fname) {
        // Create file if it doesn't exist
        ofstream file(filename, ios::binary | ios::app);
        file.close();
    }

    void insert(const string& key, int value) {
        // Check if entry already exists
        if (exists(key, value)) {
            return;  // Already exists, no need to insert
        }

        Entry new_entry;
        strncpy(new_entry.key, key.c_str(), 64);
        new_entry.key[64] = '\0';
        new_entry.value = value;

        // Read all entries and insert in sorted position
        vector<Entry> entries = read_all_entries();

        // Find insertion position using binary search
        auto pos = lower_bound(entries.begin(), entries.end(), new_entry);
        entries.insert(pos, new_entry);

        write_all_entries(entries);
    }

    void remove(const string& key, int value) {
        Entry target;
        strncpy(target.key, key.c_str(), 64);
        target.key[64] = '\0';
        target.value = value;

        // Read all entries and remove target
        vector<Entry> entries = read_all_entries();

        auto pos = lower_bound(entries.begin(), entries.end(), target);
        if (pos != entries.end() && *pos == target) {
            entries.erase(pos);
            write_all_entries(entries);
        }
    }

    vector<int> find(const string& key) {
        vector<Entry> entries = read_all_entries();
        vector<int> values;

        // Binary search for first occurrence of key
        Entry search_key;
        strncpy(search_key.key, key.c_str(), 64);
        search_key.key[64] = '\0';
        search_key.value = -1;  // Minimum value

        auto start = lower_bound(entries.begin(), entries.end(), search_key);

        // Collect all values for this key
        for (auto it = start; it != entries.end() && strcmp(it->key, key.c_str()) == 0; ++it) {
            values.push_back(it->value);
        }

        return values;
    }

private:
    vector<Entry> read_all_entries() {
        vector<Entry> entries;
        ifstream file(filename, ios::binary);

        Entry entry;
        while (file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            entries.push_back(entry);
        }
        file.close();

        return entries;
    }

    void write_all_entries(const vector<Entry>& entries) {
        ofstream file(filename, ios::binary | ios::trunc);
        for (const auto& entry : entries) {
            file.write(reinterpret_cast<const char*>(&entry), sizeof(Entry));
        }
        file.close();
    }

    bool exists(const string& key, int value) {
        Entry target;
        strncpy(target.key, key.c_str(), 64);
        target.key[64] = '\0';
        target.value = value;

        vector<Entry> entries = read_all_entries();
        auto pos = lower_bound(entries.begin(), entries.end(), target);
        return pos != entries.end() && *pos == target;
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