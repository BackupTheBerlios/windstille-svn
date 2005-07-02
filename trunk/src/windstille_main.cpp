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

#include <ClanLib/core.h>
#include <ClanLib/vorbis.h>
#include <ClanLib/display.h>
#include <physfs.h>

#include "windstille_error.hpp"
#include "globals.hpp"
#include "screen.hpp"
#include "game_session.hpp"
#include "windstille_main.hpp"
#include "windstille_menu.hpp"
#include "windstille_bonus.hpp"
#include "fonts.hpp"
#include "sector.hpp"
#include "input/input_manager.hpp"
#include "sound/sound_manager.hpp"
#include "tile_factory.hpp"
#include "script_manager.hpp"
#include "tinygettext/gettext.hpp"
#include "gameconfig.hpp"

using namespace Windstille;

WindstilleMain main_app;
CL_ResourceManager* resources;

WindstilleMain::WindstilleMain()
  : the_game(0)
{
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
          config->use_fullscreen = true;
          break;

        case 'g':
          if (sscanf(argp.get_argument().c_str(), "%dx%d",
                     &config->screen_width, &config->screen_height) == 2)
            std::cout << "Geometry: " << config->screen_width
                      << "x" << config->screen_height << std::endl;
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

std::string dirname(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('/');
  if(p == std::string::npos)
    return "";

  return filename.substr(0, p+1);        
}

std::string basename(const std::string& filename)
{
  std::string::size_type p = filename.find_last_of('/');
  if(p == std::string::npos)
    return filename;

  return filename.substr(p, filename.size()-p);
}

int 
WindstilleMain::main(int argc, char** argv)
{
#ifdef WIN32
  CL_ConsoleWindow console("Console Output");
  console.redirect_stdio("windstille.log");
#endif

  try {
    init_physfs(argv[0]);
    
    dictionaryManager = new TinyGetText::DictionaryManager();
    dictionaryManager->set_charset("iso8859-1");
    dictionaryManager->add_directory("locale");                    

    config = new Config();
    config->load();
  } catch(std::exception& e) {
    std::cout << "std::exception: " << e.what() << std::endl;
    return 1;
  }

  // Init the path
  bindir  = CL_System::get_exe_path();

  if (datadir.empty())
    datadir = bindir + "data/";

  try {
    parse_command_line(argc, argv);
    init_modules();

    if (playback_file.empty())
      {
        if (!controller_file.empty())
          InputManager::init(controller_file);
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
        game_main_state = LOAD_MENU;
      }
    else 
      {
        std::string leveldir = dirname(levelfile);
        PHYSFS_addToSearchPath(leveldir.c_str(), true);
        GameSession game(basename(levelfile));
        the_game = new GameSession(basename(levelfile));
        game_main_state = RUN_GAME;
      }
      
    while (game_main());
    
    TileFactory::deinit();
    InputManager::deinit();

    deinit_modules();

  } catch (CL_Error& error) {
    std::cout << "CL_Error: " << error.message << std::endl;
  } catch (WindstilleError& err) {
    std::cout << "WindstilleError: " << err.what() << std::endl;
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
  
  PHYSFS_deinit();

  return 0;
}

void
WindstilleMain::init_modules()
{
  if (debug) std::cout << "Initialising ClanLib" << std::endl;
  // Init ClanLib
  CL_SetupCore::init();
  
  if (debug) std::cout << "Initialising GL" << std::endl;
  CL_SetupGL::init();

  CL_SetupDisplay::init();

  window = new CL_DisplayWindow("Windstille",
                                config->screen_width, config->screen_height,
                                config->use_fullscreen, false);
  CL_Display::clear();
  CL_Display::flip(0);

  resources =  new CL_ResourceManager();
  resources->add_resources(CL_ResourceManager(datadir + "windstille.xml", false));
  resources->add_resources(CL_ResourceManager(datadir + "tiles.xml", false));

  if (debug) std::cout << "Initialising Fonts" << std::endl;
  Fonts::init(); 
  sound_manager = new SoundManager();
  sound_manager->enable_sound(config->sound_enabled);
  sound_manager->enable_music(config->music_enabled);

  if (debug) std::cout << "Initialising ScriptManager" << std::endl;
  script_manager = new ScriptManager();
}

void
WindstilleMain::deinit_modules()
{
  delete script_manager;
  script_manager = 0;
  
  delete sound_manager;
  sound_manager = 0;
  Fonts::deinit();

  delete window;

  CL_SetupDisplay::init();

  CL_SetupGL::init();

  CL_SetupCore::init(); 
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
  std::string dir = PHYSFS_getBaseDir();
  dir += "data";
  std::string testfname = dir;
  testfname += "/tiles.scm";
  bool sourcedir = false;
  FILE* f = fopen(testfname.c_str(), "r");
  if(f) {
    fclose(f);
    if(!PHYSFS_addToSearchPath(dir.c_str(), 1)) {
      std::cout << "Warning: Couldn't add '" << dir
                << "' to physfs searchpath: " << PHYSFS_getLastError() << "\n";
    } else {
      sourcedir = true;
    }
  }

  if(!sourcedir) {
#if defined(APPDATADIR) || defined(ENABLE_BINRELOC)
    std::string datadir;
#ifdef ENABLE_BINRELOC
    char* brdatadir = br_strcat(DATADIR, "/" PACKAGE_NAME);
    datadir = brdatadir;
    free(brdatadir);
#else
    datadir = APPDATADIR;
#endif
    if(!PHYSFS_addToSearchPath(datadir.c_str(), 1)) {
      std::cout << "Couldn't add '" << datadir
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

bool
WindstilleMain::game_main()
{
  switch (game_main_state)
    {
    case RUN_GAME:
      the_game->display();
      break;
    case LOAD_MENU:
      if (the_game)
        delete the_game;
      the_game = new WindstilleMenu();
      game_main_state = RUN_GAME;
      break;
    case LOAD_GAME_SESSION:
      if (the_game)
        delete the_game;
      the_game = new GameSession("levels/newformat2.wst");
      game_main_state = RUN_GAME;
      break;
    case LOAD_BONUS:
      if (the_game)
        delete the_game;
      the_game = new WindstilleBonus();
      game_main_state = RUN_GAME;
      break;
    case QUIT_GAME:
      if (the_game)
        delete the_game;
      return false;
      break;
    }
  
  return true;
}

/* EOF */
