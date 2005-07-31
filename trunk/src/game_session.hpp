//  $Id$
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

#ifndef GAME_SESSION_HXX
#define GAME_SESSION_HXX

#include <string>
#include <ClanLib/Display/sprite.h>
#include <ClanLib/Display/font.h>
#include <ClanLib/Signals/slot_container.h>
#include "display/scene_context.hpp"
#include "console.hpp"
#include "squirrel/include/squirrel.h"
#include "screen.hpp"
#include "sprite2d/sprite.hpp"
#include "globals.hpp"

class CL_InputEvent;

class EnergyBar;
class View;
class Sector;
class Player;
class DialogManager;
class Conversation;

class GameSession : public Windstille::Screen
{
private:
  SceneContext sc;

  float fadeout_value;

  Sprite control_dialog;

  std::string filename;
  Sector* sector;
  View* view;
  EnergyBar* energy_bar;
  DialogManager* dialog_manager;
  Conversation*  conversation;

  enum { FADEIN, RUNNING, FADEOUT } fade_state;
  enum { DIALOG, GAME } control_state;
  GameMainState target_state;

  std::vector<CL_Slot> slots;
  void on_key_down  (const CL_InputEvent& event);
  void on_mouse_down  (const CL_InputEvent& event);
  void change_sector ();

  static GameSession* current_; 
public:
  static GameSession* current() { return current_; } 

  GameSession (const std::string& arg_filename);
  ~GameSession ();

  void set_dialog_state() { control_state = DIALOG; }
  void set_game_state()   { control_state = GAME; }
  void set_sector(const std::string& arg_filename);
  const std::string& get_filename () const { return filename; }

  void draw();
  void draw_game();
  void update(float delta);

  void quit();
};

#endif

/* EOF */
