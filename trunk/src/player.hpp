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

#ifndef PLAYER_HXX
#define PLAYER_HXX

#include <ClanLib/core.h>

#include "sprite2d/sprite.hpp"
#include "input/controller.hpp"
#include "sprite3d/sprite3d.hpp"
#include "math/vector.hpp"
#include "globals.hpp"
#include "entity.hpp"
#include "ref.hpp"
#include "glutil/surface.hpp"

class Controller;

class Player : public Entity
{
private:
  Controller controller;

  Sprite   light;
  Sprite3D sprite;
  Sprite3D grenade;

  bool jumping;
  bool bomb_placed;
  float hit_count;
  int energy;

  CollisionObject* c_object;
  Slot slot;

public:
  enum State
  {
    STAND,
    WALK,
    TURNAROUND,
    DUCKING,
    DUCKED,
    ACCELERATE,
    RUN,
    SLOWDOWN,
    STAND_TO_LISTEN,
    LISTEN,
    JUMP_BEGIN,
    JUMP_AIR,
    JUMP_LAND
  };

private:
  State state;

  // jump state info
  enum Foot {
    LEFT_FOOT, RIGHT_FOOT
  };
  Foot jump_foot;
  
  double reload_time;
  static Player* current_;
public:
  Player ();
  virtual ~Player ();

  static Player* current() { return current_; }

  int get_movement_state() { return state; }

  void draw (SceneContext& gc);
  void update (float delta);

  void start_listening();
  void stop_listening();
  
  int get_energy() const;
  int get_max_energy() const;
  void hit(int points);
  
  void collision(const CollisionData& data, CollisionObject& other);

  Entity* find_useable_entity();

  virtual void set_pos(Vector pos);

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
  void set_run();
  void update_run();
  void set_jump_begin();
  void update_jump_begin();
  void set_jump_air();
  void update_jump_air();
  void set_jump_land();
  void update_jump_land();

  /**
   * Sets an action for the sprite. In contrast to sprite->set_action this
   * function will not restart the action if it was already running
   */
  void try_set_action(const std::string& name, float speed = 1.0);

  Direction get_direction() const;
};

#endif

/* EOF */
