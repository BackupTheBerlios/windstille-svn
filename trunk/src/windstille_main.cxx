//  $Id: windstille_main.cxx,v 1.30 2003/11/13 12:59:42 grumbel Exp $
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

#include <ruby.h>
//#include <ClanLib/gl.h>
#include <ClanLib/core.h>
#include <ClanLib/vorbis.h>
#include <ClanLib/sound.h>
#include <ClanLib/display.h>

#include "config.h"
#include "ruby_functor.hxx"
#include "string_converter.hxx"
#include "windstille_error.hxx"
#include "globals.hxx"
#include "windstille_game.hxx"
#include "windstille_level.hxx"
#include "windstille_main.hxx"
#include "windstille_menu.hxx"
#include "fonts.hxx"
#include "feuerkraft_error.hxx"
#include "input/input_manager.hxx"
#include "music_manager.hxx"
#include "tile_factory.hxx"

extern "C" void Init_windstille(void);

WindstilleMain main_app;
CL_ResourceManager* resources;

WindstilleMain::WindstilleMain()
{
  screen_width  = 800;
  screen_height = 600;
#ifdef WIN32
  fullscreen    = true;
#else
  fullscreen    = false;
#endif
  allow_resize  = false;
  game_definition_file = "windstille.rb";
}

WindstilleMain::~WindstilleMain()
{
}

void
WindstilleMain::parse_command_line(int argc, char** argv)
{
  CL_CommandLine argp;

  const int debug_flag = 256;
  const int game_flag = 257;
    
  argp.set_help_indent(22);
  argp.add_usage ("[LEVELFILE]");
  argp.add_doc   ("Windstille is a classic Jump'n Run game.");

  argp.add_group("Display Options:");
  argp.add_option('g', "geometry",   "WxH", "Change window size to WIDTH and HEIGHT");
  argp.add_option('f', "fullscreen", "", "Launch the game in fullscreen");

  argp.add_group("Sound Options:");
  argp.add_option('s', "disable-sound", "", "Disable sound");
  argp.add_option('S', "enable-sound", "", "Enable sound");

  argp.add_group("Controlls Options:");
  argp.add_option('c', "controller", "FILE", "Use controller as defined in FILE");

  argp.add_group("Misc Options:");
  argp.add_option(game_flag, "game", "GAME", "Load the game definition file at startup");
  argp.add_option('d', "datadir",    "DIR", "Fetch game data from DIR");
  argp.add_option(debug_flag, "debug",      "", "Turn on debug output");
  argp.add_option('h', "help",       "", "Print this help");

  argp.add_group("Demo Recording/Playback Options:");
  argp.add_option('r', "record",      "FILE", "Record input events to FILE");
  argp.add_option('a', "record-video","DIR",  "Record a gameplay video to DIR");
  argp.add_option('p', "play",        "FILE", "Playback input events from FILE");

  argp.parse_args(argc, argv);

  while (argp.next())
    {
      switch (argp.get_key())
        {
        case 'r':
          recorder_file = argp.get_argument();
          break;

        case 'a':
          screenshot_dir = argp.get_argument();
          break;

        case 'p':
          playback_file = argp.get_argument();
          break;

        case 'd':
          datadir = argp.get_argument();
          break;

        case game_flag:
          game_definition_file = argp.get_argument();
          break;

        case debug_flag:
          debug = 1;
          break;

        case 'f':
          fullscreen = true;
          break;

        case 'g':
          if (sscanf(argp.get_argument().c_str(), "%dx%d", &screen_width, &screen_height) == 2)
            std::cout << "Geometry: " << screen_width << "x" << screen_height << std::endl;
          else
            throw CL_Error("Geometry option '-g' requires argument of type {WIDTH}x{HEIGHT}");
          break;
        
        case 's':
          sound_disabled = true;
          break;

        case 'S':
          sound_disabled = false;
          break;  

        case 'c':
          controller_file = argp.get_argument();
          break;

        case 'h':
          argp.print_help();
          exit(EXIT_SUCCESS);
          break;

        case CL_CommandLine::REST_ARG:
          levelfile = argp.get_argument();
          break;
        }
    }
}

int 
WindstilleMain::main(int argc, char** argv)
{
#ifdef WIN32
  CL_ConsoleWindow console;
  console.redirect_stdio("windstille.log");
#endif

  // Init the path
  bindir  = CL_System::get_exe_path();

  if (datadir.empty())
    datadir = bindir + "../data/";

#ifndef WIN32
  char* home_c = getenv("HOME");
  if (home_c) 
    {
      std::string home = home_c; 
      home += "/.windstille";
      if (CL_Directory::create(home))
        std::cout << "Created " << home << std::endl;
      homedir = home + "/";
    }
  else
    {
      throw WindstilleError("Couldn't find environment variable HOME");
    }
#else
  homedir = "config/";
#endif
  
  try {
    parse_command_line(argc, argv);
    init_modules();
    
    std::cout << "Detected " << CL_Joystick::get_device_count() << " joysticks" << std::endl;
        
    if (playback_file.empty())
      {
        if (!controller_file.empty())
          InputManager::init(controller_file);
        else
          InputManager::init(datadir + "controller/keyboard.scm");
      }
    else
      {
        InputManager::init_playback(playback_file);
      }

    if (!recorder_file.empty())
      InputManager::setup_recorder(recorder_file);

    TileFactory::init();
    if (levelfile.empty())
      {
        if (debug) std::cout << "Starting Menu" << std::endl;
        WindstilleMenu menu;
        menu.display();
      }
    else 
      {
        WindstilleGame game (levelfile);
        if (debug) std::cout << "WindstilleMain: entering main-loop..." << std::endl;
        game.display ();
      }
    TileFactory::deinit();
    InputManager::deinit();

    deinit_modules();

  } catch (CL_Error& error) {
    std::cout << "CL_Error: " << error.message << std::endl;
  } catch (FeuerkraftError& err) {
    std::cout << "FeuerkraftError: " << err.what() << std::endl;
  } catch (std::exception& err) {
    std::cout << "std::exception: " << err.what() << std::endl;
  } catch (...) {
    std::cout << "Error catched something unknown?!" << std::endl;
  }

  return 0;
}

void
WindstilleMain::init_modules()
{
  ruby_init();
  Init_windstille();
  
  // Init ClanLib
  CL_SetupCore::init();
  
  CL_SetupGL::init();
  //CL_SetupSDL::init();

  CL_SetupDisplay::init();

  if (!sound_disabled)
    {
      CL_SetupSound::init();
      CL_SetupVorbis::init();
    }

  window = new CL_DisplayWindow(PACKAGE_STRING,
                                screen_width, screen_height, fullscreen, allow_resize);
  CL_Display::clear();
  CL_Display::flip();

  if (!sound_disabled)
    sound = new CL_SoundOutput(44100);

  resources =  new CL_ResourceManager();
  //resources->add_resources(CL_ResourceManager(datadir + "tiles.xml", false));
  resources->add_resources(CL_ResourceManager(datadir + "windstille.xml", false));
  
  //std::cout << "Loading all resources..." << std::endl;
  //resources->load_all();
  //std::cout << "Loading all resources... done" << std::endl;

  std::cout << "Loading Windstille startup script: " << game_definition_file << std::endl;
  //gh_load((datadir + game_definition_file).c_str());
  RubyFunctor::load_file((datadir + game_definition_file).c_str());
  std::cout << "done" << std::endl;

  Fonts::init(); 
  MusicManager::init();
}

void
WindstilleMain::deinit_modules()
{
  MusicManager::deinit();
  Fonts::deinit();

  if (!sound_disabled)
    delete sound;
  
  delete window;

  if (!sound_disabled)
    {
      CL_SetupVorbis::init();
      CL_SetupSound::init();
    }

  CL_SetupDisplay::init();

  CL_SetupGL::init();
  //CL_SetupSDL::init();

  CL_SetupCore::init(); 
}

/* EOF */
