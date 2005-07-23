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

#include "collision.hpp"
#include "collision_test.hpp"
#include "collision_object.hpp"

#include <assert.h>

#include <ClanLib/display.h>

CollPrimitive::CollPrimitive(const CL_Rectf &r_, CollisionObject *object_)
  : object(object_), rect(r_)
{
}

float
CollPrimitive::x_velocity() const
{
  return object->get_velocity ().x;
}

float
CollPrimitive::y_velocity() const
{
  return object->get_velocity ().y;
}

CL_Vector
CollPrimitive::get_velocity() const
{
  return object->get_velocity();
}

float
CollPrimitive::x_pos() const
{
  return rect.left+object->get_pos ().x;
}

float
CollPrimitive::y_pos() const
{
  return rect.top+object->get_pos ().y;
}

float
CollPrimitive::width() const
{
  return rect.get_width ();
}

float
CollPrimitive::height() const
{
  return rect.get_height ();
}

void
CollPrimitive::drawCollision() const
{
  CL_Vector v=object->get_pos ();
  CL_Rectf r=rect;
  r += CL_Pointf (v.x, v.y);

  CL_Display::fill_rect (r, CL_Color (255, 255, 255));
  
  CL_Display::draw_rect (r,
			 CL_Color(155, 155, 155));        
  
  CL_Display::draw_line (r.left + r.get_width ()/2,
			 r.top  + r.get_height ()/2,
			 r.left + r.get_width ()/2 + object->get_velocity ().x,
			 r.top  + r.get_height ()/2 + object->get_velocity ().y,
			 CL_Color (255, 0, 255));
}

std::ostream& operator<<(std::ostream& out, const CollPrimitive &b)
{
  out << "(" 
      << b.x_pos() << ", " << b.y_pos() << ", " 
      << b.width() << ", " << b.height() << ", " 
      << b.object->get_velocity().x << ", " << b.object->get_velocity().y
      << ")";
  return out;
}

/* EOF */
