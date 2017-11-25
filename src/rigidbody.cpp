#include "rigidbody.hpp"
#include <cstring>
#include <cstdio>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

static int rigid_body_ode(double t, const double y[], double dydt[], void *params){
  RigidBody *rigidbody = (RigidBody *) params;
  int s = 0;
  double dm = 1.0; /* loss of mass due to fuel */
  gsl_vector *force = gsl_vector_calloc(3);
  gsl_vector *torque = gsl_vector_calloc(3);

  /* inertia inversion stuff */
  gsl_matrix *inertia_copy = gsl_matrix_calloc(3,3);
  gsl_matrix *inertia_inverse = gsl_matrix_calloc(3,3);
  gsl_permutation *p = gsl_permutation_alloc(3);

  gsl_matrix_memcpy(inertia_copy,rigidbody->getInertiaTensor());
  /* compute inverse of inertia tensor */
  gsl_linalg_LU_decomp(inertia_copy,p,&s);
  gsl_linalg_LU_invert(inertia_copy,p,inertia_inverse);

  gsl_permutation_free(p);
  gsl_matrix_free(inertia_copy);

  memset(dydt,0,20*sizeof(double));

  /* compute force from the combination of gravity, drag, lift, thrust */

  /* compute torque from force */

  /* compute dr/dt () */
  gsl_matrix_const_view r_view = gsl_matrix_const_view_array(&y[3],3,3);
  const gsl_matrix *rotation = &r_view.matrix;
  gsl_matrix *product = gsl_matrix_alloc(3,3);
  gsl_matrix *Iinv = gsl_matrix_alloc(3,3);
  gsl_vector *w = gsl_vector_alloc(3);

  gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, inertia_inverse, rotation, 0.0, product);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, rotation, product, 0.0, Iinv);

  gsl_vector_const_view angular_momentum = gsl_vector_const_view_array(&y[15],3);
  gsl_blas_dgemv(CblasNoTrans,1.0,Iinv,&angular_momentum.vector,0.0,w);

  gsl_matrix *w_star = RigidBody::star(w);

  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,w_star,rotation,0.0,product);

  gsl_vector_free(w);
  gsl_matrix_free(Iinv);
  gsl_matrix_free(w_star);
  gsl_matrix_free(inertia_inverse);

  /* set dx/dt as velocity (P/m) */
  for(int i = 0; i < 3; ++i){
    dydt[i] = y[12+i]/y[19];
  }
  memcpy(&dydt[3],product->data,9*sizeof(double));
  memcpy(&dydt[12],force->data,3*sizeof(double)); /* set dP/dt as force */
  memcpy(&dydt[15],torque->data,3*sizeof(double)); /* set dL/dt as torque */

  /* recompute inertia tensor and save in rigidbody */

  gsl_matrix_free(product);
  gsl_vector_free(force);
  gsl_vector_free(torque);
  return GSL_SUCCESS;
}


RigidBody::RigidBody(const double mass, const double time):
  time(time),
  state(gsl_vector_calloc(STATE_SIZE)),
  inertia_tensor(gsl_matrix_calloc(3,3))
  {
    /* rotation matrix starts as identity matrix */
    gsl_vector_set(this->state,3,1);
    gsl_vector_set(this->state,7,1);
    gsl_vector_set(this->state,11,1);

    this->ode_system = new gsl_odeiv2_system;
    ode_system->function = rigid_body_ode;
    ode_system->jacobian = NULL;
    ode_system->dimension = STATE_SIZE;
    ode_system->params = this;

    this->ode_driver = gsl_odeiv2_driver_alloc_y_new(ode_system, gsl_odeiv2_step_rkf45,
      0.01, 1e-6, 0.0);
  }

RigidBody::~RigidBody(){
  gsl_vector_free(this->state);
  gsl_matrix_free(this->inertia_tensor);

  gsl_odeiv2_driver_free(this->ode_driver);
  delete this->ode_system;
}

gsl_matrix *RigidBody::star(gsl_vector *vector){
  gsl_matrix *out = gsl_matrix_alloc(3,3);

  gsl_matrix_set(out,0,0,0);
  gsl_matrix_set(out,1,0,gsl_vector_get(vector,2));
  gsl_matrix_set(out,2,0,-gsl_vector_get(vector,1));
  gsl_matrix_set(out,0,1,-gsl_vector_get(vector,2));
  gsl_matrix_set(out,1,1,0);
  gsl_matrix_set(out,2,1,gsl_vector_get(vector,0));
  gsl_matrix_set(out,0,2,gsl_vector_get(vector,1));
  gsl_matrix_set(out,1,2,-gsl_vector_get(vector,0));
  gsl_matrix_set(out,2,2,0);

  return out;
}

void RigidBody::update(const double dt){
  gsl_odeiv2_driver_apply(this->ode_driver,&this->time,dt,this->state->data);
}

gsl_matrix *RigidBody::getInertiaTensor(){
  return this->inertia_tensor;
}

void RigidBody::updateInertiaTensor(double inertia_tensor[]){
  memcpy(this->inertia_tensor->data,inertia_tensor,9*sizeof(double));
}

double RigidBody::getMass(){
  return gsl_vector_get(this->state,19);
}
