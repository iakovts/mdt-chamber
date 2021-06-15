#include <TH1.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TEllipse.h>


int main(){
  int n = 200;
  int x[200];
  int y[200];
  for (int i = 0; i < n; i ++){
    x[i] = i;
    y[i] = i + 1;
  }
  TCanvas *c1 = new TCanvas("c1", "SIMPLE", 200, 10, 500, 500);
  TGraph *g = new TGraph(n, x, y);
  g->Draw();

  // TCanvas *c2 = new TCanvas("c2", "circle", 200, 10, 500, 500);
  TCanvas *c3 = new TCanvas("c2", "circle");
  TEllipse *el1 = new TEllipse(0.25, 0.25, 0.1, 0.1);
  el1->Draw();
  c1 = new TCanvas();
  

}
