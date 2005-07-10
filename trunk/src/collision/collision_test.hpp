/*
 * Copyright (c) 2005 by the Windstille team. All rights reserved.
 *
 * collision_test.hxx
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

#ifndef HEADER_COLLTEST_HXX
#define HEADER_COLLTEST_HXX

#define MAXF 100000.0f

#include <iostream>
#include <ClanLib/Core/Math/vector2.h>

struct SweepResult
{
  enum CollisionState { COL_AT, COL_ALWAYS, COL_NEVER };
  CollisionState state;
  float t0,t1;

  SweepResult()
  {
    state=COL_NEVER;
  }

  bool collision(float delta) const
  {
    return state==COL_ALWAYS || (state==COL_AT && t1>0 && t0<delta);
  }

  float begin(float delta) const
  {
    if(state==COL_ALWAYS || state==COL_NEVER)
      return MAXF;

    if(t0>=0.0f && t0<delta)
      return t0;
    else
      return MAXF;
  }

  bool always() const
  {
    return state==COL_ALWAYS;
  }

};

inline std::ostream &operator<<(std::ostream &o,const SweepResult &r)
{
  o<<"("<<r.state<<","<<r.t0<<","<<r.t1<<")";
  return o;
}


SweepResult simple_sweep_1d(float a, float aw, float av,
			    float b, float bw, float bv);

class CollRect;
class CollTri;

struct InstantCollisionResult
{
  float depth;
  CL_Vector2 direction;
  bool collision;

  InstantCollisionResult()
  {
    collision=false;
  }
};

void collideRectRect(InstantCollisionResult &result,const CollRect &a,const CollRect &b);
void collideRectTri(InstantCollisionResult &result,const CollRect &a,const CollTri &b);
void collideTriTri(InstantCollisionResult &result,const CollTri &a,const CollTri &b);


#endif
