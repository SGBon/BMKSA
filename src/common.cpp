#include "common.hpp"

#include <cmath>

double normalize(double x, double min, double max){
  return (x - min)/(max-min);
}

double orbital_velocity(double mass, double radius){
  return sqrt(gravitiational_constant*mass*radius);
}
