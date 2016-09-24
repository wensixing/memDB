#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using namespace std;
class Transaction {
private:
    vector<vector<string>> recoverLogs;
public:
    Transaction() {
    }
    void addLog(vector<string> cmd) {
        recoverLogs.push_back(cmd);
    }
    vector<vector<string>> getRecoverLogs() {
        return recoverLogs;
    }
};
class SimpleDatabase {
private:
    unordered_map<string, int> data;
    unordered_map<int, unordered_set<string>> dataKey;
    vector<shared_ptr<Transaction>> trans;
    shared_ptr<Transaction> cur;
    SimpleDatabase() {
        cur = nullptr;
    }
    void set(string key, int value) {
        if (data.find(key) != data.end() && data[key] == value) {
            return;
        }
        int originValue = data[key];
        dataKey[originValue].erase(key);
        data[key] = value;
        dataKey[value].insert(key);
    }
    void get(string key) {
        if (data.find(key) == data.end()) {
            cout << "> NULL";
        } else {
            cout << "> " << data[key];
        }
        cout << endl;
    }
    void numberEqualTo(int value) {
        if (dataKey.find(value) == dataKey.end()) {
            cout << "> 0";
        } else {
            cout << "> " <<int(dataKey[value].size());
        }
        cout << endl;
    }
    void unset(string key) {
        if (data.find(key) == data.end()) {
            return;
        }
        int value = data[key];
        data.erase(key);
        dataKey[value].erase(key);
        if (dataKey[value].size() == 0) {
            dataKey.erase(value);
        }
    }
    vector<string> spliteCmd(string cmd) {
        vector<string> res;
        int start = 0;
        // remove head spaces
        for (int i = 0; i < cmd.length(); i ++) {
            if (cmd[i] != ' ') {
                start = i;
                break;
            }
        }
        string cur;
        for (int i = start; i < cmd.length(); i ++) {
            if (cmd[i] == ' ') {
                if (cur.length() > 0) {
                    res.push_back(cur);
                    cur.clear();
                }
            } else {
                cur += cmd[i];
            }
            if (i == cmd.length() - 1 && cur.length() > 0) {
                res.push_back(cur);
            }
        }
        return res;
    }
    void showUsage() {
        cout << ">          Simple DataBase Manual" << endl;
        cout << "> SET <name> <value> - Set the variable <name> to the <value>" << endl;
        cout << "> GET <name>         - Print out the value of variable <name>" << endl;
        cout << "> UNSET <name>       - Unset the variable <name>" << endl;
        cout << "> NUMEQUALTO <value> - Print out the number of variables that are currently to <value>" << endl;
        cout << "> END                - exit the program" << endl;
    }
    void inValidCmd(string s) {
        cout << "> Invalid command !" << endl;
        cout << "> " << s << endl;
        showUsage();
    }
    void startTransaction() {
        if (cur != nullptr) {
            trans.push_back(cur);
        }
        cur = make_shared<Transaction>();
    }
    void commitTransaction() {
        if (cur == nullptr) {
            cout << "> NO TRANSACTION" << endl;
            return;
        }
        cur = nullptr;
        trans.clear();
    }
    void rollbackTransaction() {
        if (cur == nullptr) {
            cout << "> NO TRANSACTION" << endl;
            return;
        }
        vector<vector<string>> logs = cur->getRecoverLogs();
        while (logs.size() > 0) {
            executeSingleCmd(logs.back());
            logs.pop_back();
        }
        if (trans.size() > 0) {
            cur = trans.back();
            trans.pop_back();
        } else {
            cur = nullptr;
        }
    }
    void addTransactionLog(vector<string> cmd) {
        if (cmd[0] == "SET") {
            string key = cmd[1];
            if (data.find(key) == data.end()) {
                vector<string> log = {"UNSET", key};
                cur->addLog(log);
            } else {
                int originValue = data[key];
                vector<string> log = {"SET", key, to_string(originValue)};
                cur->addLog(log);
            }
        } else if (cmd[0] == "UNSET") {
            string key = cmd[1];
            if (data.find(key) != data.end()) {
                int originValue = data[key];
                vector<string> log = {"SET", key, to_string(originValue)};
                cur->addLog(log);
            }
        }
    }
    int executeSingleCmd(vector<string> cmd, bool executeLog = false) {
        if (cmd[0] == "SET") {
            if (cmd.size() != 3) {
                return 0;
            } else {
                if (cur != nullptr && !executeLog) {
                    addTransactionLog(cmd);
                }
                string key = cmd[1];
                int value = stoi(cmd[2]);
                set(key, value);
            }
        } else if (cmd[0] == "UNSET") {
            if (cmd.size() != 2) {
                return 0;
            } else {
                if (cur != nullptr && !executeLog) {
                    addTransactionLog(cmd);
                }
                string key = cmd[1];
                unset(key);
            }
        } else if (cmd[0] == "GET") {
            if (cmd.size() != 2) {
                return 0;
            } else {
                string key = cmd[1];
                get(key);
            }
        } else if (cmd[0] == "NUMEQUALTO") {
            if (cmd.size() != 2) {
                return 0;
            } else {
                int value = stoi(cmd[1]);
                numberEqualTo(value);
            }
        } else if (cmd[0] == "BEGIN") {
            startTransaction();
        } else if (cmd[0] == "ROLLBACK") {
            rollbackTransaction();
        } else if (cmd[0] == "COMMIT") {
            commitTransaction();
        } else if (cmd[0] == "END") {
            return 2;
        } else {
            return 0;
        }
        return 1;
    }
public:
    static SimpleDatabase* getInstance() {
        static SimpleDatabase* sdInstance;
        if (sdInstance == nullptr) {
            sdInstance = new SimpleDatabase();
        }
        return sdInstance;
    }
    bool execute(string input) {
        vector<string> cmd = spliteCmd(input);
        if (cmd.size() == 0) {
            inValidCmd(input);
            return false;
        }
        int res = executeSingleCmd(cmd);
        if (res == 0) {
            inValidCmd(input);
            return false;
        }
        return (res == 2);
    }
};
int main(int argc, const char * argv[]) {
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    SimpleDatabase* inst = SimpleDatabase::getInstance();
    vector<string> res;
    while (true) {
        string cmd;
        getline(cin, cmd);
        if (inst->execute(cmd)) {
            break;
        }
    }
    delete inst;
    return 0;
}

