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

        Entry entry;
        strncpy(entry.key, key.c_str(), 64);
        entry.key[64] = '\0';  // Ensure null termination
        entry.value = value;

        ofstream file(filename, ios::binary | ios::app);
        file.write(reinterpret_cast<char*>(&entry), sizeof(Entry));
        file.close();
    }

    void remove(const string& key, int value) {
        // Read all entries except the one to delete
        vector<Entry> entries;
        ifstream inFile(filename, ios::binary);

        Entry entry;
        while (inFile.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            if (strcmp(entry.key, key.c_str()) != 0 || entry.value != value) {
                entries.push_back(entry);
            }
        }
        inFile.close();

        // Rewrite file without the deleted entry
        ofstream outFile(filename, ios::binary | ios::trunc);
        for (const auto& e : entries) {
            outFile.write(reinterpret_cast<const char*>(&e), sizeof(Entry));
        }
        outFile.close();
    }

    vector<int> find(const string& key) {
        vector<int> values;
        ifstream file(filename, ios::binary);

        Entry entry;
        while (file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            if (strcmp(entry.key, key.c_str()) == 0) {
                values.push_back(entry.value);
            }
        }
        file.close();

        sort(values.begin(), values.end());
        return values;
    }

private:
    bool exists(const string& key, int value) {
        ifstream file(filename, ios::binary);

        Entry entry;
        while (file.read(reinterpret_cast<char*>(&entry), sizeof(Entry))) {
            if (strcmp(entry.key, key.c_str()) == 0 && entry.value == value) {
                file.close();
                return true;
            }
        }
        file.close();
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