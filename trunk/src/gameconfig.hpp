#ifndef __GAMECONFIG_HPP__
#define __GAMECONFIG_HPP__

#include <string>

class Config
{
public:
  Config();
  ~Config();

  void load();
  void save();

  int screen_width;
  int screen_height;
  bool use_fullscreen;
  bool show_fps;

  bool sound_enabled;
  bool music_enabled;
};

extern Config* config;

#endif

