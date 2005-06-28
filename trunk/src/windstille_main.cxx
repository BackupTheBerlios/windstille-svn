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

//#include <ruby.h>
//#include <ClanLib/gl.h>
#include <ClanLib/core.h>
#include <ClanLib/vorbis.h>
#include <ClanLib/display.h>
#include <physfs.h>

#include "windstille_error.hxx"
#include "globals.hxx"
#include "game_session.hxx"
#include "windstille_main.hxx"
#include "windstille_menu.hxx"
#include "fonts.hxx"
#include "sector.hxx"
#include "input/input_manager.hxx"
#include "sound/sound_manager.hpp"
#include "tile_factory.hxx"
#include "script_manager.hpp"

//extern "C" void Init_windstille(void);

WindstilleMain main_app;
CL_ResourceManager* resources;

//static members
int WindstilleMain::screen_width;
int WindstilleMain::screen_height;

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
  CL_ConsoleWindow console("Console Output");
  console.redirect_stdio("windstille.log");
#endif

  try {
    init_physfs(argv[0]);
  } catch(std::exception& e) {
    std::cout << "std::exception: " << e.what() << std::endl;
    return 1;
  }

  // Init the path
  bindir  = CL_System::get_exe_path();

  if (datadir.empty())
    datadir = bindir + "data/";

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

#ifndef DEBUG // we wanna have a stacktrace in debug mode
  try {
#endif
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

    TileFactory::init();
    if (levelfile.empty())
      {
        if (debug) std::cout << "Starting Menu" << std::endl;
        WindstilleMenu menu;
        menu.display();
      }
    else 
      {
        GameSession game (levelfile);
        game.display ();
      }
    TileFactory::deinit();
    InputManager::deinit();

    deinit_modules();

#ifndef DEBUG
  } catch (CL_Error& error) {
    std::cout << "CL_Error: " << error.message << std::endl;
  } catch (WindstilleError& err) {
    std::cout << "WindstilleError: " << err.what() << std::endl;
  } catch (std::exception& err) {
    std::cout << "std::exception: " << err.what() << std::endl;
  } catch (...) {
    std::cout << "Error catched something unknown?!" << std::endl;
  }
#endif

  return 0;
}

void
WindstilleMain::init_modules()
{
  // ruby_init();
  //  Init_windstille();
  
  // Init ClanLib
  CL_SetupCore::init();
  
  CL_SetupGL::init();

  CL_SetupDisplay::init();

  window = new CL_DisplayWindow("Windstille",
                                screen_width, screen_height, fullscreen, allow_resize);
  CL_Display::clear();
  CL_Display::flip(0);

  resources =  new CL_ResourceManager();
  resources->add_resources(CL_ResourceManager(datadir + "windstille.xml", false));
  resources->add_resources(CL_ResourceManager(datadir + "tiles.xml", false));

  Fonts::init(); 
  sound_manager = new SoundManager();

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
  dir += "/data";
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

/* EOF */
