#include "gameconfig.hpp"

#include <memory>
#include <iostream>
#include "lisp/lisp.hpp"
#include "lisp/parser.hpp"
#include "lisp/writer.hpp"

Config* config = 0;

Config::Config()
{
  // set default values
  screen_width = 800;
  screen_height = 600; 
  use_fullscreen = true;
  show_fps = false;
  sound_enabled = true;
  music_enabled = true;
}

Config::~Config()
{
}

void
Config::load()
{
  try {
    std::auto_ptr<lisp::Lisp> root(lisp::Parser::parse("config"));

    const lisp::Lisp* config_lisp = root->get_lisp("windstille-config");
    if(!config) {
      std::cerr << "Warning: Config file is not a windstille-config file.\n";
      return;
    }

    config_lisp->get("screen_width", screen_width);
    config_lisp->get("screen_height", screen_height);
    config_lisp->get("fullscreen", use_fullscreen);
    config_lisp->get("show_fps", show_fps);
    config_lisp->get("sound_enabled", sound_enabled);
    config_lisp->get("music_enabled", music_enabled);

    // TODO read controller config
  } catch(std::exception& e) {
    std::cerr << "Couldn't open config file: " << e.what() << "\n"
              << "This is normal on first startup!\n";
    return;
  }
}

void
Config::save()
{
  try {
    lisp::Writer writer("config");

    writer.start_list("windstille-config");

    writer.write_int("screen_width", screen_width);
    writer.write_int("screen_height", screen_height);
    writer.write_bool("fullscreen", use_fullscreen);
    writer.write_bool("show_fps", show_fps);
    writer.write_bool("sound_enabled", sound_enabled);
    writer.write_bool("music_enabled", music_enabled);

    // TODO write controller config
    
    writer.end_list("windstille-config");
  } catch(std::exception& e) {
    std::cerr << "Couldn't write config file: " << e.what() << "\n";
  }
}

