#include "common.hpp"

#include <cmath>

double normalize(double x, double min, double max){
  return (x - min)/(max-min);
}

double orbital_velocity(double mass, double radius){
  return sqrt(gravitiational_constant*mass*radius);
}

/* https://gist.github.com/jmbr/668083 cross product computation */
void cross_product(const gsl_vector *u, const gsl_vector *v, gsl_vector *product){
  double p1 = gsl_vector_get(u, 1)*gsl_vector_get(v, 2)
          - gsl_vector_get(u, 2)*gsl_vector_get(v, 1);

  double p2 = gsl_vector_get(u, 2)*gsl_vector_get(v, 0)
          - gsl_vector_get(u, 0)*gsl_vector_get(v, 2);

  double p3 = gsl_vector_get(u, 0)*gsl_vector_get(v, 1)
          - gsl_vector_get(u, 1)*gsl_vector_get(v, 0);

  gsl_vector_set(product, 0, p1);
  gsl_vector_set(product, 1, p2);
  gsl_vector_set(product, 2, p3);
}
