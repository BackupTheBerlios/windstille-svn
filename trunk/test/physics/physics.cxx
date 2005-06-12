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

#include <iostream>
#include <assert.h>
#include <ClanLib/display.h>
#include "physics.hxx"

Physics::Physics()
{
  friction       = 0.01f;
  
  x_acceleration = 0.0f;
  y_acceleration = 0.0f;
}

Physics::~Physics()
{
}

void
Physics::draw()
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      if (!i->collision)
        CL_Display::fill_rect(CL_Rectf(CL_Pointf(i->x_pos, i->y_pos),
                                       CL_Sizef(i->width, i->height)),
                              CL_Color(255, 255, 255));        
      else
        CL_Display::fill_rect(CL_Rectf(CL_Pointf(i->x_pos, i->y_pos),
                                       CL_Sizef(i->width, i->height)),
                              CL_Color(255, 0, 255));        
      
      CL_Display::draw_line(i->x_pos + i->width/2,
                            i->y_pos + i->height/2,
                            i->x_pos + i->width/2 + i->x_velocity,
                            i->y_pos + i->height/2 + i->y_velocity,
                            CL_Color(255, 0, 255));
    }
}

Physics::CollisionResult
Physics::simplesweep1d(float a, float aw, float av,
                       float b, float bw, float bv)
{
  // Normalize the calculation so that only A moves and B stands still
  float v = av - bv;

  if (v > 0)
    {
      CollisionResult res;
      res.u0 = (b - (a + aw)) / v;
      res.u1 = (b + bw - a) / v;
      res.state = COL_AT;

      assert(res.u0 < res.u1);
      return res;
    }
  else if (v < 0)
    {
      CollisionResult res;
      res.u0 = (b + bw - a) / v;
      res.u1 = (b - (a + aw)) / v;
      res.state = COL_AT;

      assert(res.u0 < res.u1);
      return res;
    }
  else // (v == 0)
    {
      CollisionResult res;

      if ((a + aw) < b || (a > b + bw))
        res.state = COL_NEVER;
      else
        res.state = COL_ALWAYS;

      return res;
    }
}
  
void
Physics::resolve_collision(PhysicObject& a, PhysicObject& b, CollisionResult& x, CollisionResult& y, 
                           float delta)
{
  if (x.state == COL_NEVER || y.state == COL_NEVER)
    {
      // nothing todo
    }
  else 
    {
      // code here
      if (x.state == COL_AT)
        {
          //std::cout << a.id << ": " << x.u0 << " " << x.u1 << " " << delta << std::endl;
          if (x.u1 < 0 || x.u0 > delta)
            {
              // miss, no collision in the current time frame
            }
          else
            {
              if (y.state == COL_ALWAYS)
                {
                  //std::cout << "col: " << a << " " << b << std::endl;
                  a.collision = true;
                }
              else if (y.state == COL_AT)
                {
                  if (y.u1 < 0 || y.u0 > delta)
                    {
                      // miss, no collision in the current time frame
                    }
                  else
                    {
                      //std::cout << "col: " << a << " " << b << std::endl;
                      a.collision = true;
                    }
                }
              else
                {
                  assert(!"Never reached");
                }
            }
        }
      else if (x.state == COL_ALWAYS)
        {
          if (y.state == COL_AT)
            {
              if (y.u1 < 0 || y.u0 > delta)
                {
                  // miss, no collision in the current time frame
                }
              else
                {
                  // collision
                  //std::cout << "col: " << a << " " << b << std::endl;
                  a.collision = true;
                }
            }
          else if (y.state == COL_ALWAYS)
            {
              // stuck
              //std::cout << "col: " << a << " " << b << std::endl;
              a.collision = true;
            }
          else
            {
              assert(!"Never reached");
            }
        }
      else
        {
          assert(!"Never reached");
        }
    }
}

void
Physics::update(float delta)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      i->collision = false;
                
      if (i->movable)
        {
          //                        i - why doesn't this work?
          for(Objects::iterator j = objects.begin(); j != objects.end(); ++j)
            {
              if (i != j)
                {
                  CollisionResult x_res = simplesweep1d(i->x_pos, i->width, i->x_velocity,
                                                        j->x_pos, j->width, j->x_velocity);
                  CollisionResult y_res = simplesweep1d(i->y_pos, i->height, i->y_velocity,
                                                        j->y_pos, j->height, j->y_velocity);
                  resolve_collision(*i, *j, x_res, y_res, delta);
                }
            }

          // Insert Collisions handling
          i->x_pos += i->x_velocity * delta;
          i->y_pos += i->y_velocity * delta;

          i->x_velocity -= (i->x_velocity * friction) * delta;
          i->y_velocity -= (i->y_velocity * friction) * delta;
              
          i->x_velocity += x_acceleration * delta;
          i->y_velocity += y_acceleration * delta;
        }
    }
}

PhysicObject&
Physics::add_object(PhysicObject obj)
{
  objects.push_back(obj);

  objects.back().id = objects.size();

  return objects.back();
}

/* EOF */
