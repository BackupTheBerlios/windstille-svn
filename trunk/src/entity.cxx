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

#include <assert.h>
#include "entity.hxx"

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

/* EOF */
