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

#include <stdexcept>
#include "controller_def.hpp"

ControllerDef::ControllerDef()
{
  add_button("primary-button",   PRIMARY_BUTTON);
  add_button("secondary-button", SECONDARY_BUTTON);
  add_button("tertiary-button",  TERTIARY_BUTTON);

  add_button("pda-button", PDA_BUTTON);
  add_button("inventory-button", INVENTORY_BUTTON);

  add_button("aim-button",       AIM_BUTTON);
  add_button("pause-button",     PAUSE_BUTTON);

  add_axis("x-axis", X_AXIS);
  add_axis("y-axis", Y_AXIS);
}

ControllerDef::~ControllerDef()
{
}

void
ControllerDef::add_button(const std::string& name, int id)
{
  InputEventDefinition event;

  event.type = BUTTON_EVENT;
  event.name = name;
  event.id   = id;

  str_to_event[event.name] = event;
  id_to_event[event.id]    = event;
}

void
ControllerDef::add_axis  (const std::string& name, int id)
{
  InputEventDefinition event;

  event.type = AXIS_EVENT;
  event.name = name;
  event.id   = id;

  str_to_event[event.name] = event;
  id_to_event[event.id]    = event;
}

const InputEventDefinition&
ControllerDef::get_definition(int id) const
{
  std::map<int, InputEventDefinition>::const_iterator i = id_to_event.find(id);
  if (i == id_to_event.end())
    throw std::runtime_error("Unknown event id");

  return i->second;
}

const InputEventDefinition&
ControllerDef::get_definition(const std::string& name) const
{
  std::map<std::string, InputEventDefinition>::const_iterator i = str_to_event.find(name);
  if (i == str_to_event.end())
    throw std::runtime_error("Unknown event str");

  return i->second;
}

/* EOF */
