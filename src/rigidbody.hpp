#ifndef RSIM_RIGIDBODY_HPP
#define RSIM_RIGIDBODY_HPP
/* collisionless rigid bodies in the rocket model */

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>

class RigidBody{
public:
  RigidBody(const double mass, const double time);

  ~RigidBody();

  void update(const double dt);

  /* compute the star of angular velocity given as a vector
   * this matrix should be freed using gsl_matrix_free()
   */
  static gsl_matrix *star(gsl_vector *vector);

  /* 20 state variables */
  static const unsigned int STATE_SIZE = 20;

  gsl_matrix const *getInertiaTensor() const;

  gsl_vector const *getThrustDirection() const;

  void setThrustDirection(double direction[]);

  void updateInertiaTensor(double inertia_tensor[]);

  double getMass();

  void print();

  /* set speed and fuel consumption */
  void throttle(double throttle);

  double getMassFlow() const;

  /* make changes for stage 2 */
  void nextstage(double newmass);

  bool vac_thruster = false;

  double getTime();

  void setCentreOfMass(double com[]);

  double const *getCentreOfMass() const;

  gsl_vector const *getState() const;

private:
  double time;
  double mass_flow; /* consumption of fuel in kg/s */
  double max_flow;
  double centre_of_mass[3];
  gsl_vector *state;
  gsl_vector *thrust_direction;
  gsl_matrix *inertia_tensor;

  gsl_odeiv2_system *ode_system;
  gsl_odeiv2_step *ode_step;
};

#endif /*header guard */
