#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <map>

using namespace std;

class FileStorage {
private:
    string data_file;
    string index_file;

public:
    FileStorage(const string& base_name) :
        data_file(base_name + ".dat"),
        index_file(base_name + ".idx") {
        // Create files if they don't exist
        ofstream dfile(data_file, ios::binary | ios::app);
        dfile.close();
        ofstream ifile(index_file, ios::binary | ios::app);
        ifile.close();
    }

    void insert(const string& key, int value) {
        // Check if entry already exists
        if (exists(key, value)) {
            return;
        }

        // Append to data file
        ofstream data(data_file, ios::binary | ios::app);

        // Write key length and key
        uint8_t key_len = key.length();
        data.write(reinterpret_cast<char*>(&key_len), sizeof(key_len));
        data.write(key.c_str(), key_len);

        // Write value
        data.write(reinterpret_cast<char*>(&value), sizeof(value));
        data.close();

        // Update index
        update_index();
    }

    void remove(const string& key, int value) {
        // Mark as deleted in index
        map<string, vector<int>> index = read_index();

        auto it = index.find(key);
        if (it != index.end()) {
            auto& values = it->second;
            auto val_it = find(values.begin(), values.end(), value);
            if (val_it != values.end()) {
                values.erase(val_it);
                if (values.empty()) {
                    index.erase(it);
                }
                write_index(index);
            }
        }
    }

    vector<int> find(const string& key) {
        map<string, vector<int>> index = read_index();
        auto it = index.find(key);
        if (it == index.end()) {
            return {};
        }

        vector<int> result = it->second;
        sort(result.begin(), result.end());
        return result;
    }

private:
    bool exists(const string& key, int value) {
        map<string, vector<int>> index = read_index();
        auto it = index.find(key);
        if (it == index.end()) {
            return false;
        }

        const auto& values = it->second;
        return find(values.begin(), values.end(), value) != values.end();
    }

    void update_index() {
        // Rebuild index from data file
        map<string, vector<int>> index;

        ifstream data(data_file, ios::binary);
        if (!data.is_open()) {
            return;
        }

        while (data.peek() != EOF) {
            // Read key length
            uint8_t key_len;
            data.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));

            // Read key
            char key_buf[65];
            data.read(key_buf, key_len);
            key_buf[key_len] = '\0';
            string key(key_buf);

            // Read value
            int value;
            data.read(reinterpret_cast<char*>(&value), sizeof(value));

            // Add to index
            index[key].push_back(value);
        }

        data.close();
        write_index(index);
    }

    map<string, vector<int>> read_index() {
        map<string, vector<int>> index;

        ifstream idx(index_file, ios::binary);
        if (!idx.is_open()) {
            return index;
        }

        while (idx.peek() != EOF) {
            // Read key length
            uint8_t key_len;
            idx.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));

            // Read key
            char key_buf[65];
            idx.read(key_buf, key_len);
            key_buf[key_len] = '\0';
            string key(key_buf);

            // Read value count
            uint32_t value_count;
            idx.read(reinterpret_cast<char*>(&value_count), sizeof(value_count));

            // Read values
            vector<int> values(value_count);
            for (uint32_t i = 0; i < value_count; i++) {
                int value;
                idx.read(reinterpret_cast<char*>(&value), sizeof(value));
                values[i] = value;
            }

            index[key] = values;
        }

        idx.close();
        return index;
    }

    void write_index(const map<string, vector<int>>& index) {
        ofstream idx(index_file, ios::binary | ios::trunc);

        for (const auto& pair : index) {
            const string& key = pair.first;
            const vector<int>& values = pair.second;

            // Write key length and key
            uint8_t key_len = key.length();
            idx.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
            idx.write(key.c_str(), key_len);

            // Write value count
            uint32_t value_count = values.size();
            idx.write(reinterpret_cast<const char*>(&value_count), sizeof(value_count));

            // Write values
            for (int value : values) {
                idx.write(reinterpret_cast<const char*>(&value), sizeof(value));
            }
        }

        idx.close();
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