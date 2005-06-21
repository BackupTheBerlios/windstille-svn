//  $Id$
// 
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_SECTOR_HXX
#define HEADER_SECTOR_HXX

#include <string>
#include <vector>
#include <ClanLib/Display/color.h>

class GameObj;
class TileMap;
class Player;
class SceneContext;

/** */
class Sector
{
private:
  std::string name;

  typedef std::vector<GameObj*> Objects;
  Objects objects;

  CL_Color ambient_light;

  /** The TileMap with which the player interacts */
  TileMap* interactive_tilemap;

  Player* player;

  void parse_file(const std::string& filename);

  static Sector* current_;
public:
  Sector(const std::string& filename);
  ~Sector();

  static Sector* current() { return current_; }

  void draw(SceneContext& gc);
  void update(float delta);

  int get_width () const;
  int get_height () const;

  void add(GameObj*);
  void remove(GameObj*);

  std::vector<GameObj*>* get_objects() { return &objects; }
  TileMap* get_tilemap() { return interactive_tilemap; }
private:
  Sector (const Sector&);
  Sector& operator= (const Sector&);
};

#endif

/* EOF */
