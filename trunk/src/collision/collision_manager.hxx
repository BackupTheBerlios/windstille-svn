//  $Id$
// 
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_COLLISION_MANAGER_HXX
#define HEADER_COLLISION_MANAGER_HXX

#include <vector>
#include <ClanLib/Core/Math/rect.h>
#include <ClanLib/Core/Math/cl_vector.h>
#include <ClanLib/Core/Math/size.h>

/** */
class CollisionManager
{
private:
  std::vector<CL_Rect> rects;
public:
  enum Side { CO_NONE, // Objects did not collide
              CO_UNKNOWN, // Objects collide, but its not know where
              CO_TOP, CO_BOTTOM, CO_LEFT, CO_RIGHT }; 

  enum Num { CO_ALWAYS, CO_NEVER, CO_SOMETIME };
  struct SweepResult {
    Num   num;
    float u0;
    float u1;
  };

  struct CollisionResult {
    /** Side at which the collision starts */
    Side  side;

    /** Side at which the collision ends */
    Side  side_out;

    /** first times of overlap along each axis */
    float u0;
    
    /** last times of overlap along each axis, if 1.0f collision
        occurs still at the end of the movement */
    float u1;
    
    CollisionResult()
      : side(CO_NONE), side_out(CO_UNKNOWN), u0(0.0f), u1(1.0f)
    {
    }

    CollisionResult(Side side_, float u0_, float u1_)
      : side(side_), u0(u0_), u1(u1_)
    {}
  };

  CollisionManager();
  
  void update(float delta);

  void add(const CL_Rect& rect);
  CollisionResult sweep(CL_Sizef a, CL_Vector a_pos1, CL_Vector a_pos2,
                        CL_Sizef b, CL_Vector b_pos1, CL_Vector b_pos2);

  bool range_overlaps(float a1, float a2, float b1, float b2);

  /** Sweep test for a single axis, returns true if [a1,a2[ and
      [b1,b2[ collide at some point in time, the time is saved in u0
      and u1, if false is returned u0 and u1 are undefined.

      @param a1 start of the first range
      @param a2 end of the first range
      @param b1 start of the second range
      @param b2 end of the second range
      @param v  the amount of space that b moves relative to a
      @param u0 time at which both ranged a and b start to collide
      @param u1 time at which both ranged a and b get disjunked and no longer collide
  */
  inline SweepResult sweep_helper(float a1, float a2,
                                  float b1, float b2, float v);
private:
  CollisionManager (const CollisionManager&);
  CollisionManager& operator= (const CollisionManager&);
};

#endif

/* EOF */
