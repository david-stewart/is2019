/* #include <iostream> */
#include "TSystem.h"
#include "run_map.h"

#include <run_map.h>
using namespace std;


void printit(){
    cout << "Something else!" << endl;
};

void test() {
    /* gSystem->Load("run_map.so"); */
    /* gSystem->Include("/star/u/djs232/software/RooUnfold/libRooUnfold.so"); */

    cout << "This is it!" << endl;
    printit();
    TripleFloat a;
    a.p0 = 1;
    map<int,TripleFloat> mymap = get_runmap();
}
