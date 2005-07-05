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

#define MAX_ENERGIE 16

Player* Player::current_ = 0;

Player::Player () :
  pos (320, 200),
  velocity (0, 0),
  light    ("hero/light", resources),

  state (WALKING),
  gun_state (GUN_READY),
  ground_state (IN_AIR)
{
  sprite = sprite3d_manager->create("3dsprites/heroken.wsprite");
  light.set_blend_func(blend_src_alpha, blend_one);

  jumping = false;
  energie = MAX_ENERGIE;
  current_ = this;

  direction = WEST;
  hit_count = 0.0f;
}

Player::~Player()
{
  delete sprite;
}

void
Player::draw (SceneContext& gc)
{
  gc.light().draw(light, pos.x, pos.y, 0);
  
  sprite->set_rot(direction == EAST);

#if 0
  if (hit_count > 0)
  {
    if (rand()%2)
      sprite->set_alpha(1.0f);
    else
      sprite->set_alpha(1.0f - hit_count);
  }
  else
    sprite->set_alpha(1.0f);
#endif

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

SubTilePos
Player::get_subtile_pos()
{
  return SubTilePos(int(pos.x/TILE_SIZE), int(pos.y/TILE_SIZE));
}

void
Player::switch_movement_state(MovementState newstate)
{
  if(state == newstate)
    return;

  switch(newstate) {
    case KILLED:
    case DEAD:
      sprite->set_action("Hang"); // TODO
      break;
    case STANDING:
      sprite->set_action("Stand");
      break;
    case SITTING:
      sprite->set_action("Ducking"); // TODO
      break;
    case TURN:
      sprite->set_action("Turn");
      break;
    case RUNNING:
      sprite->set_action("Run");
      break;
    case WALKING:
      sprite->set_action("Walk");
      break;
  }
  state = newstate;
}

void 
Player::update (float delta)
{
  controller = InputManager::get_controller();

  if (state == KILLED) 
    {
      switch (ground_state)
        {
        case IN_AIR:
          update_air(delta);
          break;
        case ON_GROUND:
          // TODO wait for kill animation to finish
          switch_movement_state(DEAD);
          break;
        }
    }
  else if (state == DEAD)
    {
#if 0
      if (controller.get_button_state(FIRE_BUTTON))
        {
          set_position(CL_Vector(258, 0));
          set_direction(EAST);
          switch_movement_state(WALKING);
          gun_state = GUN_READY;
          ground_state = IN_AIR;
          energie = MAX_ENERGIE;
          velocity = CL_Vector();
        }
#endif
    }
  else
    {
      if (hit_count > 0)
        hit_count -= delta;

      if (controller.get_button_state(LEFT_BUTTON))
        direction = WEST;
      else if  (controller.get_button_state(RIGHT_BUTTON))
        direction = EAST;

      switch(ground_state)
        {
        case ON_GROUND:
          update_ground (delta);
          if (!on_ground ())
            ground_state = IN_AIR;
          break;
        case IN_AIR:
          update_air (delta);
          break;
        }

      SubTilePos new_subtile_pos = get_subtile_pos();
      if (!(subtile_pos == new_subtile_pos))
        {
          if (get_world()->get_tilemap()->get_pixel(new_subtile_pos.x, new_subtile_pos.y))
            {
              pos.x = subtile_pos.x * TILE_SIZE;
              pos.y = subtile_pos.y * TILE_SIZE;
            }
          else
            {
              subtile_pos = new_subtile_pos;
            }
        }
    }

  sprite->update(delta);
}

void 
Player::update_ground (float delta)
{
  velocity = CL_Vector();

  if (controller.get_button_state(JUMP_BUTTON) && !jumping)
    {
      jumping = true;
      velocity.y = -750;
      ground_state = IN_AIR;
    } 
  else
    {
      if (!controller.get_button_state(JUMP_BUTTON))
        jumping = false;

      float tmp_x_pos = pos.x;

      if (controller.get_button_state(DOWN_BUTTON))
        {
          switch_movement_state(SITTING);
          if (controller.get_button_state(FIRE_BUTTON) && !bomb_placed)
            {
              Sector::current()->add(new Bomb(int(pos.x), int(pos.y)));
              bomb_placed = true;
            }
        }
      else
        {
          bomb_placed = false;
          if (controller.get_button_state(RUN_BUTTON))
            {
              if (controller.get_button_state(LEFT_BUTTON))
                {
                  pos.x -= 256 * delta;
                  switch_movement_state(RUNNING);
                }
              else if (controller.get_button_state(RIGHT_BUTTON))
                {
                  pos.x += 256 * delta;
                  switch_movement_state(RUNNING);
                }
            }
          else
            {
              if (controller.get_button_state(LEFT_BUTTON))
                {
                  pos.x -= 128 * delta;
                  switch_movement_state(WALKING);
                }
              else if (controller.get_button_state(RIGHT_BUTTON))
                {
                  pos.x += 128 * delta;
                  switch_movement_state(WALKING);
                }
              else
                {
                  switch_movement_state(STANDING);
                }
            }
          
          if (stuck ()) 
            {
              // FIXME: Calculate nearest position to colliding object here
              pos.x = tmp_x_pos;
            }
        }
    }
}

void 
Player::update_air (float delta)
{
  if (!controller.get_button_state(JUMP_BUTTON) && velocity.y < 0) 
    {
      velocity.y = velocity.y/2;
      //ground_state = IN_AIR;
    }

  float tmp_x_pos = pos.x;
  if (controller.get_button_state(LEFT_BUTTON))
    pos.x -= 300 * delta;
  else if (controller.get_button_state(RIGHT_BUTTON))
    pos.x += 300 * delta;
  if (stuck ())
    pos.x = tmp_x_pos;

  pos += velocity * delta;
  velocity.y += 1500 * delta;

  if (on_ground () && velocity.y > 0) 
    {
      ground_state = ON_GROUND;
      // Cut the position to the tile size 
      pos.y = int(pos.y / TILE_SIZE) * TILE_SIZE + TILE_SIZE - 1;
    } 
}
  
void 
Player::set_position (const CL_Vector& arg_pos)
{
  pos = arg_pos;
}

void 
Player::set_direction (Direction dir)
{
  direction = dir;
}

bool
Player::on_ground ()
{
  return get_world ()->get_tilemap()->is_ground(pos.x, pos.y+16);
}

bool 
Player::stuck ()
{
  return get_world ()->get_tilemap()->is_ground(pos.x, pos.y);
}

int
Player::get_energie()
{
  return energie;
}

int
Player::get_max_energie()
{
  return MAX_ENERGIE;
}

void
Player::hit(int points)
{
  if (energie > 0 && hit_count <= 0)
    {
      energie -= points;
      hit_count = 1.0f;

      if (energie <= 0)
        {
          switch_movement_state(KILLED);
          hit_count = 0;
        }
    }
}

/* EOF */
