//  $Id: animation_obj.hpp,v 1.3 2003/09/12 16:31:20 grumbel Exp $
// 
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_CHARACTER_HPP
#define HEADER_CHARACTER_HPP

#include "entity.hpp"
#include "sprite3d/sprite.hpp"
#include "lisp/lisp.hpp"

class Character : public Entity
{
private:
  sprite3d::Sprite* sprite;
    
public:
  Character(const lisp::Lisp* lisp);
  ~Character();
  
  void update(float delta);
  void draw (SceneContext& gc);
  void use();
  bool useable() const { return true; }
  
  void collision(const CollisionData& data, CollisionObject& other);
  bool unstuck() const {return true;}
  bool unstuck_movable() const {return true;}
};

#endif

/* EOF */
