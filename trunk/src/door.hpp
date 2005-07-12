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

#ifndef HEADER_DOOR_HXX
#define HEADER_DOOR_HXX

#include "entity.hpp"
#include "game_object.hpp"
#include "lisp/lisp.hpp"

class Door : public Entity
{
private:
  CL_Sprite highlight;
  CL_Sprite color;
public:
  Door(const lisp::Lisp* lisp);
  virtual ~Door() {}

  void draw (SceneContext& sc);
  void update (float);
  
  void collision(const CollisionData& data, CollisionObject& other);
  bool unstuck() const {return true;}
  bool unstuck_movable() const {return true;}
};

#endif

/* EOF */
