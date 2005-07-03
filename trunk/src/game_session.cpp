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
#include "animation_obj.hpp"
#include "tile_map.hpp"
#include "view.hpp"
#include "door.hpp"
#include "timer.hpp"
#include "energiebar.hpp"
#include "sprite3d.hpp"
#include "dialog_manager.hpp"
#include "windstille_main.hpp"
#include "display/scene_context.hpp"
#include "scripting/wrapper_util.hpp"
#include "scripting/wrapper.hpp"
#include "input/input_manager.hpp"
#include "particle_system.hpp"
#include "script_manager.hpp"
#include "sound/sound_manager.hpp"

#include "game_session.hpp"

using namespace Windstille;

GameSession* GameSession::current_ = 0; 

GameSession::GameSession(const std::string& arg_filename)
  : console(16, CL_Display::get_height()-16),
    control_dialog("controldialog", resources),
    world (0)
{
  current_ = this;
  filename = arg_filename;
  change_sector();
  
  slots.push_back(CL_Keyboard::sig_key_down().connect(this, &GameSession::on_key_down));
  slots.push_back(CL_Mouse::sig_key_down().connect(this, &GameSession::on_mouse_down));

  blink = 0.0f;

  view = new View();
  
  energiebar = new Energiebar();
  dialog_manager = new DialogManager();

  logo       = CL_Sprite("logo", resources);
  logo_black = CL_Sprite("logo_black", resources);
}

GameSession::~GameSession()
{
  delete energiebar;
  delete view;
  delete dialog_manager;
  delete world;
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

  // Draw HUD
  energiebar->draw();

  if (control_state == DIALOG)
    dialog_manager->draw(); 
  
  control_dialog.set_alignment(origin_bottom_right);
  control_dialog.draw(800-16, 600-16);

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
  //std::cout << gluErrorString(glGetError()) << std::endl;

  draw_game();
  console.draw();

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

  if (world->get_player()->get_movement_state() == Player::DEAD)
    {
      CL_Font font = Fonts::dialog;
      font.set_alignment(origin_bottom_center);
      font.draw(CL_Display::get_width()/2, 200,
                "..:: Press Fire to restart ::..");
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
  console.update(delta);

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
GameSession::change_sector()
{
  if (world)
    delete world;

  world = new Sector(filename);
  
  if (1)
    {
      world->add(new Door(24, 6));
      world->add(new Door(32, 14));
      world->add(new Door(8, 22));

      CL_Surface surface1("particles/smoke", resources);
      CL_Surface surface2("particles/smoke2", resources);

      ParticleSystem* psystem2 = new ParticleSystem();
      psystem2->set_drawer(new SparkDrawer());
      psystem2->set_pos(0,0);
      psystem2->set_speed(300, 550);
      psystem2->set_cone(-25-90, 25-90);
      psystem2->set_gravity(0, 5);
      psystem2->set_line_distribution(-50, 0, 50, 0);

      ParticleSystem* psystem3 = new ParticleSystem();
      psystem3->set_lifetime(8);
      psystem3->set_count(30);
      surface2.set_blend_func(blend_src_alpha, blend_one_minus_src_alpha);
      surface2.set_alignment(origin_center);
      psystem3->set_drawer(new SurfaceDrawer(surface2));
      psystem3->set_pos(0,0);
      psystem3->set_speed(70, 100);
      psystem3->set_cone(-25-90, 25-90);
      psystem3->set_gravity(0, -1);
      psystem3->set_size(1.0f, 3.0f);
      psystem3->set_line_distribution(-50, 0, 50, 0);
 
      ParticleSystem* psystem = new ParticleSystem();
      psystem->set_count(100);
      surface1.set_blend_func(blend_src_alpha, blend_one);
      surface1.set_alignment(origin_center);
      psystem->set_drawer(new SurfaceDrawer(surface1));
      psystem->set_pos(0,0);
      psystem->set_speed(200, 300);
      psystem->set_cone(-5-90, 5-90);
      psystem->set_gravity(0, 0);
      psystem->set_line_distribution(-50, 0, 50, 0);

      psystem->set_spawn_point (768, 832);
      psystem2->set_spawn_point(768, 832);
      psystem3->set_spawn_point(768, 832);
      
      world->add(psystem3);
      world->add(psystem2);
      world->add(psystem);
    }
  
  world->add(new Sprite3D("3dsprites/3dsprites"));

  world->activate();
  world->spawn_player("default");
  
  GameObject::set_world (world);
  
  fade_state = FADEIN;
  fadeout_value = 0;
  control_state = GAME;
  target_state = RUN_GAME;
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
      console.add("Click at: ", CL_Point(view->screen2world(event.mouse_pos)));
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
        case CL_KEY_F1:
          console.activate();
          break;

        case CL_KEY_1:
          sc.set_render_mask(sc.get_render_mask() ^ SceneContext::COLORMAP);
          console.add("Toggled COLORMAP: ", (sc.get_render_mask() & SceneContext::COLORMAP) > 0);
          break;

        case CL_KEY_2:
          sc.set_render_mask(sc.get_render_mask() ^ SceneContext::LIGHTMAP);
          console.add("Toggled LIGHTMAP: ", (sc.get_render_mask() & SceneContext::LIGHTMAP) > 0);
          break;

        case CL_KEY_3:
          sc.set_render_mask(sc.get_render_mask() ^ SceneContext::HIGHLIGHTMAP);
          console.add("Toggled HIGHLIGHTMAP: ", (sc.get_render_mask() & SceneContext::HIGHLIGHTMAP) > 0);
          break;      

        default:
          // ignore key
          //console.add("Key pressed:: ", event.id);
          break;
        }
    }
}

void
GameSession::execute(const std::string& str_)
{
  std::string str = str_; //"return (" + str_ + ")";

  int i = str.length();
  const char* buffer = str.c_str();

  HSQUIRRELVM vm = script_manager->get_vm();
  int oldtop=sq_gettop(vm); 
  try {
    int retval = 0;

    if(i>0){
      if(SQ_SUCCEEDED(sq_compilebuffer(vm,buffer,i,_SC("interactive console"),SQTrue))){
        sq_pushroottable(vm);
        if(SQ_SUCCEEDED(sq_call(vm,1, retval))) {
          scprintf(_SC("\n"));
          sq_pushroottable(vm);
          sq_pushstring(vm,_SC("print"),-1);
          sq_get(vm,-2);
          sq_pushroottable(vm);
          sq_push(vm,-4);
          sq_call(vm,2,SQFalse);
          scprintf(_SC("\n"));
        }
      }
    }
  } catch(std::exception& e) {
    std::cerr << "Couldn't execute command '" << str_ << "': "
      << e.what() << "\n";
  }
  sq_settop(vm,oldtop);
}

/* EOF */
