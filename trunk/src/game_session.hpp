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

#ifndef GAME_SESSION_HXX
#define GAME_SESSION_HXX

#include <string>
#include "display/scene_context.hpp"
#include "console.hpp"
#include "squirrel/include/squirrel.h"
#include "screen.hpp"
#include "controller_help_window.hpp"
#include "pda.hpp"
#include "sprite2d/sprite.hpp"
#include "globals.hpp"

class EnergyBar;
class View;
class Sector;
class Player;
class DialogManager;
class Conversation;

class GameSession : public Screen
{
public:
  enum ControlState { DIALOG, GAME, CONVERSATION };
private:
  SceneContext sc;

  float fadeout_value;

  ControllerHelpWindow controller_help_window;

  Sector*        sector;
  View*          view;
  EnergyBar*     energy_bar;
  DialogManager* dialog_manager;
  Conversation*  conversation;

  std::string filename;

  bool pause;

  enum { NO_ACTION, QUIT_ACTION, CHANGE_SECTOR_ACTION } next_action;
  
  enum { FADEIN, RUNNING, FADEOUT } fade_state;
  ControlState control_state;

  static GameSession* current_; 

public:
  static GameSession* current() { return current_; } 

  GameSession (const std::string& arg_filename);
  ~GameSession ();

  SceneContext& get_scene_context() { return sc; }
  View* get_view() { return view; }
  PDA pda;

  void set_control_state(ControlState state) { control_state = state; }

  /** Switches the sector instantly without fadeout */
  void set_sector(const std::string& filename);

  /** Fades out then switches sectors and fades in again */
  void change_sector(const std::string& filename);

  ControlState get_game_state() const { return control_state; }
  const std::string& get_filename () const { return filename; }

  void draw();
  void draw_game();
  void update(float delta, const Controller& controller);
  void handle_event(const SDL_Event& event);

  void quit();
};

#endif

/* EOF */
