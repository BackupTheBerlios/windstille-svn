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
#include "windstille_error.hpp"
#include "blitter.hpp"
#include "globals.hpp"
#include "tile_packer.hpp"

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

static void generate_border(CL_PixelBuffer buffer, int x_pos, int y_pos)
{
  buffer.lock();
  unsigned char* data = static_cast<unsigned char*>(buffer.get_data());
  int pitch = buffer.get_pitch();

  // duplicate the top line
  memcpy(data + (y_pos-1)*pitch + 4*x_pos, 
         data + (y_pos)*pitch + 4*x_pos,
         4*(TILE_RESOLUTION+2));
  // duplicate the bottom line
  memcpy(data + (y_pos+TILE_RESOLUTION)*pitch + 4*x_pos, 
         data + (y_pos+TILE_RESOLUTION-1)*pitch + 4*x_pos,  
         4*(TILE_RESOLUTION+2));

  // duplicate left and right borders
  for(int y = y_pos-1; y < y_pos + TILE_RESOLUTION+1; ++y)
    {
      data[y*pitch + 4*(x_pos-1) + 0] = data[y*pitch + 4*(x_pos) + 0];
      data[y*pitch + 4*(x_pos-1) + 1] = data[y*pitch + 4*(x_pos) + 1];
      data[y*pitch + 4*(x_pos-1) + 2] = data[y*pitch + 4*(x_pos) + 2];
      data[y*pitch + 4*(x_pos-1) + 3] = data[y*pitch + 4*(x_pos) + 3];

      data[y*pitch + 4*(x_pos + TILE_RESOLUTION) + 0] = data[y*pitch + 4*(x_pos + TILE_RESOLUTION-1) + 0];
      data[y*pitch + 4*(x_pos + TILE_RESOLUTION) + 1] = data[y*pitch + 4*(x_pos + TILE_RESOLUTION-1) + 1];
      data[y*pitch + 4*(x_pos + TILE_RESOLUTION) + 2] = data[y*pitch + 4*(x_pos + TILE_RESOLUTION-1) + 2];
      data[y*pitch + 4*(x_pos + TILE_RESOLUTION) + 3] = data[y*pitch + 4*(x_pos + TILE_RESOLUTION-1) + 3];
    }

  buffer.unlock();
}

/** Pack a tile and return the position where it is placed in the
    pixel buffer */
CL_Rectf
TilePacker::pack(CL_PixelBuffer tile)
{
  assert(tile.get_width() == TILE_RESOLUTION && tile.get_height() == TILE_RESOLUTION);
  assert(!is_full());

  blit_opaque(impl->buffer, tile, impl->x_pos+1, impl->y_pos+1);
  generate_border(impl->buffer, impl->x_pos+1, impl->y_pos+1);

  CL_Rectf rect(CL_Pointf((impl->x_pos+1)/1024.0f, 
                          (impl->y_pos+1)/1024.0f), 
                CL_Sizef((TILE_RESOLUTION)/1024.0f, 
                         (TILE_RESOLUTION)/1024.0f));

  // we move by TILE_RESOLUTION+1 to avoid tiles bleeding into each other
  // when blending
  impl->x_pos += TILE_RESOLUTION + 2; 

  if (impl->x_pos + TILE_RESOLUTION > impl->buffer.get_width())
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
  return (impl->y_pos + TILE_RESOLUTION > impl->buffer.get_height());
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
