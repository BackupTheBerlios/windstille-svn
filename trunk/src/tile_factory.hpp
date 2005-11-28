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

#ifndef TILEFACTORY_HXX
#define TILEFACTORY_HXX

#include <map>
#include <string>
#include "lisp/lisp.hpp"

class Tile;
class TilePacker;

/** */
class TileFactory
{
private:
  typedef std::vector<Tile*> Tiles;
  Tiles tiles;
  typedef std::vector<TilePacker*> TilePackers;
  TilePackers packers;
  int color_packer;

  static TileFactory* current_;

  friend class TileDescription;
public:
  static std::string tile_def_file;

  typedef Tiles::iterator iterator;
  
  iterator begin() { return tiles.begin(); }
  iterator end()   { return tiles.end(); }

  /** Create a TileFactory from a given tile definition file */
  TileFactory(const std::string& filename);
  ~TileFactory();

  /** Create a new tile, or loads&creates it if it is not already available */
  Tile* create(int tile_id);

  Texture get_texture(int tile_id);

  void add(int id, Tile* tile);

  /** Create the default TileFactor*/
  static void init();

  /** Destroy the default TileFactor*/
  static void deinit();

  /** Access the default TileFactor*/
  static TileFactory* current() { return current_; }

private:
  void parse_tiles(const lisp::Lisp* data);
};

#endif

/* EOF */
