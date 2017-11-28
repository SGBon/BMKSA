// C Standard Libraries
#include <cstdio>

// GSL
#include <gsl/gsl_matrix.h>

// Project
#include "rocket.hpp"
#include "demorocket.hpp"

int main(int argc, char** argv) {
  Rocket rocket(0.1);
  rocket.print();
  int ret = demoRocket(rocket, &argc, argv);
  rocket.print();
  return 0;
}
