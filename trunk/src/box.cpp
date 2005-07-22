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

#include "box.hpp"
#include "globals.hpp"
#include "lisp/list_iterator.hpp"
#include "collision/collision.hpp"
#include "collision/collision_engine.hpp"
#include "tile_map.hpp"
#include "random.hpp"

#define BOX_HEIGHT 16

Box::Box(const lisp::Lisp* lisp)
  : sprite("box", resources),
    colobj(new CollisionObject())
{
  std::string spritename = "box";
  CL_Vector vel;
  lisp::ListIterator iter(lisp);
  while(iter.next()) {
    if(iter.item() == "sprite") {
      spritename = iter.value().get_string();
    } else if(iter.item() == "x") {
      pos.x = iter.value().get_float();
    } else if(iter.item() == "y") {
      pos.y = iter.value().get_float();
    } else if (iter.item() == "vx") {
      vel.x = iter.value().get_float();
    } else if (iter.item() == "vy") {
      vel.y = iter.value().get_float();
    } else if(iter.item() == "name") {
      name = iter.value().get_string();
    } else {
      std::cerr << "Skipping unknown attribute '" 
                << iter.item() << "' in Box\n";
    }
  }

  if(spritename == "")
    throw std::runtime_error("No sprite name specified in Box");
  sprite = CL_Sprite(spritename, resources);

  colobj->insertCollPrimitive(CollPrimitive(CL_Rectf(0,0,64,64), colobj));

  Sector::current()->get_collision_engine()->add(colobj);

  colobj->set_velocity(vel);
  colobj->set_pos(CL_Vector(pos.x, pos.y));

  slot = colobj->sig_collision().connect(this, &Box::collision);
}

void 
Box::collision(const CollisionData& data, CollisionObject& other)
{
  (void) data;
  (void) other;
  std::cout << this << ": Collision Event" << std::endl;
  colobj->set_velocity(CL_Vector(-colobj->get_velocity().x, 0));
}

void 
Box::update(float delta)
{
  sprite.update(delta);
  pos = colobj->get_pos();
}

void 
Box::draw(SceneContext& sc)
{
  sc.color().draw(sprite, colobj->get_pos().x, colobj->get_pos().y, 10);
}

/* EOF */
