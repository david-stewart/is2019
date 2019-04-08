// Original form taken from N. Elsey in March 2019
// Heavily modified (and paired down) by D. Stewart (also March 2019)

class StMaker;
class StChain;
StChain *chain;

int trackEmbedding(int nEvents = -1,
             const char* muFileList = "events.list", // the minimc.list is in the same place
             std::string outname    = "short"
             )
{
    ofstream log;
    log.open((outname+".log").c_str(), std::ofstream::out | std::ofstream::app);
    cout << " Reading inputfile: " << muFileList << endl;
    cout << " Output location: " << outname << endl;

    log << " Reading inputfile: " << muFileList << endl;
    log << " Output location: " << outname << endl;

    gROOT->Macro("loadMuDst.C");
    gSystem->Load("StUtilities");
    gSystem->Load("StMiniMcEvent");
    gSystem->Load("StarClassLibrary");
    gSystem->Load("TrackEmbedding");
    gSystem->Load("StChain");

    chain = new StChain();
    StMuDstMaker* muDstMaker = new StMuDstMaker(0, 0, "",muFileList, "", 10000000);
    muDstMaker->SetStatus("*",0);
    muDstMaker->SetStatus("MuEvent",1);
    muDstMaker->SetStatus("PrimaryVertices",1);
    muDstMaker->SetStatus("StEventInfo",1) ;    // Turn on the primary track data
  
    // make StEfficiencyMaker
    std::ifstream file(muFileList);
    TChain* mcChain = new TChain("StMiniMcTree");

    string line;
    while (std::getline(file, line)) {
        cout << " Getline: " << line << endl;
        line.replace( line.find("MuDst"), 5, "minimc");
        mcChain->Add(line.c_str());
    }
    file.close();

    log  << " Starting: " << mcChain->GetEntries() << " events." << endl;
    cout << " Starting: " << mcChain->GetEntries() << " events." << endl;


    TrackEmbedding* embtree_mk = new TrackEmbedding(mcChain, muDstMaker, log, outname);
    /* cout << " These lines: " << muDstMaker->GetEntries() << endl; */

    /* StMemStat memory; */
    /* memory.PrintMem(NULL); */
	
    if (chain->Init()) { cout<<"StChain failed init: exiting"<<endl; return;}
    cout << "chain initialized" << endl;
	
    TStopwatch total;
    TStopwatch timer;
	
    int i=0;
    /* if (nEvents == -1) nEvents = chain->GetEntries(); */
    cout << " starting to work on " << nEvents << endl;
    /* for (int i{0}; i<nEvents; i++){ */
    while (true) {
      /* cout << " i: " << i << endl; */
      if ( i % 500 == 0 ) {
          cout <<"done with event "<<i;
          cout <<"\tcpu: " <<timer.CpuTime()
               <<"\treal: "<<timer.RealTime()<<"\tratio: "
               <<timer.CpuTime()/timer.RealTime();
          timer.Start();
          /* memory.PrintMem(NULL); */
        }
        chain->Clear();
        int iret = chain->Make(i);
        if (iret != kStOk) break;
        ++i;
  }
  chain->Finish();
  ostringstream msg;
  
  msg <<"my macro processed " << i << " events in " 
      << outname << ".root" << endl
      << "\tcpu: "   << total.CpuTime()  << endl
      << "\treal: "  << total.RealTime()  << endl
      << "\tratio: " << total.CpuTime() / total.RealTime() << endl
      << endl
      << "--------------" << endl
      << "|    Done    |" << endl
      << "--------------" << endl
      << endl;

  cout << msg.str();
  log  << msg.str();
  return 0;
}
