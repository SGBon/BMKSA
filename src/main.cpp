#include <cstdio>
#include <gsl/gsl_matrix.h>
#include "rocket.hpp"

int main(){
  int i,j;
  gsl_matrix *m = gsl_matrix_alloc(10,3);
  for(i = 0; i < 10; ++i)
    for(j = 0; j < 3; ++j)
      gsl_matrix_set(m,i,j,0.23 + 100*i + j);

  for(i = 0; i < 10; ++i)
    for(j = 0; j < 3; ++j)
      printf("m(%d,%d) = %g\n",i,j,gsl_matrix_get(m,i,j));

  gsl_matrix_free(m);

  /*
  RigidBody rigidbody(100,0.0);
  rigidbody.update(0.1);
  */
  
  Rocket rocket(0.1);
  rocket.step();


  return 0;
}
