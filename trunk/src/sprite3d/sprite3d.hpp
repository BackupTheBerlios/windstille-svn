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
#ifndef HEADER_SPRITE3D_HPP
#define HEADER_SPRITE3D_HPP

#include <string>
#include <stdint.h>
#include "game_object.hpp"
#include "display/scene_context.hpp"
#include "math/vector.hpp"
#include "math/matrix.hpp"

class SceneContext;
class Sprite3DData;
struct Action;
struct ActionFrame;

/** */
class Sprite3D
{
public:
  ~Sprite3D();

  void draw(SceneContext& sc, const Vector& pos);
  
  void set_action(const std::string& name);  
  const std::string& get_action() const;

  void set_speed(float speed);
  float get_speed() const;

private:
  friend class Sprite3DDrawingRequest;
  friend class Sprite3DManager;
  Sprite3D(const Sprite3DData* data);
  
  Sprite3D (const Sprite3D&);
  Sprite3D& operator= (const Sprite3D&);

  /**
   * blends 2 frames and renders,
   * blending formula: time*frame1 + (1.0-time)*frame2
   */
  void blend_frames(CL_GraphicContext* gc, const ActionFrame* frame1,
                    const ActionFrame* frame2, float time,
                    const Vector& pos, const Matrix& modelview);

  const Sprite3DData* data;
  const Action* current_action;
  float time_delta;
  float speed; 
  const Action* next_action;
};

#endif
