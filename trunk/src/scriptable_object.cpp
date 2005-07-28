/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <config.h>

#include "globals.hpp"
#include "scriptable_object.hpp"
#include "lisp/properties.hpp"
#include "windstille_getters.hpp"
#include "sprite2d/manager.hpp"
#include "script_manager.hpp"

ScriptableObject::ScriptableObject(const lisp::Lisp* lisp)
  : sprite(0),
    highlight(0),
    target_speed(0),
    acceleration(0),
    flash_speed(0)
{ 
  std::string spritename;
  std::string highlightname;
  
  lisp::Properties props(lisp);
  props.get("name", name);
  props.get("sprite", spritename);
  props.get("highlight", highlightname);
  props.get("pos", pos);
  props.get("script", use_script);
  props.get("use-verb", use_verb);
  props.get("active", active);
  props.print_unused_warnings("scriptable-object");
  
  if (use_verb != "")
    set_useable(true);
  
  if(spritename == "")
    throw std::runtime_error("No sprite name specified in ScriptableObject");
  sprite = sprite2d_manager->create(spritename);
  
  if (highlightname != "")
    highlight = sprite2d_manager->create(highlightname);
    
  flash_delta = game_time;
  target_x = pos.x;
  target_y = pos.y;
}

ScriptableObject::~ScriptableObject()
{
  if (sprite)
    delete sprite;
  if (highlight)
    delete highlight;
}

void
ScriptableObject::draw(SceneContext& sc)
{
  if (flash_speed != 0)
    flash();
 
  sprite->draw(sc, pos, 1);
  
  if (highlight) {
    highlight->draw(sc, pos, 2);
    highlight->draw_light(sc, pos, 1);
  }
}

void
ScriptableObject::update(float delta)
{
  sprite->update(delta);
  if (highlight)
    highlight->update(delta);
    
  if (target_speed > 0)
    move(delta);
}

void
ScriptableObject::use()
{
  script_manager->run_script(use_script, "ScriptableObject");
}

void
ScriptableObject::move_to(float x, float y, float arg_target_speed, float arg_acceleration)
{
  target_x = x;
  target_y = y;
  target_speed = arg_target_speed;
  acceleration = arg_acceleration;
}

void
ScriptableObject::start_flash(float speed)
{
  active = true;
  flash_speed = speed;
  flash_delta = game_time;
}

void
ScriptableObject::move(float delta)
{
  float dx = target_x - pos.x;
  float dy = target_y - pos.y;
  
  if (!dx && !dy) {
    target_speed = 0;
    return;
  }
  
  //Work out proportion of x/y in direction vector
  float propx = 0;
  float propy = 0;
  
  float adx = fabs(dx);
  float ady = fabs(dy);
  
  //we can't divide by 0 because we return above if !dx and !dy
  if (dx < 0)
    propx = -(adx / (adx + ady));
  else
    propx = adx / (adx + ady);
  
  if (dy < 0)
    propy = -(ady / (adx + ady));
  else
    propy = ady / (adx + ady);
  
  //accelerate
  float dvx = acceleration * delta * propx;
  float dvy = acceleration * delta * propy;  
  velocity.x += dvx;
  velocity.y += dvy;
    
  //don't accelerate past max speed
  float max_vx = fabs(target_speed * propx);
  float max_vy = fabs(target_speed * propy);
  
  if (velocity.x > max_vx)
    velocity.x = max_vx;
  else if (velocity.x < -max_vx)
    velocity.x = -max_vx;
  
  if (velocity.y > max_vy)
    velocity.y = max_vy;
  else if (velocity.y < -max_vy)
    velocity.y = -max_vy;
  
  //don't move past the target
  float distx = velocity.x * delta;
  float disty = velocity.y * delta;
  
  if (adx - fabs(distx) < 0)
    distx = dx;
  if (ady - fabs(disty) < 0)
    disty = dy;
    
  pos.x += distx;
  pos.y += disty;
}

void
ScriptableObject::flash()
{
  float time = game_time - flash_delta;
  if(static_cast<int>(time/flash_speed) % 2 == 0) {    
    float alpha = fmodf(time, flash_speed) / flash_speed;
    // fade on
    sprite->set_alpha(alpha);
  } else {
    float alpha = 1.0 - (fmodf(time, flash_speed) / flash_speed);
    // fade off
    sprite->set_alpha(alpha);
  }
}

/* EOF */
