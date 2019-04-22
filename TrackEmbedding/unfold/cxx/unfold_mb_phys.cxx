/* In order to compile:
    eventually:
    g++ -std=c++11 -o bin/unfold_mb_phys cxx/unfold_mb_phys.cxx `root-config --cflags --glibs` -I$ROOUNFOLD/src -L$ROOUNFOLD -lRooUnfold
*/

// make a tree and split it into truth (T) meausred (M) and response (MT)
// give results for two random sub-groups -- a & b
// unfold the M to T in all groups
// unfold M to T in a & b with the other response matrix

#define emb_tree_cxx
/* #include "emb_tree.h" */
#include "myRooUnfoldLib.h"
#include "TFile.h"
/* #include "mult_by_response.h" */
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TRandom3.h"

#include <functional>

#include <iostream>
#include <sstream>

#include "RooUnfoldResponse.h"
#include "RooUnfold.h"
#include "RooUnfoldBayes.h"

#include <map>
using namespace std;
/* #include "signal_corrections.h" */
/* #include "run_map.h" */
/* #include "WhichDeciles.h" */

// for all ten deciles, get the histogroms, and unfold the ptAll and ptMax 
// bins
int main (int argc, const char** argv) {
    std::ostringstream fout_name;
    int nbins_per_GeV;
 
    /* std::istringstream iss_nbins_per_GeV    { (argc > 1) ? argv[1] : "10." } ; */
    string in_root_file  { (argc > 1) ? argv[1] : "in_root/stracks_phys"};
    /* string in_root_file  { (argc > 2) ? argv[2] : "short.root"}; */
    fout_name << ( (argc > 2) ? argv[2] : "out_root/stracks_phys" );

    /* iss_nbins_per_GeV >> nbins_per_GeV; */
    cout << " in file: " << in_root_file << endl;
    fout_name << "_unfolded.root";
    cout << "Writing file: " << fout_name.str() << endl;
    string fname = fout_name.str() ;


    RooUnfoldResponse* response;
    RooUnfoldBayes*    bayes_ptAll;
    RooUnfoldBayes*    bayes_ptMax;

    map<int, string> titles { 
        {0, "All towers"},
        {4, "Towers w/HT1>4GeV"},
        {8, "Towers w/HT1 in (8,12) GeV"},
        {12,"Towers w/HT1 in (12,30) GeV"}
    };

    map<int, string> names { 
        {0, "all_"},
        {4, "4up_"},
        {8, "8t12_"},
        {12,"12t30_"}
    };


    TFile* hg_in = new TFile(in_root_file.c_str(), "READ");
    TFile* mc_in = new TFile("out_root/unfolded_10perGeV.root","READ");
    TFile* fout =  new TFile(fname.c_str(), "RECREATE");

    for (int bin{0}; bin<10; ++bin) {
            /* cout << "a::-2" << endl; */
        TH1D *T, *M;
        TH2D *MT, *TM;

            /* cout << "a::-1" << endl; */
        mc_in->cd();
        mc_in->GetObject( Form("bbcDec_%iT",bin), T);
        mc_in->GetObject( Form("bbcDec_%iM",bin), M);
        mc_in->GetObject( Form("bbcDec_%iMT",bin), MT);
        mc_in->GetObject( Form("bbcDec_%iTM",bin), TM);


            /* cout << "a::0" << endl; */
        fout->cd();
        TH1D* cp1;
        TH2D* cp2;
        cp1 = (TH1D*) T->Clone(); cp1->Write(); cp1->Delete();
        cp1 = (TH1D*) M->Clone(); cp1->Write(); cp1->Delete();
        cp2 = (TH2D*) MT->Clone(); cp2->Write(); cp2->Delete();
        cp2 = (TH2D*) TM->Clone(); cp2->Write(); cp2->Delete();

        for (int group : {0,4,8,12}) {
            TH1D *ptAll;
            TH1D *ptMax;
            
            hg_in->cd();
            hg_in->GetObject( Form("ptAll%i_bin%i",group,bin), ptAll);
            hg_in->GetObject( Form("ptMax%i_bin%i",group,bin), ptMax);

            fout->cd();
            cp1 = (TH1D*) ptMax->Clone(); cp1->Write(); cp1->Delete();
            cp1 = (TH1D*) ptAll->Clone(); cp1->Write(); cp1->Delete();

            RooUnfoldResponse* resMT = new RooUnfoldResponse(M, T, MT);
            RooUnfoldBayes*    bayesPtAll = new RooUnfoldBayes(resMT,ptAll,5);
            RooUnfoldBayes*    bayesPtMax = new RooUnfoldBayes(resMT,ptMax,5);

            TH1D* ptMaxU =  (TH1D*) bayesPtMax->Hreco();
            ptMaxU->SetName(Form("ptMax%i_bin%i_U",group,bin));
            ptMaxU->GetYaxis()->SetTitle("N_{tracks}");
            ptMaxU->Write();
            cp1 = (TH1D*) ptMax->Clone(Form("ptMax%i_bin%i_rU",group,bin));
            cp1->SetTitle("Ratio M to Unfolded");
            cp1->GetYaxis()->SetTitle("N_{tracks}");
            cp1->Divide(ptMaxU);
            cp1->Write();
            cp1->Delete();
            ptMaxU->Delete();

            TH1D* ptAllU =  (TH1D*) bayesPtAll->Hreco();
            ptAllU->SetName(Form("ptAll%i_bin%i_U",group,bin));
            ptAllU->GetYaxis()->SetTitle("N_{tracks}");
            ptAllU->Write();
            cp1 = (TH1D*) ptAll->Clone(Form("ptAll%i_bin%i_rU",group,bin));
            cp1->SetTitle("Ratio M to Unfolded");
            cp1->GetYaxis()->SetTitle("N_{tracks}");
            cp1->Divide(ptAllU);
            cp1->Write();
            cp1->Delete();
            ptAllU->Delete();


            // copy over the nEv data
            if (bin == 0) {
                TH1D* nEv;
                hg_in->cd();
                hg_in->GetObject( Form("nEv%i", group), nEv);
                fout->cd();
                cp1 = (TH1D*) nEv->Clone(); cp1->Write(); cp1->Delete();
            };
        }
    }
    fout->Close();
};
