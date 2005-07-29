/*  $Id: windstille_main.hpp 752 2005-07-25 10:00:44Z grumbel $
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>,
**                          Matthias Braun <matze@braunis.de>
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

#include <stdexcept>
#include <GL/gl.h>
#include <GL/glext.h>
#include "texture.hpp"
#include "util.hpp"

class TextureImpl
{
public:
  GLuint handle;
  int    width;
  int    height;

  TextureImpl()
  {
    glGenTextures(1, &handle);
  }

  TextureImpl::~TextureImpl()
  {
    glDeleteTextures(1, &handle);
  }
};

Texture::Texture()
{
}

Texture::Texture(int width, int height)
  : impl(new TextureImpl())
{
  impl->width  = width;
  impl->height = height;

  glBindTexture(GL_TEXTURE_2D, impl->handle);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
}

static inline bool is_power_of_2(int v)
{
  return (v & (v-1)) == 0;
}

Texture::Texture(SDL_Surface* image)
  : impl(new TextureImpl())
{
  impl->width  = image->w;
  impl->height = image->h;

  const SDL_PixelFormat* format = image->format;
  if(!is_power_of_2(image->w) || !is_power_of_2(image->h))
    throw std::runtime_error("image has no power of 2 size");
  if(format->BitsPerPixel != 24 && format->BitsPerPixel != 32)
    throw std::runtime_error("image has not 24 or 32 bit color depth");

  glEnable(GL_TEXTURE_2D);

  assert_gl("creating texture handle.");

  try 
    {
      GLint maxt;
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxt);
      if(image->w > maxt || image->h > maxt)
        throw std::runtime_error("Texture size not supported");

      glBindTexture(GL_TEXTURE_2D, impl->handle);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glPixelStorei(GL_UNPACK_ROW_LENGTH, image->pitch/format->BytesPerPixel);
      glTexImage2D(GL_TEXTURE_2D, 0, format->BytesPerPixel,
                   image->w, image->h, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, image->pixels);

      assert_gl("creating texture");

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);

      assert_gl("setting texture parameters");
    } 
  catch(...)
    {
      throw;
    }
}

Texture::~Texture()
{
}

int
Texture::get_width() const
{
  return impl->width;
}

int
Texture::get_height() const
{
  return impl->height;
}

GLuint
Texture::get_handle() const
{
  return impl->handle;
}

void
Texture::put(SDL_Surface* image, int x, int y)
{
  // FIXME: Add some checks here to make sure image has the right format 
  glBindTexture(GL_TEXTURE_2D, impl->handle);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH,
                image->pitch / image->format->BytesPerPixel);

  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y,
                  image->w, image->h, GL_RGBA, GL_UNSIGNED_BYTE,
                  image->pixels);
}

void 
Texture::bind()
{
  glBindTexture(GL_TEXTURE_2D, impl->handle);
}

Texture::operator bool() const
{
  return !impl.is_null();
}

/* EOF */
