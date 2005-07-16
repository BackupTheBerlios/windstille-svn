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

#include "spider_mine.hpp"
#include "sector.hpp"
#include "game_session.hpp"
#include "scripting/wrapper.interface.hpp"
#include "dialog_manager.hpp"
#include "script_manager.hpp"
#include "physfs/physfs_stream.hpp"
#include "console.hpp"
#include "sprite3d/sprite3d_manager.hpp"
#include "util.hpp"
#include "lisp/list_iterator.hpp"
#include "player.hpp"
#include "tile_map.hpp"

#include <exception>

// !line72 - shouldn't this be a global constant ?
static const float GRAVITY = 1500;

SpiderMine::SpiderMine(const lisp::Lisp* lisp)
  : spider_mine("spider_mine", resources),
    explode("explo", resources),
    explode_light("explolight", resources)
{
  pos.z = 100;
  lisp::ListIterator iter(lisp);
  while(iter.next()) {
    if(iter.item() == "name") {
      name = iter.value().get_string();
    } else if(iter.item() == "x") {
      pos.x = iter.value().get_float();
    } else if(iter.item() == "y") {
      pos.y = iter.value().get_float();
    } else {
      std::cerr << "Skipping unknown attribute '" 
                << iter.item() << "' in SpiderMine\n";
    }
  }

  spider_mine.set_scale(.5, .5);
  initial_position = pos;
  walk_speed = 160;
  exploded = false;
  state = WAIT;
  jump_time = 0.0;
}

SpiderMine::~SpiderMine()
{
}

void
SpiderMine::update(float delta)
{   
  search_for_player(delta);

  if (state == EXPLODE) {
    explode.update(delta);
    if (explode.is_finished())
      remove();
  }
}

void
SpiderMine::draw (SceneContext& gc)
{
  if (state == EXPLODE) {
    explode.draw(pos.x, pos.y);
    gc.light().draw(explode_light, pos.y, pos.y, 0);
    explode_light.set_alpha(0.5);
    explode_light.set_scale(.5, .5);
    gc.highlight().draw(explode_light, pos.x, pos.y, 0);
    explode_light.set_alpha(1.0);
    explode_light.set_scale(1.0, 1.0);
  }
  else {
    gc.color().draw(spider_mine, pos.x, pos.y, 2);
  }
}

void
SpiderMine::collision(const CollisionData& data, CollisionObject& other)
{
  // !line72 - does this not get called yet ?

  (void) data;
  (void) other;
}

void
SpiderMine::search_for_player(float delta)
{
  Player *player = Player::current();
  if (!player)
    return;

  jump_time += delta;

  // !line72 - I should be using the real collision stuff for this
  CL_Vector position = player->get_pos();
  if (state != EXPLODE) { // if we are in explode we are done
    if (state == JUMP && jump_time > 0.3) { // check for explosion
      state = EXPLODE;
      velocity.x = 0;
      velocity.y = 0;
      // if close enough to the player hit them
      if ((fabs(position.x - get_pos().x) < 15) &&
	  ((get_pos().y - 32 - 15 < position.y) &&
	   (get_pos().y > position.y - 135 + 15))) {
	player->hit(1); // lower the players energy
      }
    }
    else if (state != JUMP &&
	     (fabs(position.x - get_pos().x) <= 45) && // check to see if we should jump
	     ((get_pos().y - 32 < position.y) &&
	      (get_pos().y > position.y - 135))) {
      if (state != JUMP || on_ground()) {
	state = JUMP;
	velocity.y = -400;
	jump_time = 0.0;
      }
    }
    else if (state != JUMP &&
	     fabs(position.x - get_pos().x) < 200 && // check to see if we should attack the player
	     fabs(position.x - get_pos().x) > 45) {
      state = ATTACK;
      if (position.x < get_pos().x) {
	if (on_ground())
	  velocity.x = -walk_speed;
      }
      else {
	if (on_ground())
	  velocity.x = walk_speed;
      }
    }
    else if (state != JUMP &&
	     fabs(position.x - get_pos().x) >= 200 && // check to see if we should go back to starting position (ignores y)
	     fabs(get_pos().x - initial_position.x) > 15) {
      // go back to start position
      state = RETURN;
      if (initial_position.x < get_pos().x) {
	if (on_ground())
	  velocity.x = -walk_speed;
      }
      else {
	if (on_ground())
	  velocity.x = walk_speed;
      }
    } // wait
    else if (state != JUMP) {
      state = WAIT;
      velocity = 0;
    }
  }

  if (state != EXPLODE) { // move if we haven't exploded
    if(on_ground()) {
      if(velocity.y > 0) {
	velocity.y = 0;
	pos.y = int(pos.y / TILE_SIZE) * TILE_SIZE + TILE_SIZE - 1;
      }
    } else {
      velocity.y += GRAVITY * delta;
    }
    pos += velocity * delta;
  }
}

bool
SpiderMine::on_ground() const
{
  return get_world ()->get_tilemap()->is_ground(pos.x, pos.y+16);
}


/* EOF */
