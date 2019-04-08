#include "TString.h"
#include "TMySQLResult.h"
#include "TMySQLServer.h"
#include "TMySQLRow.h"
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

int getRunDuration(TMySQLServer* mysql, int runId) {
    TString query;
    TMySQLResult* result;
    query = Form("select distinct startRunTime from `RunLog`.`runDescriptor` where runNumber = %d",runId);
    result = (TMySQLResult*)mysql->Query(query);

    int i_begin;
    int i_end;
    string str;
    if (result) {
        TMySQLRow* row;
        int n_results = 0;
        while (row = (TMySQLRow*)result->Next()) {
            ++n_results;
            if (n_results>1) {
                cout << "Warning, more than one start time for " << runId << endl;
                return -1;
            }
            istringstream begin;
            begin.str(Form("%s", row->GetField(0)));
            begin >> i_begin;
            delete row;
        }
        /* cout << "n_results " << n_results << endl; */
        result->Close();
    } else {
        cout << "Warning, no start time for " << runId << endl;
        return -1;
    }

    query = Form("select distinct endRunTime from `RunLog`.`runDescriptor` where runNumber = %d",runId);
    result = (TMySQLResult*)mysql->Query(query);
    if (result) {
        TMySQLRow* row;
        int n_results = 0;
        while (row = (TMySQLRow*)result->Next()) {
            ++n_results;
            if (n_results>1) {
                cout << "Warning, more than one start time for " << runId << endl;
                return -1;
            }
            istringstream end;
            end.str(Form("%s", row->GetField(0)));
            end >> i_end;
            delete row;
        }
        /* cout << "n_results " << n_results << endl; */
        result->Close();
    } else {
        cout << "Warning, no start time for " << runId << endl;
        return -1;
    }

    /* cout << "begin: " << i_begin << " end: " << i_end << endl; */

    ostringstream os;
    return i_end-i_begin;
};
