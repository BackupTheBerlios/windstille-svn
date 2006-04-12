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

#include <iostream>
#include "math.hpp"
#include "input/controller.hpp"
#include "sprite2dview.hpp"

Sprite2DView::Sprite2DView()
{
  index = 0;

  directory.push_back("images/test/mech1.png");
  directory.push_back("images/test/mech2.png");
  directory.push_back("images/test/mech3.png");

  sprite      = Sprite(directory.back());
  next_sprite = Sprite(directory.front());
  offset = 0.0f;

  mode = MANUAL; 
  //mode = SLIDESHOW;
  zoom = 1.0f;
  pos  = Vector(0,0);
}

Sprite2DView::~Sprite2DView()
{
  
}

void
Sprite2DView::draw()
{ 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  switch(mode)
    {
    case SLIDESHOW:
      if (aspect > 4.0/3.0)
        {
          sprite.draw(Vector(-offset, 0));
        }
      else
        {
          sprite.draw(Vector(0, -offset));
        } 
      break;
      
    case MANUAL:
      sprite.set_scale(zoom);
      sprite.draw(pos);
      break;
    }
}

void
Sprite2DView::update_slideshow(float delta, const Controller& controller)
{
  offset += delta * 50.0f;

  width  = sprite.get_width();
  height = sprite.get_height();
  aspect = width/height;

  if (aspect > 4.0/3.0)
    { // expand vertical
      float scale = 600.0f/height;
      width  *= scale;
      height *= scale;
      sprite.set_scale(scale);

      if (offset - (width - 800) > 0)
        {
          offset = 0;

          if (++index >= int(directory.size()))
            index = 0;

          sprite = next_sprite;
          next_sprite = Sprite(directory[index]);
        }
    }
  else
    { // expand horizontal
      float scale = 800.0f/width;
      width  *= scale;
      height *= scale;
      sprite.set_scale(scale);

      if (offset - (height - 600) > 0)
        {
          offset = 0;

          if (++index >= int(directory.size()))
            index = 0;

          sprite = next_sprite;
          next_sprite = Sprite(directory[index]);
        }
    }
}

void
Sprite2DView::update_manual(float delta, const Controller& controller)
{
  pos.x += controller.get_axis_state(X_AXIS) * 100.0f * delta;
  pos.y += controller.get_axis_state(Y_AXIS) * 100.0f * delta;
  
  if (controller.get_button_state(PRIMARY_BUTTON))
    zoom *= 1.0f + 0.3f * delta;

  if (controller.get_button_state(SECONDARY_BUTTON))
    zoom /= 1.0f + 0.3f * delta;

  sprite.update(delta);
}

void
Sprite2DView::update(float delta, const Controller& controller)
{  
  switch(mode) {
  case SLIDESHOW:
    update_slideshow(delta, controller);
    break;
  case MANUAL:
    update_manual(delta, controller);
    break;
  }
}

void
Sprite2DView::set_sprite(const std::string& filename)
{
  sprite = Sprite(filename);
}

/* EOF */
