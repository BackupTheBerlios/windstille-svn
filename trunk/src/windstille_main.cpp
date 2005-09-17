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
#include <config.h>

#include <stdio.h>
#include <physfs.h>

#include "globals.hpp"
#include "screen.hpp"
#include "windstille_main.hpp"
#include "font/fonts.hpp"
#include "game_session.hpp"
#include "sector.hpp"
#include "input/input_manager.hpp"
#include "sound/sound_manager.hpp"
#include "tile_factory.hpp"
#include "script_manager.hpp"
#include "tinygettext/gettext.hpp"
#include "gameconfig.hpp"
#include "util.hpp"
#include "font/ttf_font.hpp"
#include "display/display.hpp"
#include "glutil/surface_manager.hpp"
#include "glutil/texture_manager.hpp"
#include "sprite3d/manager.hpp"
#include "screen_manager.hpp"
#include "command_line.hpp"
#include "sprite2d/manager.hpp"

WindstilleMain::WindstilleMain()
{
}

WindstilleMain::~WindstilleMain()
{
}

void
WindstilleMain::parse_command_line(int argc, char** argv)
{
  CommandLine argp;

  const int debug_flag = 256;
    
  argp.set_help_indent(24);
  argp.add_usage ("[LEVELFILE]");
  argp.add_doc   ("Windstille is a classic Jump'n Run game.");

  argp.add_group("Display Options:");
  argp.add_option('g', "geometry",   "WxH", "Change window size to WIDTH and HEIGHT");
  argp.add_option('f', "fullscreen", "", "Launch the game in fullscreen");
  argp.add_option('a', "anti-aliasing", "NUM", "Enable NUMx Anti-Aliasing");

  argp.add_group("Sound Options:");
  argp.add_option('s', "disable-sound", "", "Disable sound");
  argp.add_option('S', "enable-sound", "", "Enable sound");

  argp.add_group("Controlls Options:");
  argp.add_option('c', "controller", "FILE", "Use controller as defined in FILE");

  argp.add_group("Misc Options:");
  argp.add_option('d', "datadir",    "DIR", "Fetch game data from DIR");
  argp.add_option(debug_flag, "debug",      "", "Turn on debug output");
  argp.add_option('x', "version",       "", "Print Windstille Version");
  argp.add_option('h', "help",       "", "Print this help");

  argp.add_group("Demo Recording/Playback Options:");
  argp.add_option('r', "record",      "FILE", "Record input events to FILE");
  argp.add_option('v', "record-video","DIR",  "Record a gameplay video to DIR");
  argp.add_option('p', "play",        "FILE", "Playback input events from FILE");

  argp.parse_args(argc, argv);

  while (argp.next())
    {
      switch (argp.get_key())
        {
        case 'a':
          if (sscanf(argp.get_argument().c_str(), "%d", &config->antialiasing) != 1)
            {
              throw std::runtime_error("Anti-Aliasing option '-a' requires argument of type {NUM}");
            }
          break;

        case 'r':
          recorder_file = argp.get_argument();
          break;

        case 'x':
          screenshot_dir = argp.get_argument();
          break;

        case 'p':
          playback_file = argp.get_argument();
          break;

        case 'd':
          datadir = argp.get_argument();
          break;

        case debug_flag:
          debug = 1;
          break;

        case 'f':
          config->use_fullscreen = true;
          break;

        case 'g':
          if (sscanf(argp.get_argument().c_str(), "%dx%d",
                     &config->screen_width, &config->screen_height) == 2)
            std::cout << "Geometry: " << config->screen_width
                      << "x" << config->screen_height << std::endl;
          else
            throw std::runtime_error("Geometry option '-g' requires argument of type {WIDTH}x{HEIGHT}");
          break;
        
        case 's':
          config->sound_enabled = false;
          break;

        case 'S':
          config->sound_enabled = true;
          break;  

        case 'c':
          controller_file = argp.get_argument();
          break;

        case 'v':
          std::cout << "Windstille " << PACKAGE_VERSION << std::endl;
          exit(EXIT_SUCCESS);
          break;

        case 'h':
          argp.print_help();
          exit(EXIT_SUCCESS);
          break;

        case CommandLine::REST_ARG:
          levelfile = argp.get_argument();
          break;
        }
    }
}

int 
WindstilleMain::main(int argc, char** argv)
{
  try {
    init_physfs(argv[0]);
    init_sdl();

    dictionaryManager = new TinyGetText::DictionaryManager();
    dictionaryManager->set_charset("iso8859-1");
    dictionaryManager->add_directory("locale");

    config = new Config();
    config->load();
  } catch(std::exception& e) {
    std::cout << "std::exception: " << e.what() << std::endl;
    return 1;
  }

  try {
    parse_command_line(argc, argv);
    init_modules();

    if (playback_file.empty())
      {
        if (!controller_file.empty())
          InputManager::init(controller_file);
        else if (PHYSFS_exists("controller.cfg"))
          InputManager::init("controller.cfg");
        else
          InputManager::init("controller/keyboard.scm");
      }
    else
      {
        InputManager::init_playback(playback_file);
      }

    if (!recorder_file.empty())
      InputManager::setup_recorder(recorder_file);
    
    if (debug) std::cout << "Initialising TileFactory" << std::endl;
    TileFactory::init();

    if (levelfile.empty())
      {
        screen_manager.set_screen(new GameSession("levels/newformat2.wst"));
      }
    else
      {
        std::string leveldir = dirname(levelfile);
        PHYSFS_addToSearchPath(leveldir.c_str(), true);
        screen_manager.set_screen(new GameSession(basename(levelfile)));
      }
        
    console << "Press F1 to open the console" << std::endl;
    screen_manager.run();
    
    TileFactory::deinit();
    InputManager::deinit();

    deinit_modules();

  } catch (std::exception& err) {
    std::cout << "std::exception: " << err.what() << std::endl;
  } catch (...) {
    std::cout << "Error catched something unknown?!" << std::endl;
  }

  config->save();
  delete config;
  config = 0;

  delete dictionaryManager;
  dictionaryManager = 0;
  
  SDL_Quit();
  PHYSFS_deinit();

  return 0;
}

void
WindstilleMain::init_modules()
{
  if (debug) std::cout << "Initialising Freetype2" << std::endl;
    
  TTFFont::init();
  
  Display::init();

  if (debug) std::cout << "Initialising Fonts" << std::endl;
  Fonts::init(); 
  sound_manager = new SoundManager();
  sound_manager->enable_sound(config->sound_enabled);
  sound_manager->enable_music(config->music_enabled);

  if (debug) std::cout << "Initialising ScriptManager" << std::endl;
  texture_manager  = new TextureManager();
  surface_manager  = new SurfaceManager();
  script_manager   = new ScriptManager();
  sprite2d_manager = new sprite2d::Manager();
  sprite3d_manager = new sprite3d::Manager();

  script_manager->run_script_file("scripts/windstille.nut");
}

void
WindstilleMain::deinit_modules()
{
  delete sprite3d_manager;
  sprite3d_manager = 0;

  delete sprite2d_manager;
  sprite2d_manager = 0;

  delete script_manager;
  script_manager = 0;

  delete surface_manager;
  surface_manager = 0;

  delete texture_manager;
  texture_manager = 0;
  
  delete sound_manager;
  sound_manager = 0;
  Fonts::deinit();

  TTFFont::deinit();
}

void
WindstilleMain::init_sdl()
{
#ifdef DEBUG
  // I wanna have usefull backtraces in debug mode
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE) < 0) {
#else
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
#endif
    std::stringstream msg;
    msg << "Couldn't initialize SDL: " << SDL_GetError();
    throw std::runtime_error(msg.str());
  }

  SDL_EnableUNICODE(1);
}

void
WindstilleMain::init_physfs(const char* argv0)
{
  if(!PHYSFS_init(argv0)) {
    std::stringstream msg;
    msg << "Couldn't initialize physfs: " << PHYSFS_getLastError();
    throw std::runtime_error(msg.str());
  }

  // Initialize physfs (this is a slightly modified version of
  // PHYSFS_setSaneConfig
  const char* application = PACKAGE_NAME;
  const char* userdir = PHYSFS_getUserDir();
  const char* dirsep = PHYSFS_getDirSeparator();
  char* writedir = new char[strlen(userdir) + strlen(application) + 2];

  // Set configuration directory
  sprintf(writedir, "%s.%s", userdir, application);
  if(!PHYSFS_setWriteDir(writedir)) {
    // try to create the directory
    char* mkdir = new char[strlen(application) + 2];
    sprintf(mkdir, ".%s", application);
    if(!PHYSFS_setWriteDir(userdir) || !PHYSFS_mkdir(mkdir)) {
      std::ostringstream msg;
      msg << "Failed creating configuration directory '"
          << writedir << "': " << PHYSFS_getLastError();
      delete[] writedir;
      delete[] mkdir;
      throw std::runtime_error(msg.str());
    }
    delete[] mkdir;

    if(!PHYSFS_setWriteDir(writedir)) {
      std::ostringstream msg;
      msg << "Failed to use configuration directory '"
          <<  writedir << "': " << PHYSFS_getLastError();
      delete[] writedir;
      throw std::runtime_error(msg.str());
    }
  }
  PHYSFS_addToSearchPath(writedir, 0);
  delete[] writedir;

  // Search for archives and add them to the search path
  const char* archiveExt = "zip";
  char** rc = PHYSFS_enumerateFiles("/");
  size_t extlen = strlen(archiveExt);

  for(char** i = rc; *i != 0; ++i) {
    size_t l = strlen(*i);
    if((l > extlen) && ((*i)[l - extlen - 1] == '.')) {
      const char* ext = (*i) + (l - extlen);
      if(strcasecmp(ext, archiveExt) == 0) {
        const char* d = PHYSFS_getRealDir(*i);
        char* str = new char[strlen(d) + strlen(dirsep) + l + 1];
        sprintf(str, "%s%s%s", d, dirsep, *i);
        PHYSFS_addToSearchPath(str, 1);
        delete[] str;
      }
    }
  }

  PHYSFS_freeList(rc);

  // when started from source dir...
  ::datadir = PHYSFS_getBaseDir();
  ::datadir += "data/";
  std::string testfname = ::datadir;
  testfname += "tiles.scm";
  bool sourcedir = false;
  FILE* f = fopen(testfname.c_str(), "r");
  if(f) {
    fclose(f);
    if(!PHYSFS_addToSearchPath(::datadir.c_str(), 1)) {
      std::cout << "Warning: Couldn't add '" << ::datadir
                << "' to physfs searchpath: " << PHYSFS_getLastError() << "\n";
    } else {
      sourcedir = true;
    }
  }

  if(!sourcedir) {
#if defined(APPDATADIR) || defined(ENABLE_BINRELOC)
#ifdef ENABLE_BINRELOC
    char* brdatadir = br_strcat(DATADIR, "/" PACKAGE_NAME);
    ::datadir = brdatadir;
    free(brdatadir);
#else
    ::datadir = APPDATADIR;
#endif
    if(!PHYSFS_addToSearchPath(::datadir.c_str(), 1)) {
      std::cout << "Couldn't add '" << ::datadir
        << "' to physfs searchpath: " << PHYSFS_getLastError() << "\n";
    }
#endif
  }

  // allow symbolic links
  PHYSFS_permitSymbolicLinks(1);

  //show search Path
  for(char** i = PHYSFS_getSearchPath(); *i != NULL; i++)
    printf("[%s] is in the search path.\n", *i);
}

int main(int argc, char** argv)
{
  WindstilleMain main_app;

  main_app.main(argc, argv);

  return 0;
}

/* EOF */
