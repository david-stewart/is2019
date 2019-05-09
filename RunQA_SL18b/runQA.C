#include <TSystem>
#include <ctime>
#include <string>

class StMaker;
class StChain;
class StPicoDstMaker;

StChain *chain;
void runQA(
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
    gSystem->Load("RunQA");
    gSystem->Load("StRoot/macros/getRunBeginTime_C.so");
    gSystem->Load("StRoot/macros/getRunDuration_C.so");

    //-----------------------------------------
    //  output file
    //-----------------------------------------
    ofstream log;
    log.open(Form("%s.log",out_name));
    if (!log.is_open()) {
        cout << "Fatal error: could not open output file " << Form("%s.root",out_name) << endl;
        return kFatal;
    }

    //-----------------------------------------
    //  get run parameters of the file list
    //-----------------------------------------
    log << "Reading input picoDst list: " << picoDstList << endl;
    vector<int> runId;
    vector<int> runDuration;
    vector<string> runBeginTime;
    set<int> runId_have;

    const char* database = "mysql://db04.star.bnl.gov:3414/RunLog?timeout=60";
    const char* user = "djs232";
    const char* pass = "";
    TMySQLServer* mysql = TMySQLServer::Connect(database,user,pass);
    
    ifstream file;
    file.open(picoDstList);
    if (!file.is_open()){
        cout << "Fatal error: Could not open " << picoDstList << endl;
        log  << "Fatal error: Could not open " << picoDstList << endl;
        return kFatal;
    }
    string str;
    set<int> runId_have;
    while (getline(file,str)) {
        TString tstr = str;
        tstr = tstr(TPRegexp("_16\\d\\d\\d\\d\\d\\d_"));
        tstr = tstr(1,8);
        int id = tstr.Atoi();
        if (runId_have.count(id)) continue; // already have the information for this run
        runId_have.insert(id);
        runId.push_back(id); 
        int duration = getRunDuration(mysql, id);
        if (duration == -1) {
            cout << "Fatal error: could not get run duration for " << id << endl;
            log  << "Fatal error: could not get run duration for " << id << endl;
            return kFatal;
        }
        runDuration.push_back(duration);
        
        string beginTime = getRunBeginTime(mysql, id);
        if (beginTime == "-1") {
            cout << "Fatal error: could not get run duration for " << id << endl;
            log  << "Fatal error: could not get run duration for " << id << endl;
            return kFatal;
        }
        runBeginTime.push_back(beginTime);
        cout << " run: " << id << "  duration: " << duration << "  beginTime: " << beginTime << endl;
    }
    ifstream file;
    file.close();
    mysql->Close();
    log << "Finished run ids, times, and durations. Closing mysql database." << endl << endl;

    //-----------------------------------------
    // process the input file for xrootd files
    //-----------------------------------------
    // process in the input file to get rid of the run numbers (if present)
    // for when the input is like: 
    // /tmp/djs232/D96D05E45C742BAAF8F88D9560575DEE_0/INPUTFILES/st_ssdmb_16124017_raw_2500003.picoDst.root 8586
    // but the picoDst wants the line without the 8586 appended
    file.open(picoDstList);
    ofstream fout;
    fout.open("linted_picos.list");
    while(getline(file,str)){
        TString tstr = str;
        cout << "Line before: " << tstr << endl;
        TPRegexp("(\\S+) (\\d+)").Substitute(tstr,("$1"));
        fout << tstr << endl;
        cout << "Line after " << tstr << endl;
    };
    fout.close();

    //----------------------------------------
    // Get the bemc maker
    //----------------------------------------
    chain = new StChain();
    log << "Opening BEMC database." << endl;


    //-----------------------------------
    // Make chain and run main macro
    //-----------------------------------
    StPicoDstMaker *picoMaker =0X0; 
    StPicoDstMaker::PicoIoMode IoMode = 2;
    cout << "picoMaker0: " << picoMaker << endl;
    cout << "picoDstList: " << picoDstList << endl;
    picoMaker = new StPicoDstMaker(IoMode,"linted_picos.list","picoDst");
    cout << "picoMaker1: " << picoMaker << endl;

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

    RunQA *myrunQA = new RunQA(
        "pf",
        out_name,
        log,
        picoMaker,
        bad_run_list,
        bad_tow_list,
        starDb,
        runId,
        runDuration,
        runBeginTime
    );

    log << "chain->Init()" <<endl;
    chain->Init();

    int total = picoMaker->chain()->GetEntries();
    cout << " Total entries = " << total << endl;
    log  << "- Total events: = " << total << endl;
    int nEvents = nEv_input;
    if(nEvents>total||nEvents<0) nEvents = total;
    cout<<"Number of Events = "<<nEvents<<endl;
    log <<"- Number of events to read = "<<nEvents<<endl;

    for (Int_t i=0; i<nEvents; i++){   
        if(i%5000==0) {
            cout << "- Finished " << i << " events." << endl;
            log  << "- Finished " << i << " events." << endl;
        }
        chain->Clear();
        int iret = chain->Make(i);
        if (iret) { cout << "Bad return code!" << iret << endl; break;}
        total++;
    } 

    current_time = time(NULL);
    loc = localtime(&current_time);

	log << endl;	
	log << "----------------------" << endl;
	log << "|  Script complete!  |"<< endl;
	log << "----------------------" << endl;
	chain->Finish();

	delete chain;
}
