/* 
 * File:   point3d.hpp
 * Author: sascha
 *
 * Created on 27. Oktober 2009, 15:56
 */

#ifndef _POINT3D_HPP
#define	_POINT3D_HPP

class vector3d;

class point3d
{
public:
    //c'tors
    point3d();
    point3d(double x, double y, double z);

    //d'tor
    ~point3d();

    //Indexoperatoren
    double& operator[](unsigned index);
    double const& operator[](unsigned index) const;


private:
    double data_[4];  //3+1 für homogene Koordinaten
};

double distance(point3d const& lhs, point3d const& rhs);

point3d  operator+(point3d const& lhs, vector3d const& rhs);
point3d  operator-(point3d const& lhs, vector3d const& rhs);

#endif	/* _POINT3D_HPP */

