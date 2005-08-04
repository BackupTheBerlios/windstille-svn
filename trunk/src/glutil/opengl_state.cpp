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

#include "texture.hpp"
#include "color.hpp"
#include "opengl_state.hpp"

class OpenGLStateImpl
{
public:
  bool was_activated;

  OpenGLStateImpl()
    : was_activated(false)
  {}

};

// The code here is just as placeholder for the moment, should be
// replaced with something more optimized later
OpenGLState::OpenGLState()
  : impl(new OpenGLStateImpl())
{
  // init some defaults
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

OpenGLState::~OpenGLState()
{
  assert(impl->was_activated);
}

void
OpenGLState::bind_texture(const Texture& texture)
{
  glBindTexture(GL_TEXTURE_2D, texture.get_handle());
}

void
OpenGLState::set_blend_func(GLenum sfactor, GLenum dfactor)
{
  glBlendFunc(sfactor, dfactor);
}

void
OpenGLState::enable(GLenum cap)
{
  glEnable(cap);
}

void
OpenGLState::disable(GLenum cap)
{  
  glDisable(cap);
}
  
void
OpenGLState::enable_client_state(GLenum array)
{
  glEnableClientState(array);
}

void
OpenGLState::disable_client_state(GLenum array)
{
  glDisableClientState(array);
}

void
OpenGLState::color(const Color& color)
{
  glColor4f(color.r, color.g, color.b, color.a);
}

void
OpenGLState::activate()
{
  // do nothing for now, should be implemented later on
  impl->was_activated = true;
}

/* EOF */
