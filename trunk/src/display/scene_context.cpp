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

#include "display/display.hpp"
#include "glutil/surface.hpp"
#include "glutil/opengl_state.hpp"
#include "scene_context.hpp"

// The lightmap has a resolution of screen.w/LIGHTMAP, screen.h/LIGHTMAP
#define LIGHTMAP_DIV 4

class SceneContextImpl
{
public:
  DrawingContext color;
  DrawingContext light;
  DrawingContext highlight; 
  unsigned int render_mask;

  Surface lightmap;

  SceneContextImpl() 
    : render_mask(SceneContext::COLORMAP |
                  SceneContext::LIGHTMAP | 
                  SceneContext::HIGHLIGHTMAP | 
                  SceneContext::LIGHTMAPSCREEN),
      lightmap(800/LIGHTMAP_DIV, 600/LIGHTMAP_DIV)
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
  glClear(GL_DEPTH_BUFFER_BIT);
      
  if (impl->render_mask & LIGHTMAPSCREEN)
    {
      Display::push_cliprect(Rect(Point(0, 0),
                                  Size(impl->lightmap.get_width(), impl->lightmap.get_height())));

      glPushMatrix();
      glScalef(1.0f/LIGHTMAP_DIV, 1.0f/LIGHTMAP_DIV, 1.0f);

      impl->light.render();
      glPopMatrix();
      
      {
        OpenGLState state;
        
        // Weird y-pos is needed since OpenGL is upside down when it comes to y-coordinate
        state.bind_texture(impl->lightmap.get_texture());
        state.activate();

        glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
                            0, 0, 
                            0, Display::get_height() - impl->lightmap.get_height(),
                            impl->lightmap.get_width(), impl->lightmap.get_height());
      }

      Display::pop_cliprect();
    }

  if (impl->render_mask & COLORMAP)
    {
      impl->color.render();
    }

  if (impl->render_mask & LIGHTMAP)
    {
      OpenGLState state;

      Rectf uv = impl->lightmap.get_uv();

      state.enable(GL_TEXTURE_2D);
      state.bind_texture(impl->lightmap.get_texture());

      state.enable(GL_BLEND);
      state.set_blend_func(GL_DST_COLOR, GL_ZERO);
      state.activate();

      glBegin(GL_QUADS);

      glTexCoord2f(uv.left, uv.bottom);
      glVertex2f(0, 0);

      glTexCoord2f(uv.right, uv.bottom);
      glVertex2f(impl->lightmap.get_width() * LIGHTMAP_DIV, 0);

      glTexCoord2f(uv.right, uv.top);
      glVertex2f(impl->lightmap.get_width()  * LIGHTMAP_DIV,
                 impl->lightmap.get_height() * LIGHTMAP_DIV);

      glTexCoord2f(uv.left, uv.top);
      glVertex2f(0, impl->lightmap.get_height() * LIGHTMAP_DIV);

      glEnd();

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

DrawingContext&
SceneContext::get_layer(unsigned int type)
{
  switch(type)
    {
    case COLORMAP:
      return impl->color;

    case LIGHTMAP:
      return impl->light;

    case HIGHLIGHTMAP:
      return impl->highlight;

    default:
      assert(!"SceneContext::get_layer(): Unknown type");
    }
}

/* EOF */
