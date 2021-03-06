#include "cuboid.hpp"
#include "point3d.hpp"
#include <vector3d.hpp>
#include <cassert>
#include <algorithm>
#include <matrix.hpp>
#include <iostream>

cuboid::cuboid(point3d a, point3d b, material m)
: fll_(), bur_(), shape(m)
{
  fll_[0] = std::min(a[0], b[0]);
  fll_[1] = std::min(a[1], b[1]);
  fll_[2] = std::min(a[2], b[2]);
  bur_[0] = std::max(a[0], b[0]);
  bur_[1] = std::max(a[1], b[1]);
  bur_[2] = std::max(a[2], b[2]);
  bbox();
}

cuboid::cuboid(double x1,double y1,double z1,double x2,double y2,double z2, material const& m)
: fll_(point3d(x1,y1,z1)), bur_(point3d(x2,y2,z2)), shape(m)
{
  fll_[0] = std::min(x1, x2);
  fll_[1] = std::min(y1, y2);
  fll_[2] = std::min(z1, z2);
  bur_[0] = std::max(x1, x2);
  bur_[1] = std::max(y1, y2);
  bur_[2] = std::max(z1, z2);
  bbox();
}

cuboid::~cuboid()
{
}

bool
cuboid::intersect(ray r, shade& rec) const
{
  if(is_tformed()) //Wenn wir eine Transformationsmatrix haben
  {
    matrix to = gettformi();
    r.ori = to * r.ori;     //transformation auf den Ray anwenden
    r.dir = to * r.dir;
    r.dir.normalize();
  }
  //Begrenzung des Richtungsvektors auf den Wertebereich des Cuboid durch
  //Einschränkung der Faktoren für jede Koordinate einzeln
  //Speicherung des max. und min. Faktors für jede Koordinate in Vektor
  vector3d tmin(0.0, 0.0, 0.0);
  vector3d tmax(0.0, 0.0, 0.0);
  //für x:
  //Division durch Richtungsvektor um Faktor für diesen zu erhalten
  double x_inv = 1.0/ r.dir[0];
  //Unterscheidung aus welcher Richtung wir kommen

  if(x_inv >= 0) // "von vorne"
  {
    //der kleinste Faktor ist der Abstand zum Cuboid durch den Vektor(mal x_inv)
    //um Faktor zu erhalten
    tmin[0] = (fll_[0] - r.ori[0]) * x_inv;
    //der größte is der Abstand zur "Rückwand"
    tmax[0] = (bur_[0] - r.ori[0]) * x_inv;
  }
  else //"von hinten"
  {
    //wie oben nur das Rückseite näher am Strahlursprung ist
    tmin[0] =  (bur_[0] - r.ori[0]) * x_inv;
    //und Vorderseite weiter weg
    tmax[0] =  (fll_[0] - r.ori[0]) * x_inv;
  }

  //für y:
  double y_inv = 1.0/ r.dir[1];
  if(y_inv >= 0)
  {
    tmin[1] = (fll_[1] - r.ori[1]) * y_inv;
    tmax[1] = (bur_[1] - r.ori[1]) * y_inv;
  }
  else
  {
    tmin[1] =  (bur_[1] - r.ori[1]) * y_inv;
    tmax[1] =  (fll_[1] - r.ori[1]) * y_inv;
  }
  //für z:
  double z_inv = 1.0/ r.dir[2];
  if(z_inv >= 0)
  {
    tmin[2] = (fll_[2] - r.ori[2]) * z_inv;
    tmax[2] = (bur_[2] - r.ori[2]) * z_inv;
  }
  else
  {
    tmin[2] =  (bur_[2] - r.ori[2]) * z_inv;
    tmax[2] =  (fll_[2] - r.ori[2]) * z_inv;
  }


  //Bestimmung der Schnittseite -> Normalenvektor:
  //größter Faktor in tmin = Faktor beim Betreten der Box
  double tminmax = std::max(tmin[0], std::max(tmin[1], tmin[2]));
  //kleinster Faktor in tmax = Faktor beim Verlassen
  double tmaxmin = std::min(tmax[0], std::min(tmax[1], tmax[2]));

  point3d p(0,0,0);
  vector3d normal(0,0,0);
  if(tmaxmin > 0.01 && tminmax < tmaxmin) //Überlappen sich die slopes?
  {                                    //= Strahl schneidet Box
    if(tminmax > 0.001) //Strahl kommt von außerhalb
    {
      p = r.ori + (tminmax * r.dir);
      //Bestimmung des Normalenvektors:
      if(tmin[0] == tminmax && x_inv >= 0.0){normal[0] = -1;}     // -x
      if(tmin[0] == tminmax && x_inv < 0.0){normal[0] =   1;}     // +x
      if(tmin[1] == tminmax && y_inv >= 0.0){normal[1] = -1;}     // -y
      if(tmin[1] == tminmax && y_inv < 0.0){normal[1] =   1;}     // +y
      if(tmin[2] == tminmax && z_inv >= 0.0){normal[2] = -1;}     // -z
      if(tmin[2] == tminmax && z_inv < 0.0){normal[2] =   1;}     // +z
    }
    else  //Strahlursprung liegt in der Box
    {
      p = r.ori + (tmaxmin * r.dir);
      //Bestimmung des Normalenvektors:
      //wie oben nur umgedreht, da der Stehl die Box verlässt und der normalen
      //Vektor damit in die Box zeigt
      if(tmin[0] == tminmax && x_inv >= 0.0){normal[0] = 1;}   //   +x
      if(tmin[0] == tminmax && x_inv < 0.0){normal[0] = -1;}     // -x
      if(tmin[1] == tminmax && y_inv >= 0.0){normal[1] = 1;}  //    +y
      if(tmin[1] == tminmax && y_inv < 0.0){normal[1] = -1;}    //  -y
      if(tmin[2] == tminmax && z_inv >= 0.0){normal[2] = 1;}  //    +z
      if(tmin[2] == tminmax && z_inv < 0.0){normal[2] = -1;}    //  -z
    }
    //das heißt noch lange nicht, das es der naheliegendste Schnittpunkt ist
    if(distance(r.ori, p) < rec.distance)
    {
      rec.didhit = true; //Juchu getroffen
      rec.material_ref = getmater();
      rec.hitpoint = p;
      if(is_tformed())
      {
        matrix fro = gettform();
        //Tranformation des Hitpoints
        rec.hitpoint = fro * rec.hitpoint;
        //Rücktransformation der Normalen mit der Transponierten
        fro.transpose();
        normal = fro * normal;
        normal.normalize();
      }
      rec.hitpoint = rec.hitpoint + 0.01*normal; //minimal Verschiebung verhindert Schnitt mit sich selbst
      rec.n = normal;
      rec.distance = distance(r.ori, p);
      return (true);
    }
  }
 return (false);
}

void
cuboid::bbox()
{
  ppp temp;
  temp.first = fll_;
  temp.second = bur_;
  if(is_tformed()) //Müssen wir transformieren
  {
    matrix to = gettform();
    temp.first = to * temp.first;
    temp.second = to * temp.second;
  }
  setbbox(temp);
}