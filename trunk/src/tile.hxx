//  $Id: tile.hxx,v 1.6 2003/09/22 18:37:05 grumbel Exp $
// 
//  Windstille - A Jump'n Shoot Game
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

#ifndef HEADER_TILE_HXX
#define HEADER_TILE_HXX

#include <assert.h>
#include <ClanLib/Display/sprite.h>
#include <ClanLib/GL/opengl_surface.h>

/** A Tile is a surface or sprite together with information for
 *  collision detection (aka colmap). The collision map is at a
 *  resolution of 8x8 bits. Position information is handled in the
 *  TileMap and not here. (flyweight pattern). */
class Tile
{
private:
  CL_Sprite color;
  CL_Sprite highlight;

public:
  CL_Rectf         color_rect;
  int              color_packer;

  CL_Rectf         highlight_rect;
  int              highlight_packer;

  int id; 
  /** Bitmaps that holds the collision attributes for this tile */
  unsigned int colmap;

  /** @param filename Surface to use 
   *  @param arg_colmap a 8 char long array */
  Tile(const std::string& filename, 
       const std::string& highlight_filename, 
       unsigned int arg_colmap);

  Tile(const CL_PixelBuffer& buffer, 
       const CL_PixelBuffer& hl_buffer, 
       unsigned int arg_colmap);

  unsigned get_colmap() const { return colmap; }
  CL_Sprite& get_color_sprite();
  CL_Sprite& get_highlight_sprite();
};

#endif

/* EOF */
