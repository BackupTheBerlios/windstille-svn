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
#include "math/vector.hpp"
#include "math/rect.hpp"
#include "collision_data.hpp"

class TileMap;
class CollisionEngine;
class GameObject;

/** The CollisionObject provides way to handle collisions in a nice
    and predictable way. To use it a GameObject/Entity simply
    registeres a CollisionObject in the CollisionEngine and updates
    its position via set_velocity(). As soon as the CollisionEngine
    registers a collision the CollisionObject::collision() signal is
    called to which the user can register a callback function. In the
    collision() callback he can then handle the collision reaction. */
class CollisionObject
{
private:
  enum ObjectType { RECTANGLE, TILEMAP };

  ObjectType object_type;

  /// position of the object
  Vector pos;

  /// velocity of the object
  Vector velocity;

  GameObject* game_object;

  CL_Signal_v2<const CollisionData &, CollisionObject &> collision;

  Rectf primitive;
  TileMap* tilemap;

  bool is_unstuckable;
  bool is_unstuck_movable;

public:
  CollisionObject(const Rectf& rect_);
  CollisionObject(TileMap* tilemap_);

  virtual ~CollisionObject();

  /** Sets the velocity of this object, this is the primary way how
      you move a CollisionObject. Velocity simply means the difference
      between the old position and the new position, no acceleration
      or deceleration is involved, so the velocity stays constant for
      the CollisionObject unless you change it with a new call to
      set_velocity */
  void set_velocity(const Vector &v);
  Vector get_velocity() const;

  /** Sets the CollisionObject to the given pos, it should only be
      called to set the initial position and *not* to update the
      position each update(), see \a set_velocity()
      
      FIXME: unstucking is currently not handled special here, but
      simply in the next run of the collision engine, this should
      probally be changed so that the object is unstuck here without
      affecting other objects and then the real placement position is
      returned */
  void set_pos(const Vector& p);
  Vector get_pos() const;

  void set_game_object(GameObject* game_object);
  GameObject* get_game_object() const;

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

  CL_Signal_v2<const CollisionData &, CollisionObject &>& sig_collision() { return collision; }

  friend class CollisionEngine;
  friend CollisionData collide(CollisionObject& a, CollisionObject& b, float delta);
};

#endif

/* EOF */
