// C Standard Libraries
#include <cstdio>

// GSL
#include <gsl/gsl_matrix.h>

// Project
#include "rocket.hpp"
#include "demorocket.hpp"

int main(int argc, char** argv){
  Rocket rocket(0.1);
  demoRocket(rocket, &argc, argv);
  return 0;
}
