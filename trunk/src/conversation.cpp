//  $Id$
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "input/controller.hpp"
#include "input/input_manager.hpp"
#include "fonts.hpp"
#include "math/rect.hpp"
#include "script_manager.hpp"
#include "display/display.hpp"
#include "conversation.hpp"

Conversation* Conversation::current_ = 0;

Conversation::Conversation()
{
  current_ = this;
  visible = false;
  selection = 0;
}

void
Conversation::add(const std::string& text)
{
  if (!visible)
    {
      choices.push_back(text);
      if (selection >= int(choices.size()))
        selection = 0;
    }
}

void
Conversation::draw()
{
  if (visible)
    {
      int x = 100;
      int y = 200;

      Rect rect(Point(x - 20, y - 20 + Fonts::ttffont->get_height()/2 - 5),
                Size(300 + 20, // FIXME:
                     (Fonts::ttffont->get_height() + 10) * choices.size() + 20));
      
      Display::fill_rect(rect, Color(0,0,0,0.5f));
      Display::draw_rect(rect, Color(1.0f, 1.0f, 1.0f, 0.3f));
 
      for(int i = 0; i < int(choices.size()); ++i)
        {
          if (i == selection)
            Fonts::ttffont->draw(x, y, choices[i]);
          else
            Fonts::ttffont->draw(x, y, choices[i], Color(0.5f, 0.5f, 0.5f));
      
          y += Fonts::ttffont->get_height() + 10;
        }
    }
}

void
Conversation::update(float delta)
{
  (void) delta;
  if (visible)
    {
      Controller controller = InputManager::get_controller();
      const InputEventLst& events = controller.get_events();
  
      for(InputEventLst::const_iterator i = events.begin(); i != events.end(); ++i)
        {
          if (i->type == BUTTON_EVENT && i->button.down)
            {
              switch (i->button.name)
                {
                case UP_BUTTON:
                  selection -= 1;
                  if (selection < 0)
                    selection = choices.size() - 1;
                  break;
              
                case DOWN_BUTTON:
                  selection += 1;
                  if (selection >= int(choices.size()))
                    selection = 0;
                  break;

                case FIRE_BUTTON:
                  visible = false;
                  choices.clear();
                  script_manager->fire_wakeup_event(ScriptManager::CONVERSATION_CLOSED);
                  return;
                  break;
                }
            }
        }
    }
}

int
Conversation::get_selection() const
{
  return selection;
}

void
Conversation::show()
{
  visible = true;
}

/* EOF */
