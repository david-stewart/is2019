#include "TString.h"
#include "TMySQLResult.h"
#include "TMySQLServer.h"
#include "TMySQLRow.h"
#include <string>
#include <iostream>

using namespace std;

string getRunBeginTime(TMySQLServer* mysql, int runId) {
    TString query;
    TMySQLResult* result;
    query = Form("select distinct beginTime from `RunLog`.`runDescriptor` where runNumber = %d",runId);
    result = (TMySQLResult*)mysql->Query(query);

    string str;
    if (result) {
        TMySQLRow* row;
        int n_results = 0;
        while (row = (TMySQLRow*)result->Next()) {
            ++n_results;
            if (n_results>1) {
                cout << "Warning, more than one start time for " << runId << endl;
                return "-1";
            }
            str = Form("%s", row->GetField(0));
            delete row;
        }
        /* cout << "n_results " << n_results << endl; */
        result->Close();
    } else {
        cout << "Warning, no start time for " << runId << endl;
        return "-1";
    }
    return str;
};
