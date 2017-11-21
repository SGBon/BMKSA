#include "rigidbody.hpp"
#include <cstring>

int rigid_body_ode(double t, const double y[], double dydt[], void *params){

  return GSL_SUCCESS;
}

RigidBody::RigidBody(const double mass):
  mass(mass),
  state(gsl_vector_alloc(19)),
  force(gsl_vector_alloc(3)),
  torque(gsl_vector_alloc(3))
  {
    gsl_vector_set_zero(this->state);
    gsl_vector_set_zero(this->force);
    gsl_vector_set_zero(this->torque);

    memset(&this->ode_system,0,sizeof(this->ode_system));
    this->ode_system.function = rigid_body_ode;
    this->ode_system.dimension = 19;
  }

RigidBody::~RigidBody(){
  gsl_vector_free(this->state);
  gsl_vector_free(this->force);
  gsl_vector_free(this->torque);
}

gsl_matrix *RigidBody::star(gsl_vector *vector){
  gsl_matrix *out = gsl_matrix_alloc(3,3);

  gsl_matrix_set(out,0,0,0);
  gsl_matrix_set(out,0,0,gsl_vector_get(vector,2));
  gsl_matrix_set(out,0,0,-gsl_vector_get(vector,1));
  gsl_matrix_set(out,0,0,-gsl_vector_get(vector,2));
  gsl_matrix_set(out,0,0,0);
  gsl_matrix_set(out,0,0,gsl_vector_get(vector,0));
  gsl_matrix_set(out,0,0,gsl_vector_get(vector,1));
  gsl_matrix_set(out,0,0,-gsl_vector_get(vector,0));
  gsl_matrix_set(out,0,0,0);

  return out;
}
