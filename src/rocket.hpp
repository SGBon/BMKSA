#ifndef RSIM_ROCKET_HPP
#define RSIM_ROCKET_HPP
/* rocket class based on the SpaceX Falcon 9 */
#include "rigidbody.hpp"
#include <glm/glm.hpp>

class Rocket{
public:
  Rocket(const double dt);
  ~Rocket();

  void step();

  void print();

  glm::vec4 getPositionGLM();

  glm::vec4 getThrustDirectionGLM();

private:
  unsigned int stage; /* stage rocket is on */
  const double dt;
  double centre_of_mass[3];
  double inertia_tensor_s2[9];
  double inertia_tensor_s1[9];
  RigidBody rigid_body;

  const double radius;

  enum stage_progress{
    S1LAUNCH,
    S1ASCENT,
    S1COURSE,
    S2SPLIT,
    S2BURN
  } stage_progress;

  double target_orbital_velocity;

  void recomputeInertiaTensor();

  void recomputeCentreMass();

  void nextstage();
};

#endif
