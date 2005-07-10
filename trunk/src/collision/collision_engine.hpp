/*
 * Copyright (c) 2005 by the Windstille team. All rights reserved.
 *
 * collision_engine.hxx
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

#ifndef HEADER_COLLISION_ENGINE_HPP
#define HEADER_COLLISION_ENGINE_HPP

#include "collision_object.hpp"

class CollisionEngine
{
  typedef std::list<CollisionObject*> Objects;

public:

  CollisionEngine();
  ~CollisionEngine();

  void draw();
  void update(float delta);
  void update(CollisionObject& obj, float delta);
  void collision(CollisionObject& a, CollisionObject& b, const CollisionData &result, float delta);

  CollisionObject* add_object(CollisionObject *obj);
  void remove_object(CollisionObject *obj);


  float get_min_velocity() const;
  float get_friction() const;
  CL_Vector2 get_graphity() const;

private:
  Objects objects;

  float friction;
  float unstuck_velocity;

  float x_acceleration;
  float y_acceleration;

  float minimum_velocity;

  void unstuck(CollisionObject& a, CollisionObject& b, float delta);

};


#endif
