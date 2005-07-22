//  $Id: view.hpp,v 1.2 2003/10/11 12:15:59 grumbel Exp $
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

#ifndef HEADER_VIEW_HXX
#define HEADER_VIEW_HXX

#include <ClanLib/Core/Math/rect.h>
#include <ClanLib/Core/Math/cl_vector.h>
#include "camera.hpp"
#include "graphic_context_state.hpp"
#include "math/vector.hpp"

class Player;
class Sector;
class SceneContext;

/** This class is the gui component which renders the world to the
    screen */
class View
{
private:
  GraphicContextState state;
  Camera camera;

  // debugging helpers
  float zoom;
  Vector transform;

public:
  View();

  GraphicContextState get_gc_state() { return state; }

  /** @return the rectangle which represents the currently visible
      area, everything outside of it doesn't have to be drawn */
  CL_Rectf get_clip_rect();
  CL_Pointf screen2world(CL_Pointf point);

  void draw(SceneContext& gc);
  void update(float delta);

  static View* current() { return current_; }

protected:
  static View* current_;
};

#endif

/* EOF */
