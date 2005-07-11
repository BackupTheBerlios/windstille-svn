/*
 * Copyright (c) 2005 by the Windstille team. All rights reserved.
 *
 * collision.hxx
 * by David Kamphausen (david.kamphausen@web.de)
 *    Ingo Ruhnke
 *
 * The "Windstille" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */

#ifndef HEADER_COLLISION_HXX
#define HEADER_COLLISION_HXX

#include <ClanLib/Core/Math/cl_vector.h>

#include <iostream>
#include <list>
#include <vector>

#include <ClanLib/Core/Math/rect.h>

using namespace std;

class CollisionObject;
class CollRect;

struct CollPrimitive
{
protected:
  CollisionObject *object;
public:
  CollPrimitive(CollisionObject *object_);
  virtual ~CollPrimitive();
  virtual CollPrimitive *clone(CollisionObject *o) const=0;

  virtual float x_pos() const=0;
  virtual float y_pos() const=0;
  virtual float width() const=0;
  virtual float height() const=0;

  float x_velocity() const;
  float y_velocity() const;
  CL_Vector get_velocity() const;

  virtual void drawCollision() const=0;

  friend std::ostream& operator<<(std::ostream& out, const CollRect &b);

};


class CollRect:public CollPrimitive
{
  CL_Rectf rect;

public:
  CollRect(const CL_Rectf &r_,CollisionObject *object_);

  virtual CollPrimitive *clone(CollisionObject *o) const;
  float x_pos() const;
  float y_pos() const;
  float width() const;
  float height() const;

  CL_Vector get_vector0() const;
  CL_Vector get_vector1() const;
  CL_Vector get_vector2() const;
  CL_Vector get_vector3() const;

  virtual void drawCollision() const;
};

inline std::ostream& operator<<(std::ostream& out, const CollRect &b);

class CollTri:public CollPrimitive
{
  CL_Vector base;

  float dx;
  float dy;

public:
  CollTri(CollisionObject *object_);
  CollTri(const CL_Vector &base_, float w_, float h_,CollisionObject *object_);
  virtual CollPrimitive *clone(CollisionObject *o) const;

  float x_pos() const;
  float y_pos() const;
  float width() const;
  float height() const;

  // normal - normalized!
  CL_Vector normal() const;

  CL_Vector get_vector0() const;
  CL_Vector get_vector1() const;
  CL_Vector get_vector2() const;

  virtual void drawCollision() const;
};

struct CollisionData
{
  enum State {NONE,STUCK,COLLISION};
  // points into direction from where the other object came
  CL_Vector direction;
  
  State state;

  // size of time frame
  float delta; 

  // time of collision
  float col_time;

  CollisionData()
  {
    state=NONE;
    col_time=0;
  }

  CollisionData merge(const CollisionData &r);

  CollisionData invert() const
  {
    CollisionData r(*this);
    r.direction*=-1;

    return r;
  }

  
};
inline std::ostream &operator<<(std::ostream &o,const CollisionData &r)
{
  o<<"("<<r.direction.x<<","<<r.direction.y<<","<<r.col_time<<")";
  return o;
}






#endif
