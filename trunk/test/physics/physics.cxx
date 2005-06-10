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

#include <ClanLib/display.h>
#include "physics.hxx"

Physics::Physics()
{
  friction       = 0.01f;
  
  x_acceleration = 0.0f;
  y_acceleration = 2.0f;
}

Physics::~Physics()
{
}

void
Physics::draw()
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      CL_Display::fill_rect(CL_Rectf(CL_Pointf(i->x_pos, i->y_pos),
                                    CL_Sizef(i->width, i->height)),
                            CL_Color(255, 255, 255));
    }
}
  
void
Physics::update(float delta)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      i->x_velocity -= (i->x_velocity * friction) * delta;
      i->y_velocity -= (i->y_velocity * friction) * delta;

      i->x_velocity += x_acceleration * delta;
      i->y_velocity += y_acceleration * delta;

      // Insert Collisions handling
      i->x_pos += i->x_velocity * delta;
      i->y_pos += i->y_velocity * delta;
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
