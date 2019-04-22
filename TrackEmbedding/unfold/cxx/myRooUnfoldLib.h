#ifndef MYROOUNFOLDLIB_H
#define MYROOUNFOLDLIB_H

#include "RooUnfoldResponse.h"
#include "RooUnfold.h"
#include "RooUnfoldBayes.h"
#include "TH1D.h"
#include "TH2D.h"

TH1D* myBayesianUnfold(TH1D* to_unfold, TH1D* M, TH1D* T, TH2D* MT, 
    // uses Bayesian unfolding to return a new TH1D
        const char* name, const char* title,
        const char* x_title="", const char* y_title="",
        int n_iter=5
){
    RooUnfoldResponse* bayes_response = new RooUnfoldResponse (M, T, MT); 
    RooUnfoldBayes*    bayes = new RooUnfoldBayes(bayes_response, to_unfold, n_iter);
    TH1D* unfolded = (TH1D*) bayes->Hreco();
    unfolded->SetName(name);
    unfolded->SetTitle(title);
    unfolded->GetXaxis()->SetTitle(x_title);
    unfolded->GetYaxis()->SetTitle(y_title);
    delete bayes_response;
    delete bayes;
    return unfolded;
};

#endif
