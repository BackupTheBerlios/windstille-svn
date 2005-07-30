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

#include <math.h>
#include <ClanLib/gl.h>
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

#include "game_session.hpp"

using namespace Windstille;

GameSession* GameSession::current_ = 0; 

GameSession::GameSession(const std::string& arg_filename)
  : control_dialog("controldialog", resources),
    sector (0)
{
  if (debug) std::cout << "Creating new GameSession" << std::endl;
  current_ = this;
  
  slots.push_back(CL_Keyboard::sig_key_down().connect(this, &GameSession::on_key_down));
  slots.push_back(CL_Mouse::sig_key_down().connect(this, &GameSession::on_mouse_down));

  view = new View();  
  energy_bar = new EnergyBar();
  dialog_manager = new DialogManager();
  conversation  = new Conversation();

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
  if (0)
    {
      // Generic blue background
      // FIXME: let the level decide which kind of background he wants 
      CL_Display::fill_rect(CL_Rect(0, 0, 800, 300),
                            CL_Gradient(CL_Color(  0,   0,  50),
                                        CL_Color(  0,   0,  50),
                                        CL_Color( 50,  50, 128),
                                        CL_Color( 50,  50, 128)));
      CL_Display::fill_rect(CL_Rect(0, 300, 800, 600),
                            CL_Gradient(CL_Color( 50,  50, 128),
                                        CL_Color( 50,  50, 128),
                                        CL_Color(  0,   0,   0),
                                        CL_Color(  0,   0,   0)));
    }

  view->draw(sc);

  // Render the scene to the screen
  sc.render();

  if (debug)
    {
      view->get_gc_state().push();
      sector->get_collision_engine()->draw();
      view->get_gc_state().pop();
    }

  // Draw HUD
  energy_bar->draw();

  if (control_state == DIALOG)
    dialog_manager->draw(); 
  
  conversation->draw();

  control_dialog.set_alignment(origin_bottom_right);
  control_dialog.draw(800-16, 600-16);
}

void
GameSession::draw()
{
  //std::cout << gluErrorString(glGetError()) << std::endl;
  draw_game();

  switch (fade_state)
    {
    case FADEOUT:
      CL_Display::fill_rect(CL_Rect(0, 0, 
                                    CL_Display::get_width(), CL_Display::get_height()),
                            CL_Color(0,0,0, std::min(int(fadeout_value*255), 255)));
      break;
    case FADEIN:
      CL_Display::fill_rect(CL_Rect(0, 0, 
                                    CL_Display::get_width(), CL_Display::get_height()),
                            CL_Color(0,0,0, 255-std::min(int(fadeout_value*255), 255)));
      break;

    default:
      break;
    }

  if (!main_app.screenshot_dir.empty())
    {
      std::stringstream filename;
      filename << main_app.screenshot_dir;
      filename.width(8);
      filename.fill('0');
      filename << frames;
      filename << ".png";
      CL_ProviderFactory::save(CL_Display::get_front_buffer(), filename.str());
    }
}

void
GameSession::update(float delta)
{  
  if(CL_Keyboard::get_keycode(CL_KEY_NUMPAD1))
    game_speed *= 1.0 - delta;
  if(CL_Keyboard::get_keycode(CL_KEY_NUMPAD3))
    game_speed *= 1.0 + delta;
  if(CL_Keyboard::get_keycode(CL_KEY_NUMPAD5))
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
  
  if (CL_Keyboard::get_keycode(CL_KEY_ESCAPE))
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

void
GameSession::on_mouse_down(const CL_InputEvent& event)
{
  switch(event.id)
    {
    case CL_MOUSE_LEFT:
      console << "Click at: " << CL_Point(view->screen2world(event.mouse_pos)) << std::endl;
      break;
    default:
      break;
    }
}

void
GameSession::on_key_down(const CL_InputEvent& event)
{
  if (!console.is_active())
    {
      switch(event.id)
        {
        case CL_KEY_1:
          sc.set_render_mask(sc.get_render_mask() ^ SceneContext::COLORMAP);
          console << "Toggled COLORMAP: " << ((sc.get_render_mask() & SceneContext::COLORMAP) > 0) << std::endl;
          break;

        case CL_KEY_2:
          sc.set_render_mask(sc.get_render_mask() ^ SceneContext::LIGHTMAP);
          console << "Toggled LIGHTMAP: " << ((sc.get_render_mask() & SceneContext::LIGHTMAP) > 0) << std::endl;
          break;
          
        case CL_KEY_3:
          sc.set_render_mask(sc.get_render_mask() ^ SceneContext::HIGHLIGHTMAP);
          console << "Toggled HIGHLIGHTMAP: " << ((sc.get_render_mask() & SceneContext::HIGHLIGHTMAP) > 0) << std::endl;
          break;      
      
        case CL_KEY_4:
          sc.set_render_mask(sc.get_render_mask() ^ SceneContext::LIGHTMAPSCREEN);
          console << "Toggled LIGHTMAP: " << ((sc.get_render_mask() & SceneContext::LIGHTMAPSCREEN) > 0) << std::endl;
          break;


        default:
          // ignore key
          //console.add("Key pressed:: ", event.id);
          break;
        }
    }
}

/* EOF */
