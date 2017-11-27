#include <cstdio>
#include <gsl/gsl_matrix.h>
#include "rocket.hpp"

int main(){
  Rocket rocket(0.01);
  rocket.print();
  /* run for 100 seconds */
  for(int i = 0; i < 10000; ++i){
    rocket.step();
  }
  rocket.print();

  return 0;
}
