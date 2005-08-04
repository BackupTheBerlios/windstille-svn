/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <cmath>
#include <stdexcept>
#include <SDL.h>
#include "gameconfig.hpp"
#include "glutil/opengl_state.hpp"
#include "display.hpp"

SDL_Surface* window;

void
Display::draw_line(const Vector& pos1, const Vector& pos2, const Color& color)
{
  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  glBegin(GL_LINES);
  glVertex2f(pos1.x, pos1.y);
  glVertex2f(pos2.x, pos2.y);
  glEnd(); 
}

void
Display::fill_rect(const Rectf& rect, const Color& color)
{
  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  glBegin(GL_QUADS);
  glVertex2f(rect.left,  rect.top);
  glVertex2f(rect.right, rect.top);
  glVertex2f(rect.right, rect.bottom);
  glVertex2f(rect.left,  rect.bottom);
  glEnd();
}

void
Display::draw_rect(const Rectf& rect, const Color& color)
{
  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  glBegin(GL_LINE_STRIP);
  glVertex2f(rect.left,  rect.top);
  glVertex2f(rect.right, rect.top);
  glVertex2f(rect.right, rect.bottom);
  glVertex2f(rect.left,  rect.bottom);
  glVertex2f(rect.left,  rect.top);
  glEnd();
}

void
Display::fill_rounded_rect(const Rectf& rect, float radius, const Color& color)
{
  // Keep radius in the limits, so that we get a circle instead of
  // just graphic junk
  radius = std::min(radius, std::min(rect.get_width()/2, rect.get_height()/2));

  // inner rectangle
  Rectf irect(rect.left    + radius,
              rect.top     + radius,
              rect.right   - radius,
              rect.bottom  - radius);


  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  int n = 8;
  glBegin(GL_QUAD_STRIP);
  for(int i = 0; i <= n; ++i)
    {
      float x = sinf(i * (M_PI/2) / n) * radius;
      float y = cosf(i * (M_PI/2) / n) * radius;

      glVertex2f(irect.left  - x, irect.top - y);
      glVertex2f(irect.right + x, irect.top - y);
    }
  for(int i = 0; i <= n; ++i)
    {
      float x = cosf(i * (M_PI/2) / n) * radius;
      float y = sinf(i * (M_PI/2) / n) * radius;

      glVertex2f(irect.left  - x, irect.bottom + y);
      glVertex2f(irect.right + x, irect.bottom + y);
    }
  glEnd();
}

void
Display::draw_rounded_rect(const Rectf& rect, float radius, const Color& color)
{
  // Keep radius in the limits, so that we get a circle instead of
  // just graphic junk
  radius = std::min(radius, std::min(rect.get_width()/2, rect.get_height()/2));

  // inner rectangle
  Rectf irect(rect.left    + radius,
              rect.top     + radius,
              rect.right   - radius,
              rect.bottom  - radius);

  OpenGLState state;

  state.enable(GL_BLEND);
  state.set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.color(color);
  state.activate();

  int n = 4;
  glBegin(GL_LINE_STRIP);
  for(int i = 0; i <= n; ++i)
    {
      float x = sinf(i * (M_PI/2) / n) * radius;
      float y = cosf(i * (M_PI/2) / n) * radius;

      glVertex2f(irect.left  - x, irect.top - y);
    }
  for(int i = 0; i <= n; ++i)
    {
      float x = cosf(i * (M_PI/2) / n) * radius;
      float y = sinf(i * (M_PI/2) / n) * radius;

      glVertex2f(irect.left  - x, irect.bottom + y);
    }

  for(int i = 0; i <= n; ++i)
    {
      float x = sinf(i * (M_PI/2) / n) * radius;
      float y = cosf(i * (M_PI/2) / n) * radius;

      glVertex2f(irect.right + x, irect.bottom + y);
    }

  for(int i = 0; i <= n; ++i)
    {
      float x = cosf(i * (M_PI/2) / n) * radius;
      float y = sinf(i * (M_PI/2) / n) * radius;
        
      glVertex2f(irect.right + x, irect.top - y);
    }
  // go back to start
  glVertex2f(irect.left, irect.top - radius);

  glEnd();
}

int
Display::get_width()
{
  return window->w;
}

int
Display::get_height()
{
  return window->h;
}

void
Display::init()
{
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

  window = SDL_SetVideoMode(config->screen_width, config->screen_height,
                            0, SDL_OPENGL | (config->use_fullscreen ? SDL_FULLSCREEN : 0));
  if (!window)
    {
      throw std::runtime_error("Display:: Couldn't create window");
    }

  SDL_WM_SetCaption("Windstille", 0 /* icon */);

  glViewport(0, 0, window->w, window->h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

#define cl_pixelcenter_constant 0.375

  glOrtho(0.0, window->w, window->h, 0.0, -1000.0, 1000.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(cl_pixelcenter_constant, cl_pixelcenter_constant, 0.0);
}

void
Display::set_fullscreen(bool fullscreen)
{
  if (fullscreen)
    {
      window = SDL_SetVideoMode(config->screen_width, config->screen_height,
                                0, SDL_OPENGL | SDL_FULLSCREEN);
      if (!window)
        {
          throw std::runtime_error("Display:: Couldn't create window");
        }

    }
  else
    {
      window = SDL_SetVideoMode(config->screen_width, config->screen_height,
                                0, SDL_OPENGL);
      if (!window)
        {
          throw std::runtime_error("Display:: Couldn't create window");
        }
    }
}

/* EOF */
