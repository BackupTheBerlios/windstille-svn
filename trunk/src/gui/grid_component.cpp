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
#include "display/display.hpp"
#include "input/controller.hpp"
#include "grid_component.hpp"

namespace GUI {

GridComponent::GridComponent(const Rectf& rect, int weight, int height, Component* parent)
  : Component(rect, parent),
    grid(weight, height),
    child_active(false) 
{
}

GridComponent::~GridComponent()
{
}

void
GridComponent::draw()
{
  Display::fill_rect(rect, Color(0.0f, 0.0f, 0.0f, 0.5f));
  Display::draw_rect(rect, Color(1.0f, 1.0f, 1.0f, 0.5f));

  for(int y = 0; y < grid.get_height(); ++y)
    for(int x = 0; x < grid.get_width(); ++x)
      {
        if (grid(x, y))
          {
            if (x == pos.x && y == pos.y)
              Display::fill_rect(grid(x, y)->get_screen_rect(), Color(1.0f, 1.0f, 1.0f, 0.5f));
            grid(x, y)->draw();
          }
      }
}

void
GridComponent::update(float delta, const Controller& controller)
{
  for(int y = 0; y < grid.get_height(); ++y)
    for(int x = 0; x < grid.get_width(); ++x)
      {
        if (grid(x, y)) 
          {
            if (child_active && pos.x == x && pos.y == y)
              grid(x, y)->update(delta, controller);
            else
              grid(x, y)->update(delta, Controller());
          }
      }

  for(InputEventLst::const_iterator i = controller.get_events().begin(); i != controller.get_events().end(); ++i) 
    {
      if (i->type == BUTTON_EVENT)
        {
          if (i->button.name == OK_BUTTON)
            {
              child_active = true;
            }
          else if (i->button.name == CANCEL_BUTTON)
            {
              
            }
        }
      else if (i->type == AXIS_EVENT)
        {
          if (i->axis.name == X_AXIS)
            {
              if (i->axis.pos < 0)
                {
                  pos.x -= 1;
                  if (pos.x < 0)
                    pos.x = grid.get_width()-1;
                }
              else if (i->axis.pos > 0)
                {
                  pos.x += 1;
                  if (pos.x >= grid.get_width())
                    pos.x = 0;
                }
            }
          else if (i->axis.name == Y_AXIS)
            {
              if (i->axis.pos < 0)
                {
                  pos.y -= 1;
                  if (pos.y < 0)
                    pos.y = grid.get_height()-1;
                }
              else if (i->axis.pos > 0)
                {
                  pos.y += 1;
                  if (pos.y >= grid.get_height())
                    pos.y = 0;
                }
            }            
        }
    }
}

void
GridComponent::pack(Component* component, int x, int y)
{
  assert(x >= 0);
  assert(y >= 0);
  assert(x < grid.get_width());
  assert(y < grid.get_height());

  if (grid(x, y) != 0)
    {
      std::cout << "Warning component already at: " << x << ", " << y << ", ignoring" << std::endl;
    }
  else
    {
      Rectf rect = get_screen_rect();
      grid(x, y) = component;

      float padding = 15.0f;

      component->set_screen_rect(Rectf(Vector(rect.left + x * (rect.get_width()/grid.get_width()) + padding,
                                              rect.top  + y * (rect.get_height()/grid.get_height()) + padding),
                                       Sizef(rect.get_width()/grid.get_width() - 2*padding,
                                             rect.get_height()/grid.get_height() - 2*padding)));
    }
}

} // namespace GUI

/* EOF */
