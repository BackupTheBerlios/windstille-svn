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

#include <assert.h>
#include "collision_manager.hpp"

CollisionManager::CollisionManager()
{
}

void
CollisionManager::update(float )
{
}

void
CollisionManager::add(const CL_Rect& rect)
{
  rects.push_back(rect);
}

bool
CollisionManager::range_overlaps(float a1, float a2, float b1, float b2)
{
  assert(a1 < a2);
  assert(b1 < b2);

  return !(a2 < b1 || b2 < a1);
}

CollisionManager::SweepResult
CollisionManager::sweep_helper(float a1, float a2,
                               float b1, float b2, float v)
{
  assert(a1 < a2);
  assert(b1 < b2);
  
  SweepResult result;

  if( v < 0 ) 
    { // collision from u0, to u1
      result.u0 = (a2 - b1) / v;
      result.u1 = (a1 - b2) / v; 

      result.num = CO_SOMETIME;
    } 
  else if( v > 0 ) 
    { // collision from u0, to u1
      result.u0 = (a1 - b2) / v;
      result.u1 = (a2 - b1) / v; 

      result.num = CO_SOMETIME;
    }
  else // v == 0
    {
      if (a2 <= b1 || a1 >= b2)
        result.num = CO_NEVER;
      else
        result.num = CO_ALWAYS;
    }

  return result;
}

CollisionManager::CollisionResult
CollisionManager::sweep(CL_Sizef a_s, CL_Vector a_pos1, CL_Vector a_pos2,
                        CL_Sizef b_s, CL_Vector b_pos1, CL_Vector b_pos2)
{
  // For more info on the algorithm see:
  // http://www.gamasutra.com/features/19991018/Gomez_3.htm

  CL_Rectf a(CL_Pointf(a_pos1.x, a_pos1.y), a_s);
  CL_Rectf b(CL_Pointf(b_pos1.x, b_pos1.y), b_s);

  CL_Vector v = (b_pos2 - b_pos1) - (a_pos2 - a_pos1);

  // the problem is solved in A's frame of reference, v is the
  // relative velocity of B in relation to A

  SweepResult x_axis = sweep_helper(a.left, a.right,  b.left, b.right,  v.x);
  SweepResult y_axis = sweep_helper(a.top,  a.bottom, b.top,  b.bottom, v.y);

  CollisionResult result(CO_NONE, 0, 0);

  if (x_axis.num == CO_NEVER || y_axis.num == CO_NEVER)
    { // One axis doesn't collide, so both can never collide at
      // the same time
      return CollisionResult(CO_NONE, 0, 0);
    }
  else if (x_axis.num == CO_ALWAYS && y_axis.num == CO_ALWAYS)
    {
      return CollisionResult(CO_UNKNOWN, 0, 0);
    }
  else if (x_axis.num == CO_SOMETIME && y_axis.num == CO_SOMETIME)
    { // Both axis collide sometime, so figure out when
      result.u0 = std::max(x_axis.u0, y_axis.u0);
      result.u1 = std::min(x_axis.u1, y_axis.u1);

      if (result.u0 == x_axis.u0)
        {
          if (v.x > 0)
            result.side = CO_LEFT;
          else if (v.x < 0)
            result.side = CO_RIGHT;
          else
            result.side = CO_UNKNOWN;
        }
      else if (result.u0 == y_axis.u0)
        {
          if (v.y > 0)
            result.side = CO_TOP;
          else if (v.y < 0)
            result.side = CO_BOTTOM;
          else
            result.side = CO_UNKNOWN;
        }
      else
        {
          assert(!"Should never be reached");
        }

      if (result.u1 == x_axis.u1)
        {
          if (v.x < 0)
            result.side_out = CO_LEFT;
          else if (v.x > 0)
            result.side_out = CO_RIGHT;
          else
            result.side_out = CO_UNKNOWN;
        }
      else if (result.u1 == y_axis.u1)
        {
          if (v.y < 0)
            result.side_out = CO_TOP;
          else if (v.y > 0)
            result.side_out = CO_BOTTOM;
          else
            result.side_out = CO_UNKNOWN;
        }
      else
        {
          assert(!"Should never be reached");
        }
    }
  else if (x_axis.num == CO_ALWAYS && y_axis.num == CO_SOMETIME)
    {
      result.u0 = y_axis.u0;
      result.u1 = y_axis.u1;
      
      if (v.y > 0)
        result.side = CO_TOP;
      else if (v.y < 0)
        result.side = CO_BOTTOM;
      else
        assert(0);
    }
  else if (x_axis.num == CO_SOMETIME && y_axis.num == CO_ALWAYS)
    {
      result.u0 = x_axis.u0;
      result.u1 = x_axis.u1;

      if (v.x > 0)
        result.side = CO_LEFT;
      else if (v.x < 0)
        result.side = CO_RIGHT;
      else
        assert(0);
    }
  else
    {
      assert(!"Should never be reached");
    }

  if (result.u0 < result.u1)
    { // Range is valid
      if (result.u0 < 0 && result.u1 >= 1.0f)
        { // We are inside a collision
          return CollisionResult(CO_UNKNOWN, 0, 0);
        }
      else if (result.u0 >= 0 && result.u0 < 1.0f)
        { // Collision happens in the range of the movement
        }
      else
        {
          result.side     = CO_NONE;
          return CollisionResult(CO_NONE, 0, 0);
        }
      // FIXME: Add side_out code
    }
  else
    {
      result.side     = CO_NONE;
      result.side_out = CO_NONE;
    }

  return result;
}

/* EOF */

