#include "TF2.h"
#include "TH2.h"
#include "TMath.h"
#include "TProfile2D.h"

Double_t g2(Double_t *x, Double_t *par) {
   Double_t r1 = Double_t((x[0]-par[1])/par[2]);
   Double_t r2 = Double_t((x[1]-par[3])/par[4]);
   return par[0]*TMath::Exp(-0.5*(r1*r1+r2*r2));
}

Double_t fun2(Double_t *x, Double_t *par) {
   Double_t *p1 = &par[0];
   Double_t *p2 = &par[5];
   Double_t *p3 = &par[10];
   Double_t result = g2(x,p1) + g2(x,p2) + g2(x,p3);
   return result;
}

double bbcvz_fn(double* x, double *p){
    return p[0] + p[1]*x[0] + p[2]*x[0]*x[0] 
                + p[3]*x[1] + p[4]*x[1]*x[1] + p[5]*x[0]*x[1];
}

void fit2D(TProfile2D* p2D, double vz_range, const char* parameter_out) {

   /* const Int_t npar = 6; */
   const Int_t npar = 6;
   Double_t f2params[npar] =
      {40000., 1, 0.01, .01, 0.0001, 0.0};
      /* {40000., 1, 0.01, .01, 0.0001}; */
   /* TF2 *f2 = new TF2("f2",bbcvz_fn,-vz_range,vz_range,0,35000, npar); */
   TF2 *f2 = new TF2("f2",bbcvz_fn,-10,10,7000,24000, npar);
   f2->SetParameters(f2params);
   p2D->SetStats(111111111);

   gStyle->SetOptFit(1011);
   f2->SetParameters(f2params);
   p2D->Fit("f2","N");

   ofstream f_out;
   f_out.open(parameter_out);
   int i = 0;
   while (i<npar) {
       cout << " " << i<<": " << f2->GetParameter(i) << " +/- " << f2->GetParError(i) << endl;
       f_out << f2->GetParameter(i) << "  " << f2->GetParError(i) << endl;
       ++i;
   }
   f_out.close();

   p2D->Draw("colz");
}
