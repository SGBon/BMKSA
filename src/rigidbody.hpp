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

  gsl_matrix *getInertiaTensor();

private:
  double time;
  gsl_vector *state;
  gsl_matrix *inertia_tensor;

  gsl_odeiv2_driver *ode_driver;
};

#endif /*header guard */
