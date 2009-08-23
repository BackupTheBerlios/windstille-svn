/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000,2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**  
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**  
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SDL.h"

#include "windstille.hpp"

#include <boost/scoped_array.hpp>
#include <boost/filesystem.hpp>
#include <stdio.h>
#include <physfs.h>

#include "config.hpp"
#include "display/display.hpp"
#include "display/opengl_state.hpp"
#include "display/opengl_window.hpp"
#include "display/surface_manager.hpp"
#include "display/texture_manager.hpp"
#include "engine/script_manager.hpp"
#include "engine/sector.hpp"
#include "font/fonts.hpp"
#include "font/ttf_font.hpp"
#include "globals.hpp"
#include "input/input_manager.hpp"
#include "screen/game_session.hpp"
#include "screen/particle_viewer.hpp"
#include "screen/screen.hpp"
#include "screen/screen_manager.hpp"
#include "screen/sprite2dview.hpp"
#include "screen/sprite3dview.hpp"
#include "screen/title_screen.hpp"
#include "sound/sound_manager.hpp"
#include "sprite2d/manager.hpp"
#include "sprite3d/manager.hpp"
#include "tile/tile_factory.hpp"
#include "util/util.hpp"
#include "util/pathname.hpp"
#include "util/system.hpp"
#include "windstille_main.hpp"

WindstilleMain::WindstilleMain()
{
}

WindstilleMain::~WindstilleMain()
{
}

int 
WindstilleMain::main(int argc, char** argv)
{
  try 
  {
    Pathname::set_datadir(System::find_default_datadir());
    Pathname::set_userdir(System::find_default_userdir());

    config.parse_args(argc, argv);

    init_physfs(argv[0]);
    init_sdl();

    config.load();
    
    config.parse_args(argc, argv);

    {
      OpenGLWindow      window;
      TTFFontManager    ttffont_manager;
      Fonts             fonts;
      SoundManager      sound_manager;
      TextureManager    texture_manager;
      SurfaceManager    surface_manager;
      SpriteManager     sprite_manager;
      sprite3d::Manager sprite3d_manager;
      ScriptManager     script_manager;
      InputManager      input_manager;
      TileFactory       tile_factory("tiles.scm");

      init_modules();
    
      run();

      config.save();
    
      PHYSFS_deinit();
    }
  }
  catch (std::exception& err)
  {
    std::cout << "std::exception: " << err.what() << std::endl;
  }
  catch (...)
  {
    std::cout << "Error catched something unknown?!" << std::endl;
  }

  return 0;
}

void
WindstilleMain::run()
{  
  if (config.get<std::string>("levelfile").is_set())
  {
    Pathname filename(config.get_string("levelfile"), Pathname::kSysPath);

    if (debug) 
      std::cout << "Starting file: " << filename << std::endl;

    const std::string file_type = filename.get_extension();

    if (file_type == "wsprite")
    {
      std::auto_ptr<Sprite3DView> sprite3dview(new Sprite3DView());

      if (!filename.empty())
        sprite3dview->set_model(filename);

      // Launching Sprite3DView instead of game
      screen_manager.push_screen(sprite3dview.release());
    }
    else if (file_type == "sprite" || file_type == "png" || file_type == "jpg")
    {
      std::auto_ptr<Sprite2DView> sprite2dview(new Sprite2DView());

      if (!filename.empty())
        sprite2dview->set_sprite(filename);

      // Launching Sprite2DView instead of game
      screen_manager.push_screen(sprite2dview.release());
    }
    else if (file_type == "particles")
    {
      ParticleViewer* particle_viewer = new ParticleViewer();
      if (!filename.empty())
        particle_viewer->load(filename);
      screen_manager.push_screen(particle_viewer);
    }
    else if (file_type == "wst")
    {
      screen_manager.push_screen(new GameSession(filename));
    }
    else
    {
      throw std::runtime_error("Unknown filetype '" + file_type + "'");
    }
  }
  else
  {
    screen_manager.push_screen(new TitleScreen());
  }
  
  screen_manager.run();
}

void
WindstilleMain::init_modules()
{
  if (debug) std::cout << "Initialising Fonts" << std::endl;
  SoundManager::current()->set_master_volume(config.get_int("master-volume")/100.0f);
  SoundManager::current()->enable_sound(config.get_bool("sound"));
  SoundManager::current()->enable_music(config.get_bool("music"));

  ScriptManager::current()->run_script_file("scripts/windstille.nut", true);

  { // Fill controller_description with data
    
    // DPad
    controller_description.add_button("menu-up-button",    MENU_UP_BUTTON);
    controller_description.add_button("menu-down-button",  MENU_DOWN_BUTTON);
    controller_description.add_button("menu-left-button",  MENU_LEFT_BUTTON);
    controller_description.add_button("menu-right-button", MENU_RIGHT_BUTTON);

    // Face Button
    controller_description.add_button("primary-button",    PRIMARY_BUTTON);
    controller_description.add_button("secondary-button",  SECONDARY_BUTTON);
    controller_description.add_button("tertiary-button",   TERTIARY_BUTTON);
    controller_description.add_button("quaternary-button", QUATERNARY_BUTTON);

    // Stick Buttons
    controller_description.add_button("left-stick-button",  LEFT_STICK_BUTTON);
    controller_description.add_button("right-stick-button", RIGHT_STICK_BUTTON);

    // Shoulder Button
    controller_description.add_button("left-shoulder-button",  LEFT_SHOULDER_BUTTON);
    controller_description.add_button("right-shoulder-button", RIGHT_SHOULDER_BUTTON);

    // Back/Start
    controller_description.add_button("select-button",    SELECT_BUTTON);
    controller_description.add_button("start-button",     START_BUTTON);
    controller_description.add_button("debug-button",     DEBUG_BUTTON);

    controller_description.add_axis("left-trigger-axis",  LEFT_TRIGGER_AXIS);
    controller_description.add_axis("right-trigger-axis", RIGHT_TRIGGER_AXIS);

    controller_description.add_axis("x-axis", X_AXIS);
    controller_description.add_axis("y-axis", Y_AXIS);

    controller_description.add_axis("x2-axis", X2_AXIS);
    controller_description.add_axis("y2-axis", Y2_AXIS);

    controller_description.add_ball("mouse-motion-x", MOUSE_MOTION_X);
    controller_description.add_ball("mouse-motion-y", MOUSE_MOTION_Y);
  }
    
  {     
    if (config.get<std::string>("primary-controller-file").is_set())
      InputManager::current()->load(config.get<std::string>("primary-controller-file").get());
    else
      InputManager::current()->load("controller/keyboard.scm");

    if (config.get<std::string>("secondary-controller-file").is_set())
      InputManager::current()->load(config.get<std::string>("secondary-controller-file").get());
  }
}

void
WindstilleMain::init_sdl()
{
#ifdef DEBUG
  // I wanna have usefull backtraces in debug mode
  Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE;
#else
  Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#endif

  if (SDL_Init(flags) < 0)
    {
      std::stringstream msg;
      msg << "Couldn't initialize SDL: " << SDL_GetError();
      throw std::runtime_error(msg.str());
    }

  SDL_EnableUNICODE(1);
}

void
WindstilleMain::init_physfs(const char* argv0)
{
  if (!PHYSFS_init(argv0))
  {
    std::stringstream msg;
    msg << "Couldn't initialize physfs: " << PHYSFS_getLastError();
    throw std::runtime_error(msg.str());
  }
  else
  {
    boost::filesystem::create_directory(Pathname::get_userdir());
    boost::filesystem::create_directory(Pathname("screenshots", Pathname::kUserPath).get_sys_path());

    PHYSFS_setWriteDir(Pathname::get_userdir().c_str());
    PHYSFS_addToSearchPath(Pathname::get_userdir().c_str(), 0);
    PHYSFS_addToSearchPath(Pathname::get_datadir().c_str(), 0);

    // allow symbolic links
    PHYSFS_permitSymbolicLinks(1);

    if (debug)
    { //show search Path
      std::cout << "userdir: " << Pathname::get_userdir() << std::endl;
      std::cout << "datadir: " << Pathname::get_datadir() << std::endl;

      std::cout << "SearchPath:" << std::endl;
      char** search_path = PHYSFS_getSearchPath();
      for(char** i = search_path; *i != NULL; i++)
        std::cout << "  " << *i << std::endl;;
      PHYSFS_freeList(search_path);
    }
  }
}

int main(int argc, char** argv)
{
  return WindstilleMain().main(argc, argv);
}

/* EOF */
