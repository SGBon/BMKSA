#include "common.hpp"

#include <cmath>
#include <cstring>

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

void rotation_matrix(gsl_matrix *matrix,double theta,rotation_axis_t axis){
  double r[9];
  memset(r,0,sizeof(double)*9);
  const double sintheta = sin(theta);
  const double costheta = cos(theta);
  switch(axis){
  case ROTATION_AXIS_X:
    r[0] = 1.0;
    r[4] = costheta;
    r[5] = -sintheta;
    r[7] = sintheta;
    r[8] = costheta;
    break;
  case ROTATION_AXIS_Y:
    r[0] = costheta;
    r[2] = sintheta;
    r[4] = 1.0;
    r[6] = -sintheta;
    r[8] = costheta;
    break;
  case ROTATION_AXIS_Z:
    r[0] = costheta;
    r[1] = -sintheta;
    r[3] = sintheta;
    r[4] = costheta;
    r[8] = 1.0;
    break;
  }
  memcpy(matrix->data,r,9*sizeof(double));
}
