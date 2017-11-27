#ifndef RSIM_EARTH_HPP
#define RSIM_EARTH_HPP

/* the earth whose centre is one earth radius away from the rocket start position
* ie. the rocket is resting on the earth's surface
*/
struct Earth{
  double position[3];
  double mass = 5.972e24;
  double radius = 6371000;

  Earth(){
    position[0] = 0;
    position[1] = -this->radius;
    position[2] = 0;
  }
};

#endif
