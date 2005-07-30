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
#include "scene_context.hpp"

// The lightmap has a resolution of screen.w/LIGHTMAP, screen.h/LIGHTMAP
#define LIGHTMAP_DIV 1

class SceneContextImpl
{
public:
  DrawingContext color;
  DrawingContext light;
  DrawingContext highlight; 
  unsigned int render_mask;

  CL_OpenGLSurface lightmap;

  SceneContextImpl() 
    : render_mask(SceneContext::COLORMAP |
                  SceneContext::LIGHTMAP | 
                  SceneContext::HIGHLIGHTMAP | 
                  SceneContext::LIGHTMAPSCREEN),
      lightmap(CL_PixelBuffer(800/LIGHTMAP_DIV, 
                              600/LIGHTMAP_DIV,
                              800/LIGHTMAP_DIV*4, CL_PixelFormat::rgba8888))
  {
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


/** Translate the drawing context */
void
SceneContext::translate(float x, float y)
{
  impl->color.translate(x, y);
  impl->light.translate(x, y);
  impl->highlight.translate(x, y);
}

/** Set the rotation of the drawing context */
void
SceneContext::rotate(float angel)
{
  impl->color.rotate(angel);
  impl->light.rotate(angel);
  impl->highlight.rotate(angel);
}

/** Set the scaling of the drawing context */
void
SceneContext::scale(float x, float y)
{
  impl->color.scale(x, y);
  impl->light.scale(x, y);
  impl->highlight.scale(x, y);
}

void
SceneContext::push_modelview()
{
  impl->color.push_modelview();
  impl->light.push_modelview();
  impl->highlight.push_modelview();
}

void
SceneContext::pop_modelview()
{
  impl->color.pop_modelview();
  impl->light.pop_modelview();
  impl->highlight.pop_modelview();
}

void
SceneContext::reset_modelview()
{
  impl->color.reset_modelview();
  impl->light.reset_modelview();
  impl->highlight.reset_modelview();
}

void
SceneContext::render()
{
  if (impl->render_mask & LIGHTMAPSCREEN)
    {
      CL_Display::clear();
      
      CL_Display::push_modelview();
      CL_Display::add_scale(1.0f/LIGHTMAP_DIV, 1.0f/LIGHTMAP_DIV, 1.0f);
      impl->light.render();
      CL_Display::pop_modelview();
      
      // Weird y-pos is needed since OpenGL is upside down when it comes to y-coordinate
      impl->lightmap.bind();
      glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
                          0, 0, 
                          0, 600 - impl->lightmap.get_height(),
                          impl->lightmap.get_width(), impl->lightmap.get_height());
    }

  if (impl->render_mask & COLORMAP)
    {
      impl->color.render();
    }

  if (impl->render_mask & LIGHTMAP)
    {
      impl->lightmap.set_blend_func(blend_dest_color, blend_zero);
      impl->lightmap.set_scale(LIGHTMAP_DIV, -LIGHTMAP_DIV);
      impl->lightmap.draw(0, 600);
    }

  if (impl->render_mask & HIGHLIGHTMAP)
    {
      impl->highlight.render();
    }

  // Clear all DrawingContexts
  impl->color.clear();
  impl->light.clear();
  impl->highlight.clear();
}

void
SceneContext::set_render_mask(unsigned int mask)
{
  impl->render_mask = mask;
}

unsigned int
SceneContext::get_render_mask()
{
  return impl->render_mask;
}

/* EOF */
