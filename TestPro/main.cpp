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
class Transaction {
private:
    unordered_map<string, int> dataToBeSet;
    unordered_set<string> dataToBeUnset;
    unordered_map<int, unordered_set<string>> cnt;
    void removekeyFromCnt(string key) {
        int originVal = dataToBeSet[key];
        cnt[originVal].erase(key);
        if (cnt[originVal].size() == 0) {
            cnt.erase(originVal);
        }
    }
public:
    Transaction() {
    }
    void setTransactionData(shared_ptr<Transaction> tran) {
        dataToBeSet = tran->getDataToBeSet();
        dataToBeUnset = tran->getDataToBeUnset();
        cnt = tran->getCnt();
    }
    unordered_map<string, int> getDataToBeSet() {
        return dataToBeSet;
    }
    unordered_set<string> getDataToBeUnset() {
        return dataToBeUnset;
    }
    unordered_map<int, unordered_set<string>> getCnt() {
        return cnt;
    }
    bool ifToBeSet(string key) {
        return dataToBeSet.find(key) != dataToBeSet.end();
    }
    bool ifToBeUnset(string key) {
        return dataToBeUnset.find(key) != dataToBeUnset.end();
    }
    void get(string key) {
        if (ifToBeSet(key)) {
            cout << "> " << dataToBeSet[key] << endl;
        } else if (ifToBeUnset(key)) {
            cout << "> NULL" << endl;
        }
    }
    void set(string key, int value) {
        if (dataToBeSet.find(key) != dataToBeSet.end()) {
            if (dataToBeSet[key] == value) {
                return;
            } else {
                removekeyFromCnt(key);
            }
        }
        if (dataToBeUnset.find(key) != dataToBeUnset.end()) {
            dataToBeUnset.erase(key);
        }
        dataToBeSet[key] = value;
        cnt[value].insert(key);
    }
    void unset(string key) {
        if (ifToBeUnset(key)) {
            return;
        }
        if (ifToBeSet(key)) {
            removekeyFromCnt(key);
            dataToBeSet.erase(key);
        }
        dataToBeUnset.insert(key);
    }
    void numberEqualTo(int value, unordered_set<string> data) {
        unordered_set<string>::iterator it;
        for (it = data.begin(); it != data.end(); it ++) {
            if (dataToBeUnset.find(*it) != dataToBeUnset.end() || dataToBeSet.find(*it) != dataToBeSet.end()) {
                data.erase(it);
            }
        }
        int len = int(data.size());
        if (cnt.find(value) != cnt.end()) {
            len += cnt.size();
        }
        cout << "> " << len << endl;
    }
};
class SimpleDatabase {
private:
    unordered_map<string, int> data;
    unordered_map<int, unordered_set<string>> cnt;
    vector<shared_ptr<Transaction>> trans;
    shared_ptr<Transaction> cur;
    SimpleDatabase() {
        cur = nullptr;
    }
    void removeKeyFromCnt(string key) {
        int originValue = data[key];
        cnt[originValue].erase(key);
        if (cnt[originValue].size() == 0) {
            cnt.erase(originValue);
        }
    }
    void set(string key, int value) {
        if (data.find(key) != data.end() && data[key] == value) {
            return;
        }
        removeKeyFromCnt(key);
        data[key] = value;
        cnt[value].insert(key);
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
        int num = 0;
        if (cnt.find(value) != cnt.end()) {
            num = int(cnt[value].size());
        }
        cout << "> " << num << endl;
    }
    void unset(string key) {
        if (data.find(key) == data.end()) {
            return;
        }
        removeKeyFromCnt(key);
        data.erase(key);
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
        unordered_map<string, int> setData = cur->getDataToBeSet();
        unordered_set<string> unsetData = cur->getDataToBeUnset();
        unordered_map<string, int>::iterator itSet;
        for (itSet = setData.begin(); itSet != setData.end(); itSet ++) {
            set(itSet->first, itSet->second);
        }
        unordered_set<string>::iterator itUnset;
        for (itUnset = unsetData.begin(); itUnset != unsetData.end(); itUnset ++) {
            unset(*itUnset);
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
                    set(key, value);
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
                    unset(key);
                }
            }
        } else if (cmd[0] == "GET") {
            if (cmd.size() != 2) {
                return 0;
            } else {
                string key = cmd[1];
                if (cur != nullptr && (cur->ifToBeSet(key) || cur->ifToBeUnset(key))) {
                    cur->get(key);
                } else {
                    get(key);
                }
            }
        } else if (cmd[0] == "NUMEQUALTO") {
            if (cmd.size() != 2) {
                return 0;
            } else {
                int value = stoi(cmd[1]);
                if (cur != nullptr) {
                    unordered_set<string> data;
                    if (cnt.find(value) != cnt.end()) {
                        data = cnt[value];
                    }
                    cur->numberEqualTo(value, data);
                } else {
                    numberEqualTo(value);
                }
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

