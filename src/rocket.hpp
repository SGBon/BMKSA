#ifndef RSIM_ROCKET_HPP
#define RSIM_ROCKET_HPP
/* rocket class based on the SpaceX Falcon 9 */
#include "rigidbody.hpp"
#include <glm/glm.hpp>

class Rocket{
public:
  Rocket(const double dt);
  ~Rocket();

  enum stage_progress{
      S1LAUNCH,
      S1ASCENT,
      S1COURSE,
      S2SPLIT,
      S2BURN,
      NUMBER_OF_STAGES // keep track of stage progress size
    } stage_progress;

  void step();

  void print(bool use_spreadsheet=false);

  glm::vec4 getPositionGLM();

  glm::vec4 getThrustDirectionGLM();

  unsigned int getStageProgress();

private:
  unsigned int stage; /* stage rocket is on */
  const double dt;
  double centre_of_mass[3];
  double inertia_tensor_s2[9];
  double inertia_tensor_s1[9];
  RigidBody rigid_body;

  const double radius;

  double target_orbital_velocity;

  void recomputeInertiaTensor();

  void recomputeCentreMass();

  void nextstage();
};

#endif
