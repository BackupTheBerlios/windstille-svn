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

#include "input/input_manager.hpp"
#include "display/display.hpp"
#include "text_area.hpp"
#include "fonts.hpp"
#include "pda.hpp"

PDA::PDA()
  : visible(false)
{ 
  text_area = 0;
}

void
PDA::draw()
{
  if (!visible)
    return;
    
  const Rectf& rect = text_area->get_rect().grow(8.0f);

  Display::fill_rounded_rect(rect, 16.0f, Color(0.3f, 0.3f, 0.5f, 0.5f));
  Display::draw_rounded_rect(rect, 16.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));
  text_area->draw();
}

void
PDA::update(float delta)
{
  int width  = 200;
  int height = 120;
  std::string text;
  text = "<large>PDA</large>\n"
         "------------------------\n";

  for (unsigned i = 0; i != messages.size(); ++i)
    text += messages[i] + '\n';
  
  if (text != old_text)
  {
    delete text_area;
    text_area = new TextArea(Rect(Point(16,
                Display::get_height() - height - 16),
                Size(width, height)));
    text_area->set_font(Fonts::ttffont);
    text_area->set_text(text);
    
    old_text = text;
  }
  
  text_area->update(delta);
}

void
PDA::add_dialog(const std::string& text)
{
  messages.push_back(text);
}

/* EOF */
