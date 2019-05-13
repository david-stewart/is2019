#include <string.h>
void treeRootAnalysis(
        const char* options,
        TString which_analysis = "make_TProfile2D"
          ){
    gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
    gSystem->Load("TreeRootsAnalysis");

    TreeRootsAnalysis* myAnalysis = new TreeRootsAnalysis(options);
    if ( which_analysis == "make_TProfile2D") {
        myAnalysis->make_TProfile2D();
    } else {
        cout << " Fatal error, which_analysis option \"" << which_analysis << "\" isn't recognized." << endl;
    }
}
