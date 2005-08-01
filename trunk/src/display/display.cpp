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

#include <ClanLib/gl.h>
#include <ClanLib/display.h>
#include <cmath>
#include "display.hpp"

void
VDisplay::draw_line(const Vector& pos1, const Vector& pos2, const Color& color)
{
  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active();
  state.setup_2d();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(color.r, color.g, color.b, color.a);

  glBegin(GL_LINES);
  glVertex2f(pos1.x, pos1.y);
  glVertex2f(pos2.x, pos2.y);
  glEnd(); 
}

void
VDisplay::fill_rect(const Rectf& rect, const Color& color)
{
  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active();
  state.setup_2d();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(color.r, color.g, color.b, color.a);

  glBegin(GL_QUADS);
  glVertex2f(rect.left,  rect.top);
  glVertex2f(rect.right, rect.top);
  glVertex2f(rect.right, rect.bottom);
  glVertex2f(rect.left,  rect.bottom);
  glEnd();
}

void
VDisplay::draw_rect(const Rectf& rect, const Color& color)
{
  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active();
  state.setup_2d();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(color.r, color.g, color.b, color.a);

  glBegin(GL_LINE_STRIP);
  glVertex2f(rect.left,  rect.top);
  glVertex2f(rect.right, rect.top);
  glVertex2f(rect.right, rect.bottom);
  glVertex2f(rect.left,  rect.bottom);
  glVertex2f(rect.left,  rect.top);
  glEnd();
}

void
VDisplay::fill_rounded_rect(const Rectf& rect, float radius, const Color& color)
{
  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active();
  state.setup_2d();

  // Keep radius in the limits, so that we get a circle instead of
  // just graphic junk
  radius = std::min(radius, std::min(rect.get_width()/2, rect.get_height()/2));

  // inner rectangle
  Rectf irect(rect.left    + radius,
              rect.top     + radius,
              rect.right   - radius,
              rect.bottom  - radius);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(color.r, color.g, color.b, color.a);

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
VDisplay::draw_rounded_rect(const Rectf& rect, float radius, const Color& color)
{
  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active();
  state.setup_2d();

  // Keep radius in the limits, so that we get a circle instead of
  // just graphic junk
  radius = std::min(radius, std::min(rect.get_width()/2, rect.get_height()/2));

  // inner rectangle
  Rectf irect(rect.left    + radius,
              rect.top     + radius,
              rect.right   - radius,
              rect.bottom  - radius);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(color.r, color.g, color.b, color.a);

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
VDisplay::get_width()
{
  return 800;
}

int
VDisplay::get_height()
{
  return 600;
}

/* EOF */
