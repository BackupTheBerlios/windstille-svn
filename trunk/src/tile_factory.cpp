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
#include <SDL.h>
#include <SDL_image.h>
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
#include "glutil/surface_manager.hpp"
#include "glutil/texture.hpp"
#include "physfs/physfs_sdl.hpp"

TileFactory* TileFactory::current_ = 0;

std::string TileFactory::tile_def_file = "tiles.scm";

/** Check if the given region of the given image is fully transparent */
static bool surface_empty(SDL_Surface* image, int sx, int sy, int w, int h)
{
  SDL_LockSurface(image);
  
  unsigned char* data = static_cast<unsigned char*>(image->pixels);
  
  for(int y = sy; y < sy + h; ++y)
    for(int x = sx; x < sx + w; ++x)
      {
        if (data[y * image->pitch + 4*x + 3] != 0)
          { 
            SDL_UnlockSurface(image);
            return false;
          }
      }

  SDL_UnlockSurface(image);
  return true;
}

class TileDescription
{
public:
  TileFactory* factory;

  std::vector<int> ids;
  std::vector<int> colmap;
  std::string filename;
  int width;
  int height;


  TileDescription(TileFactory* factory_, const lisp::Lisp* data)
    : factory(factory_), width(0), height(0)
  {
    assert(data);
    lisp::Properties props(data);

    props.get("ids",    ids);
    props.get("image",  filename);
    props.get("colmap", colmap);
    props.get("width",  width);
    props.get("height", height);
    
    props.print_unused_warnings("tiles");

    if(filename == "")
      throw std::runtime_error("Missing color-image");
  }

  void load()
  {
    if (debug)
      std::cout << "Loading tiles: " << filename << std::endl;

    SDL_Surface* image = IMG_Load_RW(get_physfs_SDLRWops(filename), 1);
    if(!image) 
      {
        std::ostringstream msg;
        msg << "Couldn't load image '" << filename << "': " << SDL_GetError();
        throw std::runtime_error(msg.str());
      }
 
    try {
      int num_tiles = width * height; //(image->w/TILE_RESOLUTION) * (image->h/TILE_RESOLUTION);
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
      for (int y = 0; y < height*TILE_RESOLUTION; y += TILE_RESOLUTION)
        {
          for (int x = 0; x < width*TILE_RESOLUTION; x += TILE_RESOLUTION)
            {
              int id = ids[i];
              int collider = colmap[i];
              i++;

              if(id == -1)
                continue;

              if(id < (int) factory->tiles.size() && factory->tiles[id] != 0)
                {
                  std::ostringstream msg;
                  msg << "Duplicate tile id: " << filename << " " << id;
                  throw std::runtime_error(msg.str());
                } 

              if (id >= int(factory->tiles.size()))
                factory->tiles.resize(id + 1, 0);

              factory->tiles[id] = new Tile(collider); 
              Tile& tile = *(factory->tiles[id]);
              tile.id = id;

              if (surface_empty(image, x, y, TILE_RESOLUTION, TILE_RESOLUTION))
                continue;
            
              if(factory->packers[factory->color_packer]->is_full())
                {
                  factory->packers.push_back(new TilePacker(1024, 1024));
                  factory->color_packer = factory->packers.size() - 1;
                }

              Rectf rect = factory->packers[factory->color_packer]->pack(image, x, y,
                                                                         TILE_RESOLUTION, TILE_RESOLUTION);
              tile.color_rect   = rect;
              tile.color_packer = factory->color_packer;
              tile.texture      = factory->packers[factory->color_packer]->get_texture();
            }
        }
    } catch(...) {
      SDL_FreeSurface(image);
      throw;
    }
    SDL_FreeSurface(image);
  }
};

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

  TileDescription desc(this, data);

  desc.load();
}

Tile*
TileFactory::create(int id)
{
  if(id < 0 || id >= (int) tiles.size())
    return 0;
  return tiles[id];
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
