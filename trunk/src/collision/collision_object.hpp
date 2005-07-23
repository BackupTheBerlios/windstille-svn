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
#include "collision_data.hpp"
#include "collision.hpp"

class CollisionEngine;

class CollisionObject
{
private:
  /// position of the object
  CL_Vector pos;

  /// velocity of the object
  CL_Vector velocity;

  CollisionObject* parent;
  std::vector<CollisionObject*> children;

  CL_Signal_v2<const CollisionData &, CollisionObject &> collision;

  std::vector<CollPrimitive> colliders;
  CollisionEngine *coll_engine;
  
  bool is_unstuckable;
  bool is_unstuck_movable;

public:
  CollisionObject();
  virtual ~CollisionObject();

  /** Sets the velocity of this object */
  void set_velocity(const CL_Vector &v);
  CL_Vector get_velocity() const;

  /** Sets the CollisionObject to the given pos FIXME: unstucking is
      currently not handled special here, but simply in the next run
      of the collision engine, this should probally be changed so that
      the object is unstuck here without affecting other objects and
      then the real placement position is returned */
  void set_pos(const CL_Vector& p);
  CL_Vector get_pos() const;

  void insertCollPrimitive(const CollPrimitive& primitive);

  void update(float delta);

  // debugging helpers
  void drawCollision();

  // this functions support unstucking, which needs to be done, when more than 2 object stack over one another
  // should this object be unstuck ??
  bool unstuck() const { return is_unstuckable; }
  // is this object movable within unstucking ?
  bool unstuck_movable() const { return is_unstuck_movable; }
  
  void set_unstuck(bool s) { is_unstuckable = s; }
  void set_unstuck_movable(bool s) { is_unstuck_movable = s; }

  CL_Rectf get_bounding_box() const;

  CL_Signal_v2<const CollisionData &, CollisionObject &>& sig_collision() { return collision; }

  friend class CollisionEngine;
  friend CollisionData collide(CollisionObject &a,CollisionObject &b,float delta);
};

#endif

/* EOF */
