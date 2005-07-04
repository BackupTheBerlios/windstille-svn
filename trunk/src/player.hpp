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
#include "globals.hpp"
#include "game_object.hpp"

class Controller;

/** A position in units of subtiles */
struct SubTilePos {
  SubTilePos() : x(0), y(0) {}

  SubTilePos(int x_, int y_) 
    : x(x_), y(y_)
  {}

  bool operator==(const SubTilePos& pos) {
    return pos.x == x && pos.y == y;
  }

  int x;
  int y;
};

class Player : public GameObject
{
private:
  Controller controller;

  /** Position as a float */
  CL_Vector pos;
  
  SubTilePos subtile_pos;

  /** X-Position in subtile coordinates */
  int x_pos;
  /** Y-Position in subtile coordinates */
  int y_pos;
  
  CL_Vector velocity;
 
#if 0
  CL_Sprite walk;
  CL_Sprite run;
  CL_Sprite turn;
  CL_Sprite sit;
  CL_Sprite jump;
  CL_Sprite stand;
  CL_Sprite killed;
  CL_Sprite dead;
#endif
  CL_Sprite light;

#if 0
  CL_Sprite roll;
  CL_Sprite surround;
#endif
  Sprite3D* sprite;

  bool jumping;
  bool bomb_placed;
  float hit_count;
  int energie;
public:
  typedef enum { WALKING, RUNNING, TURN, SITTING, STANDING, KILLED, DEAD } MovementState;
  typedef enum { GUN_READY, GUN_RELOADING } GunState;
  typedef enum { ON_GROUND, IN_AIR } GroundState;

  //{ BOUNCE, SPREAD, LASER }
private:
  MovementState state;
  GunState gun_state;
  Direction direction;
  GroundState ground_state;

  void switch_movement_state(MovementState state);
  
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
  void set_direction (Direction dir);

  CL_Vector get_pos () const { return pos; }
  SubTilePos get_subtile_pos();
  
  int get_energie();
  int get_max_energie();
  void hit(int points);

  CL_Rect get_bounding_rect() const; 
private:
  // true if the tile under Player is ground
  bool on_ground ();
  // true if Player is inside a ground tile
  bool stuck ();

  void update_ground (float delta);
  void update_air (float delta);
};

#endif

/* EOF */
