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

#ifndef WINDSTILLEMAIN_HPP
#define WINDSTILLEMAIN_HPP

#include <ClanLib/gl.h>
#include <ClanLib/application.h>

#include "screen.hpp"

class WindstilleMain : public CL_ClanApplication
{
public:
  std::string levelfile;
  std::string controller_file;
  std::string recorder_file;
  std::string playback_file;
  std::string screenshot_dir;
  
  CL_DisplayWindow* window;
public:
  WindstilleMain();
  ~WindstilleMain();

  virtual char* get_title() { return "Windstille"; }
  virtual int main(int argc, char** argv);

private:
  void init_sdl();
  void init_physfs(const char* argv0);
  void parse_command_line(int argc, char** argv);
  void init_modules();
  void deinit_modules();
  bool game_main();

  Windstille::Screen* screen;
};

extern WindstilleMain main_app;

#endif

/* EOF */
