#include <TSystem>

class StMaker;
class StChain;
class StPicoDstMaker;

StChain *chain;
void peds(
        const Int_t nEv=-1,
        const Char_t *inputFile="/star/data94/reco/production_pAu200_2015/ReversedFullField/P18ih/2015/141/16141035/st_ssdmb_adc_16141035_raw_2000009.picoDst.root",
        const Char_t *outputFile="PEDs",
        const Char_t *badTowerList=
          "/star/u/djs232/pAu-2015/picoQA/list/2019.02.21_BadTowerStatus_update.list",
        const Char_t *_index_runId=
          "/direct/star+u/djs232/pAu-2015/picoQA/list/runId_P18id.list")
{
    gROOT->Macro("loadMuDst.C");
    gSystem->Load("StDbBroker");
    gSystem->Load("St_db_Maker.so");
    gSystem->Load("StEmcUtil");
    gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
    loadSharedLibraries();
    gSystem->Load("StPicoEvent");
    gSystem->Load("StPicoDstMaker");
    gSystem->Load("PEDs");
    gSystem->Load("St_base");
    gSystem->Load("StChain");
    gSystem->Load("StUtilities");
    gSystem->Load("StIOMaker");
    gSystem->Load("StarClassLibrary");

    //Load all the System libraries
    int nEvents=nEv;

    chain = new StChain();
    cout<<"chain->Init();"<<endl;

    StPicoDstMaker *picoMaker =0X0; 
    StPicoDstMaker::PicoIoMode IoMode = 2;
    cout<<"Input file is"<<inputFile<<endl;

    // make the picoDstMaker
    picoMaker = new StPicoDstMaker(IoMode,inputFile,"picoDst");

    // -----------------------------------
    // Make the database for making calls
    // -----------------------------------

    // continually set the time inside of the executable
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

    // -------------------------------------
    // run my own local analysis
    // -------------------------------------
    PEDs *pfMaker = new PEDs( "pf", picoMaker, outputFile, badTowerList, 
         starDb, _index_runId,
         "/star/u/djs232/pAu-2015/picoQA/list/BadTowers_Comb.08.02.list" 
    );

    chain->Init();
    int total = picoMaker->chain()->GetEntries();
    cout << " Total entries = " << total << endl;
    if(nEvents>total||nEvents<0) nEvents = total;
    cout<<"Number of Events = "<<nEvents<<endl;

    for (Int_t i=0; i<nEvents; i++){   
        if(i%5000==0) cout << "Working on eventNumber " << i << endl;
        chain->Clear();
        int iret = chain->Make(i);
        /* cout << " chain time " << chain->GetDateTime() << endl; */
        /* cout << " pico  time " << chain->GetDateTime() << endl; */
        
        if (iret) { cout << "Bad return code!" << iret << endl; break;}
        total++;
    } 

    cout << endl;	
    cout << "****************************************** " << endl;
    cout << "Work done... now its time to close up shop!"<< endl;
    cout << "****************************************** " << endl;
    chain->Finish();
    cout << "****************************************** " << endl;

    delete chain;
}
