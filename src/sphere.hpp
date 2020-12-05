#ifndef _CYLAKS_SPHERE_H_
#define _CYLAKS_SPHERE_H_
#include "object.hpp"

class Sphere : public Object {
private:
  double radius_{0.0};

public:
  Sphere(size_t sid, size_t id, double radius)
      : Object(sid, id), radius_{radius} {}
};
#endif