#include <string.h>
void runDataAnalysis(
        const char* options, // options include the input file
        TString which_analysis = "make_TProfile2D"
          ){
    gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
    gSystem->Load("RunDataAnalysis");

    RunDataAnalysis* myAnalysis = new RunDataAnalysis(options);
    if ( which_analysis == "Loop") {
        myAnalysis->Loop();
    } else {
        cout << " Fatal error, which_analysis option \"" << which_analysis << "\" isn't recognized." << endl;
    }
}
