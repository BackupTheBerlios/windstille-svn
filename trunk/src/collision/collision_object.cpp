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

#include "collision_object.hpp"
#include "collision_engine.hpp"

#include <assert.h>

/***********************************************************************
 * CollisionObject
 ***********************************************************************/

CollisionObject::CollisionObject()
{
  parent=0;
  coll_engine=0;
}

CollisionObject::~CollisionObject()
{
}

void
CollisionObject::insertCollPrimitive(const CollPrimitive& primitive)
{
  colliders.push_back(primitive);
}

void
CollisionObject::drawCollision()
{
  for(std::vector<CollPrimitive>::iterator j = colliders.begin(); j != colliders.end(); j++)
    {
      (*j).drawCollision();
    }
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

CL_Rectf
CollisionObject::get_bounding_box() const
{
  assert(!colliders.empty());
  CL_Rectf rect = colliders.front().rect;
 
  for(std::vector<CollPrimitive>::const_iterator i = colliders.begin()+1; i != colliders.end(); ++i)
    {
      rect = rect.calc_union(i->rect);
    }

  return rect;
}

/* EOF */
