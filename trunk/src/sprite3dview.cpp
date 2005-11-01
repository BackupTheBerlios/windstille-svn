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
#include "gameconfig.hpp"
#include "input/controller.hpp"
#include "sprite3dview.hpp"

Sprite3DView::Sprite3DView()
{
  current_action = 0;
  //sprite = Sprite3D("models/characters/bob/bob.wsprite");
  sprite = Sprite3D("models/characters/jane/jane.wsprite");
  actions = sprite.get_actions();

  for(std::vector<std::string>::iterator i = actions.begin(); i != actions.end(); ++i)
    {
      std::cout << "Action: " << *i << std::endl;
    }
  
  sprite.set_action(actions[current_action]);
}

Sprite3DView::~Sprite3DView()
{
}

void
Sprite3DView::draw()
{
  sc.reset_modelview();
  sc.translate(-config->screen_width/2, -config->screen_height/2 + 150);
  sc.scale(2.0f, 2.0f);
  
  sc.color().fill_screen(Color(0.5, 0.0, 0.5));
  sprite.draw(sc.color(), Vector(config->screen_width/2, config->screen_height/2), 0); 
  sc.light().fill_screen(Color(1.0, 1.0, 1.0));
  //sc.color().draw("Hello World", 100, 100);
  sc.render();
}

void
Sprite3DView::update(float delta, const Controller& controller)
{
  sprite.update(delta);
  //std::cout << "Delta: " << delta << std::endl;

  int last_action = current_action;
  if (controller.axis_was_pressed_up(Y_AXIS))
    {
      if (current_action == int(actions.size())-1)
        current_action = 0;
      else
        current_action += 1;
    }
  else if (controller.axis_was_pressed_down(Y_AXIS))
    {
      if (current_action == 0)
        current_action = actions.size()-1;
      else
        current_action -= 1;
    }

  if (last_action != current_action)
    {
      std::cout << "Setting actions: " << actions[current_action] << std::endl;
      sprite.set_action(actions[current_action]);
    }
}

/* EOF */
