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

DialogEntry::DialogEntry(const std::string& arg_character, const std::string& arg_text)
  : character(arg_character),
    text(arg_text)
{
}

ObjectiveEntry::ObjectiveEntry(const std::string& arg_name, const std::string& arg_text)
  : name(arg_name),
    text(arg_text),
    complete(false)
{
}

PDA::PDA()
  : active(false),
    state(PDA_INVENTORY)
{ 
  text_area = 0;
}

void
PDA::draw()
{
  if (!active)
    return;
    
  if (text_area)
    {
      const Rectf& rect = text_area->get_rect().grow(8.0f);

      Display::fill_rounded_rect(rect, 16.0f, Color(0.3f, 0.3f, 0.5f, 0.5f));
      Display::draw_rounded_rect(rect, 16.0f, Color(1.0f, 1.0f, 1.0f, 0.5f));

      text_area->draw();
    }
}

void
PDA::update(float delta, const Controller& controller)
{
  if (!active)
    return;
  
  const InputEventLst& events = controller.get_events();
  for(InputEventLst::const_iterator i = events.begin(); i != events.end(); ++i) {
    if (i->type == AXIS_EVENT && i->axis.name == X_AXIS) {
      if (i->axis.pos > 0) {
        state = static_cast<pda_state>(state + 1);
        if (state > PDA_DIALOGS)
          state = PDA_INVENTORY;
      }
      else if (i->axis.pos < 0) {
        state = static_cast<pda_state>(state - 1);
        if (state < PDA_INVENTORY)
          state = PDA_DIALOGS;
      }
    }
  }
  
  int width  = 600;
  int height = 400;
  
  switch (state) {
    case PDA_INVENTORY:
      show_inventory();
      break;
    case PDA_OBJECTIVES:
      show_objectives();
      break;
    case PDA_DIALOGS:
      show_dialogs();
      break;
  }
 
  if (new_text != old_text) {
    delete text_area;
    text_area = new TextArea(Rect(Point(100,
                100),
                Size(width, height)), false);
    text_area->set_font(Fonts::ttffont);
    text_area->set_text(new_text);
    
    old_text = new_text;
  }
  
  text_area->update(delta);
}

void
PDA::add_dialog(const std::string& character, const std::string& text)
{
  dialogs.push_back(DialogEntry(character, text));
}

void
PDA::add_objective(const std::string& name, const std::string& text)
{
  objectives.push_back(ObjectiveEntry(name, text));
}

void
PDA::objective_complete(const std::string& name)
{
  for (std::vector<ObjectiveEntry>::iterator i = objectives.begin(); i != objectives.end(); ++i) {
    if (i->name == name)
      i->complete = true;
      return;
  }
}

bool
PDA::is_objective_given(const std::string& name)
{
  for (std::vector<ObjectiveEntry>::iterator i = objectives.begin(); i != objectives.end(); ++i) {
    if (i->name == name)
      return true;
  }
  
  return false;
}

bool
PDA::is_objective_complete(const std::string& name)
{
  for (std::vector<ObjectiveEntry>::iterator i = objectives.begin(); i != objectives.end(); ++i) {
    if (i->name == name && i->complete)
      return true;
  }
  
  return false;
}

void
PDA::show_inventory()
{
  new_text = "<large>Personal Digital Assistant</large>\n";
  new_text += "<b>inventory</b> - objectives - dialogs\n\n";
}

void
PDA::show_objectives()
{
  new_text = "<large>Personal Digital Assistant</large>\n";
  new_text += "inventory - <b>objectives</b> - dialogs\n\n";
  
  for (std::vector<ObjectiveEntry>::reverse_iterator i = objectives.rbegin(); i != objectives.rend(); ++i) {
    new_text += i->name;
    new_text += ": ";
    if (i->complete)
      new_text += "complete\n";
    else
      new_text += "incomplete\n";
    new_text += i->text + "\n\n";
  }
}

void
PDA::show_dialogs()
{
  new_text = "<large>Personal Digital Assistant</large>\n";
  new_text += "inventory - objectives - <b>dialogs</b>\n\n";
  
  for (std::vector<DialogEntry>::reverse_iterator i = dialogs.rbegin(); i != dialogs.rend(); ++i) {
    new_text += i->character;
    new_text += ": ";
    new_text += i->text + '\n';
  }
}

/* EOF */
