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
#include "tile_map.hpp"

#define BOX_HEIGHT 16

Box::Box(const lisp::Lisp* lisp):
  light_sprite("bomblight", resources)
{
  std::string spritename;

  lisp::ListIterator iter(lisp);
  while(iter.next()) {
    if(iter.item() == "sprite") {
      spritename = iter.value().get_string();
    } else if(iter.item() == "x") {
      pos.x = iter.value().get_float();
    } else if(iter.item() == "y") {
      pos.y = iter.value().get_float();
    } else if(iter.item() == "name") {
      name = iter.value().get_string();
    } else {
      std::cerr << "Skipping unknown attribute '" 
                << iter.item() << "' in FlashingSign\n";
    }
  }

  if(spritename == "")
    throw std::runtime_error("No sprite name specified in Box");
  sprite = CL_Sprite(spritename, resources);

  bbox=CL_Rectf(0,0,22,16);
  insertCollPrimitive(new CollRect(bbox,this));


  light_sprite.set_blend_func(blend_src_alpha, blend_one);

}

void 
Box::collision(const CollisionData& data, CollisionObject& other)
{
  (void) data;
  (void) other;
  movement.y=0;
}

bool 
Box::unstuck() const
{
  return true;
}

bool 
Box::unstuck_movable() const
{
  bool v= !on_ground();
  return v;
  //  return true;
}

void 
Box::update(float delta)
{
  (void) delta;
}

void 
Box::move(float delta)
{
  Entity::move(delta);
  CollisionObject::move(delta);

  if(stuck())
    {
      movement.y=0;
      while(stuck())
	{
	  pos.y-=0.1;
	}
      assert(on_ground());
    }
}


void 
Box::draw(SceneContext& sc)
{
  sc.light().draw(light_sprite, pos.x, pos.y, 0);
  //sc.color().draw(sprite, pos.x, pos.y, 10);
  Entity::draw(sc);
}

bool
Box::on_ground() const
{
  return get_world ()->get_tilemap()->is_ground(pos.x, pos.y-16+BOX_HEIGHT+1);
}


bool 
Box::stuck () const
{
  return get_world ()->get_tilemap()->is_ground(pos.x, pos.y-16+BOX_HEIGHT);
}
