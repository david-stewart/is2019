#include <TSystem>
#include <ctime>
#include <string>

class StMaker;
class StChain;
class StPicoDstMaker;

StChain *chain;
void ea_metrics(
        const Int_t nEv_input=-1,
        const Char_t *picoDstList="test_files.list",
        const Char_t *out_name   ="test",
        const char* bad_tow_list ="bad_towers.list",
        const char* bad_run_list ="bad_runs.list",
        int debug = 0
){
    //------------------------------------------------------
    // Load required libraries
    //------------------------------------------------------
    gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
    loadSharedLibraries();
    gSystem->Load("StPicoEvent");
    gSystem->Load("StPicoDstMaker");
    gSystem->Load("StDbBroker");
    gSystem->Load("St_db_Maker.so");
    gSystem->Load("EA_metrics");
    gSystem->Load("StRoot/macros/getRunBeginTime_C.so");
    gSystem->Load("StRoot/macros/getRunDuration_C.so");

    //-----------------------------------------
    //  output file
    //-----------------------------------------
    cout << "c c c c c c c   0" << endl;
    ofstream log;
    log.open(Form("%s.log",out_name));
    if (!log.is_open()) {
        cout << "Fatal error: could not open output file " << Form("%s.root",out_name) << endl;
        return kFatal;
    }
    cout << "c c c c c c c   1" << endl;

    //-----------------------------------------
    //  get run parameters of the file list
    //-----------------------------------------
    log << "Reading input picoDst list: " << picoDstList << endl;
    const char* database = "mysql://db04.star.bnl.gov:3414/RunLog?timeout=60";
    const char* user = "djs232";
    const char* pass = "";
    TMySQLServer* mysql = TMySQLServer::Connect(database,user,pass);
    
    cout << "c c c c c c c   2" << endl;
    ifstream file;
    file.open(picoDstList);
    if (!file.is_open()){
        cout << "Fatal error: Could not open " << picoDstList << endl;
        log  << "Fatal error: Could not open " << picoDstList << endl;
        return kFatal;
    }
    cout << "c c c c c c c   3" << endl;
    //-----------------------------------------
    // process the input file for xrootd files
    //-----------------------------------------
    // process in the input file to get rid of the run numbers (if present)
    // for when the input is like: 
    // /tmp/djs232/D96D05E45C742BAAF8F88D9560575DEE_0/INPUTFILES/st_ssdmb_16124017_raw_2500003.picoDst.root 8586
    // but the picoDst wants the line without the 8586 appended
    cout << "c c c c c c c   4" << endl;
    string str;
    ofstream fout;
    fout.open("linted_picos.list");
    while(getline(file,str)){
        TString tstr = str;
        TPRegexp("(\\S+) (\\d+)").Substitute(tstr,("$1"));
        fout << tstr << endl;
        cout << "Line after " << tstr << endl;
    };
    cout << "c c c c c c c   5" << endl;
    fout.close();
    cout << "c c c c c c c   6" << endl;

    //----------------------------------------
    // Get the bemc maker
    //----------------------------------------
    chain = new StChain();
    log << "Opening BEMC database." << endl;
    cout << "c c c c c c c   7" << endl;


    //-----------------------------------
    // Make chain and run main macro
    //-----------------------------------
    StPicoDstMaker *picoMaker =0X0; 
    StPicoDstMaker::PicoIoMode IoMode = 2;
    cout << "picoMaker0: " << picoMaker << endl;
    cout << "picoDstList: " << picoDstList << endl;
    picoMaker = new StPicoDstMaker(IoMode,"linted_picos.list","picoDst");
    cout << "picoMaker1: " << picoMaker << endl;
    cout << "c c c c c c c   8" << endl;

    St_db_Maker* starDb = new St_db_Maker("db","MySQL:StarDb","$STAR/StarDb");
    starDb->SetFlavor("ofl");
    starDb->SetAttr("blacklist", "eemc");
    starDb->SetAttr("blacklist", "svt");
    starDb->SetAttr("blacklist", "tpc");
    starDb->SetAttr("blacklist", "tof");
    starDb->SetAttr("blacklist", "pxl");
    starDb->SetAttr("blacklist", "ist");
    starDb->SetAttr("blacklist", "bprs");
    starDb->SetAttr("blacklist", "smd");
    starDb->SetAttr("blacklist", "bsmd");
    cout << "c c c c c c c   9" << endl;

    EA_metrics *my_ea_metrics = new EA_metrics(
        "pf",
        out_name,
        log,
        picoMaker,
        bad_run_list,
        bad_tow_list,
        starDb
    );
    cout << "c c c c c c c   10" << endl;

    log << "chain->Init()" <<endl;
    chain->Init();

    int total = picoMaker->chain()->GetEntries();
    cout << " Total entries = " << total << endl;
    log  << "- Total events: = " << total << endl;
    int nEvents = nEv_input;
    if(nEvents>total||nEvents<0) nEvents = total;
    cout<<"Number of Events = "<<nEvents<<endl;
    log <<"- Number of events to read = "<<nEvents<<endl;

    cout << "c c c c c c c   11" << endl;

    total = 0;
    for (Int_t i=0; i<nEvents; i++){   
    /* cout << "c c c c c c c   12" << endl; */
        if(i%5000==0) {
            cout << "- Finished " << i << " events." << endl;
            log  << "- Finished " << i << " events." << endl;
        }
        chain->Clear();
        int iret = chain->Make(i);
        if (iret) { cout << "Bad return code!" << iret << endl; break;}
        total++;
    /* cout << "c c c c c c c   13" << endl; */
    } 

    cout << "c c c c c c c   14" << endl;
	log << endl;	
	log << "----------------------" << endl;
	log << "|  Script complete!  |"<< endl;
	log << "----------------------" << endl;
	chain->Finish();

	delete chain;
}
