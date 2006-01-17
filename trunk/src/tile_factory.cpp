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
#include "tile_description.hpp"
#include "lisp/lisp.hpp"
#include "lisp/parser.hpp"
#include "lisp/properties.hpp"
#include "display/surface_manager.hpp"
#include "display/texture.hpp"
#include "physfs/physfs_sdl.hpp"

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

  lisp::Properties props(data);
  TileDescription desc(props);
  desc.load(this);
}

Tile*
TileFactory::create(int id)
{
  if(id < 0 || id >= (int) tiles.size())
    {
      return 0;
    }
  else
    {
      return tiles[id];
    }
}

void
TileFactory::init()
{
  assert(current_ == 0);
  current_ = new TileFactory(tile_def_file);
}

/** Destroy the default TileFactor */
void
TileFactory::deinit()
{
  delete current_;
}

/* EOF */
