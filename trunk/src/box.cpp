//  $Id: box.cpp,v 1.11 2005/07/11 14:06:01 godrin Exp $
// 
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>
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
#include <config.h>

#include "box.hpp"
#include "globals.hpp"
#include "lisp/lisp.hpp"
#include "lisp/properties.hpp"
#include "windstille_getters.hpp"
#include "collision/collision_engine.hpp"
#include "math/vector.hpp"
#include "tile_map.hpp"
#include "random.hpp"

#define BOX_HEIGHT 16

Box::Box(const lisp::Lisp* lisp)
  : sprite("box", resources)
{
  gravity = 0.0f;
  float width  = 64.0f;
  float height = 64.0f;
  std::string spritename = "box";
  Vector vel;

  lisp::Properties props(lisp);
  props.get("sprite", spritename);
  props.get("pos", pos);
  props.get("width", width);
  props.get("height", height);
  props.get("vel", vel);
  props.get("name", name);
  props.get("gravity", gravity);
  props.print_unused_warnings("box object");

  if (!spritename.empty())
    sprite = CL_Sprite(spritename, resources);

  colobj = new CollisionObject(CL_Rectf(0, 0, width, height));
  colobj->set_velocity(vel);
  colobj->set_pos(Vector(pos.x, pos.y));

  Sector::current()->get_collision_engine()->add(colobj);

  slot = colobj->sig_collision().connect(this, &Box::collision);
}

void 
Box::collision(const CollisionData& data, CollisionObject& other)
{
  (void) data;
  (void) other;
  //std::cout << this << ": Collision Event" << std::endl;
  if ((data.direction.x > 0 && colobj->get_velocity().x < 0) ||
      (data.direction.x < 0 && colobj->get_velocity().x > 0))
    {
      colobj->set_velocity(Vector(-colobj->get_velocity().x, colobj->get_velocity().y));
    }
  
  if ((data.direction.y > 0 && colobj->get_velocity().y < 0) ||
      (data.direction.y < 0 && colobj->get_velocity().y > 0))
    {
      colobj->set_velocity(Vector(colobj->get_velocity().x, -colobj->get_velocity().y));
    }
}

void 
Box::update(float delta)
{
  colobj->set_velocity(Vector(colobj->get_velocity().x, 
                              colobj->get_velocity().y + gravity * delta));

  sprite.update(delta);
  pos = colobj->get_pos();
}

void 
Box::draw(SceneContext& sc)
{
  if (sprite)
    sc.color().draw(sprite, colobj->get_pos().x, colobj->get_pos().y, 10);
}

/* EOF */
