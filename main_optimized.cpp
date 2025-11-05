#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <cstring>
#include <filesystem>

using namespace std;

class FileStorage {
private:
    string data_dir;

public:
    FileStorage(const string& dir) : data_dir(dir) {
        // Create data directory if it doesn't exist
        filesystem::create_directories(data_dir);
    }

    void insert(const string& key, int value) {
        string filename = data_dir + "/" + key + ".dat";

        // Read existing values
        set<int> values = read_values(filename);

        // Insert new value
        values.insert(value);

        // Write back to file
        write_values(filename, values);
    }

    void remove(const string& key, int value) {
        string filename = data_dir + "/" + key + ".dat";

        // Check if file exists
        if (!filesystem::exists(filename)) {
            return;
        }

        // Read existing values
        set<int> values = read_values(filename);

        // Remove value if exists
        values.erase(value);

        // Write back to file or delete if empty
        if (values.empty()) {
            filesystem::remove(filename);
        } else {
            write_values(filename, values);
        }
    }

    vector<int> find(const string& key) {
        string filename = data_dir + "/" + key + ".dat";

        // Check if file exists
        if (!filesystem::exists(filename)) {
            return {};
        }

        // Read values
        set<int> values = read_values(filename);
        return vector<int>(values.begin(), values.end());
    }

private:
    set<int> read_values(const string& filename) {
        set<int> values;

        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            return values;
        }

        int value;
        while (file.read(reinterpret_cast<char*>(&value), sizeof(int))) {
            values.insert(value);
        }

        file.close();
        return values;
    }

    void write_values(const string& filename, const set<int>& values) {
        ofstream file(filename, ios::binary | ios::trunc);

        for (int value : values) {
            file.write(reinterpret_cast<const char*>(&value), sizeof(int));
        }

        file.close();
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    FileStorage storage("data");

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