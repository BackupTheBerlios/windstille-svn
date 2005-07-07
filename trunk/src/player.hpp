//  $Id: player.hpp,v 1.11 2003/11/05 13:36:17 grumbel Exp $
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

#ifndef PLAYER_HXX
#define PLAYER_HXX

#include <ClanLib/core.h>
#include <ClanLib/gl.h>
#include <ClanLib/display.h>

#include "input/controller.hpp"
#include "sprite3d/sprite3d.hpp"
#include "math/vector.hpp"
#include "globals.hpp"
#include "game_object.hpp"

class Controller;

class Player : public GameObject
{
private:
  Controller controller;

  /** Position as a float */
  Vector pos;
  Vector velocity;
 
  CL_Sprite light;
  Sprite3D* sprite;

  bool jumping;
  bool bomb_placed;
  float hit_count;
  int energy;
public:
  enum State
  {
    STAND,
    WALK,
    TURNAROUND,
    DUCKING,
    DUCKED,
    STAND_TO_LISTEN,
    LISTEN
  };

private:
  State state;

  double reload_time;
  static Player* current_;
public:
  Player ();
  virtual ~Player ();

  static Player* current() { return current_; }

  int get_movement_state() { return state; }

  void draw (SceneContext& gc);
  void update (float delta);

  void set_position (const CL_Vector& arg_pos);

  void start_listening();
  void stop_listening();

  CL_Vector get_pos () const { return pos; }
  
  int get_energy() const;
  int get_max_energy() const;
  void hit(int points);

  CL_Rect get_bounding_rect() const; 
private:
  void update_walk_stand();
  void set_stand();
  void update_stand();
  void set_walk(Direction direction);
  void update_walk();
  void set_ducking();
  void update_ducking();
  void set_ducked();
  void update_ducked();
  void set_turnaround();
  void update_turnaround();
  void set_stand_to_listen(bool backwards);
  void update_stand_to_listen();
  void set_listen();
  void update_listen();

  /**
   * Sets an action for the sprite. In contrast to sprite->set_action this
   * function will not restart the action if it was already running
   */
  void try_set_action(const std::string& name);

  Direction get_direction() const;
  
  // true if Player is inside a ground tile
  bool stuck () const;
  bool on_ground() const;
};

#endif

/* EOF */
