/*
 * Copyright (c) 2005 by the Windstille team. All rights reserved.
 *
 * collision_object.cxx
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

#include <ClanLib/display.h>
#include <assert.h>

#include "collision_object.hpp"
#include "collision_engine.hpp"

/***********************************************************************
 * CollisionObject
 ***********************************************************************/

CollisionObject::CollisionObject(const CL_Rectf& rect_)
  : primitive(rect_)
{
  object_type        = RECTANGLE;
  parent             = 0;
  coll_engine        = 0;
  is_unstuckable     = true;
  is_unstuck_movable = true;
  velocity           = CL_Vector(0,0,0);
  pos                = CL_Vector(0,0,0);
}

CollisionObject::CollisionObject(TileMap* tilemap_)
  : tilemap(tilemap_)
{
  object_type        = TILEMAP;
  parent             = 0;
  coll_engine        = 0;
  is_unstuckable     = true;
  is_unstuck_movable = false;
  velocity           = CL_Vector(0,0,0);
  pos                = CL_Vector(0,0,0);
}

CollisionObject::~CollisionObject()
{
}

void
CollisionObject::drawCollision()
{
  CL_Vector v = get_pos ();
  CL_Rectf  r = primitive;

  r += CL_Pointf (v.x, v.y);

  CL_Display::fill_rect (r, CL_Color (255, 255, 255));
  
  CL_Display::draw_rect (r,
			 CL_Color(155, 155, 155));        
  
  CL_Display::draw_line (r.left + r.get_width ()/2,
			 r.top  + r.get_height ()/2,
			 r.left + r.get_width ()/2  + get_velocity ().x,
			 r.top  + r.get_height ()/2 + get_velocity ().y,
			 CL_Color (255, 0, 255));
}

void CollisionObject::update(float delta)
{
  assert(coll_engine);

  pos += velocity * delta;
}

void 
CollisionObject::set_velocity(const CL_Vector &m)
{
  velocity=m;
}

CL_Vector
CollisionObject::get_pos() const
{
  if(parent != 0)
    return parent->get_pos() + pos;
  
  return pos;
}

CL_Vector
CollisionObject::get_velocity() const
{
  if (parent != 0)
    return parent->get_velocity() + velocity;
  
  return velocity;
}

void
CollisionObject::set_pos(const CL_Vector& p)
{
  // FIXME: Do this somewhat more clever to avoid stuck issues
  pos = p;
}

/* EOF */
