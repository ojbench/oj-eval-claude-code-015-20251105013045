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

        // Use append-only approach for better performance
        ofstream file(filename, ios::binary | ios::app);
        file.write(reinterpret_cast<char*>(&new_entry), sizeof(Entry));
        file.close();
    }

    void remove(const string& key, int value) {
        // Mark entry as deleted by writing to a separate file
        // This avoids rewriting the entire file
        string delete_filename = filename + ".deleted";
        ofstream delete_file(delete_filename, ios::binary | ios::app);

        Entry delete_entry;
        strncpy(delete_entry.key, key.c_str(), 64);
        delete_entry.key[64] = '\0';
        delete_entry.value = value;

        delete_file.write(reinterpret_cast<char*>(&delete_entry), sizeof(Entry));
        delete_file.close();
    }

    vector<int> find(const string& key) {
        // Read all entries from main file
        vector<Entry> entries = read_all_entries(filename);

        // Read deleted entries
        vector<Entry> deleted_entries = read_all_entries(filename + ".deleted");

        // Filter out deleted entries and entries with different keys
        vector<int> values;
        for (const auto& entry : entries) {
            if (strcmp(entry.key, key.c_str()) == 0) {
                // Check if this entry is deleted
                bool is_deleted = false;
                for (const auto& deleted : deleted_entries) {
                    if (entry == deleted) {
                        is_deleted = true;
                        break;
                    }
                }
                if (!is_deleted) {
                    values.push_back(entry.value);
                }
            }
        }

        sort(values.begin(), values.end());
        return values;
    }

private:
    vector<Entry> read_all_entries(const string& fname) {
        vector<Entry> entries;
        ifstream file(fname, ios::binary);

        if (!file.is_open()) {
            return entries;
        }

        Entry entry;
        while (file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            entries.push_back(entry);
        }
        file.close();

        return entries;
    }

    bool exists(const string& key, int value) {
        Entry target;
        strncpy(target.key, key.c_str(), 64);
        target.key[64] = '\0';
        target.value = value;

        // Read all entries from main file
        vector<Entry> entries = read_all_entries(filename);

        // Read deleted entries
        vector<Entry> deleted_entries = read_all_entries(filename + ".deleted");

        // Check if entry exists and is not deleted
        for (const auto& entry : entries) {
            if (entry == target) {
                // Check if this entry is deleted
                for (const auto& deleted : deleted_entries) {
                    if (entry == deleted) {
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
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