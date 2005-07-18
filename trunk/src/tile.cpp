//  $Id: tile.cxx,v 1.4 2003/09/22 18:37:05 grumbel Exp $
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

#include <ClanLib/Core/System/error.h>
#include <ClanLib/Display/sprite_description.h>
#include <ClanLib/Display/pixel_buffer.h>
#include <iostream>
#include "globals.hpp"
#include "tile.hpp"

Tile::Tile(const std::string& filename, 
           const std::string& highlight_filename,
           unsigned int arg_colmap)
  : colmap(arg_colmap)
{
  color     = CL_Sprite(filename, resources);
  highlight = CL_Sprite(highlight_filename, resources);

  color_packer = -1;
  highlight_packer = -1;
}

Tile::Tile(const CL_PixelBuffer& buffer, 
           const CL_PixelBuffer& hl_buffer, 
           unsigned int arg_colmap)
  : colmap(arg_colmap)
{
  CL_SpriteDescription desc;
  desc.add_frame(buffer);
  color = CL_Sprite(desc);

  color.set_scale(0.5f, 0.5f);
  if (hl_buffer)
    {
      CL_SpriteDescription hl_desc;
      hl_desc.add_frame(hl_buffer);
      highlight = CL_Sprite(hl_desc);
      highlight.set_scale(0.5f, 0.5f);
    }

  color_packer = -1;
  highlight_packer = -1;
}

CL_Sprite&
Tile::get_color_sprite()
{
  return color;
}

CL_Sprite&
Tile::get_highlight_sprite()
{
  return highlight;
}

/* EOF */
