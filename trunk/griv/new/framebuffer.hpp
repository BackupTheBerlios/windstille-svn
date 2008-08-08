/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2007 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_DISPLAY_HPP
#define HEADER_DISPLAY_HPP

#include <GL/gl.h>
#include <GL/glu.h>
#include <sstream>
#include <stdexcept>
#include <math.h>
#include "SDL.h"

class Rectf;

static inline void assert_gl(const char* message)
{
  GLenum error = glGetError();
  if(error != GL_NO_ERROR) {
    std::ostringstream msg;
    msg << "OpenGLError while '" << message << "': "
        << gluErrorString(error);
    throw std::runtime_error(msg.str());
  }
}

/** */
class Framebuffer
{
private:
  static SDL_Surface* screen;
  static Uint32 flags;

public:
  static void init();
  static void deinit();

  static void toggle_fullscreen();

  static int get_width()  { return screen->w; }
  static int get_height() { return screen->h; }

  static SDL_Surface* get_screen() { return screen; }
  static void resize(int w, int h);
  static void flip();
  static void clear();

  static void draw_rect(const Rectf& rect);
};

#endif

/* EOF */
