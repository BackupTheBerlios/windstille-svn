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
#include "sprite2d/manager.hpp"

UseableObject::UseableObject(const lisp::Lisp* lisp)
  : sprite(0),
    highlight(0)
{
  set_useable(true);  
  std::string spritename;
  std::string highlightname;
  
  lisp::Properties props(lisp);
  props.get("sprite", spritename);
  props.get("highlight", highlightname);
  props.get("pos", pos);
  props.get("script", use_script);
  props.get("use-verb", use_verb);
  props.print_unused_warnings("usableobject");
  
  if(spritename == "")
    throw std::runtime_error("No sprite name specified in PictureEntity");
  sprite = sprite2d_manager->create(spritename);
  
  if (highlightname != "")
    highlight = sprite2d_manager->create(highlightname);
}

UseableObject::~UseableObject()
{
  if (sprite)
    delete sprite;
  if (highlight)
    delete highlight;
}

void
UseableObject::draw (SceneContext& sc)
{
  sprite->draw(sc, pos, 1);
  
  if (highlight) {
    highlight->draw(sc, pos, 2);
    highlight->draw_light(sc, pos, 1);
  }
}

void
UseableObject::update(float delta)
{
  sprite->update(delta);
  if (highlight)
    highlight->update(delta);
}

/* EOF */
