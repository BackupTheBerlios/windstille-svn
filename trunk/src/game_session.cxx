//  $Id: windstille_game.cxx,v 1.33 2003/11/13 12:59:42 grumbel Exp $
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
#include <math.h>
#include <ClanLib/gl.h>
#include <sstream>

#include "fonts.hxx"
#include "sector.hxx"
#include "game_object.hxx"
#include "player.hxx"
#include "animation_obj.hxx"
#include "tile_map.hxx"
#include "music_manager.hxx"
#include "display.hxx"
#include "view.hxx"
#include "energiebar.hxx"
#include "dialog_manager.hxx"
#include "windstille_main.hxx"
#include "display/scene_context.hxx"
#include "input/input_manager.hxx"

#include "game_session.hxx"

using namespace Windstille;

GameSession* GameSession::current_ = 0; 

GameSession::GameSession(const std::string& arg_filename)
  : frames(0), filename (arg_filename)
{
  current_ = this;
  world = new Sector(filename);

  state = FADEIN;
  fadeout_value = 0;

  control_state = GAME;
}

GameSession::GameSession(Sector* w)
{
  current_ = this;
  world = w;
  control_state = GAME;
}

GameSession::~GameSession()
{
  delete world;
}

void
GameSession::draw_game()
{
  // Generic blue background
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

  view->draw(sc);

  // Render the scene to the screen
  sc.render();

  // Draw HUD
  energiebar->draw();

  switch (control_state)
    {
    case DIALOG:
      dialog_manager->draw();      
      break;
    default:
      break;
    }
  
  // Draw Logo
  if (0)
    {     
      //logo.set_blend_func(blend_src_alpha, blend_one);
      logo.set_alpha(cos(blink)*0.5f + 0.5f);
      logo.draw(800 - 302, 600 - 95);
      logo_black.draw(800 - 302, 600 - 95);
    }
}

void
GameSession::draw()
{
  draw_game();

  switch (state)
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

  if (player->get_movement_state() == Player::DEAD)
    {
      CL_Font font = Fonts::dialog;
      font.set_alignment(origin_bottom_center);
      font.draw(CL_Display::get_width()/2, 200,
                "..:: Press Fire to restart ::..");
    }

  CL_Display::flip();

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
  ++frames;
}

void
GameSession::update(float delta)
{
  InputManager::update(delta);
  delta *= game_speed;

  view->update(delta);

  switch (state)
    {
    case FADEIN:
      if (fadeout_value > 1.0f)
        state = RUNNING;
      fadeout_value += delta;
      break;
    case FADEOUT:
      if (fadeout_value > 1.0f)
        Screen::quit();

      fadeout_value += delta;
      break;
    case RUNNING:
      switch (control_state) 
        {
        case DIALOG:
          dialog_manager->update(delta);
          break;
        case GAME:
          world->update (delta);
          energiebar->update(delta);
          break;
        }
      break;
    }
  
  if (CL_Keyboard::get_keycode(CL_KEY_ESCAPE))
    quit();
  
  InputManager::clear();

  blink += delta * 3.141f;
}

void
GameSession::on_startup ()
{ 
  CL_Display::get_current_window()->hide_cursor();

  MusicManager::current()->play(datadir + "music/techdemo.ogg", true);
  blink = 0.0f;

  GameObject::set_world (world);
  
  player = new Player();
  view   = new View();
  
  energiebar = new Energiebar();
  dialog_manager = new DialogManager();

  world->add(player);

  logo       = CL_Sprite("logo", resources);
  logo_black = CL_Sprite("logo_black", resources);

  if (1)
    {
      // FIXME: Move this thing into the scripting part
      DialogManager::current()->add_dialog("human/portrait", 
                                           "Welcome to the VR training programm. Here you"
                                           "will learn the basic manovering abilities of your "
                                           "powersuit, jumping, running, climbing and shooting."
                                           "We will start with climbing, see the block infront of"
                                           "you? Press [Right] and [Jump] to hang on the ledge.");
      set_dialog_state();
    }
}

void
GameSession::on_shutdown ()
{
  MusicManager::current()->stop();

  delete energiebar;
  delete view;
  delete dialog_manager;
}

void
GameSession::quit()
{
  fadeout_value = 0;
  state = FADEOUT;
  MusicManager::current()->stop();
}

/* EOF */
