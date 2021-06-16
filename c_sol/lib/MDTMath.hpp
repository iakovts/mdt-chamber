#ifndef MDTMATH_H
#define MDTMATH_H

#include <vector>
#include <cmath>
#include <iostream>

struct Tube {
  /* Contains tube's information in SI units */
  double r = 0.015;
  double d = 0.03;
} tube;


class MDTMath {
public:
  double x_bot, x_top;
  double c_height, c_width, m_height, min_height; // Chamber max height and width, multi-layer height and min height;
  double calc_slope();
  double calc_inter();
  void calc_line(std::vector<double> &vec);
  double dist_from_line(std::vector<double> &lin_eq, std::vector<double> &pt);
  void extrapolate_line(std::vector<double> &lin_eq, std::vector<double> &pts);
};

double MDTMath::calc_slope(){
  /* Returns the slope of a line passing through a multi-layer. Takes into
   account that the y-difference will always be 4 * tube.d, while x-diff is
  (x_top - x_bot) * tube.d . */
  return (m_height / (x_top - x_bot));
}

double MDTMath::calc_inter(){
  /* Calculates the y interect based on the y = mx + b, for
   the bottom row where y = 0 and x = random. */
  return (- calc_slope() * x_bot);
}

void MDTMath::calc_line(std::vector<double> &vec){
  /* Returns a vectors containg the line equation stats where vec[0] = slope,
   vec[1] = intercept. The line can the be reconstructed.*/
  vec.at(0) = calc_slope();
  vec.at(1) = calc_inter();
}


double MDTMath::dist_from_line(std::vector<double> &lin_eq, std::vector<double> &pt){
  /* Calculates the closest distance of tube centers from the muon
     trajectory, given their line equation. */ 
  double numen = std::abs(- lin_eq[0] * pt[0] + pt[1] - lin_eq[1]);
  double denom = std::sqrt(std::pow(lin_eq[0], 2) + std::pow(1, 2));
  return (double)(numen / denom);
}

void MDTMath::extrapolate_line(std::vector<double> &lin_eq, std::vector<double> &pts){
  /* Extrapolates the line to the maximum or minimum chamber height. */
  pts[0] = (min_height - lin_eq[1]) / lin_eq[0];
  pts[1] = (c_height - lin_eq[1]) / lin_eq[0]; // x = (y - c) / m
}

#endif
