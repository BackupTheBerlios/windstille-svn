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
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "globals.hpp"
#include "player.hpp"
#include "sector.hpp"
#include "hedgehog.hpp"
#include "lisp/properties.hpp"
#include "windstille_getters.hpp"

Hedgehog::Hedgehog(const lisp::Lisp* lisp)
  : sprite("hedgehog", resources),
    die_sprite("hedgehog_die", resources),
    light("hedgehog_light", resources),
    highlight("hedgehog_highlight", resources)
{
  lisp::Properties props(lisp);
  props.get("name", name);
  props.get("pos", pos);
  props.print_unused_warnings("hedgehog");
  
  direction_left = false;
  state = WALKING;
  light.set_blend_func(blend_src_alpha, blend_one);
  highlight.set_blend_func(blend_src_alpha, blend_one);
}

Hedgehog::~Hedgehog()
{
}

void
Hedgehog::draw(SceneContext& gc)
{
  CL_Sprite* s;

  if (state == DYING)
    s = &die_sprite;
  else
    s = &sprite;

  if (direction_left)
    s->set_scale(1.0f, 1.0f);
  else
    s->set_scale(-1.0f, 1.0f);

  gc.color().draw(*s, pos.x, pos.y, 2);
  //s->draw(int(pos.x), int(pos.y));
  gc.light().draw(light, pos.x, pos.y, 0);
  gc.highlight().draw(highlight, pos.x, pos.y, 0);
}

void
Hedgehog::update(float delta)
{
  if (state == DYING)
    {
      if (die_sprite.is_finished())
        remove();
      die_sprite.update(delta);
    }
  else
    {
      sprite.update(delta);
      
      CL_Vector old_pos = pos;
      
      if (on_ground())
        {
          pos.y = int(pos.y)/TILE_SIZE * TILE_SIZE;
          
          if (direction_left)
            pos.x -= 32 * delta;
          else
            pos.x += 32 * delta;      
          
          if (!on_ground() || in_wall())
            {
              direction_left = !direction_left;
              pos = old_pos;
            }
        }
      else
        { // Fall
          pos.y += 450 * delta;
        }

      // Check if the player got hit
      // FIXME: Insert pixel perfect collision detection here
      CL_Vector player_pos = Player::current()->get_pos();
      if (pos.x - 20 < player_pos.x
          && pos.x + 20 > player_pos.x
          && pos.y - 20 < player_pos.y
          && pos.y + 5  > player_pos.y)
        Player::current()->hit(5);
    }
}

void
Hedgehog::die()
{
  state = DYING;
}

/* EOF */
