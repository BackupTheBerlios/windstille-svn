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

#ifndef HEADER_NIGHTVISION_HXX
#define HEADER_NIGHTVISION_HXX

#include <ClanLib/Display/surface.h>
#include <ClanLib/GL/opengl_surface.h>
#include "lisp/lisp.hpp"
#include "game_object.hpp"

// FIXME: shouldn't really be a game object, but makes testing easier
class Nightvision : public GameObject
{
private:
  CL_Surface nightvision;
  CL_OpenGLSurface noise;
public:
  Nightvision(const lisp::Lisp* lisp);
  ~Nightvision();

  void draw(SceneContext& sc);
  void update(float delta);
  
private:
  Nightvision (const Nightvision&);
  Nightvision& operator= (const Nightvision&);
};

#endif

/* EOF */
