#ifndef RSIM_ROCKET_HPP
#define RSIM_ROCKET_HPP
/* rocket class based on the SpaceX Falcon 9 */
#include "rigidbody.hpp"

class Rocket{
public:
  Rocket(const double dt);
  ~Rocket();

  void step();

private:
  unsigned int stage; /* stage rocket is on */
  const double dt;
  RigidBody rigid_body;

  void recomputeInertiaTensor();
};

#endif