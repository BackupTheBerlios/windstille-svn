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
{
}

Entity::~Entity()
{
}

CL_Vector
Entity::get_pos() const
{
    return pos;
}

void
Entity::debug_draw()
{
  if (parent)
    CL_Display::draw_line(int(get_pos().x), int(get_pos().y),
                          int(parent->get_pos().x), int(parent->get_pos().y),
                          CL_Color(255, 255, 255, 255));
}

void
Entity::set_pos(float x, float y)
{
  pos.x = x;
  pos.y = y;
}

/* EOF */
