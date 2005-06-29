//  $Id$
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <assert.h>
#include <ClanLib/Core/Math/rect.h>
#include <ClanLib/Core/Math/point.h>
#include <ClanLib/Display/pixel_buffer.h>
#include <ClanLib/Display/pixel_format.h>
#include <ClanLib/gl.h>
#include "windstille_error.hxx"
#include "blitter.hxx"
#include "globals.hxx"
#include "tile_packer.hxx"

class TilePackerImpl
{
public:
  // Position for the next tile
  int x_pos;
  int y_pos;

  CL_PixelBuffer buffer;
  CL_OpenGLSurface texture;
};

TilePacker::TilePacker(int width, int height)
  : impl(new TilePackerImpl())
{
  impl->x_pos = 0;
  impl->y_pos = 0;

  impl->buffer = CL_PixelBuffer(width, height, width*4,
                                CL_PixelFormat::rgba8888);
}

TilePacker::~TilePacker()
{
  delete impl;
}

/** Pack a tile and return the position where it is placed in the
    pixel buffer */
CL_Rectf
TilePacker::pack(CL_PixelBuffer tile)
{
  assert(tile.get_width() == TILE_SIZE && tile.get_height() == TILE_SIZE);
  assert(!is_full());

  blit_opaque(impl->buffer, tile, impl->x_pos+1, impl->y_pos+1);
  blit_opaque(impl->buffer, tile, impl->x_pos+1, impl->y_pos);
  blit_opaque(impl->buffer, tile, impl->x_pos, impl->y_pos+1);

  blit_opaque(impl->buffer, tile, impl->x_pos, impl->y_pos);

  float factor = 0;
  CL_Rectf rect(CL_Pointf((impl->x_pos)/1024.0f, 
                          (impl->y_pos)/1024.0f), 
                CL_Sizef((TILE_SIZE+factor)/1024.0f, 
                         (TILE_SIZE+factor)/1024.0f));

  // we move by TILE_SIZE+1 to avoid tiles bleeding into each other
  // when blending
  impl->x_pos += TILE_SIZE + 2; 

  if (impl->x_pos + TILE_SIZE > impl->buffer.get_width())
    {
      impl->x_pos = 0;
      impl->y_pos += TILE_SIZE + 2;
    }

  return rect;
}

/** Return true if the PixelBuffer is full */
bool
TilePacker::is_full() const
{
  return (impl->y_pos + TILE_SIZE > impl->buffer.get_height());
}

CL_OpenGLSurface
TilePacker::get_texture()
{
  if (impl->texture)
    {
      return CL_Surface(impl->texture);
    }
  else
    {
      impl->texture = CL_Surface(CL_OpenGLSurface(impl->buffer));
      return CL_Surface(impl->texture);
    }
}

CL_PixelBuffer
TilePacker::get_pixelbuffer() const
{
  return impl->buffer;
}

/* EOF */
