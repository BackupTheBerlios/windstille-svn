//  $Id: game.cxx,v 1.16 2003/11/05 11:08:31 grumbel Exp $
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
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

#include "game.hxx"
#include "../water_map.hxx"
#include "../game_world.hxx"
#include "../igel.hxx"
#include "../brush.hxx"
#include "../bomb.hxx"
#include "../windstille_game.hxx"
#include "../trigger.hxx"
#include "../diamond_map.hxx"
#include "../dialog_manager.hxx"
#include "../player.hxx"
#include "../tile_factory.hxx"
#include "../coroutine_manager.hxx"

void
coroutine_add(VALUE val)
{
  CoroutineManager::current()->add(new RubyCoroutine(val));
}

void
game_set_tilesize(int size, int subsize)
{
  TILE_SIZE = size;
  SUBTILE_SIZE = subsize;
  SUBTILE_NUM = (TILE_SIZE/SUBTILE_SIZE);
}

void
game_load_tiles(const char* resourcefile)
{
  std::cout << "game_load_tiles: " << resourcefile << std::endl;
  TileFactory::tile_def_file = resourcefile;
}

void
game_load_resources(const char* resourcefile)
{
  try {
    resources->add_resources(CL_ResourceManager(datadir + std::string(resourcefile), false));
  } catch (CL_Error& err) {
    std::cout << "CL_Error: " << err.message << std::endl;
  }
}

void
game_add_water(int x1, int y1, int x2, int y2)
{
  GameWorld::current()->get_watermap()->add_water(x1, y1,
                                                  x2 - x1, y2 - y1);
}

void
game_add_bomb(int x, int y)
{
  GameWorld::current()->add(new Bomb(x, y));
}

void
game_add_brush_light(int x, int y)
{
  GameWorld::current()->add(new Brush("brush/light", CL_Point(x, y), true));
}

void
game_add_brush_exit(int x, int y)
{
  GameWorld::current()->add(new Brush("brush/exit", CL_Point(x, y), false));
}

void
game_add_igel(int x, int y)
{
  GameWorld::current()->add(new Igel(x, y));
}

GameWorld* make_game_world(int w, int h)
{
  return new GameWorld(w, h);
}

void start_game(GameWorld* world)
{
  WindstilleGame game (world);
  game.display ();
}

void add_region_trigger(int x, int y, int w, int h, VALUE func)
{
  GameWorld::current()->add(new Trigger(new RegionTriggerCondition(CL_Rectf(x, y, w, h)), 
                                        RubyFunctor(func)));
}

void player_set_pos(float x, float y)
{
  Player::current()->set_position(CL_Vector(x, y));
}

float player_get_pos_x()
{
  return Player::current()->get_pos().x;
}

float player_get_pos_y()
{
  return Player::current()->get_pos().y;
}

void player_set_direction(const char* direction)
{
  if (strcmp(direction, "east") == 0)
    {
      Player::current()->set_direction(EAST);
    }
  else if (strcmp(direction, "west") == 0)
    {
      Player::current()->set_direction(WEST);
    }
  else
    {
      std::cout << __FUNCTION__ << ": Unknown direction:" << direction << std::endl;
    }
}

int mouse_get_x() 
{
  return CL_Mouse::get_x();
}

int mouse_get_y() 
{
  return CL_Mouse::get_y();
}


int screen_get_x() 
{
  return CL_Mouse::get_x();
}

int screen_get_y() 
{
  return CL_Mouse::get_y();
}

int player_get_x()
{
  return int(Player::current()->get_pos().x);
}

int player_get_y()
{
  return int(Player::current()->get_pos().y);
}

void game_set_pause(bool p)
{
  WindstilleGame::current()->set_pause(p);
}

bool game_get_pause()
{
  return WindstilleGame::current()->get_pause();
}

void game_quit()
{
  WindstilleGame::current()->quit();
}

void dialog_add(const char* portrait, const char* text)
{
  DialogManager::current()->add_dialog(portrait, text);
}

void dialog_show()
{
  WindstilleGame::current()->set_dialog_state();
}

void dialog_hide()
{
  WindstilleGame::current()->set_game_state();
}

void dialog_clear()
{
  DialogManager::current()->clear();
}

void dialog_add_answer(const char* text, VALUE func)
{
  DialogManager::current()->add_answer(text, RubyFunctor(func));
}

void remove_trigger()
{
  GameWorld::current()->remove(Trigger::get_current());
}

float game_get_time()
{
  return GameWorld::current()->get_time();
}

int game_get_diamonds()
{
  return GameWorld::current()->get_diamond_map()->get_num_diamonds();
}

int game_get_max_diamonds()
{
  return GameWorld::current()->get_diamond_map()->get_num_max_diamonds();
}

float get_game_speed()
{
  return game_speed;
}

void  set_game_speed(float s)
{
  game_speed = s;
}

void coroutine_wait(int id)
{
  CoroutineManager::current()->register_wait(id);
}

void spawn_entity(const std::string& name)
{
  std::cout << "Spawning entity: " << name << std::endl;
}

/* EOF */
