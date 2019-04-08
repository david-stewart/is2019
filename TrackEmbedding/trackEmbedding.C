// Original form taken from N. Elsey in March 2019
// Heavily modified (and paired down) by D. Stewart (also March 2019)

class StMaker;
class StChain;
StChain *chain;

int trackEmbedding(int nEvents = -1,
             const char* muFileList = "loc_list.list", // the minimc.list is in the same place
             std::string outname    = "short"
             )
{
    cout << "z.z.z.z z0"<<endl;
    ofstream log;
    cout << "z.z.z.z z1"<<endl;
    log.open((outname+".log").c_str(), std::ofstream::out | std::ofstream::app);
    cout << "z.z.z.z z2"<<endl;
    cout << " Reading inputfile: " << muFileList << endl;
    cout << "z.z.z.z z3"<<endl;
    cout << " Output location: " << outname << endl;
    cout << "z.z.z.z z4"<<endl;

    log << " Reading inputfile: " << muFileList << endl;
    log << " Output location: " << outname << endl;
    cout << "z.z.z.z z5"<<endl;

    cout << "z.z.z.z z6"<<endl;
    gROOT->Macro("loadMuDst.C");
    cout << "z.z.z.z z7"<<endl;
    gSystem->Load("StUtilities");
    cout << "z.z.z.z z8"<<endl;
    gSystem->Load("StMiniMcEvent");
    cout << "z.z.z.z z9"<<endl;
    gSystem->Load("StarClassLibrary");
    cout << "z.z.z.z z10"<<endl;
    gSystem->Load("TrackEmbedding");
    cout << "z.z.z.z z11"<<endl;
    gSystem->Load("StChain");
    cout << "z.z.z.z z12"<<endl;

    cout << "z.z.z.z z13"<<endl;
    chain = new StChain();
    cout << "z.z.z.z z14"<<endl;
    StMuDstMaker* muDstMaker = new StMuDstMaker(0, 0, "",muFileList, "", 10000000);
    muDstMaker->SetStatus("*",0);
    muDstMaker->SetStatus("MuEvent",1);
    muDstMaker->SetStatus("PrimaryVertices",1);
    muDstMaker->SetStatus("StEventInfo",1) ;    // Turn on the primary track data
    cout << "z.z.z.z z15"<<endl;
  
    // make StEfficiencyMaker
    std::ifstream file(muFileList);
    TChain* mcChain = new TChain("StMiniMcTree");

    cout << "z.z.z.z z16"<<endl;
    string line;
    while (std::getline(file, line)) {
        cout << " Getline: " << line << endl;
        line.replace( line.find("MuDst"), 5, "minimc");
        cout << "This line: " << line << endl;
        mcChain->Add(line.c_str());
    }
    file.close();

    cout << "z.z.z.z z17"<<endl;
    log  << " Starting: " << mcChain->GetEntries() << " events." << endl;
    cout << " Starting: " << mcChain->GetEntries() << " events." << endl;
    cout << "z.z.z.z z18"<<endl;


    TrackEmbedding* embtree_mk = new TrackEmbedding(mcChain, muDstMaker, log, outname);
    cout << "z.z.z.z z19"<<endl;
    /* cout << " These lines: " << muDstMaker->GetEntries() << endl; */

    /* StMemStat memory; */
    /* memory.PrintMem(NULL); */
    cout << "------- ------------ ----------- a1" << endl;
    cout << "z.z.z.z z20"<<endl;
	
    if (chain->Init()) { cout<<"StChain failed init: exiting"<<endl; return;}
    cout << "chain initialized" << endl;
	
    cout << "z.z.z.z z21"<<endl;
    TStopwatch total;
    TStopwatch timer;
	
    cout << "z.z.z.z z22"<<endl;
    int i=0;
    /* if (nEvents == -1) nEvents = chain->GetEntries(); */
    cout << " starting to work on " << nEvents << endl;
    /* for (int i{0}; i<nEvents; i++){ */
    cout << "z.z.z.z z23"<<endl;
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
    cout << "z.z.z.z z24"<<endl;
  chain->Finish();
  ostringstream msg;
    cout << "z.z.z.z z25"<<endl;
  
    cout << "z.z.z.z z26"<<endl;
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
    cout << "z.z.z.z z22"<<endl;

    cout << "z.z.z.z z26"<<endl;
  cout << msg.str();
    cout << "z.z.z.z z27"<<endl;
  log  << msg.str();
    cout << "z.z.z.z z28"<<endl;
  return 0;
}
