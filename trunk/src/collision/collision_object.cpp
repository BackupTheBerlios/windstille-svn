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

void CollisionObject::insertCollPrimitive(CollPrimitive *primitive)
{
  colliders.push_back(primitive);
}



void CollisionObject::drawCollision()
{
  std::list<CollPrimitive*>::iterator j=colliders.begin();

  for(;j!=colliders.end();j++)
    {
      (*j)->drawCollision();
    }
}

void CollisionObject::move(float delta)
{
  assert(coll_engine);

  pos += movement * delta;
  movement -= movement * coll_engine->get_friction() * delta;
  
  movement += coll_engine->get_graphity() * delta;

  if (fabsf(movement.x) < coll_engine->get_min_velocity()*delta)
    movement.x = 0.0f;

  if (fabsf(movement.y) < coll_engine->get_min_velocity()*delta)
    movement.y = 0.0f;

}

void 
CollisionObject::set_movement(const CL_Vector &m)
{
  movement=m;
}


CL_Vector CollisionObject::get_pos() const
{
  if(parent != 0)
    return parent->get_pos() + pos;
  
  return pos;
}

CL_Vector CollisionObject::get_movement() const
{
  if(parent != 0)
    return parent->get_movement() + movement;
  
  return movement;
}

