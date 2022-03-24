#ifndef MDTCHAMBER_H
#define MDTCHAMBER_H

#include <TCanvas.h>
#include <TEllipse.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLine.h>
#include <TMath.h>
#include <TRandom3.h>

#include <array>
#include <cmath>
#include <iostream>
#include <vector>

#include "MDTFit.hpp"
#include "MDTMath.hpp"

#define max_val(a, b) (a > b ? a : b)
#define min_val(a, b) (a > b ? b : a)

class MDTChamber {
 public:
  MDTFit my_fit;
  MDTMath my_math;
  Double_t x_tubes[30][8]{0};
  Double_t y_tubes[30][8]{0};
  Double_t dist_tube[30][8]{0};
  TRandom3 *rng = new TRandom3(10);

  // Stores the distances measured for all muons
  std::vector<double> distances;

  TCanvas *c1 =
      new TCanvas("Cham", "MDT Chamber cross section", 200, 10, 600, 480);
  void init_chamber();
  void init_tubes();
  void draw_row(double offset_x, double offset_y);
  void draw_tubes();
  void draw_more(std::vector<double> &pts);
  void draw_full_traj(std::vector<double> &pts);
  void draw_traj_circl(std::vector<std::vector<double>> &hit_tubes);
  double get_random(double max, double min);
  void get_random_traj(bool draw);
  void draw_hist();
  void run_sim_chamber();
  std::vector<std::vector<double>> calc_distance(std::vector<double> &lin_eq);
};

void MDTChamber::init_chamber() {
  init_tubes();
  my_math.c_height = y_tubes[0][7] + tube.r;
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

void MDTChamber::draw_tubes() {
  c1->SetFixedAspectRatio();
  c1->Range(-.5, -.5, .5, .5);
  for (int j = 0; j < 8; j++) {
    for (int i = 0; i < 30; i++) {
      TEllipse *el = new TEllipse(x_tubes[i][j], y_tubes[i][j], tube.r, tube.r);
      el->SetFillColor(35);
      el->Draw("SAME");
    }
  }
}

void MDTChamber::draw_full_traj(std::vector<double> &pts) {
  c1->cd();
  TLine *li =
      new TLine(pts[0], y_tubes[0][0] - tube.r, pts[1], y_tubes[0][7] + tube.r);
  li->SetLineColor(2);
  gPad->Modified();
  gPad->Update();
  li->Draw("SAME");
}

void MDTChamber::draw_traj_circl(std::vector<std::vector<double>> &hit_tubes) {
  c1->SetFixedAspectRatio();
  c1->Range(-0.15, -0.25, 1, 0.5);
  for (auto hit : hit_tubes) {
    TEllipse *el = new TEllipse(hit[0], hit[1], hit[2], hit[2]);
    el->SetFillColor(3);
    el->SetLineColor(2);
    el->Draw("SAME");
  }
}

double MDTChamber::get_random(double max, double min) {
  /* Returns random double in range [min, max] */
  Double_t rand = rng->Rndm();
  double random_num = ((max - min) * rand + min);
  return random_num;
}

void MDTChamber::get_random_traj(bool draw = false) {
  /* Returns a random muon trajectory passing through at least one
   "multi-layer" of the MDT chamber. Such line should at least pass
  through the bottom and top layers of the multi-layer. */
  double x_min_bot = x_tubes[0][0], x_max_bot = x_tubes[29][0];
  double x_min_top = x_tubes[0][3], x_max_top = x_tubes[29][3];
  // Get 2 random points one on the bottom row and one on the top.
  double x_rand_bot = get_random(x_max_bot, x_min_bot);
  double x_rand_top = get_random(x_max_top, x_min_top);
  std::vector<double> line_eq(2, 0);
  std::vector<double> curr_pts(2, 0);
  curr_pts[0] = x_rand_bot;
  curr_pts[1] = x_rand_top;
  my_math.x_bot = x_rand_bot;
  my_math.x_top = x_rand_top;
  my_math.calc_line(line_eq);
  my_math.extrapolate_line(line_eq, curr_pts);
  std::vector<std::vector<double>> hit_tubes = calc_distance(line_eq);
  if (draw) {
    draw_full_traj(curr_pts);
    draw_traj_circl(hit_tubes);
  }
}

std::vector<std::vector<double>> MDTChamber::calc_distance(
    std::vector<double> &lin_eq) {
  /* Calculates the distance of all tube centers from current line
   and adds the relevant ones (those whose distance from the line is
   less than a radius) to a vector of vectors.*/
  std::vector<double> curr_cntr(2, 0);
  std::vector<double> tube_hit(4, 0);
  std::vector<std::vector<double>> rel_cntr;
  for (int j = 0; j < 8; j++) {
    for (int i = 0; i < 30; i++) {
      curr_cntr.at(0) = x_tubes[i][j];
      curr_cntr.at(1) = y_tubes[i][j];
      double dist = my_math.dist_from_line(lin_eq, curr_cntr);
      if (std::abs(dist) < tube.r) {
        tube_hit[0] = curr_cntr.at(0);
        tube_hit[1] = curr_cntr.at(1);
        tube_hit[2] = dist;
        rel_cntr.push_back(tube_hit);
        distances.push_back(dist);
      }
    }
  }
  return rel_cntr;
}

void MDTChamber::draw_hist() {
  double *fit_params = my_fit.fit_data();

  TCanvas *c3 = new TCanvas("c3", "Time required for ionization per tube", 100,
                            0, 800, 800);
  TH1F *h_rt = new TH1F("r_t_relation", ";time[ns];events", 100, 0, 500);
  bool first_run = true;
  for (auto dist : distances) {
    // Tubes distances are in SI units so multiply by 1000 to get mm.
    double time = (dist * 1000 - fit_params[1]) / fit_params[0];
    if (first_run) {
      h_rt->Fill(time);
      c3->cd();
      h_rt->Draw("SAME");
      first_run = false;
      continue;
    }
    h_rt->Fill(time);
    c3->Modified();
    c3->Update();
  }
}

void MDTChamber::run_sim_chamber() {
  /* Assuming a simulation lasting 6 seconds with a mean flux of
  cosmic muons at 1 muon per minute per sqcm == 2000 events. */
  init_chamber();
  draw_tubes();
  for (int i = 0; i < 2000; i++) {
    get_random_traj();
  }
  get_random_traj(true);
  draw_hist();
}
#endif
