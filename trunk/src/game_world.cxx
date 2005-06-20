//  $Id: game_world.cxx,v 1.14 2003/09/26 14:29:35 grumbel Exp $
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

//#include <ruby.h>
//#include "ruby_functor.hxx"
#include "display.hxx"
#include "player.hxx"
#include "gameobj.hxx"
#include "game_world.hxx"
#include "tile_map.hxx"
#include "windstille_level.hxx"

GameWorld* GameWorld::current_ = 0;

bool removable (GameObj* obj) {
  return obj->is_removable ();
}

GameWorld::GameWorld (const std::string& filename)
  : passed_time (0)
{
  current_ = this;
  
  WindstilleLevel level (filename);
  
  tilemap            = new TileMap(level.get_tilemap());
  background_tilemap = new TileMap(level.get_background_tilemap());

  scripts = *level.get_scripts();
}

GameWorld::~GameWorld()
{
  delete tilemap;
  delete background_tilemap;
}

void
GameWorld::draw (SceneContext& gc)
{
  gc.light().fill_screen(CL_Color(100, 100, 200));
  
  background_tilemap->draw (gc);

  for (std::list<GameObj*>::iterator i = objects.begin ();
       i != objects.end (); ++i)
    (*i)->draw (gc);

  tilemap->draw (gc);
}

void
GameWorld::update (float delta)
{
  //  coroutines.update(delta);
  passed_time += delta;

  std::list<GameObj*> tmp_objects (objects);

  for (std::list<GameObj*>::iterator i = tmp_objects.begin ();
       i != tmp_objects.end (); ++i)
    (*i)->update (delta);
  
  objects.remove_if (removable);
}

int  
GameWorld::get_width () const
{
  return tilemap->get_width () * TILE_SIZE;
}

int 
GameWorld::get_height () const
{
  return tilemap->get_height () * TILE_SIZE;
}

void
GameWorld::add_player (Player* obj)
{ 
  objects.push_back (obj); 
  player_objects.push_back (obj); 
}

void
GameWorld::remove_player (Player* obj)
{ 
  objects.remove (obj); 
  player_objects.remove (obj); 
}

/* EOF */
