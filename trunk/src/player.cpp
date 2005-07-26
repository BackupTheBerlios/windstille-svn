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

#include "tile_map.hpp"
#include "sector.hpp"
#include "default_shoot.hpp"
#include "input/controller.hpp"
#include "input/input_manager.hpp"
#include "controller_def.hpp"
#include "player.hpp"
#include "bomb.hpp"
#include "globals.hpp"
#include "sprite3d/manager.hpp"
#include "collision/collision_engine.hpp"

static const int MAX_ENERGY = 16;
static const float WALK_SPEED = 128.0;
static const float RUN_SPEED = 256.0;

Player* Player::current_ = 0;

Player::Player () :
  light    ("hero/light", resources),
  state (STAND)
{
  pos.x = 320;
  pos.y = 200;
  pos.z = 100;
  name = "player";
  sprite = sprite3d_manager->create("3dsprites/heroken.wsprite");
  grenade = sprite3d_manager->create("3dsprites/grenade.wsprite");
  light.set_blend_func(blend_src_alpha, blend_one);

  jumping = false;
  energy = MAX_ENERGY;
  current_ = this;

  hit_count = 0.0f;
  sprite->set_action("Stand");

  // collision detection init
  CollisionObject* c_object;

  col_objects.push_back(c_object = new CollisionObject ( CL_Rectf ( -15, -120, 15, 0 )));

  c_object->set_pos(pos);
  c_object->set_velocity(velocity);
  
  slot = c_object->sig_collision().connect(this, &Player::collision);

  Sector::current()->get_collision_engine()->add(c_object);
}

Player::~Player()
{
  delete sprite;
  delete grenade;
}

void
Player::draw (SceneContext& gc)
{
  gc.light().draw(light, pos.x, pos.y, 100);
  sprite->draw(gc, pos);

  Entity* obj = find_useable_entity();
  if (obj)
    {
      gc.highlight().draw("[Talk]", obj->get_pos().x, obj->get_pos().y - 150, 1000);
    }
  
  //BoneID id = sprite->get_bone_id("Hand.R");
  //grenade->draw(gc, sprite->get_bone_matrix(id));
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
Player::update (float delta)
{
  controller = InputManager::get_controller();

  switch(state) {
    case STAND:
    case WALK:
      update_walk_stand();
      break;
    case RUN:
      update_run();
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
    case JUMP_BEGIN:
      update_jump_begin();
      break;
    case JUMP_AIR:
      update_jump_air();
      break;
    case JUMP_LAND:
      update_jump_land();
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
    velocity.y += GRAVITY * delta;
  }

  pos += velocity * delta;
  sprite->update(delta);
  grenade->update(delta);

  // FIXME: actually movement should be done through CollisionObject
  //        and this->pos should be set by getting value from the current CollisionObject
  (*col_objects.begin ())->set_pos (pos);
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
  
Entity*
Player::find_useable_entity()
{
  std::vector<GameObject*>* objects = Sector::current()->get_objects();
  for (std::vector<GameObject*>::iterator i = objects->begin(); i != objects->end(); ++i)
    {
      Entity* object = dynamic_cast<Entity*>(*i);
      if (object && object != this && object->is_useable())
        {
          //FIXME use proper collision detection
          if (object->get_pos().x > pos.x - 32 && object->get_pos().x < pos.x + 32
              && object->get_pos().y > pos.y - 128 && object->get_pos().y < pos.y + 32)
            {
              return object;
            }
        }
    }
  return 0;  
}


void
Player::update_stand()
{ 
  if(controller.button_pressed(USE_BUTTON))
    {
      Entity* obj = find_useable_entity();
      if (obj)
        obj->use();
    }

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
  
  if(controller.get_button_state(RUN_BUTTON)) {
    set_run();
    return;
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
    state = STAND;
  } else if(controller.get_button_state(DOWN_BUTTON) 
      && sprite->get_speed() < 0) {
    printf("Changespeed2.\n");
    sprite->set_speed(1.0);
    sprite->set_next_action("Ducking");
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
    sprite->set_action("StandToDuck", -1.0);
    sprite->set_next_action("Stand");
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
  printf("turn.\n");
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
  if(sprite->get_rot() && controller.button_pressed(RIGHT_BUTTON)
     || !sprite->get_rot() && controller.button_pressed(LEFT_BUTTON)) {
    sprite->set_speed(-1.0);
    sprite->set_next_action("Walk");
    state = WALK;
  }
}

void
Player::set_stand_to_listen(bool backwards)
{
  try_set_action("StandtoListen", backwards ? -1.0 : 1.0);
  if(!backwards) {
    sprite->set_next_action("Listen");
    velocity = Vector(0, 0, 0);
  } else {
    sprite->set_next_action("Stand");
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

void
Player::set_run()
{
  try_set_action("Run");
  if(get_direction() == EAST)
    velocity.x = RUN_SPEED;
  else
    velocity.x = -RUN_SPEED;  
  state = RUN;
}

void
Player::update_run()
{
  if(!controller.get_button_state(RUN_BUTTON)) {
    set_walk(get_direction());
    return;
  }
  if(controller.get_button_state(JUMP_BUTTON)) {
    set_jump_begin();
    return;
  }
}

void
Player::set_jump_begin()
{
  if(sprite->before_marker("RightFoot")) {
    sprite->set_next_action("JumpRightFoot");
    sprite->abort_at_marker("RightFoot");
    printf("jumpright.\n");
    jump_foot = LEFT_FOOT;
  } else if(sprite->before_marker("LeftFoot")) {
    sprite->set_next_action("JumpLeftFoot");
    sprite->abort_at_marker("LeftFoot");
    printf("jumpleft.\n");
    jump_foot = RIGHT_FOOT;
  } else {
    sprite->set_next_action("JumpRightFoot");
    sprite->abort_at_marker("RightFoot");
    printf("jumpright.\n");
    jump_foot = LEFT_FOOT;
  }
  state = JUMP_BEGIN;
}

void
Player::update_jump_begin()
{
  if(sprite->switched_actions()) {
    if(sprite->get_action() == "JumpLeftFoot") {
      sprite->set_next_action("JumpLeftFootAir");
    } else if(sprite->get_action() == "JumpRightFoot") {
      sprite->set_next_action("JumpRightFootAir");
    } else {
      set_jump_air();
      return;
    }
  }
}

void
Player::set_jump_air()
{
  velocity.y = -400;
  sprite->set_next_action("JumpLandSofttoRun");
  state = JUMP_AIR;
}

void
Player::update_jump_air()
{
  if(sprite->switched_actions()) {
    set_jump_land();
    return;
  }
}

void
Player::set_jump_land()
{
  sprite->set_next_action("Run");
  state = JUMP_LAND;
}

void
Player::update_jump_land()
{
  if(sprite->switched_actions()) {
    set_run();
    return;
  }
}

Direction
Player::get_direction() const
{
  return sprite->get_rot() ? EAST : WEST;
}

void
Player::try_set_action(const std::string& name, float speed)
{
  if(sprite->get_action() == name)
    return;
  
  sprite->set_action(name, speed);
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
      //hit_count = 1.0f;

      if (energy <= 0)
        {
          //switch_movement_state(KILLED);
          hit_count = 0;
        }
    }
}

void
Player::collision(const CollisionData& data, CollisionObject& other)
{
  (void) data;
  (void) other;
}

/* EOF */
