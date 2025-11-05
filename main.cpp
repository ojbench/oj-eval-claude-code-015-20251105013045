#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <set>

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
    string delete_filename;
    int operation_count;
    static const int COMPACT_THRESHOLD = 100;

public:
    FileStorage(const string& fname) : filename(fname),
                                       delete_filename(fname + ".deleted"),
                                       operation_count(0) {
        // Create files if they don't exist
        ofstream file(filename, ios::binary | ios::app);
        file.close();
        ofstream dfile(delete_filename, ios::binary | ios::app);
        dfile.close();
    }

    void insert(const string& key, int value) {
        Entry new_entry;
        strncpy(new_entry.key, key.c_str(), 64);
        new_entry.key[64] = '\0';
        new_entry.value = value;

        // Use append-only approach for better performance
        ofstream file(filename, ios::binary | ios::app);
        file.write(reinterpret_cast<char*>(&new_entry), sizeof(Entry));
        file.close();

        operation_count++;
        if (operation_count >= COMPACT_THRESHOLD) {
            compact_files();
            operation_count = 0;
        }
    }

    void remove(const string& key, int value) {
        // Mark entry as deleted by writing to a separate file
        ofstream delete_file(delete_filename, ios::binary | ios::app);

        Entry delete_entry;
        strncpy(delete_entry.key, key.c_str(), 64);
        delete_entry.key[64] = '\0';
        delete_entry.value = value;

        delete_file.write(reinterpret_cast<char*>(&delete_entry), sizeof(Entry));
        delete_file.close();

        operation_count++;
        if (operation_count >= COMPACT_THRESHOLD) {
            compact_files();
            operation_count = 0;
        }
    }

    vector<int> find(const string& key) {
        // Read all entries from main file
        vector<Entry> entries = read_all_entries(filename);

        // Read deleted entries and create a set for fast lookup
        vector<Entry> deleted_entries = read_all_entries(delete_filename);
        set<Entry> deleted_set(deleted_entries.begin(), deleted_entries.end());

        // Use set to avoid duplicates and maintain order
        set<int> value_set;
        for (const auto& entry : entries) {
            if (strcmp(entry.key, key.c_str()) == 0) {
                // Check if this entry is deleted
                if (deleted_set.find(entry) == deleted_set.end()) {
                    value_set.insert(entry.value);
                }
            }
        }

        return vector<int>(value_set.begin(), value_set.end());
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

    void compact_files() {
        // Read all entries
        vector<Entry> all_entries = read_all_entries(filename);
        vector<Entry> deleted_entries = read_all_entries(delete_filename);

        // Create set of deleted entries for fast lookup
        set<Entry> deleted_set(deleted_entries.begin(), deleted_entries.end());

        // Filter out deleted entries
        vector<Entry> live_entries;
        for (const auto& entry : all_entries) {
            if (deleted_set.find(entry) == deleted_set.end()) {
                live_entries.push_back(entry);
            }
        }

        // Sort live entries
        sort(live_entries.begin(), live_entries.end());

        // Remove duplicates
        auto last = unique(live_entries.begin(), live_entries.end());
        live_entries.erase(last, live_entries.end());

        // Write back compacted file
        ofstream file(filename, ios::binary | ios::trunc);
        for (const auto& entry : live_entries) {
            file.write(reinterpret_cast<const char*>(&entry), sizeof(Entry));
        }
        file.close();

        // Clear deletion file
        ofstream dfile(delete_filename, ios::binary | ios::trunc);
        dfile.close();
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