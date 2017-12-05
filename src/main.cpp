// C Standard Libraries
#include <cstdio>

// GSL
#include <gsl/gsl_matrix.h>

// Project
#include "rocket.hpp"
#include "demorocket.hpp"

int main(int argc, char** argv) {
  Rocket rocket(0.01);
  rocket.print();
  int ret = demoRocket(rocket, &argc, argv);
  if(ret != 0) {
    printf("An error occured.\n");
  } else {
    rocket.print();
  }

  return ret;
}
