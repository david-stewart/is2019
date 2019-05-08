if [ -e pi_weight.root ]; then rm *weight.root; fi
for part in pi pin p pbar K Kn
do
    # echo a1
    echo working on $part
    ./weight_TM.cc $part
    ./weight_TM.cc $part A_
    ./weight_TM.cc $part B_
done

./hadd_weight.sh TM_weighted.root "*_weight.root"
./hadd_weight.sh pip_weighted.root "pi_weight.root pin_weight.root p_weight.root pbar_weight.root"

# hadd -f TM_weighted.root *_weight.root 
# rootrm TM_weighted.root:*naif_eff

# root -l<<EOF
#     TFile *f = new TFile("TM_weighted.root", "update");
#     TH1D* naif_eff = (TH1D*) M->Clone("naif_eff");
#     naif_eff->GetYaxis()->SetTitle("recon efficiency, w/ assumed prior");
#     naif_eff->Divide(T);
#     naif_eff->SetTitle("efficiency as a ratio of M/T");
#     naif_eff->Write();
# EOF

# hadd -f pip_weighted.root pi_weight.root pin_weight.root p_weight.root pbar_weight.root
# rootrm pip_weighted.root:*naif_eff
