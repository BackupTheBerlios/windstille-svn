//  $Id: box.hpp,v 1.11 2005/07/11 14:05:47 godrin Exp $
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

#ifndef BOX_HXX
#define BOX_HXX

#include "entity.hpp"
#include "lisp/lisp.hpp"
#include "sector.hpp"

class Box:public Entity
{
public:
  Box(const lisp::Lisp* lisp);

  virtual void collision(const CollisionData& data, CollisionObject& other); 

  virtual bool unstuck() const;
  virtual bool unstuck_movable() const;

  virtual void update(float delta);
  virtual void move(float delta);

  void draw(SceneContext& gc);

  bool on_ground() const;
  bool stuck() const;
private:
  CL_Sprite sprite;
  CL_Sprite light_sprite;
};

#endif

/* EOF */
