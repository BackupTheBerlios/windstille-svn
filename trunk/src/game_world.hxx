//  $Id: game_world.hxx,v 1.10 2003/09/28 10:55:34 grumbel Exp $
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

#ifndef GAMEWORLD_HXX
#define GAMEWORLD_HXX

#include <list>
#include <vector>
#include <string>
//#include "coroutine_manager.hxx"
#include "display/scene_context.hxx"

class TileMap;
class DiamondMap;
class GameObj;
class Player;

class GameWorld
{
private:
  std::list<GameObj*> objects;
  TileMap* tilemap;
  TileMap* background_tilemap;

  std::vector<std::string> scripts;

  //  CoroutineManager coroutines;

  static GameWorld* current_;
public:
  static GameWorld* current() { return current_; }

  GameWorld (const std::string& filename);
  ~GameWorld();
  
  void add (GameObj* obj) { objects.push_back (obj); }
  void remove (GameObj* obj) { objects.remove (obj); }
  
  void draw (SceneContext& gc);
  void update (float delta);

  /** return width in pixels */
  int get_width () const;

  /** return height in pixels */
  int get_height () const;

  std::list<GameObj*>* get_objects() { return &objects; }

  TileMap* get_tilemap () const { return tilemap; }
};

#endif

/* EOF */
