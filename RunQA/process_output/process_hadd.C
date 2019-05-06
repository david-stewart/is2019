#include <algorithm>
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TString.h"
#include "TKey.h"
#include "TDirectory.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TMath.h"
#include "TF1.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TCanvas.h"
#include "THistPainter.h"
/* #include "gStyle.h" */
#include <map>

// add a tuple that will store the fit parameters, per run, of each runId.
// make plots of 16 TPads at a time (in a 4x4 canvas);

using namespace std;

void process_hadd(){
    TFile *hadd_file = new TFile("hadd_file.root","READ");
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
    /* cout << "a1 " << endl; */

    std::sort(runIds.begin(), runIds.end());
    int nRunId = runIds.size();

    ofstream f_out("runId.list");
    for (int i=0; i<nRunId; ++i) f_out << runIds[i] << endl;
    f_out.close();

    //add together all of the profiles and histograms of the individual runs
    hadd_file->cd();

    TProfile* read_means;
    hadd_file->GetObject(Form("means_%i",runIds[0]),read_means);

    TProfile* hadd_pr_et_gt4;
    hadd_file->GetObject("et_gt4",hadd_pr_et_gt4);

    TProfile* read_bbcVz;
    hadd_file->GetObject(Form("bbcVz_%i",runIds[0]),read_bbcVz);

    int ibin_start = read_bbcVz->GetXaxis()->FindBin(-10.);
    int ibin_end   = read_bbcVz->GetXaxis()->FindBin( 10.);

    TH1D*     read_hardcuts;
    TH2D*     read_et;
    hadd_file->GetObject(Form("hard_cuts_%i",runIds[0]),read_hardcuts);

    //---------------------------------
    // Make the TCanvas of 4x4 plots
    //---------------------------------
    TCanvas* c = new TCanvas();
    c->Divide(4,4);
    int which_pad = 1;
    int i_pdf = 0;

    /* cout << "a2 " << endl; */

    if (debug) cout << "hadd_file " << read_means->GetName() << endl;

    TFile* f_new = new TFile("runQA_results.root","RECREATE");
    f_new->cd();
    /* cout << "a3 " << endl; */
    TProfile* pr_etgt4 = (TProfile*) hadd_pr_et_gt4->Clone("pr_etgt4");
    /* cout << "a4 " << endl; */
    TProfile* means = new TProfile("means",Form("Add all means histograms;%s;%s",
                read_means->GetXaxis()->GetTitle(), read_means->GetYaxis()->GetTitle()),
                read_means->GetXaxis()->GetNbins(), 
                read_means->GetXaxis()->GetBinLowEdge(1),
                read_means->GetXaxis()->GetBinLowEdge( read_means->GetXaxis()->GetNbins()+1));
    /* cout << "a5 " << endl; */
    TH1D* hardcuts = new TH1D("hardcuts",Form("Add all hard_cuts histograms;%s;%s",
                read_hardcuts->GetXaxis()->GetTitle(), read_hardcuts->GetYaxis()->GetTitle()),
                read_hardcuts->GetXaxis()->GetNbins(), 
                read_hardcuts->GetXaxis()->GetBinLowEdge(1),
                read_hardcuts->GetXaxis()->GetBinLowEdge( read_hardcuts->GetXaxis()->GetNbins()+1));
    /* cout << "a6 " << endl; */
    cout << read_bbcVz->GetXaxis()->GetTitle() << endl;
    /* cout << "a7 " << endl; */
    TProfile* bbcVz = new TProfile("bbcVz",Form("All bbc(Vz) histograms added;%s;%s",
                read_bbcVz->GetXaxis()->GetTitle(), read_bbcVz->GetYaxis()->GetTitle()),
                read_bbcVz->GetXaxis()->GetNbins(), 
                read_bbcVz->GetXaxis()->GetBinLowEdge(1),
                read_bbcVz->GetXaxis()->GetBinLowEdge( read_bbcVz->GetXaxis()->GetNbins()+1));
    /* cout << "a7 " << endl; */
    TNtuple* fit_tup = new TNtuple("bbcVz_10fit","Fits for bbc = p0+p1*vz+p2*vz^2, per run",
            "runId:p0:p1:p2:entries_tot:entries_10t10:vz_mean");

    /* cout << "a8 " << endl; */
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
    TH2D* et         = new TH2D("et","E_{T} for all runs for all towers",4800, 0.5, 4800.5,42,4.,25.);

    for (int i = 0; i < nRunId; ++i) {
        hadd_file->GetObject(Form("hard_cuts_%i",runIds[i]),read_hardcuts);
        hadd_file->GetObject(Form("et_%i",       runIds[i]),read_et);
        hadd_file->GetObject(Form("bbcVz_%i",    runIds[i]),read_bbcVz);
        if (read_et) et->Add(read_et);

        //fit the vz data
        hardcuts->Add(read_hardcuts);
        /* et->Add(read_et); */

    /* cout << "a9 " << i << endl; */
        hadd_file->GetObject(Form("means_%i",runIds[i]),read_means);
        means->Add(read_means);
        bbcVz->Add(read_bbcVz);

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

        //-------------------------------------------
        // Get the bbc(vz) data
        //-------------------------------------------

        // get the number of entries from -10 to 10
        double count10to10 = 0;
        for (int j = ibin_start; j <= ibin_end; ++j)
            count10to10 += (int) read_bbcVz->GetBinEntries(j);

        const char* fit_name(Form("fit_%i",runIds[i]));
        TF1* tfit = new TF1( fit_name, "pol2");
        tfit->SetLineStyle(3);
        tfit->SetLineWidth(1);
        tfit->SetLineColor(kRed);
        read_bbcVz->Fit(fit_name,"","",-10,10);

        read_bbcVz->SetTitle(Form("run(%i) fits(%f,%f,%f) nentries(%.0f, %.0f) mean(%.2f)",
            runIds[i],
            tfit->GetParameter(0),
            tfit->GetParameter(1),
            tfit->GetParameter(2),
            read_bbcVz->GetEntries(),
            count10to10,
            read_bbcVz->GetMean())
        );
        read_bbcVz->GetXaxis()->SetTitle("V_{Z}");
        read_bbcVz->GetYaxis()->SetTitle("bbcES");


    /* cout << "a10 " << endl; */
        c->cd(which_pad++);
        /* gStyle->SetStats("000000"); */
        read_bbcVz->SetStats(0);
        read_bbcVz->Draw();
        if (which_pad == 17) {
            which_pad = 1;
            c->SaveAs(Form("zfit_pdf/%i_vz_fits.pdf",i_pdf++));
        }

    /* cout << "a11 " << endl; */
        fit_tup->Fill(
            runIds[i],
            tfit->GetParameter(0),
            tfit->GetParameter(1),
            tfit->GetParameter(2),
            read_bbcVz->GetEntries(),
            count10to10,
            read_bbcVz->GetMean()
        );
    /* cout << "a12 " << endl; */
            cout << " fits: " 
                 << " " << tfit->GetParameter(0)
                 << " " << tfit->GetParameter(1)
                 << " " << tfit->GetParameter(2)
                 << endl;

    /* cout << "a13 " << endl; */
        /* bbcVz_fit->Fit(" */
        /* if (i > 50) break; */
    }
    /* if (which_pad != 17) { */
    /*     which_pad = 1; */
    /*     c->SaveAs(Form("%i_vz_fits.pdf",i_pdf++)); */
    /* } */

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
