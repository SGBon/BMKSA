#ifndef RSIM_RIGIDBODY_HPP
#define RSIM_RIGIDBODY_HPP
/* collisionless rigid bodies in the rocket model */

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>

class RigidBody{
public:
  RigidBody(const double mass);

  ~RigidBody();

  void update(double t);

  /* compute the star of angular velocity given as a vector
   * this matrix should be freed using gsl_matrix_free()
   */
  static gsl_matrix *star(gsl_vector *vector);

private:
  double mass;
  gsl_vector *state;
  gsl_vector *force;
  gsl_vector *torque;

  gsl_odeiv2_system ode_system;
};

#endif /*header guard */
