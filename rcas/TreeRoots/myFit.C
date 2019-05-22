void myFit(
        const char* infile,
        const char* outfile,
        Long64_t nentries = -1
            ){
    gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
    gSystem->Load("MyFit");
    MyFit* prog = new MyFit(infile,outfile,nentries);
    prog->Loop();
}
