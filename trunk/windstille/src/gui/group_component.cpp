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

#include <assert.h>
#include "display/display.hpp"
#include "color.hpp"
#include "font/fonts.hpp"
#include "group_component.hpp"

namespace GUI {

GroupComponent::GroupComponent(const Rectf& rect, const std::string& title_, Component* parent)
  : Component(rect, parent),
    title(title_),
    child(0)
{
  
}

GroupComponent::~GroupComponent()
{
  
}

void
GroupComponent::draw()
{
  Display::fill_rounded_rect(rect, 5.0f, Color(0.0f, 0.0f, 0.0f, 0.5f));
  Display::draw_rounded_rect(rect, 5.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));

  TTFFont* font = Fonts::vera20;
  font->draw_center(rect.left + rect.get_width()/2, rect.top + font->get_height() + 5, 
                    title, Color(1.0f, 1.0f, 1.0f));

  Display::fill_rect(Rectf(rect.left  + 8, rect.top + font->get_height() + 16,
                           rect.right - 8, rect.top + font->get_height() + 18),
                     Color(1.0f, 1.0f, 1.0f, 0.5f));

  if (child)
    child->draw();
}

void
GroupComponent::update(float delta, const Controller& controller)
{
  if (child)
    child->update(delta, controller);
}

void
GroupComponent::pack(Component* component)
{
  assert(child == 0);
  child = component;

  int padding = 6;
  child->set_screen_rect(Rectf(rect.left + padding,
                               rect.top  + padding + Fonts::vera20->get_height() + 24,
                               rect.right  - padding,
                               rect.bottom - padding
                               ));
  child->set_active(true);
}

bool
GroupComponent::is_active() const
{
  if (child)
    return child->is_active();
  else
    return false;
}

} // namespace GUI

/* EOF */
