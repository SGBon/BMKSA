#include <cstdio>
#include <gsl/gsl_matrix.h>
#include "rocket.hpp"

int main(){
  Rocket rocket(0.1);
  rocket.print();
  /* run for 100 seconds */
  for(int i = 0; i < 1000; ++i){
    printf("step: %d\n",i);
    rocket.step();
  }
  rocket.print();

  return 0;
}
