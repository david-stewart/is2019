#include "StatusUpdate.h"
#include <sstream>
#include <fstream>
#include <iostream>
using namespace std;


StatusUpdate::StatusUpdate(const char* _in_file, int* _runId) : runId(_runId) {
    ifstream in_file;
    in_file.open(_in_file);
    if (!in_file.is_open()) {
        cout << "Fatal error, failed to open " << _in_file << endl;
        exit(2);
    }

    string line;
    string word;
    while (getline(in_file, line)) {
        cout << "line: " << line << endl;
        stringstream words{line};
        words >> word;
        if (word == "#") { 
            continue;
        } else if (word == "firstRun") {
            int i;
            while (words >> i) runId_start.push_back(i);
        } else if (word == "lastRun") {
            int i;
            while (words >> i) runId_end.push_back(i);
            break;
        }
    }
    while (getline(in_file,line)) {
        stringstream words{line};
        int towId;
        words >> towId;
        towStat[towId] = vector<bool>();
        int i;
        while (words >> i) towStat[towId].push_back(i == 1);
    }
    array_size = runId_start.end() - runId_start.begin();
    
};

bool StatusUpdate::is_good(int towId) {
    /* for (auto pair : towStat) { cout << pair.first << " " << pair.second[0] << endl; } */
    if (towStat.count(towId) == 0) return true;
    // see if the runId is >= any of the values in runId_start
    ptrdiff_t pos = std::lower_bound(runId_start.begin(), runId_start.end(), *runId)-runId_start.begin();
    // if not return false
    if (pos == array_size) return false;
    // if so, see if the value is <= the value in runId_end;
    if (*runId <= runId_end[pos]) {
        /* cout << "yes" << endl; */
        return towStat[towId][pos];
    } else {
        /* cout << "no" << endl; */
        return false;
    }
};
