#include "rocket.hpp"

Rocket::Rocket(const double dt):
  stage(0),
  dt(dt),
  rigid_body(100.0,0.0){}

Rocket::~Rocket(){

}

void Rocket::step(){
  this->rigid_body.update(this->dt);
  this->recomputeInertiaTensor();
}

void Rocket::recomputeInertiaTensor(){
  double y[9];

  /* recompute the tensor */
  this->rigid_body.updateInertiaTensor(y);
}
