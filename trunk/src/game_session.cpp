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

#include "font/fonts.hpp"
#include "sector.hpp"
#include "console.hpp"
#include "game_object.hpp"
#include "player.hpp"
#include "tile_map.hpp"
#include "screen_manager.hpp"
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
#include "inventory.hpp"
#include "glutil/surface_manager.hpp"
#include "glutil/surface.hpp"
#include "display/display.hpp"
#include "pda.hpp"

#include "game_session.hpp"

GameSession* GameSession::current_ = 0; 

class GameSessionImpl
{
public:
  SceneContext sc;

  float fadeout_value;

  Sector* sector;
  View    view;

  std::string filename;

  bool pause;
  
  /** Amount of how far the cutscene bars are visible, 0.0 means not
      visible, 1.0 fully visible. */
  float cutscene_value;

  /** True if cutscene is active, false otherwhise */
  bool  cutscene_mode;

  enum { NO_ACTION, QUIT_ACTION, CHANGE_SECTOR_ACTION } next_action;
  
  enum { FADEIN, RUNNING, FADEOUT } fade_state;

  Color fade_color;

  // GUI Elements
  ControllerHelpWindow controller_help_window;
  EnergyBar     energy_bar;

  // Active GUI Elements
  DialogManager dialog_manager;
  Conversation  conversation;
  Inventory     inventory;
  PDA           pda;

  Screen* current_gui;

  GameSessionImpl() {
    sector = 0;
    current_gui = 0;
    cutscene_mode  = false;
    cutscene_value = 0.0f;
    fade_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
    fade_state = FADEOUT;
    fadeout_value = 1.0f;
  }
  ~GameSessionImpl() {
    delete sector;
  }

  void draw();
  void update(float delta, const Controller& controller);
  void handle_event(const SDL_Event& event);
};

GameSession::GameSession(const std::string& arg_filename)
  : impl(new GameSessionImpl())
{
  impl->filename = arg_filename;

  if (debug) std::cout << "Creating new GameSession" << std::endl;
  current_ = this;

  impl->pause = false;
  
  if (1)
    script_manager->run_script_file("scripts/init_script_vars.nut");
    
  set_sector(impl->filename);
}

GameSession::~GameSession()
{
}

void
GameSessionImpl::draw()
{
  view.draw(sc);

  // Render the scene to the screen
  sc.render();

  if (!cutscene_mode)
    {
      // Draw HUD
      energy_bar.draw();
      controller_help_window.draw();
    }

  if (cutscene_mode || cutscene_value > 0.0f)
    {
      int border_size = static_cast<int>(75 * cutscene_value);
      Display::fill_rect(Rect(Point(0, 0), Size(Display::get_width(), border_size)), 
                         Color(0.0f, 0.0f, 0.0f, cutscene_value));
      Display::fill_rect(Rect(Point(0, Display::get_height() - border_size), Size(Display::get_width(), border_size)), 
                         Color(0.0f, 0.0f, 0.0f, cutscene_value));
    }

  if (current_gui)
    current_gui->draw();

  if (fade_state == FADEOUT || fade_state == FADEIN)
    {
      Display::fill_rect(Rect(0, 0, 
                               Display::get_width(), Display::get_height()),
                         Color(fade_color.r, fade_color.g, fade_color.b, fadeout_value));
    }

  if (pause)
    {
      if ((SDL_GetTicks() / 1000) % 2)
        Fonts::ttfdialog->draw(Display::get_width()/2, Display::get_height()/2, "Pause");
    }
}

void
GameSessionImpl::update(float delta, const Controller& controller)
{  
  if (cutscene_mode)
    cutscene_value += delta * 0.75f;
  else
    cutscene_value -= delta * 0.75f;

  if (cutscene_value > 1.0f)
    cutscene_value = 1.0f;
  else if (cutscene_value < 0.0f)
    cutscene_value = 0.0f;

  if (controller.button_was_pressed(PAUSE_BUTTON))
    pause = !pause;

  Uint8 *keystate = SDL_GetKeyState(NULL);

  if(keystate[SDLK_KP1])
    game_speed *= 1.0 - delta;
  if(keystate[SDLK_KP3])
    game_speed *= 1.0 + delta;
  if(keystate[SDLK_KP5])
    game_speed = 1.0;
 
  delta *= game_speed;

  if (!pause)
    {
      game_time += delta;
      script_manager->update();
      
      view.update(delta);
  
      switch (fade_state)
        {
        case FADEIN:
          if (fadeout_value < 0.0f)
            {
              fade_state = RUNNING;
              fadeout_value = 0.0f;
            }
          else
            {
              fadeout_value -= delta;
            }
          break;

        case FADEOUT:
          if (fadeout_value > 1.0f)
            { 
              fadeout_value = 1.0f;

              switch(next_action)
                {
                case CHANGE_SECTOR_ACTION:
                  GameSession::current()->set_sector(filename);
                  break;

                case QUIT_ACTION:
                  screen_manager.quit();
                  break;

                default:
                  break;
                }
            }
          else
            {
              fadeout_value += delta;
            }
          break;

        case RUNNING:
          sector->update(delta);
          energy_bar.update(delta, controller);
          if (current_gui)
            current_gui->update(delta, controller);
          break;
        }
      
      controller_help_window.update(delta, controller);
    }

  if (controller.button_was_pressed(PDA_BUTTON))
    {
      if (current_gui == &pda)
        current_gui = 0;
      else
        current_gui = &pda;
    }
  else if (controller.button_was_pressed(INVENTORY_BUTTON))
    {
      if (current_gui == &inventory)
        current_gui = 0;
      else
        current_gui = &inventory;
    }

  if(keystate[SDLK_ESCAPE])
    GameSession::current()->quit();
}

void
GameSession::change_sector(const std::string& arg_filename)
{
  impl->filename = arg_filename;
 
  sound_manager->stop_music();

  impl->fade_state    = GameSessionImpl::FADEOUT;
  impl->next_action   = GameSessionImpl::CHANGE_SECTOR_ACTION;
}

void
GameSession::set_sector(const std::string& )
{
  delete impl->sector;
  impl->sector = new Sector(impl->filename);
 
  GameObject::set_world(impl->sector);

  //FIXME: does the TestObject class still need to exist?
  //sector->add(new TestObject());
  
  impl->sector->spawn_player("default");
  impl->sector->activate();
  
  impl->fade_state    = GameSessionImpl::FADEIN;
  impl->next_action   = GameSessionImpl::NO_ACTION;

  if (debug) std::cout << "Finished changing sector" << std::endl;
}

void
GameSessionImpl::handle_event(const SDL_Event& event)
{
  switch(event.type)
  {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      if (event.key.state)
        {    
          switch (event.key.keysym.sym)
            {
            case SDLK_1:
              sc.set_render_mask(sc.get_render_mask() ^ SceneContext::COLORMAP);
              console << "Toggled COLORMAP: " << ((sc.get_render_mask() & SceneContext::COLORMAP) > 0) << std::endl;
              break;

            case SDLK_2:
              sc.set_render_mask(sc.get_render_mask() ^ SceneContext::LIGHTMAP);
              console << "Toggled LIGHTMAP: " << ((sc.get_render_mask() & SceneContext::LIGHTMAP) > 0) << std::endl;
              break;
      
            case SDLK_3:
              sc.set_render_mask(sc.get_render_mask() ^ SceneContext::HIGHLIGHTMAP);
              console << "Toggled HIGHLIGHTMAP: " << ((sc.get_render_mask() & SceneContext::HIGHLIGHTMAP) > 0) << std::endl;
              break;      
  
            case SDLK_4:
              sc.set_render_mask(sc.get_render_mask() ^ SceneContext::LIGHTMAPSCREEN);
              console << "Toggled LIGHTMAP: " << ((sc.get_render_mask() & SceneContext::LIGHTMAPSCREEN) > 0) << std::endl;
              break;

            case SDLK_c:
              if (debug) {
                collision_debug = !collision_debug;
                console << "Collision Debugging " << (collision_debug ? "enabled" : "disabled") << std::endl;
              }
              break;
        
            default:
              break;
            }
        }
      break;
        
    case SDL_MOUSEBUTTONDOWN:
      {
        Vector real_pos = GameSession::current()->get_view()->screen_to_world(Vector(event.button.x,
                                                                                  event.button.y));      
        console << "Click at: " << int(real_pos.x) << ", " << int(real_pos.y) << std::endl;
      }
      break;
  }
}

void
GameSession::quit()
{
  if (impl->fade_state != GameSessionImpl::FADEOUT)
    {
      sound_manager->stop_music();
      impl->fade_state  = GameSessionImpl::FADEOUT;
      impl->next_action = GameSessionImpl::QUIT_ACTION;
    }
}

PDA&
GameSession::get_pda()
{
  return impl->pda;
}

void
GameSession::draw()
{
  impl->draw();
}

void
GameSession::update(float delta, const Controller& controller)
{
  impl->update(delta, controller);
}

void
GameSession::handle_event(const SDL_Event& event)
{
  impl->handle_event(event);
}

View*
GameSession::get_view() 
{
  return &impl->view; 
}

void
GameSession::set_control_state(ControlState state) 
{
  switch(state)
    {
    case DIALOG:
      impl->current_gui = &impl->dialog_manager;
      break;

    case CONVERSATION:
      impl->current_gui = &impl->conversation;
      break;

    case GAME:
      impl->current_gui = 0;
      break;
    }
}

bool
GameSession::is_active() const
{
  return (impl->current_gui == 0);
}

void
GameSession::set_cutscene_mode(bool t)
{
  if (t != impl->cutscene_mode)
    {
      impl->cutscene_mode  = t;
    }
}

void
GameSession::fadeout(const Color& color)
{
  impl->fade_color  = color;
  impl->fade_state  = GameSessionImpl::FADEOUT;
  impl->next_action = GameSessionImpl::NO_ACTION;
}

void
GameSession::fadein()
{
  impl->fade_state  = GameSessionImpl::FADEIN;
  impl->next_action = GameSessionImpl::NO_ACTION;
}

/* EOF */
