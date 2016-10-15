#include <iostream>
#include "SimpleDatabaseConsole.h"
using namespace std;
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

