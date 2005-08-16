#include "gameconfig.hpp"

#include <memory>
#include <iostream>
#include "lisp/lisp.hpp"
#include "lisp/parser.hpp"
#include "lisp/writer.hpp"
#include "lisp/properties.hpp"

Config* config = 0;

Config::Config()
{
  // set default values
  screen_width = 800;
  screen_height = 600; 
  use_fullscreen = false;
  show_fps = false;
  sound_enabled = true;
  music_enabled = true;

  antialiasing = 0;
}

Config::~Config()
{
}

void
Config::load()
{
  using namespace lisp;
  
  try {
    std::auto_ptr<Lisp> root(lisp::Parser::parse("config"));
    Properties rootp(root.get());
    
    const Lisp* config_lisp = 0;
    if(rootp.get("windstille-config", config_lisp) == false) {
      std::cerr << "Warning: Config file is not a windstille-config file.\n";
      return;
    }

    Properties props(config_lisp);
    props.get("screen_width", screen_width);
    props.get("screen_height", screen_height);
    props.get("fullscreen", use_fullscreen);
    props.get("show_fps", show_fps);
    props.get("anti-aliasing", antialiasing);
    props.get("sound_enabled", sound_enabled);
    props.get("music_enabled", music_enabled);
    props.print_unused_warnings("configfile");

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
    writer.write_int("anti-aliasing", antialiasing);
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

