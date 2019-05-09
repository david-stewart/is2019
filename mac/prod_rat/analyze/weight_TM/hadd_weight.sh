# short bash script that will hadd together two of the root files
# remove the ratios (naif_eff) and make a new (correct) naif_eff

newfile=$1
infiles="$2"

# echo hadd -f $newfile $infiles
hadd -f $newfile $infiles
rootrm $newfile:*naif_eff

root -l<<EOF
    TFile *f = new TFile("$newfile", "update");
    TH1D* naif_eff = (TH1D*) M->Clone("naif_eff");
    naif_eff->GetYaxis()->SetTitle("recon efficiency, w/ assumed prior");
    naif_eff->Divide(T);
    naif_eff->SetTitle("efficiency as a ratio of M/T");
    naif_eff->Write();
EOF
