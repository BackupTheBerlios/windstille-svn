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
#include <config.h>

#include <ClanLib/gl.h>
#include <ClanLib/display.h>
#include "surface.hpp"
#include "surface_manager.hpp"

class SurfaceImpl
{
public:
  Texture texture;
  float texcoords[8];
  Rectf  uv;

  int width;
  int height;
};

Surface::Surface()
{
}

Surface::Surface(const std::string& filename)
{
  // FIXME: a bit ugly, should move some of the surface_manager code over here
  *this = surface_manager->get(filename);
}

static int power_of_two(int val) {
  int result = 1;
  while(result < val)
    result *= 2;
  return result;
}

Surface::Surface(int width, int height)
  : impl(new SurfaceImpl())
{
  impl->width  = width;
  impl->height = height;

  impl->texture = Texture(power_of_two(width), power_of_two(height));
  impl->uv      = Rectf(0, 0,
                        float(impl->width)  / impl->texture.get_width(),
                        float(impl->height) / impl->texture.get_height());

  // Keep the texcoords in array form for convenience
  impl->texcoords[0] = impl->uv.left;
  impl->texcoords[1] = impl->uv.top;
  impl->texcoords[2] = impl->uv.right;
  impl->texcoords[3] = impl->uv.top;
  impl->texcoords[4] = impl->uv.right;
  impl->texcoords[5] = impl->uv.bottom;
  impl->texcoords[6] = impl->uv.left;
  impl->texcoords[7] = impl->uv.bottom;
}


Surface::Surface(Texture texture, const Rectf& rect, int width, int height)
  : impl(new SurfaceImpl())
{
  impl->texture = texture;
  impl->width   = width;
  impl->height  = height;
  impl->uv      = rect;

  // Keep the texcoords in array form for convenience
  impl->texcoords[0] = rect.left;
  impl->texcoords[1] = rect.top;
  impl->texcoords[2] = rect.right;
  impl->texcoords[3] = rect.top;
  impl->texcoords[4] = rect.right;
  impl->texcoords[5] = rect.bottom;
  impl->texcoords[6] = rect.left;
  impl->texcoords[7] = rect.bottom;
}

Surface::~Surface()
{
}

int
Surface::get_width()  const
{
  return impl->width;
}

int
Surface::get_height() const
{ 
  return impl->height; 
}

Texture
Surface::get_texture() const
{
  return impl->texture;
}

const float*
Surface::get_texcoords() const
{
  return impl->texcoords; 
}

Rectf
Surface::get_uv() const
{
  return impl->uv;
}

Surface::operator bool() const
{
  return !impl.is_null();
}

void
Surface::draw(int x, int y) const
{
  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active(); 
  state.setup_2d(); 

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, impl->texture.get_handle());

  glBegin(GL_QUADS);

  glTexCoord2f(impl->uv.left, impl->uv.top);
  glVertex2f(x, y);

  glTexCoord2f(impl->uv.right, impl->uv.top);
  glVertex2f(x + impl->width, y);

  glTexCoord2f(impl->uv.right, impl->uv.bottom);
  glVertex2f(x + impl->width, y + impl->height);

  glTexCoord2f(impl->uv.left, impl->uv.bottom);
  glVertex2f(x, y + impl->height);

  glEnd();
}

/* EOF */
