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

#ifndef HEADER_OPENGL_STATE_HPP
#define HEADER_OPENGL_STATE_HPP

#include <memory>
#include <GL/gl.h>

class Texture;
class Color;
class OpenGLStateImpl;

/** This class manages the state of a OpenGL context, meaning it tries
    to reduce state changes and in addition to that it gives a well
    default defined state to begin with. */
class OpenGLState
{
public:
  OpenGLState();
  ~OpenGLState();

  void bind_texture(const Texture& texture);
  void set_blend_func(GLenum sfactor, GLenum dfactor);

  void enable(GLenum cap);
  void disable(GLenum cap);
  
  void enable_client_state(GLenum array);
  void disable_client_state(GLenum array);

  void color(const Color& color);

  /** Activates the given state, you *must* call this before you issue
      gl commands that depend on the given state */
  void activate();

private:
  std::auto_ptr<OpenGLStateImpl> impl;

  OpenGLState (const OpenGLState&);
  OpenGLState& operator= (const OpenGLState&);
};

#endif

/* EOF */
