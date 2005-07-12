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

#include <ClanLib/Display/display.h>
#include <assert.h>
#include "globals.hpp"
#include "entity.hpp"

Entity::Entity()
  : parent(0),
    sprite("igel", resources)
{
  mover_active = false;
}

Entity::~Entity()
{
}

CL_Vector
Entity::get_pos() const
{
  if (parent)
    return parent->get_pos() + pos;
  else
    return pos;
}

void
Entity::bind(Entity* parent_, bool recalc_pos)
{
  assert(parent_);

  parent = parent_;
  if (recalc_pos)
    pos -= parent->get_pos();
}

void
Entity::unbind(bool recalc_pos)
{
  parent = 0;
  if (recalc_pos)
    pos += parent->get_pos();
}

void
Entity::draw(SceneContext& sc)
{
  sc.color().draw(sprite, pos.x, pos.y, 10);

  if (parent)
    CL_Display::draw_line(int(get_pos().x), int(get_pos().y),
                          int(parent->get_pos().x), int(parent->get_pos().y),
                          CL_Color(255, 255, 255, 255));
}

void
Entity::move(float delta)
{
  sprite.update(delta);
  
  if (mover_active) 
    {
      float dir_x = (target_pos.x - pos.x) > 0? 100.0f : -100.0f;
    
      //std::cout << this << " " << target_pos.x << " " << pos.x << std::endl;

      pos.x += dir_x * delta;

      float dir_x2 = (target_pos.x - pos.x) > 0 ? 100.0f : -100.0f;
    
      if (dir_x != dir_x2)
        {
          //std::cout << "Done!!!" << std::endl;
          mover_active = false;
          done();
        }
    }
}

void
Entity::set_pos(float x, float y)
{
  pos.x = x;
  pos.y = y;
}

void
Entity::move_to(float x, float y)
{
  mover_active = true;
  target_pos.x = pos.x + x;
  target_pos.y = pos.y + y;
}

/* EOF */
