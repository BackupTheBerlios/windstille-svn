//  $Id: tile_factory.cxx,v 1.10 2003/09/22 18:37:05 grumbel Exp $
//
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#include <string>
#include <ClanLib/gl.h>
#include <ClanLib/Core/System/system.h>
#include <ClanLib/Display/pixel_buffer.h>
#include <ClanLib/Display/pixel_format.h>
#include <ClanLib/Display/Providers/provider_factory.h>
#include <ClanLib/Display/Providers/provider_factory.h>
#include <assert.h>
#include <sstream>
#include <iostream>
#include <memory>
#include "globals.hpp"
#include "tile.hpp"
#include "tile_packer.hpp"
#include "tile_factory.hpp"
#include "lisp/lisp.hpp"
#include "lisp/parser.hpp"
#include "lisp/list_iterator.hpp"

extern CL_ResourceManager* resources;

TileFactory* TileFactory::current_ = 0;

std::string TileFactory::tile_def_file = "tiles.scm";

TileFactory::TileFactory (const std::string& filename)
{
  std::auto_ptr<lisp::Lisp> root (lisp::Parser::parse(filename));

  packers.push_back(new TilePacker(1024, 1024));
  packers.push_back(new TilePacker(1024, 1024));
  color_packer     = 0;
  highlight_packer = 1;

  const lisp::Lisp* tiles_lisp = root->get_lisp("windstille-tiles");
  if(!tiles_lisp) {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille tiles file";
    throw std::runtime_error(msg.str());
  }

  lisp::ListIterator iter(tiles_lisp);
  while(iter.next()) {
    if(iter.item() == "tiles") {
      parse_tiles(iter.lisp());
    } else {
      std::cout << "Unknown tag in tiles file: " << iter.item() << "\n";
    }
  }
}

TileFactory::~TileFactory()
{
  for(Tiles::iterator i = tiles.begin(); i != tiles.end(); ++i)
    {
      delete *i;
    }
  tiles.clear();
}

void
TileFactory::parse_tiles(const lisp::Lisp* data)
{
  assert(data);

  std::string filename;
  std::string highlight_filename;
  std::vector<int> colmap;
  std::vector<int> ids;
  
  lisp::ListIterator iter(data);
  while(iter.next()) {
    if(iter.item() == "ids") {
      iter.lisp()->get_vector(ids);
    } else if(iter.item() == "color-image") {
      filename = iter.value().get_string();
    } else if(iter.item() == "highlight-image") {
      highlight_filename = iter.value().get_string();
    } else if(iter.item() == "colmap") {
      iter.lisp()->get_vector(colmap);
    } else {
      std::cerr << "Unknown tag '" << iter.item() << "' found in tiles\n";
    }
  }

  if(filename == "")
    throw std::runtime_error("Missing color-image");
  
  CL_PixelBuffer image = CL_ProviderFactory::load(datadir + filename);
  CL_PixelBuffer hl_image;
  
  if(highlight_filename != "")
    hl_image = CL_ProviderFactory::load(datadir + highlight_filename);

  int num_tiles = (image.get_width()/TILE_SIZE) * (image.get_height()/TILE_SIZE);
  if (int(colmap.size()) != num_tiles)
    throw std::runtime_error("Not enough 'colmap' information for tiles");

  if (int(ids.size()) != num_tiles)
    throw std::runtime_error("Not enough 'ids' information for tiles");
  
  int i = 0;
  // FIMXE: Tiles should share one OpenGL texture
  for (int y = 0; y < image.get_height(); y += TILE_SIZE)
    {
      for (int x = 0; x < image.get_width(); x += TILE_SIZE)
        {
          CL_PixelBuffer chopped_image(TILE_SIZE, TILE_SIZE,
                                       image.get_format().get_depth()*TILE_SIZE,
                                       image.get_format(), NULL);
          chopped_image.lock();
          image.convert(chopped_image.get_data(), 
                        chopped_image.get_format(), 
                        image.get_format().get_depth()*TILE_SIZE, 
                        CL_Rect(CL_Point(0, 0), CL_Size(TILE_SIZE, TILE_SIZE)),
                        CL_Rect(CL_Point(x, y), CL_Size(TILE_SIZE, TILE_SIZE)));
          chopped_image.unlock();

          CL_PixelBuffer hl_chopped_image;

          if (hl_image)
            {
              hl_chopped_image = CL_PixelBuffer(TILE_SIZE, TILE_SIZE,
                                                hl_image.get_format().get_depth()*TILE_SIZE,
                                                hl_image.get_format(), NULL);
              hl_chopped_image.lock();
              hl_image.convert(hl_chopped_image.get_data(), 
                               hl_chopped_image.get_format(), 
                               hl_image.get_format().get_depth()*TILE_SIZE, 
                               CL_Rect(CL_Point(0, 0), CL_Size(TILE_SIZE, TILE_SIZE)),
                               CL_Rect(CL_Point(x, y), CL_Size(TILE_SIZE, TILE_SIZE)));
              hl_chopped_image.unlock();
            }

          pack(ids[i], colmap[y/TILE_SIZE * image.get_width()/TILE_SIZE + x/TILE_SIZE],
               chopped_image, hl_chopped_image);

          i += 1;
        }
    }
}

void
TileFactory::pack(int id, int colmap, CL_PixelBuffer color, CL_PixelBuffer highlight)
{
  if (id >= int(tiles.size()))
    tiles.resize(id + 1);

  tiles[id] = new Tile(color, highlight, colmap);
          
  tiles[id]->id = id;

  tiles[id]->color_rect     = packers[color_packer]->pack(color);
  tiles[id]->color_packer   = color_packer;

  if (highlight)
    {
      tiles[id]->highlight_rect   = packers[highlight_packer]->pack(highlight);
      tiles[id]->highlight_packer = highlight_packer;
    }

  if (packers[color_packer]->is_full())
    {
      packers.push_back(new TilePacker(1024, 1024));
      color_packer = packers.size() - 1;
    }

  if (packers[highlight_packer]->is_full())
    {
      packers.push_back(new TilePacker(1024, 1024));
      highlight_packer = packers.size() - 1;
    }
}

Tile* 
TileFactory::create (int id)
{
  // id 0 is always the empty tile
  if (id == 0)
    { 
      return 0;
    }
  else
    {
      if (id > 0 && id < int(tiles.size()))
        return tiles[id];
      else
        return 0;
    }
}

CL_OpenGLSurface
TileFactory::get_texture(int id)
{
  return packers[id]->get_texture();
}

void
TileFactory::init()
{
  assert(current_ == 0);
  current_ = new TileFactory(tile_def_file);
}

/** Destroy the default TileFactor*/
void
TileFactory::deinit()
{
  delete current_;
}

/* EOF */
