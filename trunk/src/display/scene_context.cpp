/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>

#include <iostream>

#include "util.hpp"
#include "display/display.hpp"
#include "display/surface.hpp"
#include "display/opengl_state.hpp"

#include "shader_program.hpp"
#include "shader_object.hpp"
#include "framebuffer.hpp"
#include "scene_context.hpp"
#include <assert.h>

// The lightmap has a resolution of screen.w/LIGHTMAP, screen.h/LIGHTMAP
#define LIGHTMAP_DIV 4
#define BLURMAP_DIV 1

class SceneContextImpl
{
public:
  DrawingContext color;
  DrawingContext light;
  DrawingContext highlight; 
  unsigned int render_mask;

  Framebuffer framebuffer;
  Surface lightmap;

  Framebuffer blur_framebuffer;
  ShaderProgram shader_program;
  ShaderProgram simple_program;
  Texture noise;

  SceneContextImpl() 
    : render_mask(SceneContext::COLORMAP |
                  SceneContext::LIGHTMAP | 
                  SceneContext::HIGHLIGHTMAP | 
                  SceneContext::LIGHTMAPSCREEN |
                  SceneContext::BLURMAP
                  ),
      framebuffer(GL_TEXTURE_2D, 256, 256),
      lightmap(framebuffer.get_texture(), 
               Rectf(0, 0, (800/LIGHTMAP_DIV)/256.0f, (600/LIGHTMAP_DIV)/256.0f),
               800/LIGHTMAP_DIV, 600/LIGHTMAP_DIV),
      //lightmap(800/LIGHTMAP_DIV, 600/LIGHTMAP_DIV)
      //blur_framebuffer(GL_TEXTURE_2D, 1024, 1024)
      blur_framebuffer(GL_TEXTURE_RECTANGLE_ARB, 800, 600)
  {
    shader_program.attach(ShaderObject(GL_FRAGMENT_SHADER_ARB, "data/shader/shockwave2.frag"));
    shader_program.link();
    noise = Texture("images/noise3.png");
    noise.set_wrap(GL_REPEAT);
    noise.set_filter(GL_LINEAR);

    glUseProgramObjectARB(shader_program.get_handle());
    shader_program.set_uniform1i("background_tex", 0);
    shader_program.set_uniform1i("noise_tex",   1);
    shader_program.set_uniform1f("time", fmod(SDL_GetTicks()/10000.0f, 1.0f));
    glUseProgramObjectARB(0);

    simple_program.attach(ShaderObject(GL_FRAGMENT_SHADER_ARB, "data/shader/simple.frag"));
    simple_program.link();
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
SceneContext::rotate(float angel, float x, float y, float z)
{
  impl->color.rotate(angel, x, y, z);
  impl->light.rotate(angel, x, y, z);
  impl->highlight.rotate(angel, x, y, z);
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
SceneContext::mult_modelview(const Matrix& matrix)
{
  impl->color.mult(matrix);
  impl->light.mult(matrix);
  impl->highlight.mult(matrix);
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


void draw_disc(int count)
{
  float radius = (count)*2.0f;
  float minradius = 2.0f*count - 164.0f;
  if (minradius < 0)
    minradius = 0;
  glClear(GL_DEPTH_BUFFER_BIT);

  int segments = 64;
  
  glBegin(GL_QUADS);
  for (int i = 0; i < segments; ++i)
    {
      float angel = (2*M_PI / segments);

      float x1 =  sin(angel*i)*radius;
      float y1 = -cos(angel*i)*radius;

      float x2 =  sin(angel*(i+1))*radius;
      float y2 = -cos(angel*(i+1))*radius;

      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glTexCoord2f(x1/512.0f+0.5f, y1/512.0f+0.5f);
      glVertex3f(x1+256, y1+256, 0);

      glTexCoord2f(x2/512.0f+0.5f, y2/512.0f+0.5f);
      glVertex3f(x2+256, y2+256, 0);


      float x3 =  sin(angel*i)*minradius;
      float y3 = -cos(angel*i)*minradius;

      float x4 =  sin(angel*(i+1))*minradius;
      float y4 = -cos(angel*(i+1))*minradius;

      glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
      glTexCoord2f(x4/512.0f+0.5f, y4/512.0f+0.5f);
      glVertex3f(x4+256, y4+256, 0);
      glTexCoord2f(x3/512.0f+0.5f, y3/512.0f+0.5f);
      glVertex3f(x3+256, y3+256, 0);
    }
  glEnd();
}


void
SceneContext::render()
{
  glClear(GL_DEPTH_BUFFER_BIT);
      
  if (impl->render_mask & LIGHTMAPSCREEN)
    {
      if (0)
        { // render lightmap to screen
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
      else
        {
          glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, impl->framebuffer.get_handle());
          glClear(GL_DEPTH_BUFFER_BIT);

          glPushMatrix();
          glTranslatef(0, 600-(600/LIGHTMAP_DIV), 0);
          glScalef(1.0f/LIGHTMAP_DIV, 1.0f/LIGHTMAP_DIV, 1.0f);
          impl->light.render();
          glPopMatrix();

          glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        }
    }

  if (impl->render_mask & COLORMAP)
    {
      impl->color.render();
    }

  if (impl->render_mask & LIGHTMAP)
    {
      // FIXME: 2006-01-09: shouldn't use Surface and just discard the
      // borders of a power of two texture, but instead use them and
      // set UV accordingly
      OpenGLState state;

      Rectf uv = impl->lightmap.get_uv();

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
      glVertex2f(impl->lightmap.get_width() * LIGHTMAP_DIV,
                 impl->lightmap.get_height() * LIGHTMAP_DIV);

      glTexCoord2f(uv.left, uv.top);
      glVertex2f(0, impl->lightmap.get_height() * LIGHTMAP_DIV);

      glEnd();
    }

  if (impl->render_mask & HIGHLIGHTMAP)
    {
      impl->highlight.render();
    }

  if (impl->render_mask & BLURMAP)
    {
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, impl->blur_framebuffer.get_handle());
      glClear(GL_DEPTH_BUFFER_BIT);

      glPushMatrix();
      glTranslatef(0, 600-(600/BLURMAP_DIV), 0);
      glScalef(1.0f/BLURMAP_DIV, 1.0f/BLURMAP_DIV, 1.0f);
      impl->color.render();
      glPopMatrix();

      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);     

      if (0)
        { // test render the blur buffer

          Rectf uv(0, 0, 800, 600);
          //Rectf uv(0, 0, 1, 1);

          float div = 0.5f;
          glUseProgramObjectARB(impl->simple_program.get_handle());
          impl->simple_program.set_uniform1i("background", 0);

          OpenGLState state;
          state.bind_texture(impl->blur_framebuffer.get_texture(), 0);
          state.activate();

          glBegin(GL_QUADS);

          glTexCoord2f(uv.left, uv.bottom);
          glVertex2f(0, 0);

          glTexCoord2f(uv.right, uv.bottom);
          glVertex2f(impl->blur_framebuffer.get_width() * div, 0);

          glTexCoord2f(uv.right, uv.top);
          glVertex2f(impl->blur_framebuffer.get_width() * div,
                     impl->blur_framebuffer.get_height() * div);

          glTexCoord2f(uv.left, uv.top);
          glVertex2f(0, impl->blur_framebuffer.get_height() * div);

          glEnd();         
          glUseProgramObjectARB(0);
          glDisable(GL_TEXTURE_RECTANGLE_ARB);
        }

      if (0)
        { // Draw funny effect with shader
          OpenGLState state;

          Rectf uv(0, 0, 800, 600);
          
          state.bind_texture(impl->blur_framebuffer.get_texture(), 0);
          state.bind_texture(impl->noise, 1);
          state.disable(GL_BLEND);

          //state.enable(GL_BLEND);
          //state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          //state.set_blend_func(GL_SRC_ALPHA, GL_ONE);
          state.activate();

          glUseProgramObjectARB(impl->shader_program.get_handle());

          glBegin(GL_QUADS);

          glTexCoord2f(uv.left, uv.bottom);
          glVertex2f(0, 0);

          glTexCoord2f(uv.right, uv.bottom);
          glVertex2f(impl->blur_framebuffer.get_width() * BLURMAP_DIV, 0);

          glTexCoord2f(uv.right, uv.top);
          glVertex2f(impl->blur_framebuffer.get_width() * BLURMAP_DIV,
                     impl->blur_framebuffer.get_height() * BLURMAP_DIV);

          glTexCoord2f(uv.left, uv.top);
          glVertex2f(0, impl->blur_framebuffer.get_height() * BLURMAP_DIV);

          glEnd();

          glUseProgramObjectARB(0);
        }
    }


  if (1) 
    {
      OpenGLState state;
      state.bind_texture(impl->blur_framebuffer.get_texture(), 0);
      state.bind_texture(impl->noise, 1);
      state.disable(GL_BLEND);
      state.set_blend_func(GL_SRC_ALPHA, GL_ONE);
      state.activate();

      glUseProgramObjectARB(impl->shader_program.get_handle());    
      float radius = 100 * (sinf(SDL_GetTicks()/3000.0f) + 1.0f);
      impl->shader_program.set_uniform1f("radius",   radius/512.0f*2.0f);
      draw_disc(int(radius));
      glUseProgramObjectARB(0);
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
