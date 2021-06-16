#ifndef MDTCHAMBER_H
#define MDTCHAMBER_H

#include <TCanvas.h>
#include <TEllipse.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2F.h>
#include <TLine.h>
#include <TMath.h>
#include <TRandom3.h>

#include <array>
#include <cmath>
#include <iostream>
#include <vector>

#include "MDTMath.hpp"

#define max_val(a, b) (a > b ? a : b)
#define min_val(a, b) (a > b ? b : a)

// template <class T, size_t m>
// using Arr = std::array<T, m>;

// struct Tube {
//   /* Contains tube's information in SI units */
//   double r = 0.015;
//   double d = 0.03;
// } tube;

class MDTChamber {
 public:
  // Root doesn't like c++ apparently.
  // Arr<Arr<double, 8>, 30> x_tubes{0};
  // Arr<Arr<double, 8>, 30> y_tubes{0};
  MDTMath my_math;
  Double_t x_tubes[30][8]{0};
  Double_t y_tubes[30][8]{0};
  Double_t dist_tube[30][8]{0};
  TRandom3 *rng = new TRandom3(10);

  TCanvas *c1 =
      new TCanvas("Cham", "MDT Chamber cross section", 200, 10, 600, 480);
  // TCanvas *c1 = new TCanvas("Cham", "MDT Chamber cross section", 200, 10,
  // 600, 480);
  void init_chamber();
  void init_tubes();
  void draw_row(double offset_x, double offset_y);
  void draw_all();
  void draw_more(std::vector<double> &pts);
  void draw_full_traj(std::vector<double> &pts);
  void draw_traj_circl(std::vector<std::vector<double>> &hit_tubes);
  double get_random(double max, double min);
  void get_random_traj();
  void calc_angles(std::vector<double> &angles, double x);
  std::vector<std::vector<double>> calc_distance(std::vector<double> &lin_eq);
};

void MDTChamber::init_chamber() {
  init_tubes();
  my_math.c_height = y_tubes[0][7];
  my_math.c_width = x_tubes[29][0];
  my_math.m_height = y_tubes[0][3] + tube.d;
  my_math.min_height = y_tubes[0][0] - tube.r;
}

void MDTChamber::init_tubes() {
  /* Set the centers of the tubes. Takes into account that different layers
   are stacked in a way that the distance of their centers is less than the 
  diameter of a single tube.*/
  double y_step = std::sqrt(std::pow(tube.d, 2) - std::pow(tube.r, 2));
  double x = 0, y = 0;
  for (int j = 0; j < 8; j++) {
    for (int i = 0; i < 30; i++) {
      y = (j > 3) ? 0.008 + j * y_step : j * y_step;
      x = (j % 2 != 0) ? tube.r + i * tube.d : i * tube.d;
      x_tubes[i][j] = x;
      y_tubes[i][j] = y;
    }
  }
}


void MDTChamber::draw_all() {
  c1->SetFixedAspectRatio();
  c1->Range(-.5, -.5, 1, 1);
  for (int j = 0; j < 8; j++) {
    for (int i = 0; i < 30; i++) {
      TEllipse *el = new TEllipse(x_tubes[i][j], y_tubes[i][j], tube.r, tube.r);
      el->SetFillColor(35);
      el->Draw("SAME");
    }
  }
}

// void MDTChamber::draw_more(std::vector<double> &pts) {
//   c1->cd();
//   TLine *li = new TLine(pts[0], y_tubes[0][0], pts[1], y_tubes[0][3]);
//   li->SetLineColor(2);
//   li->Draw("SAME");
// }

void MDTChamber::draw_full_traj(std::vector<double> &pts){
  c1->cd();
  TLine *li = new TLine(pts[0], y_tubes[0][0] - tube.r, pts[1], y_tubes[0][7]);
  li->SetLineColor(2);
  gPad->Modified();
  gPad->Update();
  li->Draw("SAME");
}

void MDTChamber::draw_traj_circl(std::vector<std::vector<double>> &hit_tubes){
c1->SetFixedAspectRatio();
c1->Range(-.5, -.5, 1, 1);
 for (auto hit : hit_tubes){
    // c1->cd();
    TEllipse * el = new TEllipse(hit[0], hit[1], hit[2], hit[2]);
    // std::cout << hit[0] << " " << hit[1] << " " << hit[2];
    el->SetFillColor(3);
    el->SetLineColor(2);
    // gPad->Modified();
    // gPad->Update();
    el->Draw("SAME");
  }
  
  
}

void MDTChamber::calc_angles(std::vector<double> &angles, double x) {
  /* Calculates the range of angles allowed for a muon beam, so that
   the beam will cross at least one region of 4 tubes. Defined for a beam
   entering the region from h=0 and x=L/2 where L is the length of single row.
 */
  Double_t angle = std::atan(4.0 / x);
  angles[0] = angle;
  angles[1] = M_PI_2 - angle;
}

double MDTChamber::get_random(double max, double min) {
  /* Returns random double in range [min, max] */
  // std::cout << "MAX " << max << " MIN: " << min << "\n";
  Double_t rand = rng->Rndm();
  double random_num = ((max - min) * rand + min);
  return random_num;
}

void MDTChamber::get_random_traj() {
  /* Returns a random muon trajectory passing through at least one
   "multi-layer" of the MDT chamber. Such line should at least pass
  through the bottom and top layers of the multi-layer. */
  double x_min_bot = x_tubes[0][0], x_max_bot = x_tubes[29][0];
  double x_min_top = x_tubes[0][3], x_max_top = x_tubes[29][3];
  // Get 2 random points one on the bottom row and one on the top.
  double x_rand_bot = get_random(x_max_bot, x_min_bot);
  double x_rand_top = get_random(x_max_top, x_min_top);
  std::vector<double> line_eq{2, 0};
  std::vector<double> curr_pts{2, 0};
  curr_pts[0] = x_rand_bot;
  curr_pts[1] = x_rand_top;
  my_math.x_bot = x_rand_bot;
  my_math.x_top = x_rand_top;
  my_math.calc_line(line_eq);
  my_math.extrapolate_line(line_eq, curr_pts);
  draw_full_traj(curr_pts);


  std::vector<std::vector<double>> hit_tubes = calc_distance(line_eq);
  draw_traj_circl(hit_tubes);
}

std::vector<std::vector<double>> MDTChamber::calc_distance(std::vector<double> &lin_eq){
  /* Calculates the distance of all tube centers from current line
   and adds the relevant ones (those whose distance from the line is
   less than a radius) to a vector of vectors.*/
  std::vector<double> curr_cntr(2, 0);
  std::vector<double> tube_hit(4, 0);
  std::vector<std::vector<double>> rel_cntr;
  for (int j = 0; j < 8; j++){
    for (int i = 0; i < 30; i++){
      curr_cntr.at(0) = x_tubes[i][j];
      curr_cntr.at(1) = y_tubes[i][j];
      double dist = my_math.dist_from_line(lin_eq, curr_cntr);
      // std::cout << dist << "  " << tube.r <<  "\n";
      if (std::abs(dist) < tube.r){
        // std::cout << " I am here\n";
        tube_hit[0] = curr_cntr.at(0);
        tube_hit[1] = curr_cntr.at(1);
        tube_hit[2] = dist;
        rel_cntr.push_back(tube_hit);
      }
    }
  }
  return rel_cntr;
}
#endif
