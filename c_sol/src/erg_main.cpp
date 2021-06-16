#include "../lib/MDTChamber.hpp"

int main() {
  MDTChamber mdt;
  // mdt.draw_row(0.15, 0);
  // mdt.draw_all();
  mdt.init_chamber();
  mdt.draw_all();
  // for (int i = 0; i < 10; i ++){
  //   std::cout << mdt.get_random_angle() << "\n";
  // }
  // for (int i = 0; i < 10; i++) {
    mdt.get_random_traj();
  // }
  // int n = 200;
  // int x[200];
  // int y[200];
  // for (int i = 0; i < n; i ++){
  //   x[i] = i;
  //   y[i] = i + 1;
  // }
  // TCanvas *c1 = new TCanvas("c1", "SIMPLE", 200, 10, 500, 500);
  // TGraph *g = new TGraph(n, x, y);
  // g->Draw();

  // // TCanvas *c2 = new TCanvas("c2", "circle", 200, 10, 500, 500);
  // TCanvas *c3 = new TCanvas("c2", "circle");
  // TEllipse *el1 = new TEllipse(0.25, 0.25, 0.1, 0.1);
  // el1->Draw();
  // c1 = new TCanvas();
}
// rand() % (max_number + 1 - minimum_number) + minimum_number
