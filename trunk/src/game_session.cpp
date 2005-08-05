/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include <math.h>
#include <sstream>
#include <stdarg.h>

#include <squirrel.h> 
#include <sqstdio.h> 
#include <sqstdaux.h> 

#include "fonts.hpp"
#include "sector.hpp"
#include "console.hpp"
#include "game_object.hpp"
#include "player.hpp"
#include "tile_map.hpp"
#include "view.hpp"
#include "timer.hpp"
#include "energy_bar.hpp"
#include "dialog_manager.hpp"
#include "windstille_main.hpp"
#include "display/scene_context.hpp"
#include "scripting/wrapper_util.hpp"
#include "scripting/wrapper.hpp"
#include "input/input_manager.hpp"
#include "particles/particle_system.hpp"
#include "particles/spark_drawer.hpp"
#include "particles/surface_drawer.hpp"
#include "script_manager.hpp"
#include "sound/sound_manager.hpp"
#include "conversation.hpp"
#include "collision/collision_engine.hpp"
#include "test_object.hpp"
#include "glutil/surface_manager.hpp"
#include "glutil/surface.hpp"
#include "display/display.hpp"

#include "game_session.hpp"

using namespace Windstille;

GameSession* GameSession::current_ = 0; 

GameSession::GameSession(const std::string& arg_filename)
  : control_dialog("images/controldialog.sprite"),
    sector (0)
{
  if (debug) std::cout << "Creating new GameSession" << std::endl;
  current_ = this;
  
  view           = new View();  
  energy_bar     = new EnergyBar();
  dialog_manager = new DialogManager();
  conversation   = new Conversation();

  filename = arg_filename;
  change_sector();
}

GameSession::~GameSession()
{
  delete energy_bar;
  delete view;
  delete dialog_manager;
  delete conversation;
  delete sector;
}

void
GameSession::draw_game()
{
  view->draw(sc);

  // Render the scene to the screen
  sc.render();

  // Draw HUD
  energy_bar->draw();

  if (control_state == DIALOG)
    dialog_manager->draw(); 
  
  conversation->draw();

  //control_dialog.set_alignment(origin_bottom_right);
  control_dialog.draw(Vector(800-16 - control_dialog.get_width(), 
                             600-16 - control_dialog.get_height()));
}

void
GameSession::draw()
{
  //std::cout << gluErrorString(glGetError()) << std::endl;
  draw_game();

  switch (fade_state)
    {
    case FADEOUT:
      Display::fill_rect(Rect(0, 0, 
                               Display::get_width(), Display::get_height()),
                          Color(0,0,0, fadeout_value));
      break;
    case FADEIN:
      Display::fill_rect(Rect(0, 0, 
                               Display::get_width(), Display::get_height()),
                         Color(0,0,0, 1.0f - fadeout_value));
      break;

    default:
      break;
    }
}

void
GameSession::update(float delta)
{  
  Uint8 *keystate = SDL_GetKeyState(NULL);

  if(keystate[SDLK_KP1])
    game_speed *= 1.0 - delta;
  if(keystate[SDLK_KP3])
    game_speed *= 1.0 + delta;
  if(keystate[SDLK_KP5])
    game_speed = 1.0;

  InputManager::update(delta);
  delta *= game_speed;

  game_time += delta;
  script_manager->update();

  view->update(delta);

  switch (fade_state)
    {
    case FADEIN:
      if (fadeout_value > 1.0f)
        fade_state = RUNNING;
      fadeout_value += delta;
      break;
    case FADEOUT:
      if (fadeout_value > 1.0f)
        {
          if (target_state == LOAD_GAME_SESSION)
            change_sector();
          else
            game_main_state = target_state;
        }
      fadeout_value += delta;
      break;

    case RUNNING:
      sector->update (delta);
      energy_bar->update(delta);
      switch (control_state) 
        {
        case DIALOG:
          dialog_manager->update(delta);
          break;
        case GAME:
          break;
        }
      break;
    }
  conversation->update(delta);
  
  if(keystate[SDLK_ESCAPE])
    quit();
}

void
GameSession::change_sector()
{
  if (sector)
    delete sector;

  sector = new Sector(filename);
  
  if (1)
    {
      script_manager->run_script_file("scripts/init_script_vars.nut");
    }

  //sector->add(new TestObject());
  
  sector->activate();
  sector->spawn_player("default");
  
  GameObject::set_world(sector);
  
  fade_state = FADEIN;
  fadeout_value = 0;
  control_state = GAME;
  target_state = RUN_GAME;
  if (debug) std::cout << "Finished changing sector" << std::endl;
}

void
GameSession::set_sector(const std::string& arg_filename)
{
  target_state = LOAD_GAME_SESSION;
  filename = arg_filename;
  
  fadeout_value = 0;
  sound_manager->stop_music();
  fade_state = FADEOUT;
}

void
GameSession::quit()
{
  if (fade_state != FADEOUT)
    {
      target_state = LOAD_MENU;
      
      fadeout_value = 0;
      sound_manager->stop_music();
      fade_state = FADEOUT;
    }
}

/* EOF */
