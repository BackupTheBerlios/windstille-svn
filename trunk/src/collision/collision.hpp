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

#ifndef HEADER_COLLISION_HXX
#define HEADER_COLLISION_HXX

#include <ClanLib/Core/Math/cl_vector.h>

#include <iostream>
#include <list>
#include <vector>

#include <ClanLib/Core/Math/rect.h>

using namespace std;

class CollisionObject;

struct CollPrimitive
{
public:
  CollisionObject *object;
  CL_Rectf rect;

public:
  CollPrimitive(const CL_Rectf& r_, CollisionObject* object_);

  float x_pos() const;
  float y_pos() const;
  float width() const;
  float height() const;

  float x_velocity() const;
  float y_velocity() const;

  CL_Vector get_velocity() const;

  void drawCollision() const;

  friend std::ostream& operator<<(std::ostream& out, const CollPrimitive &b);
};

inline std::ostream& operator<<(std::ostream& out, const CollPrimitive &b);

#endif

/* EOF */
