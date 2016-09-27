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
#include <memory>

using namespace std;
class DataSet {
protected:
    unordered_map<string, int> data;
    unordered_map<int, unordered_set<string>> cnt;
    void removeKeyFromCnt(string key) {
        int originVal = data[key];
        cnt[originVal].erase(key);
        if (cnt[originVal].size() == 0) {
            cnt.erase(originVal);
        }
    }
public:
    unordered_map<string, int> getData() {
        return data;
    }
    unordered_map<int, unordered_set<string>> getCnt() {
        return cnt;
    }
    bool ifContains(string key) {
        return data.find(key) != data.end();
    }
    int get(string key) {
        return data[key];
    }
    void set(string key, int value) {
        if (ifContains(key) && data[key] == value) {
            return;
        }
        removeKeyFromCnt(key);
        data[key] = value;
        cnt[value].insert(key);
    }
    void unset(string key) {
        if (!ifContains(key)) {
            return;
        }
        removeKeyFromCnt(key);
        data.erase(key);
    }
    int numberEqualTo(int value) {
        int num = 0;
        if (cnt.find(value) != cnt.end()) {
            num = int(cnt[value].size());
        }
        return num;
    }
    unordered_set<string> getCntForValue(int value) {
        unordered_set<string> res;
        if (cnt.find(value) != cnt.end()) {
            res = cnt[value];
        }
        return res;
    }
};
class Transaction : public DataSet {
private:
    unordered_set<string> dataToBeUnset;
public:
    Transaction() {
    }
    void unset(string key) {
        DataSet::unset(key);
        if (!ifToBeUnset(key)) {
            dataToBeUnset.insert(key);
        }
    }
    void set(string key, int value) {
        DataSet::set(key, value);
        if (ifToBeUnset(key)) {
            dataToBeUnset.erase(key);
        }
    }
    void setTransactionData(shared_ptr<Transaction> tran) {
        data            = tran->getData();
        dataToBeUnset   = tran->getDataToBeUnset();
        cnt             = tran->getCnt();
    }
    unordered_set<string> getDataToBeUnset() {
        return dataToBeUnset;
    }
    bool ifToBeUnset(string key) {
        return dataToBeUnset.find(key) != dataToBeUnset.end();
    }
    int getCntSizeWithoutTransactionData(int value, unordered_set<string> data) {
        unordered_set<string>::iterator it;
        for (it = data.begin(); it != data.end(); it ++) {
            if (ifToBeUnset(*it) || ifContains(*it)) {
                data.erase(it);
            }
        }
        return int(data.size());
    }
};
class Database : public DataSet {
};
class SimpleDatabaseConsole {
private:
    vector<shared_ptr<Transaction>> trans;
    shared_ptr<Transaction> cur;
    shared_ptr<Database> database;
    SimpleDatabaseConsole() {
        cur = nullptr;
        database = make_shared<Database>();
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
        if (cur == nullptr) {
            cur = make_shared<Transaction>();
            return;
        }
        trans.push_back(cur);
        cur = make_shared<Transaction>();
        cur->setTransactionData(trans.back());
    }
    void commitTransaction() {
        if (cur == nullptr) {
            cout << "> NO TRANSACTION" << endl;
            return;
        }
        unordered_map<string, int> setData  = cur->getData();
        unordered_set<string> unsetData     = cur->getDataToBeUnset();
        unordered_map<string, int>::iterator itSet;
        for (itSet = setData.begin(); itSet != setData.end(); itSet ++) {
            database->set(itSet->first, itSet->second);
        }
        unordered_set<string>::iterator itUnset;
        for (itUnset = unsetData.begin(); itUnset != unsetData.end(); itUnset ++) {
            database->unset(*itUnset);
        }
        cur = nullptr;
        trans.clear();
    }
    void rollbackTransaction() {
        if (cur == nullptr) {
            cout << "> NO TRANSACTION" << endl;
            return;
        }
        if (trans.size() > 0) {
            cur = trans.back();
            trans.pop_back();
        } else {
            cur = nullptr;
        }
    }
    int executeSingleCmd(vector<string> cmd) {
        if (cmd[0] == "SET") {
            if (cmd.size() != 3) {
                return 0;
            } else {
                string key = cmd[1];
                int value = stoi(cmd[2]);
                if (cur != nullptr) {
                    cur->set(key, value);
                } else {
                    database->set(key, value);
                }
            }
        } else if (cmd[0] == "UNSET") {
            if (cmd.size() != 2) {
                return 0;
            } else {
                string key = cmd[1];
                if (cur != nullptr) {
                    cur->unset(key);
                } else {
                    database->unset(key);
                }
            }
        } else if (cmd[0] == "GET") {
            if (cmd.size() != 2) {
                return 0;
            } else {
                string key = cmd[1];
                cout << "> ";
                if (cur != nullptr && (cur->ifContains(key) || cur->ifToBeUnset(key))) {
                    if (cur->ifContains(key)) {
                        cout << cur->get(key);
                    } else {
                        cout << "NULL";
                    }
                } else {
                    if (database->ifContains(key)) {
                        cout << database->get(key);
                    } else {
                        cout << "NULL";
                    }
                }
                cout << endl;
            }
        } else if (cmd[0] == "NUMEQUALTO") {
            if (cmd.size() != 2) {
                return 0;
            } else {
                int value = stoi(cmd[1]);
                int num = 0;
                if (cur != nullptr) {
                    num = getDataCntSizeWithout(database->getCntForValue(value)) + cur->numberEqualTo(value);
                } else {
                    num = database->numberEqualTo(value);
                }
                cout << "> " << num << endl;
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
    int getDataCntSizeWithout(unordered_set<string> current) {
        unordered_set<string>::iterator it;
        for (it = current.begin(); it != current.end(); it ++) {
            if (cur->ifToBeUnset(*it) || cur->ifContains(*it)) {
                current.erase(it);
            }
        }
        return int(current.size());
    }
public:
    static SimpleDatabaseConsole* getInstance() {
        static SimpleDatabaseConsole* sdInstance;
        if (sdInstance == nullptr) {
            sdInstance = new SimpleDatabaseConsole();
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
    SimpleDatabaseConsole* inst = SimpleDatabaseConsole::getInstance();
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

