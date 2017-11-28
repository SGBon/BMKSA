#include "rigidbody.hpp"

#include <cstring>
#include <cstdio>
#include <iostream>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

#include "common.hpp"
#include "earth.hpp"

static int rigid_body_ode(double t, const double y[], double dydt[], void *params){
  RigidBody *rigidbody = (RigidBody *) params;
  int s = 0;
  double dm = rigidbody->getMassFlow(); /* loss of mass due to fuel */
  const double origin[3] = {0,0,0};
  gsl_vector_const_view origin_view = gsl_vector_const_view_array(origin,3);
  gsl_vector *thrust_direction = gsl_vector_alloc(3);
  gsl_vector *force = gsl_vector_calloc(3);
  gsl_vector *torque = gsl_vector_calloc(3);
  gsl_vector *body_orientation = gsl_vector_calloc(3);

  memset(dydt,0,20*sizeof(double));

  /* compute force from the combination of gravity, drag, lift, thrust */
  /* gravity */
  gsl_vector *gdir = gsl_vector_calloc(3); /* gravity direction */
  gsl_vector_const_view earthpos = gsl_vector_const_view_array(earth.position,3);
  gsl_vector_const_view rocketpos = gsl_vector_const_view_array(y,3);

  gsl_vector_add(gdir,&earthpos.vector);
  gsl_vector_sub(gdir,&rocketpos.vector);
  const double dist = gsl_blas_dnrm2(gdir);

  const double gforce = gravitiational_constant*y[19]*earth.mass/(dist*dist);
  gsl_vector_scale(gdir,gforce/dist);

  /* add force of gravity after torque */

  /* thrust */
  double Isp;
  gsl_vector_memcpy(thrust_direction,rigidbody->getThrustDirection());
  if(rigidbody->vac_thruster){
    /* specific impulse based on distance from sealevel
     * the karman line begins at 100km
     */
    Isp = normalize(dist - earth.radius,0.0,100000.0);
    /* clamp range */
    if(Isp < 0){
      Isp = 0;
    }else if(Isp > 1){
      Isp = 1;
    }
    Isp = Isp*(max_ISP-min_ISP)+min_ISP;
  }else{
    /* constant specific impulse of stage 2 thruster */
    Isp = merlinvac_isp;
  }
  const double thrust = -9.81*dm*Isp;
  gsl_vector_scale(thrust_direction,thrust);
  gsl_vector_add(force,thrust_direction);
  gsl_vector_free(thrust_direction);

  /* compute torque from thrust */
  double lever[3];
  double orientation[3];
  gsl_vector_const_view com = gsl_vector_const_view_array(rigidbody->getCentreOfMass(),3);
  gsl_vector_view ori_view = gsl_vector_view_array(orientation,3);
  gsl_vector_view lev_view = gsl_vector_view_array(lever,3);
  gsl_matrix_const_view r_view = gsl_matrix_const_view_array(&y[3],3,3);

  /* at this point orientation is just the base of the rocket */
  orientation[0] = com.vector.data[0];
  orientation[2] = com.vector.data[2];

  /* get orientation of rocket */
  gsl_vector_sub(&ori_view.vector,&com.vector);
  gsl_blas_dgemv(CblasNoTrans,1.0,&r_view.matrix,&ori_view.vector,0.0,&lev_view.vector);
  cross_product(&lev_view.vector,force,torque);

  /* get orientation vector of rocket, scoping the variables */
  {
    gsl_vector_sub(body_orientation,&lev_view.vector);
    const double bonrm = gsl_blas_dnrm2(body_orientation);
    gsl_vector_scale(body_orientation,1.0/bonrm);
  }

  /* add force of gravity after torque */
  gsl_vector_add(force,gdir);
  gsl_vector_free(gdir);

  /* TODO: drag */

  /* TODO: lift */

  /* compute dr/dt TODO: turn these into matrix views on stack arrays */
  const gsl_matrix *rotation = &r_view.matrix;
  gsl_matrix *product = gsl_matrix_alloc(3,3);
  gsl_matrix *Iinv = gsl_matrix_alloc(3,3);
  gsl_vector *w = gsl_vector_alloc(3);

  gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, rigidbody->getInertiaTensor(), rotation, 0.0, product);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, rotation, product, 0.0, Iinv);

  gsl_vector_const_view angular_momentum = gsl_vector_const_view_array(&y[15],3);

  /* inertia inversion data */
  gsl_permutation *p = gsl_permutation_alloc(3);

  /* compute inverse of inertia tensor */
  gsl_linalg_LU_decomp(Iinv,p,&s);
  gsl_linalg_LU_solve(Iinv,p,&angular_momentum.vector,w);

  gsl_permutation_free(p);

  gsl_matrix *w_star = RigidBody::star(w);

  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,w_star,rotation,0.0,product);

  gsl_vector_free(w);
  gsl_matrix_free(Iinv);
  gsl_matrix_free(w_star);

  /* set dx/dt as velocity (P/m) */
  for(int i = 0; i < 3; ++i){
    dydt[i] = y[12+i]/y[19];
  }
  memcpy(&dydt[3],product->data,9*sizeof(double));
  memcpy(&dydt[12],force->data,3*sizeof(double)); /* set dP/dt as force */
  memcpy(&dydt[15],torque->data,3*sizeof(double)); /* set dL/dt as torque */
  dydt[19] = dm;

  gsl_matrix_free(product);
  gsl_vector_free(force);
  gsl_vector_free(torque);
  gsl_vector_free(body_orientation);

  return GSL_SUCCESS;
}


RigidBody::RigidBody(const double mass, const double time):
  time(time),
  mass_flow(merlin1d_fuel),
  max_flow(merlin1d_fuel),
  state(gsl_vector_calloc(STATE_SIZE)),
  thrust_direction(gsl_vector_calloc(3)),
  inertia_tensor(gsl_matrix_calloc(3,3))
  {
    /* rotation matrix starts as identity matrix */
    memcpy(&this->state->data[3],identity,9*sizeof(double));

    /* set mass */
    gsl_vector_set(this->state,19,mass);

    /* set thrust direction to be straight up at launch */
    gsl_vector_set(this->thrust_direction,1,1.0);

    this->ode_system = new gsl_odeiv2_system;
    ode_system->function = rigid_body_ode;
    ode_system->jacobian = NULL;
    ode_system->dimension = STATE_SIZE;
    ode_system->params = this;

    this->ode_driver = gsl_odeiv2_driver_alloc_y_new(ode_system, gsl_odeiv2_step_rkf45,
      0.1, 1e-6, 0.0);
    gsl_odeiv2_driver_set_hmax(this->ode_driver,10);
  }

RigidBody::~RigidBody(){
  gsl_vector_free(this->state);
  gsl_vector_free(this->thrust_direction);
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
  const int code = gsl_odeiv2_driver_apply(this->ode_driver,&this->time,this->time + dt,this->state->data);
  nop();
}

gsl_matrix *RigidBody::getInertiaTensor(){
  return this->inertia_tensor;
}

gsl_vector *RigidBody::getThrustDirection(){
  return this->thrust_direction;
}

void RigidBody::setThrustDirection(double direction[]){
  gsl_odeiv2_driver_reset(this->ode_driver);
  memcpy(this->thrust_direction->data,direction,3*sizeof(double));
}

void RigidBody::updateInertiaTensor(double inertia_tensor[]){
  memcpy(this->inertia_tensor->data,inertia_tensor,9*sizeof(double));
}

double RigidBody::getMass(){
  return gsl_vector_get(this->state,19);
}

void RigidBody::print(){
  printf("State: ");
  for(unsigned int i = 0; i < STATE_SIZE; ++i){
    printf("%lf ",this->state->data[i]);
  }
  printf("\n");
}

void RigidBody::throttle(double throttle){
  if(throttle > 1){
    throttle = 1;
  }else if(throttle < 0){
    throttle = 0;
  }
  mass_flow = throttle*max_flow;
}

double RigidBody::getMassFlow(){
  return this->mass_flow;
}

void RigidBody::nextstage(double newmass){
  vac_thruster = true;
  mass_flow = merlinvac_fuel;
  max_flow = mass_flow;
  gsl_vector_set(this->state,19,newmass);
}

double RigidBody::getTime(){
  return this->time;
}

void RigidBody::setCentreOfMass(double com[]){
  memcpy(this->centre_of_mass,com,3*sizeof(double));
}

double *RigidBody::getCentreOfMass(){
  return this->centre_of_mass;
}

gsl_vector *RigidBody::getState(){
  return this->state;
}
