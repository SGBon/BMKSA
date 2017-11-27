#ifndef RSIM_COMMON_HPP
#define RSIM_COMMON_HPP
/* common functions and definitions in RocketSim */
const double gravitiational_constant = 6.67408e-11;

const double max_ISP = 307.4; /* specific impulse in vacuum of stage 1 engine */
const double min_ISP = 281.8; /* sealevel */
const double merlinvac_isp = 348.0; /* stage 2 engine ISP */

/* fuel consumption of stage 1 and 2 */
const double merlin1d_fuel = -273.6*9;
const double merlinvac_fuel = -273.6;

/* low earth orbit */
const double LEO = 2000000;

const double identity[9] =
            { 1,0,0,
              0,1,0,
              0,0,1};

/* normalize a value between min and max */
double normalize(double x, double min, double max);

/* get the velocity necessary to orbit at a certain radius from a planet's core */
double orbital_velocity(double mass, double radius);

#endif
