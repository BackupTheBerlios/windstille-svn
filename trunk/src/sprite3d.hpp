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
#include <stdint.h>
#include "game_object.hpp"

class SceneContext;
struct Mesh;
struct Action;
struct ActionFrame;

/** */
class Sprite3D
{
public:
  Sprite3D(const std::string& filename);
  virtual ~Sprite3D();

  void draw(SceneContext& sc);

private:
  friend class Sprite3DDrawingRequest;
  Sprite3D (const Sprite3D&);
  Sprite3D& operator= (const Sprite3D&);

  void render_frame(CL_GraphicContext* gc, const ActionFrame* frame,
      const CL_Vector& pos, const CL_Matrix4x4& modelview);

  uint16_t mesh_count;
  Mesh* meshs;
  uint16_t action_count;
  Action* actions;
};

#endif

/* EOF */
