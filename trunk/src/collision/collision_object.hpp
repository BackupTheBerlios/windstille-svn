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

#include <ClanLib/core.h>
#include <ClanLib/Core/Math/cl_vector.h>
#include "collision.hpp"

class CollisionEngine;

class CollisionObject
{

public:
  CollisionObject();
  virtual ~CollisionObject();

  CL_Vector get_pos() const;
  CL_Vector get_velocity() const;

  void set_velocity(const CL_Vector &v);
  void set_pos(const CL_Vector& p);

  // this function is for preparing impulse collisions
  // you have to calculate the resulting impulse for both object before changing,
  // so this is called for both objects first and then collision() for both.
  //
  // this isn't really fast, because everything's done twice, maybe someone has a better idea?
  virtual void prepare_collision(const CollisionData &, CollisionObject &)
  { }
  //virtual void collision(const CollisionData& data, CollisionObject& other) = 0; 

  void insertCollPrimitive(CollPrimitive *primitive);

  void update(float delta);

  // debugging helpers
  void drawCollision();

  // this functions support unstucking, which needs to be done, when more than 2 object stack over one another
  // should this object be unstuck ??
  virtual bool unstuck() const { return true; }
  // is this object movable within unstucking ?
  virtual bool unstuck_movable() const { return true; }

  void set_bounding_box(const CL_Rectf& b) { bbox = b; }

  CL_Signal_v2<const CollisionData &, CollisionObject &>& sig_collision() { return collision; }
protected:
  /// only rectangular objects for now
  // FIXME: isn't this redundant?
  CL_Rectf bbox;
 
  /// position of the object
  CL_Vector pos;

  /// velocity of the object
  CL_Vector velocity;

  CollisionObject* parent;
  std::vector<CollisionObject*> children;

  CL_Signal_v2<const CollisionData &, CollisionObject &> collision;
private:
  std::list<CollPrimitive*> colliders;
  CollisionEngine *coll_engine;

  friend class CollisionEngine;
  friend CollisionData collide(CollisionObject &a,CollisionObject &b,float delta);
};


#endif
