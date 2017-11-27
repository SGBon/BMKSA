#include "rocket.hpp"
#include <cstring>
#include <cmath>
#include <cstdio>

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

/* dimensions of each stage, stage 0 represents when all stages are part of the
 * rocket (ie. the total), stage n is the value of that stage itself
 */

/* heights in metres of each stage */
static const double stage_heights[] = {70,47,12.6};

/* masses of each stage, with total in 0
 * the fuel mass for a stage is the total fuel that can be burned for it
 * ie. a stage can not use more than that much fuel
 */
static const double stage_mass_empty[] = {26200,22200,4000};
static const double stage_mass_fuel[] = {507500,398887,108185};

const double identity[9] =
            { 1,0,0,
              0,1,0,
              0,0,1};

Rocket::Rocket(const double dt):
  stage(1),
  dt(dt),
  rigid_body(stage_mass_empty[0] + stage_mass_fuel[0],0.0),
  radius(3.66/2){
    recomputeCentreMass();

    /* compute second stage inertia tensor */
    memset(inertia_tensor_s2,0,9*sizeof(double));
    double m2 = stage_mass_empty[2] + stage_mass_fuel[2];
    inertia_tensor_s2[0] = m2*(radius*radius*3.0 + stage_heights[2]*stage_heights[2])/12.0;
    inertia_tensor_s2[4] = inertia_tensor_s2[0];
    inertia_tensor_s2[8] = m2*radius*radius/2;

    recomputeInertiaTensor();
  }

Rocket::~Rocket(){

}

void Rocket::step(){
  this->rigid_body.update(this->dt);
  double fuel_in_stage = 0;
  if(stage == 1){
    fuel_in_stage = this->rigid_body.getMass() - stage_mass_empty[0] - stage_mass_fuel[2];
    if(fuel_in_stage <= 0.0){
      this->nextstage();
    }
  }else if (stage == 2){
    fuel_in_stage = this->rigid_body.getMass() - stage_mass_empty[2];
    if(fuel_in_stage <= 0.0){
      this->rigid_body.throttle(0.0);
    }
  }
  this->recomputeCentreMass();
  this->recomputeInertiaTensor();
}

void Rocket::print(){
  this->rigid_body.print();
}

void Rocket::recomputeInertiaTensor(){
  double it[9];

  memset(it,0,9*sizeof(double)); /* zero out inertia tensor */

  /* first stage inertia tensor changes, second stage remains same */
  if(stage == 1){
    memset(inertia_tensor_s1,0,9*sizeof(double));
    const double m2 = stage_mass_empty[2] + stage_mass_fuel[2];
    const double m1 = this->rigid_body.getMass() - m2;
    const double h1 = stage_heights[1];
    inertia_tensor_s1[0] = m1*(3.0*radius*radius + h1*h1)/12.0;
    inertia_tensor_s1[4] = inertia_tensor_s1[0];
    inertia_tensor_s1[8] = m1*radius*radius/2.0;

    double stage1_empty_centre[3] = {radius,stage_heights[1]/2.0,radius};
    double stage2_centre[3] = {radius,stage_heights[0] - (stage_heights[2]/2.0),radius};

    gsl_matrix_view itview = gsl_matrix_view_array(it,3,3);
    gsl_matrix_view its1 = gsl_matrix_view_array(inertia_tensor_s1,3,3);
    gsl_matrix_view its2 = gsl_matrix_view_array(inertia_tensor_s2,3,3);

    gsl_matrix_add(&itview.matrix,&its1.matrix);
    gsl_matrix_add(&itview.matrix,&its2.matrix);

    gsl_matrix_view coms1 = gsl_matrix_view_array(stage1_empty_centre,3,1);
    gsl_matrix_view coms2 = gsl_matrix_view_array(stage2_centre,3,1);
    gsl_matrix_const_view comview = gsl_matrix_const_view_array(centre_of_mass,3,1);

    /* compute dyads of centre of mass offsets */
    gsl_matrix_sub(&coms1.matrix,&comview.matrix);
    gsl_matrix_sub(&coms2.matrix,&comview.matrix);

    gsl_matrix *s1dyad = gsl_matrix_alloc(3,3);
    gsl_matrix *s2dyad = gsl_matrix_alloc(3,3);

    gsl_blas_dgemm(CblasNoTrans,CblasTrans,1.0,&coms1.matrix,&coms1.matrix,0.0,s1dyad);
    gsl_blas_dgemm(CblasNoTrans,CblasTrans,1.0,&coms2.matrix,&coms2.matrix,0.0,s2dyad);

    double dots1 = 0.0;
    double dots2 = 0.0;
    for(unsigned int i = 0; i < 3; ++i){
      dots1 += stage1_empty_centre[i]*stage1_empty_centre[i];
      dots2 += stage2_centre[i]*stage2_centre[i];
    }

    /* compute inertia tensor COM offsets */
    gsl_matrix *s1sum = gsl_matrix_alloc(3,3);
    gsl_matrix *s2sum = gsl_matrix_alloc(3,3);
    gsl_matrix_const_view iview = gsl_matrix_const_view_array(identity,3,3);

    memcpy(s1sum->data,&iview.matrix.data,9*sizeof(double));
    memcpy(s2sum->data,&iview.matrix.data,9*sizeof(double));

    /* finally sum up the mass contribution of each body into the inertia tensor */
    gsl_matrix_scale(s1sum,dots1);
    gsl_matrix_sub(s1sum,s1dyad);
    gsl_matrix_scale(s1sum,m1);
    gsl_matrix_add(&itview.matrix,s1sum);

    gsl_matrix_scale(s2sum,dots2);
    gsl_matrix_sub(s2sum,s2dyad);
    gsl_matrix_scale(s2sum,m2);
    gsl_matrix_add(&itview.matrix,s2sum);

    gsl_matrix_free(s2sum);
    gsl_matrix_free(s1sum);
    gsl_matrix_free(s2dyad);
    gsl_matrix_free(s1dyad);
  }
  /* no first stage anymore, second stage tensor changes */
  else if(stage == 2){
    const double h2 = stage_heights[2];
    const double m2 = this->rigid_body.getMass();
    it[0] = m2*(3.0*radius*radius+h2*h2)/12.0;
    it[4] = it[0];
    it[8] = m2*radius*radius/12.0;
  }

  this->rigid_body.updateInertiaTensor(it);
}

void Rocket::recomputeCentreMass(){
  if(stage == 1){
    /* stage2 is uniformly distributed mass of rocket and fuel during stage 1,
     * so the centre is right where the physical centre of stage 2
     */
    double stage2_centre[3] = {radius,stage_heights[0] - (stage_heights[2]/2),radius};
    const double stage2_mass = stage_mass_empty[2] + stage_mass_fuel[2];

    /* stage 1 contains empty portion and fuel, so TODO: centre of mass will
     * be between these 2 quantities
     */
    double stage1_empty_centre[3] = {radius,stage_heights[1]/2.0,radius};
    const double stage1_mass = this->rigid_body.getMass() - stage2_mass;

    memset(this->centre_of_mass,0,3*sizeof(double));
    gsl_vector_view COMview = gsl_vector_view_array(this->centre_of_mass,3);
    gsl_vector_view s2_com = gsl_vector_view_array(stage2_centre,3);
    gsl_vector_view s1_com = gsl_vector_view_array(stage1_empty_centre,3);

    gsl_vector_scale(&s2_com.vector,stage2_mass);
    gsl_vector_scale(&s1_com.vector,stage1_mass);

    gsl_vector_add(&COMview.vector,&s2_com.vector);
    gsl_vector_add(&COMview.vector,&s1_com.vector);

    gsl_vector_scale(&COMview.vector,1.0/(stage1_mass+stage2_mass));
  }else if(stage == 2){
    double stage2_empty_centre[3] = {radius,stage_heights[2]/2,radius};
    const double stage2_mass = this->rigid_body.getMass();
    memcpy(this->centre_of_mass,stage2_empty_centre,3*sizeof(double));
  }
}

void Rocket::nextstage(){
  printf("Fuel in %d ran out, staging\n",stage);
  rigid_body.nextstage(stage_mass_fuel[2]+stage_mass_empty[2]);
  ++stage;
}
