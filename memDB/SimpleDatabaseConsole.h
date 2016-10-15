//
//  SimpleDatabaseConsole.hpp
//  memDB
//
//  Created by Sixing Wen on 10/15/16.
//  Copyright © 2016 Sixing Wen. All rights reserved.
//

#ifndef SimpleDatabaseConsole_h
#define SimpleDatabaseConsole_h

#include "Transaction.h"
using namespace std;
class SimpleDatabaseConsole {
private:
    shared_ptr<DataSet>     database;
    shared_ptr<Transaction> trans;
    DataStorage             *cur;
    
    vector<string> spliteCmd(string cmd);
    void showUsage();
    void inValidCmd(string s);
    int executeSingleCmd(vector<string> cmd);
public:
    void init(shared_ptr<DataSet> db);
    bool execute(string input);
};

#endif /* SimpleDatabaseConsole_hpp */
