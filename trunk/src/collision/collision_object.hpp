/*
 * Copyright (c) 2005 by the Windstille team. All rights reserved.
 *
 * collision_object.hxx
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

#ifndef HEADER_COLLISION_OBJECT_HPP
#define HEADER_COLLISION_OBJECT_HPP

#include "collision.hpp"

class CollisionEngine;

class CollisionObject
{

public:
  CollisionObject();
  virtual ~CollisionObject();

  CL_Vector2 get_pos() const;
  CL_Vector2 get_movement() const;

  void set_movement(const CL_Vector2 &m);

  // this function is for preparing impulse collisions
  // you have to calculate the resulting impulse for both object before changing,
  // so this is called for both objects first and then collision() for both.
  //
  // this isn't really fast, because everything's done twice, maybe someone has a better idea?
  virtual void prepare_collision(const CollisionData &data, CollisionObject &other)
  {
  }
  virtual void collision(const CollisionData& data, CollisionObject& other) = 0; 

  virtual void move(float delta);

  void insertCollPrimitive(CollPrimitive *primitive);

  // debugging helpers
  void drawCollision();

  // this functions support unstucking, which needs to be done, when more than 2 object stack over one another
  // should this object be unstuck ??
  virtual bool unstuck() const=0;
  // is this object movable within unstucking ?
  virtual bool unstuck_movable() const=0;

protected:
  /// only rectangular objects for now
  CL_Rectf bbox;
 
  /// position of the object
  CL_Vector2 position;
  /// movement till next frame
  CL_Vector2 movement;

  CollisionObject* parent;
  std::vector<CollisionObject*> children;


private:
  std::list<CollPrimitive*> colliders;
  CollisionEngine *coll_engine;

  friend class CollisionEngine;
  friend CollisionData collide(CollisionObject &a,CollisionObject &b,float delta);
};


#endif
