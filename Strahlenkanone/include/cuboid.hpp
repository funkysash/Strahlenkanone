/* 
 * File:   cuboid.hpp
 * Author: sascha
 *
 * Created on 28. Februar 2010, 16:49
 */

#ifndef _CUBOID_HPP
#define	_CUBOID_HPP

#include <shape.hpp>
#include <ray.hpp>
#include <shade.hpp>
#include "point3d.hpp"

//Axis Aligned Box = achsenparalleler Quader
class cuboid : public shape
{
public:
  cuboid(point3d a, point3d b, material m);
  cuboid(double x1,double y1,double z1,double x2,double y2,double z2, material const& m);
  ~cuboid();
  bool intersect(ray r, shade& rec) const;

  void bbox();

private:
  point3d fll_; // front lower left
  point3d bur_; // back upper right
};

#endif	/* _CUBOID_HPP */

