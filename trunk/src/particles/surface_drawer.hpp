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

#ifndef HEADER_SURFACE_DRAWER_HXX
#define HEADER_SURFACE_DRAWER_HXX

#include <ClanLib/Display/surface.h>
#include <ClanLib/GL/opengl_surface.h>
#include "drawer.hpp"

class SurfaceDrawer : public Drawer
{
private:
  CL_OpenGLSurface surface;
  
public:
  SurfaceDrawer(const CL_Surface& sur);
  virtual ~SurfaceDrawer();
  
  void set_surface(const CL_Surface& sur);
  void draw(SceneContext& sc, ParticleSystem& psys);
};

#endif

/* EOF */
