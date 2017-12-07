#include "rigidbody.hpp"

#include <cassert>
#include <cstring>
#include <cstdio>
#include <iostream>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

#include "common.hpp"
#include "earth.hpp"

static const size_t STATE_POSITION_START = 0;
static const size_t STATE_POSITION_SIZE = 3;
static const size_t STATE_ROTATION_START = 3;
static const size_t STATE_ROTATION_SIZE = 9;
static const size_t STATE_LINEAR_MOMENTUM_START = 12;
static const size_t STATE_LINEAR_MOMENTUM_SIZE = 3;
static const size_t STATE_ANGULAR_MOMENTUM_START = 15;
static const size_t STATE_ANGULAR_MOMENTUM_SIZE = 3;
static const size_t STATE_MASS = 19;

static int rigid_body_ode(double t, const double y[], double dydt[], void *params){
  RigidBody const *rigidbody = (RigidBody *) params;
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

  const double gforce = gravitiational_constant*y[STATE_MASS]*earth.mass/(dist*dist);
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
  const double thrust = -9.81*dm*Isp; // todo: add more sig digs to gravity? [@Kathryn]
  gsl_vector_scale(thrust_direction,thrust);
  gsl_vector_add(force,thrust_direction);
  gsl_vector_free(thrust_direction);

  /* compute torque from thrust */
  double lever[3];
  double orientation[3];
  gsl_vector_const_view com = gsl_vector_const_view_array(rigidbody->getCentreOfMass(),3);
  gsl_vector_view ori_view = gsl_vector_view_array(orientation,3);
  gsl_vector_view lev_view = gsl_vector_view_array(lever,3);
  gsl_matrix_const_view r_view = gsl_matrix_const_view_array(&y[STATE_ROTATION_START],3,3);

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
  const double drag_coefficient = -0.05;
  gsl_vector *drag = gsl_vector_alloc(3);
  memcpy(drag->data,&y[12],3*sizeof(double));
  gsl_vector_scale(drag,drag_coefficient/y[19]);
  gsl_vector_add(force,drag);

  /* TODO: lift */

  /* compute dr/dt TODO: turn these into matrix views on stack arrays */
  const gsl_matrix *rotation = &r_view.matrix;
  gsl_matrix *product = gsl_matrix_alloc(3,3);
  gsl_matrix *Ibody = gsl_matrix_calloc(3,3);
  gsl_matrix *Iinv = gsl_matrix_alloc(3,3);
  gsl_vector *w = gsl_vector_alloc(3);

  gsl_matrix_memcpy(Ibody,rigidbody->getInertiaTensor());
  gsl_matrix_set(Ibody,0,0,1.0/Ibody->data[0]);
  gsl_matrix_set(Ibody,1,1,1.0/Ibody->data[4]);
  gsl_matrix_set(Ibody,2,2,1.0/Ibody->data[8]);

  gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, Ibody, rotation, 0.0, product);
  gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, rotation, product, 0.0, Iinv);

    /*
  printf("Iinv: [");
  for(int i = 0; i < 9; ++i){
    if(i % 3 == 0){
        printf("\n");
    }
    printf("%lf ",rigidbody->getInertiaTensor()->data[i]);
  }
  printf("] \n");
    */

  gsl_vector_const_view angular_momentum = gsl_vector_const_view_array(&y[STATE_ANGULAR_MOMENTUM_START],STATE_ANGULAR_MOMENTUM_SIZE);

  gsl_blas_dgemv(CblasNoTrans,1.0,Iinv,&angular_momentum.vector,0.0,w);

  gsl_matrix *w_star = RigidBody::star(w);

  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,w_star,rotation,0.0,product);

  gsl_vector_free(w);
  gsl_matrix_free(Iinv);
  gsl_matrix_free(Ibody);
  gsl_matrix_free(w_star);

  /* set dx/dt as velocity (P/m) */
  for(int i = 0; i < STATE_LINEAR_MOMENTUM_SIZE; ++i){
    dydt[i] = y[STATE_LINEAR_MOMENTUM_START+i]/y[STATE_MASS];
  }
  memcpy(&dydt[STATE_ROTATION_START],product->data,STATE_ROTATION_SIZE*sizeof(double));
  memcpy(&dydt[STATE_LINEAR_MOMENTUM_START],force->data,STATE_LINEAR_MOMENTUM_SIZE*sizeof(double)); /* set dP/dt as force */
  memcpy(&dydt[STATE_ANGULAR_MOMENTUM_START],torque->data,STATE_ANGULAR_MOMENTUM_SIZE*sizeof(double)); /* set dL/dt as torque */
  dydt[STATE_MASS] = dm;

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

    this->ode_step = gsl_odeiv2_step_alloc(gsl_odeiv2_step_rkf45, STATE_SIZE);
    //gsl_odeiv2_driver_set_hmax(this->ode_driver,10);
  }

RigidBody::~RigidBody(){
  gsl_vector_free(this->state);
  gsl_vector_free(this->thrust_direction);
  gsl_matrix_free(this->inertia_tensor);

  gsl_odeiv2_step_free(this->ode_step);
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
  // ODE
  double error[STATE_SIZE];
  const int code = gsl_odeiv2_step_apply(this->ode_step,this->time,dt,this->state->data, error, NULL, NULL, this->ode_system);
  this->time += dt;

    /*
  printf("Error: ");
  for(unsigned int i = 0; i < STATE_SIZE; ++i){
    printf("%lf ",error[i]);
  }
  printf("\n");
   */

  // throw in a switch statement here ok
  switch(code) {
    case GSL_SUCCESS:
      //std::cout << "successfully" << std::endl;
      break;

    case GSL_FAILURE:
      std::cerr << "rigidbody update: GSL_FAILURE" << std::endl;
      break;

    case GSL_EFAULT:
      std::cerr << "rigidbody update: A fault in your e" << std::endl;
      break;

    case GSL_EBADFUNC:
      std::cerr << "rigidbody update: Oh my, that's a bad func in your e" << std::endl;
      break;

    default:
      assert(false);
      break;
  }

  //this->state->data = result;
  nop();
}

gsl_matrix const*RigidBody::getInertiaTensor() const {
  return this->inertia_tensor;
}

gsl_vector const *RigidBody::getThrustDirection() const {
  return this->thrust_direction;
}

void RigidBody::setThrustDirection(double direction[]){
  memcpy(this->thrust_direction->data,direction,3*sizeof(double));
  gsl_odeiv2_step_reset(this->ode_step);
}

void RigidBody::updateInertiaTensor(double inertia_tensor[]){
  memcpy(this->inertia_tensor->data,inertia_tensor,9*sizeof(double));
  gsl_odeiv2_step_reset(this->ode_step);
}

double RigidBody::getMass(){
  return gsl_vector_get(this->state,19);
}


void RigidBody::print(bool use_spreadsheet) {
  if(use_spreadsheet) {
    printSpreadsheetStyle();
  } else {
    printDefaultStyle();
  }
}

void RigidBody::printSpreadsheetStyle(){
  printf("%lfs ", time);
  // print position
  for(unsigned int i = 0; i < STATE_POSITION_SIZE; ++i){
    printf("%lf ",this->state->data[STATE_POSITION_START+i]);
  }

  // print rotation
  //for(unsigned int i = 0; i < STATE_ROTATION_SIZE; ++i){
  //  printf("%lf ",this->state->data[STATE_ROTATION_START+i]);
  //}

  // print linear momentum
  for(unsigned int i = 0; i < STATE_LINEAR_MOMENTUM_SIZE; ++i){
    printf("%lf ",this->state->data[STATE_LINEAR_MOMENTUM_START+i]);
  }

  // print angular momentum
  for(unsigned int i = 0; i < STATE_ANGULAR_MOMENTUM_SIZE; ++i){
    printf("%lf ",this->state->data[STATE_ANGULAR_MOMENTUM_START+i]);
  }

  // print mass
  printf("%lf\n", this->state->data[STATE_MASS]);
}

void RigidBody::printDefaultStyle(){
  printf("T=%lfs: \n  position: ", time);
  // print position
  for(unsigned int i = 0; i < STATE_POSITION_SIZE; ++i){
    printf("%lf ",this->state->data[STATE_POSITION_START+i]);
  }

  // print rotation
  printf("\n  rotation: ");
  for(unsigned int i = 0; i < STATE_ROTATION_SIZE; ++i){
    if(i % 3 == 0) {
      printf("\n            ");
    }
    printf("%lf ",this->state->data[STATE_ROTATION_START+i]);
  }

  // print linear momentum
  printf("\nLinear momentum: ");
  for(unsigned int i = 0; i < STATE_LINEAR_MOMENTUM_SIZE; ++i){
    printf("%lf ",this->state->data[STATE_LINEAR_MOMENTUM_START+i]);
  }

  // print angular momentum
  printf("\nAngular momentum: ");
  for(unsigned int i = 0; i < STATE_ANGULAR_MOMENTUM_SIZE; ++i){
    printf("%lf ",this->state->data[STATE_ANGULAR_MOMENTUM_START+i]);
  }

  // print mass
  printf("mass: %lf\n", this->state->data[STATE_MASS]);
}

void RigidBody::throttle(double throttle){
  if(throttle > 1){
    throttle = 1;
  }else if(throttle < 0){
    throttle = 0;
  }
  mass_flow = throttle*max_flow;
  gsl_odeiv2_step_reset(this->ode_step);
}

double RigidBody::getMassFlow() const {
  return this->mass_flow;
}

void RigidBody::nextstage(double newmass){
  vac_thruster = true;
  mass_flow = merlinvac_fuel;
  max_flow = mass_flow;
  gsl_vector_set(this->state,19,newmass);
  gsl_odeiv2_step_reset(this->ode_step);
}

double RigidBody::getTime(){
  return this->time;
}

void RigidBody::setCentreOfMass(double com[]){
  memcpy(this->centre_of_mass,com,3*sizeof(double));
  gsl_odeiv2_step_reset(this->ode_step);
}

double const* RigidBody::getCentreOfMass() const {
  return this->centre_of_mass;
}

gsl_vector const *RigidBody::getState() const {
  return this->state;
}
