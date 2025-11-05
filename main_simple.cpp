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
    string filename;
    map<string, vector<int>> data;
    bool modified;

public:
    FileStorage(const string& fname) : filename(fname), modified(false) {
        load_data();
    }

    ~FileStorage() {
        if (modified) {
            save_data();
        }
    }

    void insert(const string& key, int value) {
        auto& values = data[key];
        if (find(values.begin(), values.end(), value) == values.end()) {
            values.push_back(value);
            modified = true;
        }
    }

    void remove(const string& key, int value) {
        auto it = data.find(key);
        if (it != data.end()) {
            auto& values = it->second;
            auto val_it = find(values.begin(), values.end(), value);
            if (val_it != values.end()) {
                values.erase(val_it);
                modified = true;
                if (values.empty()) {
                    data.erase(it);
                }
            }
        }
    }

    vector<int> find(const string& key) {
        auto it = data.find(key);
        if (it == data.end()) {
            return {};
        }

        vector<int> result = it->second;
        sort(result.begin(), result.end());
        return result;
    }

private:
    void load_data() {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            return;
        }

        while (file.peek() != EOF) {
            // Read key length
            uint8_t key_len;
            file.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));

            // Read key
            char key_buf[65];
            file.read(key_buf, key_len);
            key_buf[key_len] = '\0';
            string key(key_buf);

            // Read value count
            uint32_t value_count;
            file.read(reinterpret_cast<char*>(&value_count), sizeof(value_count));

            // Read values
            for (uint32_t i = 0; i < value_count; i++) {
                int value;
                file.read(reinterpret_cast<char*>(&value), sizeof(value));
                data[key].push_back(value);
            }
        }

        file.close();
    }

    void save_data() {
        ofstream file(filename, ios::binary | ios::trunc);

        for (const auto& pair : data) {
            const string& key = pair.first;
            const vector<int>& values = pair.second;

            // Write key length and key
            uint8_t key_len = key.length();
            file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
            file.write(key.c_str(), key_len);

            // Write value count
            uint32_t value_count = values.size();
            file.write(reinterpret_cast<const char*>(&value_count), sizeof(value_count));

            // Write values
            for (int value : values) {
                file.write(reinterpret_cast<const char*>(&value), sizeof(value));
            }
        }

        file.close();
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