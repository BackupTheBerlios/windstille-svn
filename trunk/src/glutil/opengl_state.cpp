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

#include <assert.h>
#include <map>
#include <iostream>
#include <GL/gl.h>
#include <GL/glext.h>
#include "texture.hpp"
#include "color.hpp"
#include "globals.hpp"
#include "display/display.hpp"
#include "util.hpp"
#include "opengl_state.hpp"

class OpenGLStateImpl
{
public:
  /** Used to check if the OpenGLState was properly used or if
      somebody forget the final activate() call */
  bool was_activated;

  Texture texture;

  Color color;

  GLenum blend_sfactor;
  GLenum blend_dfactor;

  /** glEnable/glDisable */
  std::map<GLenum, bool> state;

  /** glEnableClientState/glDisableClientState */
  std::map<GLenum, bool> client_state;

  OpenGLStateImpl()
    : was_activated(false)
  {}
};

OpenGLState* OpenGLState::global_ = 0;

void
OpenGLState::init()
{
  // Init the default settings
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  global_ = new OpenGLState();

  if (debug)
    global_->verify();
}

OpenGLState*
OpenGLState::global()
{
  return global_;
}

// The code here is just as placeholder for the moment, should be
// replaced with something more optimized later
OpenGLState::OpenGLState()
  : impl(new OpenGLStateImpl())
{
  impl->state[GL_TEXTURE_2D]  = false;
  impl->state[GL_DEPTH_TEST]  = false;
  impl->state[GL_BLEND]       = false;
  impl->state[GL_LINE_SMOOTH] = false;

  impl->client_state[GL_VERTEX_ARRAY]        = false;
  impl->client_state[GL_COLOR_ARRAY]         = false;
  impl->client_state[GL_TEXTURE_COORD_ARRAY] = false;
  impl->client_state[GL_NORMAL_ARRAY]        = false;
  
  impl->blend_sfactor      = GL_SRC_ALPHA;
  impl->blend_dfactor      = GL_ONE_MINUS_SRC_ALPHA;
  
  impl->color              = Color(1.0f, 1.0f, 1.0f);
}

OpenGLState::~OpenGLState()
{
  assert(impl->was_activated);
}

void
OpenGLState::bind_texture(const Texture& texture)
{
  impl->texture = texture;
}

void
OpenGLState::set_blend_func(GLenum sfactor, GLenum dfactor)
{
  impl->blend_sfactor = sfactor;
  impl->blend_dfactor = dfactor;
}

bool
OpenGLState::get_state(GLenum cap) const
{
  std::map<GLenum, bool>::const_iterator i = impl->state.find(cap);
  if (i == impl->state.end())
    {
      std::cout << "OpenGLState: Unknown state requested: " << cap << std::endl;
      return false;
    }
  else
    {
      return i->second;
    }
}

void 
OpenGLState::set_state(GLenum cap, bool value)
{
  std::map<GLenum, bool>::iterator i = impl->state.find(cap);
  if (i == impl->state.end())
    {
      std::cout << "OpenGLState: Unknown state set: " << cap << std::endl;
    }
  else
    {
      i->second = value;
    } 
}

void
OpenGLState::enable(GLenum cap)
{
  set_state(cap, true);
}

void
OpenGLState::disable(GLenum cap)
{  
  set_state(cap, false);
}

bool
OpenGLState::get_client_state(GLenum array) const
{
  std::map<GLenum, bool>::const_iterator i = impl->client_state.find(array);
  if (i == impl->client_state.end())
    {
      std::cout << "OpenGLState: Unknown client state requested: " << array << std::endl;
      return false;
    }
  else
    {
      return i->second;
    }
}

void
OpenGLState::set_client_state(GLenum array, bool value)
{
  std::map<GLenum, bool>::iterator i = impl->client_state.find(array);
  if (i == impl->client_state.end())
    {
      std::cout << "OpenGLState: Unknown client state set: " << array << std::endl;
    }
  else
    {
      i->second = value;
    }
}
  
void
OpenGLState::enable_client_state(GLenum array)
{
  set_client_state(array, true);
}

void
OpenGLState::disable_client_state(GLenum array)
{
  set_client_state(array, false);
}

void
OpenGLState::color(const Color& color)
{
  impl->color = color;
}

void
OpenGLState::activate()
{
  // do nothing for now, should be implemented later on
  impl->was_activated = true;

  OpenGLState* global = OpenGLState::global();;
  assert(global);

  // always apply color since it might have got changed between a glBegin/glEnd
  glColor4f(impl->color.r, impl->color.g, impl->color.b, impl->color.a);

  for(std::map<GLenum, bool>::iterator i = impl->state.begin();
      i != impl->state.end(); ++i)
    {
      if (global->get_state(i->first) != i->second)
        {
          if (i->second)
            glEnable(i->first);
          else
            glDisable(i->first);

          global->set_state(i->first, i->second);
        }
    }

  for(std::map<GLenum, bool>::iterator i = impl->client_state.begin();
      i != impl->client_state.end(); ++i)
    {
      if (global->get_client_state(i->first) != i->second)
        {
          if (i->second)
            glEnableClientState(i->first);
          else
            glDisableClientState(i->first);

          global->set_client_state(i->first, i->second);
        }
    }

  if (impl->blend_sfactor != global->impl->blend_sfactor ||
      impl->blend_dfactor != global->impl->blend_dfactor)
    {
      glBlendFunc(impl->blend_sfactor, impl->blend_dfactor);

      global->impl->blend_sfactor = impl->blend_sfactor;
      global->impl->blend_dfactor = impl->blend_dfactor;
    }

  if (impl->texture != global->impl->texture)
    {
      if (impl->texture)
        {
          glBindTexture(GL_TEXTURE_2D, impl->texture.get_handle());
          global->impl->texture = impl->texture;
        }
    }

  if (debug)
    verify();
}

void
OpenGLState::verify()
{
  for(std::map<GLenum, bool>::iterator i = impl->client_state.begin();
      i != impl->client_state.end(); ++i)
    {
      if (glIsEnabled(i->first) != i->second)
        {
          std::cout << "OpenGLState: client_state " << i->first << " is out of sync" << std::endl;
        }
    }  

  for(std::map<GLenum, bool>::iterator i = impl->state.begin();
      i != impl->state.end(); ++i)
    {
      if (glIsEnabled(i->first) != i->second)
        {
          std::cout << "OpenGLState: state " << i->first << " is out of sync" << std::endl;
        }
    }

  GLint sfactor; glGetIntegerv(GL_BLEND_SRC, &sfactor);
  if (sfactor != int(impl->blend_sfactor))
    {
      std::cout << "OpenGLState: src blendfunc is out of sync" << std::endl;
    }

  GLint dfactor; glGetIntegerv(GL_BLEND_DST, &dfactor);
  if (dfactor != int(impl->blend_dfactor))
    {
      std::cout << "OpenGLState: dst blendfunc is out of sync" << std::endl;
    }

  GLint texture_handle;
  glGetIntegerv(GL_TEXTURE_2D_BINDING_EXT, &texture_handle);
  if (impl->texture && static_cast<GLuint>(texture_handle) != impl->texture.get_handle())
    {
      std::cout << "OpenGLState: texture handle is out of sync: " << impl->texture.get_handle() << std::endl;
    }

  assert_gl("OpenGLState::verify");
}

/* EOF */
