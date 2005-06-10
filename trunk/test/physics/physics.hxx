//  $Id$
// 
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_PHYSICS_HXX
#define HEADER_PHYSICS_HXX

#include <vector>

/** A rectangular object that can handle physics and collision */
class PhysicObject
{
public: 
  /** Uniq Id which identifies this object */
  int id;

  // Top/Right corner of the object
  float x_pos;
  float y_pos;

  float mass;

  float width;
  float height;

  float x_velocity;
  float y_velocity;

  PhysicObject(float x_, float y_, float width_, float height_) 
  {
    id = -1;

    mass = 1.0f;

    x_pos = x_;
    y_pos = y_;
    
    width  = width_;
    height = height_;

    x_velocity = 0.0f;
    y_velocity = 0.0f;
  }
};

/** */
class Physics
{
private:
  typedef std::vector<PhysicObject> Objects;
  Objects objects;

  float friction;

  float x_acceleration;
  float y_acceleration;

public:

  Physics();
  ~Physics();

  void draw();
  void update(float delta);

  PhysicObject& add_object(PhysicObject obj);
};

#endif

/* EOF */
