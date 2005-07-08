//  $Id: player.cxx,v 1.19 2003/11/05 13:36:17 grumbel Exp $
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

#include "tile_map.hpp"
#include "sector.hpp"
#include "default_shoot.hpp"
#include "input/controller.hpp"
#include "input/input_manager.hpp"
#include "controller_def.hpp"
#include "player.hpp"
#include "bomb.hpp"
#include "globals.hpp"
#include "sprite3d/sprite3d_manager.hpp"

static const int MAX_ENERGY = 16;
static const float WALK_SPEED = 128.0;
static const float GRAVITY = 1500;

Player* Player::current_ = 0;

Player::Player () :
  pos (320, 200),
  velocity (0, 0),
  light    ("hero/light", resources),
  state (STAND)
{
  name = "Player";
  sprite = sprite3d_manager->create("3dsprites/heroken.wsprite");
  light.set_blend_func(blend_src_alpha, blend_one);

  jumping = false;
  energy = MAX_ENERGY;
  current_ = this;

  hit_count = 0.0f;
  sprite->set_action("Stand");
}

Player::~Player()
{
  delete sprite;
}

void
Player::draw (SceneContext& gc)
{
  gc.light().draw(light, pos.x, pos.y, 0);
  
  sprite->draw(gc, pos);
}

CL_Rect
Player::get_bounding_rect() const
{
  return CL_Rect(int(pos.x - 20), 
                 int(pos.y - 112),
                 int(pos.x + 20), 
                 int(pos.y));
}

void
Player::start_listening()
{
  if(state == STAND_TO_LISTEN || state == LISTEN)
    return;

  set_stand_to_listen(false);
}

void
Player::stop_listening()
{
  if(state != LISTEN && state != STAND_TO_LISTEN)
    return;
  
  set_stand_to_listen(true);
}

void 
Player::update (float elapsed_time)
{
  controller = InputManager::get_controller();

  switch(state) {
    case STAND:
    case WALK:
      update_walk_stand();
      break;
    case DUCKING:
      update_ducking();
      break;
    case DUCKED:
      update_ducked();
      break;
    case TURNAROUND:
      update_turnaround();
      break;
    case STAND_TO_LISTEN:
      update_stand_to_listen();
      break;
    case LISTEN:
      update_listen();
      break;
    default:
      assert(false);
      break;
  }

  // fall down
  if(on_ground()) {
    if(velocity.y > 0) {
      velocity.y = 0;
      pos.y = int(pos.y / TILE_SIZE) * TILE_SIZE + TILE_SIZE - 1;
    }
  } else {
    velocity.y += GRAVITY * elapsed_time;
  }

  pos += velocity * elapsed_time;
  sprite->update(elapsed_time);
}

void
Player::set_stand()
{
  try_set_action("Stand");
  velocity = Vector(0, 0, 0);
  state = STAND;
  printf("stand.\n");
}

void
Player::update_walk_stand()
{
  if(controller.get_button_state(DOWN_BUTTON)) {
    set_ducking();
    return;
  }

  if(state == STAND)
    update_stand();
  else
    update_walk();
}
  

void
Player::update_stand()
{
  if(controller.get_button_state(LEFT_BUTTON)
      && !controller.get_button_state(RIGHT_BUTTON)) {
    if(get_direction() == WEST)
      set_walk(WEST);
    else
      set_turnaround();
  } else if(controller.get_button_state(RIGHT_BUTTON)
      && !controller.get_button_state(LEFT_BUTTON)) {
    if(get_direction() == EAST)
      set_walk(EAST);
    else
      set_turnaround();
  }
}

void
Player::set_walk(Direction direction)
{
  try_set_action("Walk");
  sprite->set_rot(direction == EAST);
  state = WALK;
  if(direction == EAST)
    velocity.x = WALK_SPEED;
  else
    velocity.x = -WALK_SPEED;
  printf("walk.\n");
}

void
Player::update_walk()
{
  if(controller.get_button_state(LEFT_BUTTON)
      == controller.get_button_state(RIGHT_BUTTON)) {
    set_stand();
    return;
  }

  if(get_direction() == WEST && controller.get_button_state(RIGHT_BUTTON)
     || get_direction() == EAST && controller.get_button_state(LEFT_BUTTON)) {
    set_turnaround();
    return;
  }
  
  // test
  if(controller.get_button_state(JUMP_BUTTON)) {
    sprite->set_speed(-1.0);
  } else {
    sprite->set_speed(1.0);
  }
}

void
Player::set_ducking()
{
  try_set_action("StandToDuck");
  sprite->set_next_action("Ducking");
  state = DUCKING;
  velocity.x = 0;
  printf("ducking.\n");
}

void
Player::update_ducking()
{
  // ducking
  if(sprite->switched_actions()) {
    printf("finished.\n");
    if(sprite->get_action() == "Ducking")
      set_ducked();
    else
      set_stand();
    return;
  }
  
  if(!controller.get_button_state(DOWN_BUTTON) && sprite->get_speed() > 0) {
    printf("Changespeed1.\n");
    sprite->set_speed(-1.0);
    sprite->set_next_action("Stand");
    sprite->set_next_speed(1.0);
  } else if(controller.get_button_state(DOWN_BUTTON) 
      && sprite->get_speed() < 0) {
    printf("Changespeed2.\n");
    sprite->set_speed(1.0);
    sprite->set_next_action("Ducking");
    sprite->set_next_speed(1.0);
  }
}

void
Player::set_ducked()
{
  assert(sprite->get_action() == "Ducking");
  printf("ducked.\n");
  state = DUCKED;
}

void
Player::update_ducked()
{
  if(!controller.get_button_state(DOWN_BUTTON)) {
    printf("ducking.\n");
    state = DUCKING;
    sprite->set_action("StandToDuck");
    sprite->set_next_action("Stand");
    sprite->set_next_speed(1.0);
    sprite->set_speed(-1.0);
  }  
}

void
Player::set_turnaround()
{
  velocity.x = 0;
  try_set_action("Turn");
  sprite->set_next_action("Walk");
  sprite->set_next_rot(! sprite->get_rot());
  state = TURNAROUND;
}

void
Player::update_turnaround()
{
  if(sprite->switched_actions()) {
    if(sprite->get_rot()) {
      set_walk(EAST);
    } else {
      set_walk(WEST);
    }
  } 
  if(sprite->get_rot() && controller.get_button_state(RIGHT_BUTTON)
     || !sprite->get_rot() && controller.get_button_state(LEFT_BUTTON)) {
    sprite->set_speed(-1.0);
    sprite->set_next_action("Walk");
  }
}

void
Player::set_stand_to_listen(bool backwards)
{
  try_set_action("StandtoListen");
  sprite->set_next_speed(1.0);
  if(!backwards) {
    sprite->set_next_action("Listen");
  } else {
    sprite->set_next_action("Stand");
    sprite->set_speed(-1.0);
  }
  state = STAND_TO_LISTEN;
}

void
Player::update_stand_to_listen()
{
  if(sprite->switched_actions()) {
    if(sprite->get_action() == "Stand")
      set_stand();
    else
      set_listen();
  }
}

void
Player::set_listen()
{
  try_set_action("Listen");
  state = LISTEN;
}

void
Player::update_listen()
{
  // nothing
}

Direction
Player::get_direction() const
{
  return sprite->get_rot() ? EAST : WEST;
}

void
Player::try_set_action(const std::string& name)
{
  if(sprite->get_action() == name)
    return;
  
  sprite->set_action(name);
}

void 
Player::set_position (const CL_Vector& arg_pos)
{
  pos = arg_pos;
}

bool 
Player::stuck () const
{
  return get_world ()->get_tilemap()->is_ground(pos.x, pos.y);
}

bool
Player::on_ground() const
{
  return get_world ()->get_tilemap()->is_ground(pos.x, pos.y+16);
}

int
Player::get_energy() const
{
  return energy;
}

int
Player::get_max_energy() const
{
  return MAX_ENERGY;
}

void
Player::hit(int points)
{
  if (energy > 0 && hit_count <= 0)
    {
      energy -= points;
      hit_count = 1.0f;

      if (energy <= 0)
        {
          //switch_movement_state(KILLED);
          hit_count = 0;
        }
    }
}

/* EOF */
