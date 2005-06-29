//  $Id: tile_factory.hxx,v 1.8 2003/09/22 18:37:05 grumbel Exp $
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
  // FIXME: Replace ths with a vector, map is potentially slow
  //typedef std::map<int, Tile*> Tiles;
  typedef std::vector<Tile*> Tiles;
  Tiles tiles;
  typedef std::vector<TilePacker*> TilePackers;
  TilePackers packers;
  int highlight_packer;
  int color_packer;

  static TileFactory* current_;
public:
  static std::string tile_def_file;

  typedef Tiles::iterator iterator;
  
  iterator begin() { return tiles.begin(); }
  iterator end()   { return tiles.end(); }

  /** Create a TileFactory from a given tile definition file */
  TileFactory(const std::string& filename);
  ~TileFactory();

  CL_OpenGLSurface get_texture(int id);

  /** Check if the tile is already loaded and return it. If it is not
   *  already loaded, load it 
   *
   *  @param id The id of the tile to create as defined in the def. file
   *
   *  @return on success the tile is returned, on failure 0 */
  Tile* create(int id);

  /** Create the default TileFactor*/
  static void init();

  /** Destroy the default TileFactor*/
  static void deinit();

  /** Access the default TileFactor*/
  static TileFactory* current() { return current_; }

private:
  void parse_tiles(const lisp::Lisp* data);
  void pack(int id, int colmap, CL_PixelBuffer color, CL_PixelBuffer highlight);
};

#endif

/* EOF */
