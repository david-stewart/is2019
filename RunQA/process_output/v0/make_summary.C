#include <algorithm>
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TString.h"
#include "TKey.h"
#include "TDirectory.h"
#include "TH1.h"
#include "TProfile.h"
#include "TMath.h"
#include "TTree.h"
#include <map>

using namespace std;

void make_summary(){
    TFile *hadd_file = new TFile("hadd.root","READ");
    TDirectory *current_sourcedir = gDirectory;

    vector<int> runIds;

    TKey *key = 0;
     TIter nextkey( current_sourcedir->GetListOfKeys() );
    int j = 0;
    bool debug = false;
    while ( (key = (TKey*)nextkey())) {
        if (debug) if (++j>40) break;
        TString name = key->GetName();
        if (name.BeginsWith("means_")) {
            TString sub_name(name(6,8));
            runIds.push_back(sub_name.Atoi());
            if (debug) cout << sub_name.Atoi() << endl;
        }
    }
    
    int nRunId = runIds.size();

    ofstream f_out("runId.list");
    for (int i=0; i<nRunId; ++i) f_out << runIds[i] << endl;
    f_out.close();

    //add together all of the profiles and histograms of the individual runs
    hadd_file->cd();

    TProfile* read_means;
    hadd_file->GetObject(Form("means_%i",runIds[0]),read_means);

    TH1D*     read_hardcuts;
    hadd_file->GetObject(Form("hard_cuts_%i",runIds[0]),read_hardcuts);

    if (debug) cout << "hadd_file " << read_means->GetName() << endl;

    TFile* f_new = new TFile("summary_of_hadd.root","RECREATE");
    f_new->cd();
    TProfile* means = new TProfile("means",Form("Add all means histograms;%s;%s",
                read_means->GetXaxis()->GetTitle(), read_means->GetYaxis()->GetTitle()),
                read_means->GetXaxis()->GetNbins(), 
                read_means->GetXaxis()->GetBinLowEdge(1),
                read_means->GetXaxis()->GetBinLowEdge( read_means->GetXaxis()->GetNbins()+1));
    TH1D* hardcuts = new TH1D("hardcuts",Form("Add all hard_cuts histograms;%s;%s",
                read_hardcuts->GetXaxis()->GetTitle(), read_hardcuts->GetYaxis()->GetTitle()),
                read_hardcuts->GetXaxis()->GetNbins(), 
                read_hardcuts->GetXaxis()->GetBinLowEdge(1),
                read_hardcuts->GetXaxis()->GetBinLowEdge( read_hardcuts->GetXaxis()->GetNbins()+1));

    TH1D* zdcX = new TH1D("zdcX","zdcX of each run;index_{runId};#LTzdcX#GT",nRunId,0.5, nRunId+0.5);
    TH1D* BBCe = new TH1D("BBCe","BBC East Small of each run;index_{runId};#LTBBC_{ES}#GT",nRunId,0.5, nRunId+0.5);
    TH1D* BBCw = new TH1D("BBCw","BBC West Small of each run;index_{runId};#LTBBC_{WS}#GT",nRunId,0.5, nRunId+0.5);
    TH1D* vz   = new TH1D("vz","V_{z};index_{runId};#LTV_{z}#GT",nRunId,0.5, nRunId+0.5);
    TH1D* vzVPD= new TH1D("vzVPD","V_{z,VPD};index_{runId};#LTV_{z,VPD}#GT",nRunId,0.5, nRunId+0.5);
    TH1D* vz_vpd= new TH1D("vz_v_vpd","V_{z}-V_{z,VPD};index_{runId};#LTN_{global}#GT",nRunId,0.5, nRunId+0.5);
    TH1D* Nglobal= new TH1D("Nglobal","N_{global};index_{runId};#LTN_{global}#GT",nRunId,0.5, nRunId+0.5);
    TH1D* nch= new TH1D("Nch","N_{ch};index_{runId};#LTN_{ch}#GT",nRunId,0.5, nRunId+0.5);
    TH1D* meanpt= new TH1D("meanpt","Mean p_{T};index_{runId};#LTp_{T}#GT",nRunId,0.5, nRunId+0.5);
    TH1D* nEtgt4= new TH1D("nEtgt4","Number of Et hits > 4 GeV;index_{runId};#LTN_{E_{T}>4GeV}#GT",nRunId,0.5, nRunId+0.5);
    TH1D* meanEtgt4= new TH1D(
		"meanEtgt4","Mean E_{T} for E_{T} > 4 GeV;index_{runId};#LTE_{T} for E_{T}>4GeV}#GT",
		nRunId,0.5, nRunId+0.5);
	TH1D* nEvents = new TH1D("nEvents","N_{events};index_{runId};N_{events}",nRunId,0.5, nRunId+0.5);
	TH1D* hg_times   = new TH1D("times","Run length [secs];index_{runId};Run Time [sec]",nRunId,0.5, nRunId+0.5);

    for (int i = 0; i < nRunId; ++i) {
        hadd_file->GetObject(Form("hard_cuts_%i",runIds[i]),read_hardcuts);
        hardcuts->Add(read_hardcuts);

        hadd_file->GetObject(Form("means_%i",runIds[i]),read_means);
        means->Add(read_means);

        int ibin = i+1;
		nEvents->SetBinContent(ibin, read_means->GetBinEntries(1));
		nEvents->SetBinError(ibin,   TMath::Sqrt(read_means->GetBinEntries(1)));

        zdcX->SetBinContent(ibin, read_means->GetBinContent(1));
        zdcX->SetBinError  (ibin, read_means->GetBinError(1));

        BBCe->SetBinContent(ibin, read_means->GetBinContent(2));
        BBCe->SetBinError  (ibin, read_means->GetBinError(2));

        BBCw->SetBinContent(ibin, read_means->GetBinContent(3));
        BBCw->SetBinError  (ibin, read_means->GetBinError(3));

        vz->SetBinContent(ibin, read_means->GetBinContent(4));
        vz->SetBinError  (ibin, read_means->GetBinError(4));

        vzVPD->SetBinContent(ibin, read_means->GetBinContent(5));
        vzVPD->SetBinError  (ibin, read_means->GetBinError(5));

        vz_vpd->SetBinContent(ibin, read_means->GetBinContent(6));
        vz_vpd->SetBinError  (ibin, read_means->GetBinError(6));

        Nglobal->SetBinContent(ibin, read_means->GetBinContent(7));
        Nglobal->SetBinError  (ibin, read_means->GetBinError(7));

        nch->SetBinContent(ibin, read_means->GetBinContent(8));
        nch->SetBinError  (ibin, read_means->GetBinError(8));

        meanpt->SetBinContent(ibin, read_means->GetBinContent(9));
        meanpt->SetBinError  (ibin, read_means->GetBinContent(9));

        nEtgt4->SetBinContent (ibin, read_means->GetBinContent(10));
        nEtgt4->SetBinError   (ibin, read_means->GetBinError(10));

        meanEtgt4->SetBinContent   (ibin, read_means->GetBinContent(11));
        meanEtgt4->SetBinError   (ibin, read_means->GetBinError(11));
    }

    // get the run lengths in seconds
	map<int,int> times;
	for (int i = 0; i < nRunId; ++i){ times[runIds[i]] = 0; }

	TTree* tree;
	int sec;
	int  id;

	hadd_file->GetObject("run_data",tree);
	tree->SetBranchAddress("length_secs",&sec);
	tree->SetBranchAddress("runId",&id);
	int nEntries = tree->GetEntries();
	if (debug) cout << " nEntries: " << nEntries << " ; nRunId: " << nRunId << endl;

	for (int i=0;i<nEntries;++i){
		tree->GetEntry(i);
		if (times.count(id) == 0) {
			cout << "Warning: don't have: " << id << endl;
			continue;
		}
		if (times[id] == 0) {
			times[id] = sec;
		} else if ( times[id] != sec) {
			cout << "Fatal: difference times for same id("<<id<<"): "<<sec<<" vs. " << times[id] << endl;
		}
	}
	for (int i = 0; i < nRunId; ++i) {
		if(times[runIds[i]] == 0) {
			cout << "Warning: no time for run("<<id<<")!"<<endl;
			continue;
		}
		hg_times->SetBinContent(i+1, times[runIds[i]]);
	}
	TH1D* EvRate = (TH1D*) nEvents->Clone("EvRate");
    EvRate->SetTitle("Mean Events per Second");
    EvRate->GetYaxis()->SetTitle("Ev/Second [Hz]");
    EvRate->Divide(hg_times);

    f_new->Write();
};
