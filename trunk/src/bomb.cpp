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
#include "sector.hpp"
#include "bomb.hpp"
#include "badguy/badguy.hpp"

Bomb::Bomb(int x, int y)
  : sprite("bomb", resources),
    explo("explo", resources),
    light("bomblight", resources),
    highlight("bombhighlight", resources),
    explolight("explolight", resources),
    pos(x, int(y/TILE_SIZE+1)*TILE_SIZE),
    count(2.0f),
    state(COUNTDOWN),
    exploded(false)
{
  light.set_blend_func(blend_src_alpha, blend_one);
  highlight.set_blend_func(blend_src_alpha, blend_one);
  explolight.set_blend_func(blend_src_alpha, blend_one);
}

Bomb::~Bomb()
{
}

void
Bomb::update(float delta)
{
  if (explo.is_finished())
    remove();

  if (state == EXPLODE)
    explo.update(delta);
  else
    sprite.update(delta);

  count -= delta;

  if (count < 0 && state != EXPLODE)
    {
      state = EXPLODE;
      count = 0;
      if (!exploded)
        {
          exploded = true;
          explode();
        }

    }
}

void
Bomb::draw(SceneContext& sc)
{
  if (state == EXPLODE)
    {
      explo.draw(pos.x, pos.y);
      sc.light().draw(explolight, pos.x, pos.y, 0);
      explolight.set_alpha(0.5);
      explolight.set_scale(.5, .5);
      sc.highlight().draw(explolight, pos.x, pos.y, 0);
      explolight.set_alpha(1.0);
      explolight.set_scale(1.0, 1.0);
    }
  else
    {
      sc.color().draw(sprite, pos.x, pos.y);
      if (sprite.get_current_frame() == 0) {
        sc.light().draw(light, pos.x, pos.y, 0);
        sc.highlight().draw(highlight, pos.x, pos.y, 0);
      }
    }
}

void 
Bomb::explode()
{
  if (0)
    { // FIXME: Should be handled by the collision system
      std::vector<GameObject*>* objs = Sector::current()->get_objects();
      for(std::vector<GameObject*>::iterator i = objs->begin(); i != objs->end(); ++i)
        {
          Badguy* badguy = dynamic_cast<Badguy*>(*i);
          if (badguy)
            {
              if (badguy->get_pos().x > pos.x - 30 &&
                  badguy->get_pos().x < pos.x + 30 &&
                  badguy->get_pos().y > pos.y - 20 &&
                  badguy->get_pos().y < pos.y + 20)
                badguy->die();
            }
        }
    }
}

/* EOF */
