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

#include <ClanLib/display.h>
#include <ClanLib/gl.h>
#include "globals.hxx"
#include "scene_context.hxx"

class SceneContextImpl
{
public:
  DrawingContext color;
  DrawingContext light;
  DrawingContext highlight; 

  CL_OpenGLSurface lightmap;
  CL_Canvas        canvas;

  SceneContextImpl() 
    : lightmap("black", resources),
      canvas(lightmap)
  {
    canvas.get_gc()->set_scale(0.25, 0.25);
  }
};

SceneContext::SceneContext()
{
  impl = new SceneContextImpl();
}

SceneContext::~SceneContext()
{
  delete impl;
}

DrawingContext&
SceneContext::color()
{
  return impl->color; 
}

DrawingContext&
SceneContext::light()
{ 
  return impl->light; 
}

DrawingContext&
SceneContext::highlight()
{ 
  return impl->highlight; 
}

void
SceneContext::render()
{
  // Render all buffers
  // FIXME: Render all to pbuffer for later combining of them
  //color_.render();
  
  //impl->light.render(impl->canvas.get_gc());
  //canvas.sync_surface();

  //impl->lightmap.set_blend_func(blend_src_alpha, blend_one);
  impl->lightmap.set_blend_func(blend_dest_color, blend_zero);
  //GL_DST_COLOR, GL_ZERO
  impl->lightmap.set_scale(4.0f, 4.0f);
  impl->lightmap.draw();
  //impl->canvas.get_gc()->clear();
  //highlight_.render();

  // Clear all DrawingContexts
  impl->color.clear();
  impl->light.clear();
  impl->highlight.clear();
}

/* EOF */
