//  $Id: tile_map.hxx,v 1.7 2003/09/13 18:01:17 grumbel Exp $
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

#ifndef TILEMAP_HXX
#define TILEMAP_HXX

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <string>

#include "globals.hxx"
#include "field.hxx"
#include "lisp/lisp.hpp"
#include "game_object.hxx"
#include "display/scene_context.hxx"

class Tile;

class TileMap : public GameObject
{
private:
  friend class TileMapDrawingRequest;
  Field<Tile*> field;
  typedef Field<Tile*>::iterator FieldIter;
  std::string name;
  float z_pos; 
  float total_time;
public:
  TileMap(const lisp::Lisp* lisp);
  TileMap(const std::string& name, int w, int h);
  TileMap(Field<int>* data);
  virtual ~TileMap();

  void update (float delta);
  void draw (SceneContext& gc);
  
  /** @return the type of ground at the given world coordinates */
  bool is_ground(float x, float y);

  /** @return the type of ground at the given subtile coordinates */
  unsigned int get_pixel(int x, int y);
  
  int get_width () const { return field.get_width(); }
  int get_height () const { return field.get_height (); }

  int get_tile_size () const { return TILE_SIZE; }

  std::string get_name() const { return name; }
};

#endif

/* EOF */
