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

#include <config.h>

#include <assert.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include "display/texture.hpp"
#include "globals.hpp"
#include "tile_packer.hpp"
#include "util.hpp"
#include "blitter.hpp"

class TilePackerImpl
{
public:
  // Position for the next tile
  int x_pos;
  int y_pos;

  Texture texture;

  int width;
  int height;
};

TilePacker::TilePacker(int width, int height)
  : impl(new TilePackerImpl())
{
  impl->x_pos = 0;
  impl->y_pos = 0;

  impl->width  = width;
  impl->height = height;

  impl->texture = Texture(GL_TEXTURE_2D, width, height);
        
  assert_gl("setting TilePacker texture parameters"); 
}

TilePacker::~TilePacker()
{
  delete impl;
}

/** Pack a tile and return the position where it is placed in the
    pixel buffer */
Rectf
TilePacker::pack(SDL_Surface* image, int x, int y, int w, int h)
{
  assert(w == TILE_RESOLUTION && h == TILE_RESOLUTION);
  assert(!is_full());

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  SDL_Surface* convert = SDL_CreateRGBSurface(SDL_SWSURFACE,
    w+2, h+2, 32,
    0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
  SDL_Surface* convert = SDL_CreateRGBSurface(SDL_SWSURFACE,
    w+2, h+2, 32,
      0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
  if(convert == 0)
    throw std::runtime_error("Couldn't pack texture: out of memory");

  SDL_Rect source_rect;
  source_rect.x = x;
  source_rect.y = y;
  source_rect.w = w;
  source_rect.h = h;
  SDL_Rect dest_rect;
  dest_rect.x = 1;
  dest_rect.y = 1;
  dest_rect.w = w;
  dest_rect.h = h;
  SDL_SetAlpha(image, 0, 0);
  SDL_BlitSurface(image, &source_rect, convert, &dest_rect);
  generate_border(convert, 1, 1, TILE_RESOLUTION, TILE_RESOLUTION);

  impl->texture.put(convert, impl->x_pos, impl->y_pos);
  SDL_FreeSurface(convert);

  assert_gl("updating tilepacker texture");

  Rectf rect(Vector(static_cast<float>(impl->x_pos+1)/impl->width, 
                    static_cast<float>(impl->y_pos+1)/impl->height), 
             Sizef(static_cast<float>(TILE_RESOLUTION)/impl->width, 
                   static_cast<float>(TILE_RESOLUTION)/impl->height));

  // we move by TILE_RESOLUTION+1 to avoid tiles bleeding into each other
  // when blending
  impl->x_pos += TILE_RESOLUTION + 2; 
  if (impl->x_pos + TILE_RESOLUTION > impl->width)
    {
      impl->x_pos = 0;
      impl->y_pos += TILE_RESOLUTION + 2;
    }

  return rect;
}

/** Return true if the PixelBuffer is full */
bool
TilePacker::is_full() const
{
  return (impl->y_pos + TILE_RESOLUTION + 2 > impl->height);
}

Texture
TilePacker::get_texture() const
{
  return impl->texture;
}

/* EOF */
