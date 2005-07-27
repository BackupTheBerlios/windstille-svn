/*  $Id: bomb.cpp 752 2005-07-25 10:00:44Z grumbel $
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
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "picture_entity.hpp"
#include "lisp/properties.hpp"
#include "windstille_getters.hpp"
#include "timer.hpp"
#include "sprite2d/manager.hpp"

PictureEntity::PictureEntity(const lisp::Lisp* lisp)
  : visible(true), target_speed(0), flash_speed(0)
{
  std::string spritename;

  lisp::Properties props(lisp);
  props.get("sprite", spritename);
  props.get("pos", pos);
  props.get("flashspeed", flash_speed);
  props.get("name", name);
  props.get("visible", visible);
  props.print_unused_warnings("picture-entity");

  if(spritename == "")
    throw std::runtime_error("No sprite name specified in PictureEntity");
  sprite = sprite2d_manager->create(spritename);
  
  flash_delta = game_time;
  target_x = pos.x;
  target_y = pos.y;
}

PictureEntity::~PictureEntity()
{
  delete sprite;
}

void
PictureEntity::draw(SceneContext& sc)
{
  if(!visible)
    return;
  
  if (flash_speed != 0)
    flash();
    
  sprite->draw(sc, pos, 10.0f);
}

void
PictureEntity::update(float delta)
{
  sprite->update(delta);
  
  if (target_speed > 0)
    move(delta);
}

void
PictureEntity::move_to(float x, float y, float speed)
{
  target_x = x;
  target_y = y;
  target_speed = speed;
}

void
PictureEntity::show(bool arg_visible)
{
  visible = arg_visible;
  flash_delta = game_time;
}

void
PictureEntity::move(float delta)
{
  float dx = target_x - pos.x;
  float dy = target_y - pos.y;
  
  if (!dx && !dy) {
    target_speed = 0;
    return;
  }
  
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
  
  float distx = target_speed * delta * propx;
  float disty = target_speed * delta * propy;
  
  if (adx - fabs(distx) < 0)
    distx = dx;
  if (ady - fabs(disty) < 0)
    disty = dy;
    
  pos.x += distx;
  pos.y += disty;
}

void
PictureEntity::flash()
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

