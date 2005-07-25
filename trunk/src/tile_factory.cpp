//  $Id$
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
#include <config.h>

#include <string>
#include <sstream>
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
#include "lisp/properties.hpp"

extern CL_ResourceManager* resources;

TileFactory* TileFactory::current_ = 0;

std::string TileFactory::tile_def_file = "tiles.scm";

TileFactory::TileFactory (const std::string& filename)
{
  using namespace lisp;

  packers.push_back(new TilePacker(1024, 1024));
  packers.push_back(new TilePacker(1024, 1024));
  color_packer     = 0;

  std::auto_ptr<Lisp> root (Parser::parse(filename));
  Properties rootp(root.get());
  
  const lisp::Lisp* tiles_lisp = 0;
  if(rootp.get("windstille-tiles", tiles_lisp) == false) {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille tiles file";
    throw std::runtime_error(msg.str());
  }
  
  Properties props(tiles_lisp);
  PropertyIterator<const lisp::Lisp*> iter;
  props.get_iter("tiles", iter);
  while(iter.next()) {
    parse_tiles(*iter);
  }
  props.print_unused_warnings("windstille-tiles");
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
  using namespace lisp;
  assert(data);

  std::string filename;
  std::string highlight_filename;
  std::vector<int> colmap;
  std::vector<int> ids;

  Properties props(data);
  props.get("ids", ids);
  props.get("image", filename);
  props.get("colmap", colmap);
  props.print_unused_warnings("tiles");

  if(filename == "")
    throw std::runtime_error("Missing color-image");
  
  CL_PixelBuffer image = CL_ProviderFactory::load(datadir + filename);
  CL_PixelBuffer hl_image;
  
  int num_tiles = (image.get_width()/TILE_RESOLUTION) * (image.get_height()/TILE_RESOLUTION);
  if (int(colmap.size()) != num_tiles)
    {
      std::ostringstream str;
      str << "'colmap' information and num_tiles mismatch (" 
          << colmap.size() << " != " << num_tiles << ") for image '" << filename << "'";
      throw std::runtime_error(str.str());
    }

  if (int(ids.size()) != num_tiles)
    {
      std::ostringstream str;
      str << "'ids' information and num_tiles mismatch (" 
          << ids.size() << " != " << num_tiles << ") for image '" << filename << "'";
      throw std::runtime_error(str.str());
    }
  
  int i = 0;
  for (int y = 0; y < image.get_height(); y += TILE_RESOLUTION)
    {
      for (int x = 0; x < image.get_width(); x += TILE_RESOLUTION)
        {
          if (ids[i] == -1)
            {
              // ignore the given section of the image 
            }
          else if (ids[i] < (int) tiles.size() && tiles[ids[i]] != NULL)
            {
              std::ostringstream os;
              os << ids[i];
              throw std::runtime_error("Duplicate tile id: " + os.str());
            }
          else
            {
              CL_PixelBuffer chopped_image(TILE_RESOLUTION, TILE_RESOLUTION,
                                           image.get_format().get_depth()*TILE_RESOLUTION,
                                           image.get_format(), NULL);
              chopped_image.lock();
              image.convert(chopped_image.get_data(), 
                            chopped_image.get_format(), 
                            image.get_format().get_depth()*TILE_RESOLUTION, 
                            CL_Rect(CL_Point(0, 0), CL_Size(TILE_RESOLUTION, TILE_RESOLUTION)),
                            CL_Rect(CL_Point(x, y), CL_Size(TILE_RESOLUTION, TILE_RESOLUTION)));
              chopped_image.unlock();

              pack(ids[i], colmap[y/TILE_RESOLUTION * image.get_width()/TILE_RESOLUTION + x/TILE_RESOLUTION],
                   chopped_image);
            }
          i += 1;
        }
    }
}

static bool buffer_empty(CL_PixelBuffer buffer)
{
  buffer.lock();
  unsigned char* data = static_cast<unsigned char*>(buffer.get_data());
  int width  = buffer.get_width();
  int height = buffer.get_height();
  int pitch  = buffer.get_pitch();

  for(int y = 0; y < height; ++y)
    for(int x = 0; x < width; ++x)
      {
        if (data[y * pitch + 4*x])
          {
            buffer.unlock();
            return false;
          }
      }

  return true;
}

void
TileFactory::pack(int id, int colmap, CL_PixelBuffer color)
{
  if (id >= int(tiles.size()))
    tiles.resize(id + 1);

  tiles[id] = new Tile(colmap);
          
  tiles[id]->id = id;

  if (!buffer_empty(color))
    {
      tiles[id]->color_rect     = packers[color_packer]->pack(color);
      tiles[id]->color_packer   = color_packer;
    }

  if (packers[color_packer]->is_full())
    {
      packers.push_back(new TilePacker(1024, 1024));
      color_packer = packers.size() - 1;
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
