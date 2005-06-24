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

#ifndef HEADER_SPRITE3D_HXX
#define HEADER_SPRITE3D_HXX

#include <string>
#include "game_object.hxx"

class SceneContext;
class Sprite3DImpl;

/** */
class Sprite3D : public GameObject
{
private:
public:
  Sprite3D(const std::string& filename);
  virtual ~Sprite3D();

  void update(float delta);
  void draw(SceneContext& sc);

private:
  Sprite3D (const Sprite3D&);
  Sprite3D& operator= (const Sprite3D&);

  Sprite3DImpl* impl;
};

#endif

/* EOF */
