#include "TF2.h"
#include "TH2.h"
#include "TMath.h"
#include "TProfile2D.h"

// Fitting a 2-D histogram
// This tutorial illustrates :
//  - how to create a 2-d function
//  - fill a 2-d histogram randomly from this function
//  - fit the histogram
//  - display the fitted function on top of the histogram
//
// This example can be executed via the interpreter or ACLIC
//   root > .x fit2.C
//   root > .x fit2.C++
//Author: Rene Brun

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

void fit2(TProfile2D* p2D) {

   /* const Int_t npar = 6; */
   const Int_t npar = 5;
   Double_t f2params[npar] =
      /* {40000., 1, 0.01, .01, 0.0001, 0.0}; */
      {40000., 1, 0.01, .01, 0.0001};
   TF2 *f2 = new TF2("f2",bbcvz_fn,-10,10,0,35000, npar);
   f2->SetParameters(f2params);
   p2D->SetStats(111111111);

   //Create an histogram and fill it randomly with f2
   /* TH2F *h2 = new TH2F("h2","from f2",40,-10,10,40,-10,10); */
   /* Int_t nentries = 100000; */
   /* h2->FillRandom("f2",nentries); */
   //Fit h2 with original function f2
   /* Float_t ratio = 4*nentries/100000; */
   /* f2params[ 0] *= ratio; */
   /* f2params[ 5] *= ratio; */
   /* f2params[10] *= ratio; */
   gStyle->SetOptFit(1011);
   f2->SetParameters(f2params);
   /* h2->Fit("f2"); */
   p2D->Fit("f2");
   p2D->GetXaxis()->SetRangeUser(-10,10);
   p2D->Draw("colz");

   /* auto prof = p2D->ProfileX(); */
   /* prof->Draw("same"); */

   f2->Draw("cont1 same");

   TCanvas *c0 = new TCanvas("c0");
   c0->WaitPrimitive();
   
}
