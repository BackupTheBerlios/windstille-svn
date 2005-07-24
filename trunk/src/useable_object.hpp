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

#ifndef HEADER_USEABLEOBJECT_HXX
#define HEADER_USEABLEOBJECT_HXX

#include <string>
#include "entity.hpp"
#include "game_object.hpp"
#include "lisp/lisp.hpp"
#include "script_manager.hpp"

class UseableObject : public Entity
{
private:
  CL_Sprite highlight;
  CL_Sprite color;
  std::string use_script;
public:
  UseableObject(const lisp::Lisp* lisp);
  virtual ~UseableObject() {}

  void draw (SceneContext& sc);
  void update (float);
  void use() {script_manager->run_script(use_script, "UseableItem");}
  
  void collision(const CollisionData& data, CollisionObject& other);
  bool unstuck() const {return true;}
  bool unstuck_movable() const {return true;}
};

#endif

/* EOF */
