#include "TAxis.h"
void mySetTitle(TAxis* ax, const char* title,  double offset=3.3, int f_size=17, int sel_font=43){
    ax->SetTitle(title);

    ax->SetTitleSize(f_size);
    ax->SetTitleFont(sel_font);
    ax->SetTitleOffset(offset);

    ax->SetLabelSize(f_size);
    ax->SetLabelFont(sel_font);

};
