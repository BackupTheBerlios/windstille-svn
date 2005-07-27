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
#include <config.h>

#include "globals.hpp"
#include "useable_object.hpp"
#include "lisp/properties.hpp"
#include "windstille_getters.hpp"

UseableObject::UseableObject(const lisp::Lisp* lisp)
  : highlight("vrdoor/highlight", resources),
    color("vrdoor/color", resources)
{
  using namespace lisp;
  set_useable(true);

  Properties props(lisp);
  props.get("pos", pos);
  props.get("script", use_script);
  props.print_unused_warnings("usableobject");
}

void
UseableObject::draw (SceneContext& sc)
{
  sc.color().draw(color, pos.x, pos.y, 1);
  sc.color().draw(highlight, pos.x, pos.y, 2);
  sc.light().draw(highlight, pos.x, pos.y, 1);
}

void
UseableObject::update (float)
{
}

void
UseableObject::collision(const CollisionData& data, CollisionObject& other)
{
  (void) data;
  (void) other;
}

/* EOF */
