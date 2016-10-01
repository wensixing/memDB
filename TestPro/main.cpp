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
#include <cassert>

using namespace std;

class DataStorage {
public:
    virtual ~DataStorage() {}
    virtual bool get(const string &key, int &value) = 0; // false -- key doesn't exist
    virtual void set(const string &key, int value) = 0;
    virtual void unset(const string &key) = 0;
    virtual int  numberEqualTo(int value) = 0;
};

/* just a simple key-value database */
class DataSet : public DataStorage {
private:
    unordered_map<string, int> data;
    unordered_map<int, unordered_set<string>> cnt;
    void removeKeyFromCnt(string key) {
        int originVal = data[key];
        cnt[originVal].erase(key);
        if (cnt[originVal].size() == 0) {
            cnt.erase(originVal);
        }
    }
    bool ifContains(string key) {
        return data.find(key) != data.end();
    }
public:
    unordered_map<string, int> getData() {
        return data;
    }
    virtual bool get(const string &key, int &value) {
        if (data.find(key) == data.end()) {
            return false;
        }
        value = data[key];
        return true;
    }
    virtual void set(const string &key, int value) {
        if (ifContains(key) && data[key] == value) {
            return;
        }
        removeKeyFromCnt(key);
        data[key] = value;
        cnt[value].insert(key);
    }
    virtual void unset(const string &key) {
        if (!ifContains(key)) {
            return;
        }
        removeKeyFromCnt(key);
        data.erase(key);
    }
    virtual int numberEqualTo(int value) {
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
    
    // merge data in 'src' into 'dest' (update if exists, insert if not)
    static void merge(DataSet &dest, const DataSet &src) {
        for (auto itSet = src.data.begin(); itSet != src.data.end(); itSet ++) {
            dest.set(itSet->first, itSet->second);
        }
    }
    
};

class Transaction : public DataStorage {
private:
    typedef vector<vector<string>> TransactionLog;
    
    DataSet &ds; // DataSet to commit changes
    shared_ptr<DataSet>   transData;
    unordered_set<string> dataToBeUnset;
    vector<TransactionLog> transLogs;
    
    bool ifToBeUnset(const string &key) {
        return dataToBeUnset.find(key) != dataToBeUnset.end();
    }
    
    void addLog(vector<string> cmd) {
        transLogs.back().push_back(cmd);
    }
    
    void doSet(const string &key, int value) {
        transData->set(key, value);
        if (ifToBeUnset(key)) {
            dataToBeUnset.erase(key);
        }
    }
    
    void doUnset(const string &key) {
        transData->unset(key);
        if (!ifToBeUnset(key)) {
            dataToBeUnset.insert(key);
        }
    }
    
    void clear() {
        transData = make_shared<DataSet>();
        dataToBeUnset.clear();
        transLogs.clear();
        startNew();
    }
public:
    Transaction(DataSet &ds) : ds(ds) {
        clear();
    }
    
    virtual bool get(const string &key, int &value) {
        if (ifToBeUnset(key)) {
            return false;
        }
        if (transData->get(key, value)) {
            return true;
        }
        return ds.get(key, value);
    }
    
    virtual void set(const string &key, int value) {
        // add log
        int originValue;
        if (get(key, originValue)) {
            vector<string> log = {"SET", key, to_string(originValue)};
            addLog(log);
        } else {
            vector<string> log = {"UNSET", key};
            addLog(log);
        }
        
        doSet(key, value);
    }
    
    virtual void unset(const string &key) {
        // add log
        int originValue;
        if (get(key, originValue)) {
            vector<string> log = {"SET", key, to_string(originValue)};
            addLog(log);
        }
        
        doUnset(key);
    }
    
    virtual int numberEqualTo(int value) {
        unordered_map<string, int> trData = transData->getData();
        unordered_set<string> current = ds.getCntForValue(value);
        for (auto it = trData.begin(); it != trData.end(); it ++) {
            if (current.find(it->first) != current.end()) {
                current.erase(it->first);
            }
        }
        for (auto it = dataToBeUnset.begin(); it != dataToBeUnset.end(); it ++) {
            if (current.find(*it) != current.end()) {
                current.erase(*it);
            }
        }
        return int(current.size()) + transData->numberEqualTo(value);
    }
    
    void startNew() {
        TransactionLog newlog;
        transLogs.push_back(newlog);
    }
    
    void rollback() {
        TransactionLog &log = transLogs.back();
        
        while (log.size() > 0) {
            // executeSingleCmd(log.back());
            auto cmd = log.back();
            
            // TODO: DRY
            if (cmd[0] == "SET") {
                assert(cmd.size() == 3);
                doSet(cmd[1], stoi(cmd[2]));
            } else if (cmd[0] == "UNSET") {
                assert(cmd.size() == 2);
                doUnset(cmd[1]);
            }
            
            log.pop_back();
        }
        
        transLogs.pop_back();
    }
    
    void commit() {
        // update data
        DataSet::merge(ds, *transData);
        
        unordered_set<string>& unsetData     = dataToBeUnset;
        unordered_set<string>::iterator itUnset;
        for (itUnset = unsetData.begin(); itUnset != unsetData.end(); itUnset ++) {
            ds.unset(*itUnset);
        }
        
        clear();
    }
};

class SimpleDatabaseConsole {
private:
    shared_ptr<DataSet>     database;
    shared_ptr<Transaction> trans;
    DataStorage             *cur;
    
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
    int executeSingleCmd(vector<string> cmd) {
        if (cmd[0] == "SET" && cmd.size() == 3) {
            string key = cmd[1];
            int value = stoi(cmd[2]);
            cur->set(key, value);
        } else if (cmd[0] == "UNSET" && cmd.size() == 2) {
            string key = cmd[1];
            cur->unset(key);
        } else if (cmd[0] == "GET" && cmd.size() == 2) {
            string key = cmd[1];
            int value;
            cout << "> ";
            if (cur->get(key, value)) {
                cout << value;
            } else {
                cout << "NULL";
            }
            cout << endl;
        } else if (cmd[0] == "NUMEQUALTO" && cmd.size() == 2) {
            int value = stoi(cmd[1]);
            cout << "> " << cur->numberEqualTo(value) << endl;
        } else if (cmd[0] == "BEGIN" && cmd.size() == 1) {
            if (!trans) {
                trans = make_shared<Transaction>(*database);
                cur = trans.get();
            } else {
                trans->startNew();
            }
        } else if (cmd[0] == "ROLLBACK" && cmd.size() == 1) {
            if (trans) {
                trans->rollback();
            } else {
                cout << "> NO TRANSACTION" << endl;
            }
        } else if (cmd[0] == "COMMIT" && cmd.size() == 1) {
            if (trans) {
                trans->commit();
                trans = NULL;
                cur = database.get();
            } else {
                cout << "> NO TRANSACTION" << endl;
            }
        } else if (cmd[0] == "END" && cmd.size() == 1) {
            return 2;
        } else {
            return 0;
        }
        return 1;
    }
public:
    void init(shared_ptr<DataSet> db) {
        trans = NULL;
        database = db;
        cur = db.get();
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
    shared_ptr<DataSet> db = make_shared<DataSet>();
    shared_ptr<SimpleDatabaseConsole> simple = make_shared<SimpleDatabaseConsole>();
    simple->init(db);
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

