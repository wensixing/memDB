//
//  SimpleDatabaseConsole.cpp
//  memDB
//
//  Created by Sixing Wen on 10/15/16.
//  Copyright © 2016 Sixing Wen. All rights reserved.
//

#include "SimpleDatabaseConsole.h"
#include <iostream>
#include <regex>
using namespace std;

vector<string> SimpleDatabaseConsole::spliteCmd(string cmd) {
    regex ws_re("\\s+"); // whitespace
    vector<string> res = {std::sregex_token_iterator(cmd.begin(), cmd.end(), ws_re, -1), {}};
    return res;
}
void SimpleDatabaseConsole::showUsage() {
    cout << ">          Simple DataBase Manual" << endl;
    cout << "> SET <name> <value> - Set the variable <name> to the <value>" << endl;
    cout << "> GET <name>         - Print out the value of variable <name>" << endl;
    cout << "> UNSET <name>       - Unset the variable <name>" << endl;
    cout << "> NUMEQUALTO <value> - Print out the number of variables that are currently to <value>" << endl;
    cout << "> END                - exit the program" << endl;
}
void SimpleDatabaseConsole::inValidCmd(string s) {
    cout << "> Invalid command !" << endl;
    cout << "> " << s << endl;
    showUsage();
}
int SimpleDatabaseConsole::executeSingleCmd(vector<string> cmd) {
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
void SimpleDatabaseConsole::init(shared_ptr<DataSet> db) {
    trans = NULL;
    database = db;
    cur = db.get();
}
bool SimpleDatabaseConsole::execute(string input) {
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
