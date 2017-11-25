#include "rocket.hpp"
#include <cstring>

/* dimensions of each stage, stage 0 represents when all stages are part of the
 * rocket (ie. the total), stage n is the value of that stage itself
 */

/* heights in metres of each stage */
static double stage_heights[] = {70,47,12.6};

/* masses of each stage, with total in 0
 * the fuel mass for a stage is the total fuel that can be burned for it
 * ie. a stage can not use more than that much fuel
 * LOX is liquid oxygen, RP1 is rocket-propellant 1
 */
static double stage_mass_empty[] = {26200,22200,4000};
static double stage_mass_LOX[] = {362630,287430,75200};
static double stage_mass_RP1[] = {155870,123570,32300};

Rocket::Rocket(const double dt):
  stage(0),
  dt(dt),
  rigid_body(100.0,0.0),
  radius(3.66/2){
    recomputeInertiaTensor();
  }

Rocket::~Rocket(){

}

void Rocket::step(){
  this->rigid_body.update(this->dt);
  this->recomputeInertiaTensor();
}

void Rocket::recomputeInertiaTensor(){
  double it[9];
  memset(it,0,9*sizeof(double)); /* zero out inertia tensor */

  const double m = this->rigid_body.getMass();
  const double h = stage_heights[stage];

  /* recompute the tensor */
  this->rigid_body.updateInertiaTensor(it);
}
