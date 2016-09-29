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
#include <regex>

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


class TransactionData : public DataSet {
private:
    unordered_set<string> dataToBeUnset;
public:
    TransactionData() {
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
    unordered_set<string> getDataToBeUnset() {
        return dataToBeUnset;
    }
    bool ifToBeUnset(string key) {
        return dataToBeUnset.find(key) != dataToBeUnset.end();
    }
    void clear() {
        data.clear();
        cnt.clear();
        dataToBeUnset.clear();
    }
};

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
class TransactionSet {
private:
    vector<shared_ptr<Transaction>> trans;
    shared_ptr<TransactionData> transData;
    unordered_set<string> insertedFromDB;
public:
    TransactionSet() {
        transData = make_shared<TransactionData>();
    }
    shared_ptr<Transaction> getCur() {
        return trans.back();
    }
    shared_ptr<TransactionData> getData() {
        return transData;
    }
    bool ifInserted(string key) {
        return insertedFromDB.find(key) != insertedFromDB.end();
    }
    void markAsInserted(string key) {
        insertedFromDB.insert(key);
    }
    void startNew() {
        shared_ptr<Transaction> cur = make_shared<Transaction>();
        trans.push_back(cur);
    }
    void rollback() {
        trans.pop_back();
    }
    bool empty() {
        return trans.empty();
    }
    void clear() {
        trans.clear();
        insertedFromDB.clear();
        transData->clear();
    }
    void addTransactionLog(vector<string> cmd) {
        if (cmd[0] == "SET") {
            string key = cmd[1];
            if (transData->ifContains(key)) {
                int originValue = transData->get(key);
                vector<string> log = {"SET", key, to_string(originValue)};
                getCur()->addLog(log);
            } else {
                vector<string> log = {"UNSET", key};
                getCur()->addLog(log);
            }
        } else if (cmd[0] == "UNSET") {
            string key = cmd[1];
            if (transData->ifContains(key)) {
                int originValue = transData->get(key);
                vector<string> log = {"SET", key, to_string(originValue)};
                getCur()->addLog(log);
            }
        }
    }
};

class DatabaseData : public DataSet {
};
class SimpleDatabaseConsole {
private:
    shared_ptr<DatabaseData> database;
    shared_ptr<TransactionSet> trans;
    vector<string> spliteCmd(string cmd) {
        regex ws_re("\\s+"); // whitespace
        vector<string> res = {std::sregex_token_iterator(cmd.begin(), cmd.end(), ws_re, -1), {}};
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
        trans->startNew();
    }
    void commitTransaction() {
        if (trans->empty()) {
            cout << "> NO TRANSACTION" << endl;
            return;
        }
        unordered_map<string, int> setData  = trans->getData()->getData();
        unordered_set<string> unsetData     = trans->getData()->getDataToBeUnset();
        unordered_map<string, int>::iterator itSet;
        for (itSet = setData.begin(); itSet != setData.end(); itSet ++) {
            database->set(itSet->first, itSet->second);
        }
        unordered_set<string>::iterator itUnset;
        for (itUnset = unsetData.begin(); itUnset != unsetData.end(); itUnset ++) {
            database->unset(*itUnset);
        }
        trans->clear();
    }
    void rollbackTransaction() {
        if (trans->empty()) {
            cout << "> NO TRANSACTION" << endl;
            return;
        }
        vector<vector<string>> logs = trans->getCur()->getRecoverLogs();
        while (logs.size() > 0) {
            executeSingleCmd(logs.back());
            logs.pop_back();
        }
        trans->rollback();
    }
    int executeSingleCmd(vector<string> cmd) {
        if (cmd[0] == "SET") {
            if (cmd.size() != 3) {
                return 0;
            } else {
                string key = cmd[1];
                int value = stoi(cmd[2]);
                if (!trans->empty()) {
                    if (database->ifContains(key) && !trans->ifInserted(key)) {
                        trans->markAsInserted(key);
                        int cur = database->get(key);
                        trans->getData()->set(key, cur);
                    }
                    trans->addTransactionLog(cmd);
                    trans->getData()->set(key, value);
                } else {
                    database->set(key, value);
                }
            }
        } else if (cmd[0] == "UNSET") {
            if (cmd.size() != 2) {
                return 0;
            } else {
                string key = cmd[1];
                if (!trans->empty()) {
                    if (database->ifContains(key) && !trans->ifInserted(key)) {
                        trans->markAsInserted(key);
                        int cur = database->get(key);
                        trans->getData()->set(key, cur);
                    }
                    trans->addTransactionLog(cmd);
                    trans->getData()->unset(key);
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
                if (!trans->empty() && (trans->getData()->ifContains(key) || trans->getData()->ifToBeUnset(key))) {
                    if (trans->getData()->ifContains(key)) {
                        cout << trans->getData()->get(key);
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
                if (!trans->empty()) {
                    num = getDataCntSizeWithout(database->getCntForValue(value)) + trans->getData()->numberEqualTo(value);
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
        unordered_set<string> origin = current;
        for (it = origin.begin(); it != origin.end(); it ++) {
            if (trans->getData()->ifToBeUnset(*it) || trans->getData()->ifContains(*it)) {
                current.erase(*it);
            }
        }
        return int(current.size());
    }
public:
    void init(shared_ptr<DatabaseData> db, shared_ptr<TransactionSet> ts) {
        trans       = make_shared<TransactionSet>();
        database    = make_shared<DatabaseData>();
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
    shared_ptr<DatabaseData> db = make_shared<DatabaseData>();
    shared_ptr<TransactionSet> ts = make_shared<TransactionSet>();
    shared_ptr<SimpleDatabaseConsole> simple = make_shared<SimpleDatabaseConsole>();
    simple->init(db, ts);
    vector<string> res;
    while (true) {
        string cmd;
        getline(cin, cmd);
        if (simple->execute(cmd)) {
            break;
        }
    }
    return 0;
}

