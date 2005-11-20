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

#include "input/controller.hpp"
#include "display/display.hpp"
#include "color.hpp"
#include "slider.hpp"

namespace GUI {

Slider::Slider(Component* parent)
  : Component(parent),
    min(0),
    max(100),
    step(10),
    pos (50),
    horizontal(true)
{
}

Slider::~Slider()
{
}

int
Slider::set_minimum(int min_)
{
  min = min_;
}

int
Slider::set_maximum(int max_)
{
  max = max_;
}

void
Slider::set_range(int min_, int max_)
{
  min = min_;
  max = max_;
}

void
Slider::set_step(int step_)
{
  step = step_;
}

void
Slider::draw()
{
  Display::fill_rect(rect, Color(0.0f, 0.0f, 0.0f, 0.5f));
  Display::draw_rect(rect, Color(1.0f, 1.0f, 1.0f, 0.5f));
  
  int width = 30;
  Rectf slider_rect(Vector(rect.left + (rect.get_width()-width) * (pos/float(max - min)) ,
                           rect.top + 2),
                    Sizef(width, rect.get_height()-4));
  if (is_active())
    {
      Display::fill_rect(slider_rect, Color(1.0f, 1.0f, 1.0f, 0.5f));
      Display::draw_rect(slider_rect, Color(1.0f, 1.0f, 1.0f, 1.0f));
    }
  else
    {
      Display::draw_rect(slider_rect, Color(1.0f, 1.0f, 1.0f, 0.5f));
    }
}

void
Slider::update(float delta, const Controller& controller)
{
  for(InputEventLst::const_iterator i = controller.get_events().begin(); i != controller.get_events().end(); ++i) 
    {
      if (i->type == BUTTON_EVENT && i->button.down)
        {
          if (i->button.name == OK_BUTTON)
            {
              
            }
          else if (i->button.name == CANCEL_BUTTON)           
            {
              set_active(false);
            }
        }
      else if (i->type == AXIS_EVENT)
        {
          if (i->axis.name == X_AXIS)
            {
              if (i->axis.pos < 0)
                {
                  pos -= step;
                  if (pos < min) 
                    pos = min;
                }
              else if (i->axis.pos > 0)
                {
                  pos += step;
                  if (pos > max) 
                    pos = max;
                }
            }
        }
    }
}

int
Slider::get_pos() const
{
  return pos;
}

void
Slider::set_pos(int pos_)
{
  pos = pos_;
}

} // namespace GUI

/* EOF */
