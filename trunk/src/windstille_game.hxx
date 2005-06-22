//  $Id: windstille_game.hxx,v 1.9 2003/11/04 22:48:51 grumbel Exp $
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

#ifndef WINDSTILLEGAME_HXX
#define WINDSTILLEGAME_HXX

#include <string>
#include <ClanLib/Display/sprite.h>
#include <ClanLib/Display/font.h>
#include <ClanLib/Signals/slot_container.h>
#include "display/scene_context.hxx"
#include "screen.hxx"

class CL_InputEvent;

class Energiebar;
class View;
class Sector;
class Player;
class DialogManager;

class WindstilleGame : public Windstille::Screen
{
private:
  SceneContext sc;

  int frames;
  Player* player;

  float blink;
  float fadeout_value;

  std::string filename;
  Sector* world;
  View* view;
  Energiebar* energiebar;
  DialogManager* dialog_manager;

  enum { FADEIN, RUNNING, FADEOUT } state;
  enum { DIALOG, GAME } control_state;

  CL_Font font;
  CL_Sprite portrait;
  CL_Sprite logo;
  CL_Sprite logo_black;

  void on_mouse_up  (const CL_InputEvent& event);
  void on_mouse_down(const CL_InputEvent& event);
  void on_key_down  (const CL_InputEvent& event);

  static WindstilleGame* current_; 
public:
  static WindstilleGame* current() { return current_; } 

  WindstilleGame (const std::string& arg_filename);
  WindstilleGame (Sector* w);
  ~WindstilleGame ();

  void set_dialog_state() { control_state = DIALOG; }
  void set_game_state()   { control_state = GAME; }

  void on_startup();
  void on_shutdown();

  void draw();
  void draw_game();
  void update(float delta);

  void quit();
};

#endif

/* EOF */
