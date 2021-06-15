#ifndef MDTCHAMBER_H
#define MDTCHAMBER_H

#include <TH1.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TEllipse.h>
#include <TMath.h>
#include <TRandom3.h>
#include <TLine.h>

#include <cmath>
#include <array>
#include <vector>
#include <iostream>

#define max_val(a, b) (a > b ? a : b)
#define min_val(a, b) (a > b ? b : a)

// template <class T, size_t m>
// using Arr = std::array<T, m>;


struct Tube {
  /* Contains tube's information in SI units */
  double r = 0.015;
  double d = 0.03;
} tube;

class MDTChamber {
public:
  // Root doesn't like c++ apparently.
  // Arr<Arr<double, 8>, 30> x_tubes{0};
  // Arr<Arr<double, 8>, 30> y_tubes{0};
  Double_t x_tubes[30][8] {0};
  Double_t y_tubes[30][8] {0};
  TRandom3 *rng = new TRandom3();
  
  TCanvas *c1 = new TCanvas("Cham", "MDT Chamber cross section", 200, 10, 600, 480);
  // TCanvas *c1 = new TCanvas("Cham", "MDT Chamber cross section", 200, 10, 600, 480);
  void init_chamber();
  void init_tubes();
  void draw_row(double offset_x, double offset_y);
  void draw_all();
  void draw_more();
  double get_random(double max, double min);
  void get_random_traj(std::vector<double> &lin_eq);
  void calc_angles(std::vector<double> &angles, double x);
};

void MDTChamber::init_chamber(){
  init_tubes();
}

void MDTChamber::init_tubes(){
  double x = 0, y = 0;
  for (int j = 0; j < 8; j ++){
    for (int i = 0; i < 30; i ++){
      y = (j > 3) ? 0.008 + j * tube.d : j * tube.d;
      x = (j % 2 != 0) ? tube.r + i * tube.d : i * tube.d;
      x_tubes[i][j] = x;
      y_tubes[i][j] = y;
    }
  }
}

void MDTChamber::draw_row(double offset_x, double offset_y){
  /* Draws the tubes on the canvas taking into the account a preselected
     offset for their positions.
   */
  double x = offset_x;
  double y = offset_y;
  int N_tubes = 30; // 30 tubes per row
  for (int i = 0; i < N_tubes; i++){
    x = offset_x + i * tube.d;
    TEllipse *el = new TEllipse(x, y, tube.r, tube.r);
    el->Draw("SAME");
  }
}

void MDTChamber::draw_all(){
  c1->SetFixedAspectRatio();
  c1->Range(-.5, -.5, 1, 1);
  for (int j = 0; j < 8; j++){
    for(int i = 0; i < 30; i ++){
      TEllipse *el = new TEllipse(x_tubes[i][j], y_tubes[i][j], tube.r, tube.r);
      el->SetFillColor(35);
      el->Draw("SAME");
    }
  }
}

void MDTChamber::draw_more(){
  c1->cd();
  TLine *li = new TLine(0, - tube.r, 0.5, 0.5);
  li->SetLineColor(2);
  li->Draw();
}

void MDTChamber::calc_angles(std::vector<double> &angles, double x){
  /* Calculates the range of angles allowed for a muon beam, so that
   the beam will cross at least one region of 4 tubes. Defined for a beam entering
   the region from h=0 and x=L/2 where L is the length of single row. */
  Double_t angle = std::atan(4.0 / x);
  angles[0] = angle;
  angles[1] = M_PI_2 - angle;
}

double MDTChamber::get_random(double max, double min){
  /* Returns random double in range [min, max] */
  // std::cout << "MAX " << max << " MIN: " << min << "\n";
  Double_t rand = rng->Rndm();
  double random_num = ((max - min) * rand + min);
  return random_num;
}

void MDTChamber::get_random_traj(std::vector<double> &lin_eq){
  double x_min = x_tubes[0][0], x_max = x_tubes[29][0];
  double x_rand = get_random(x_max, x_min);
  double angle_min, angle_max;
  double rand_angle = 0, slope = 0;
  std::vector<double> angles{2, 0};

  calc_angles(angles, x_rand);
  angle_min = min_val(angles[0], angles[1]);
  angle_max = max_val(angles[0], angles[1]);

  rand_angle = get_random(angle_max, angle_min);
  slope = std::tan(rand_angle);
  lin_eq[0] = x_rand;
  lin_eq[1] = slope;
}

#endif
