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
  buttons[name] = id;
}

void
ControllerDef::add_axis  (const std::string& name, int id)
{
  axes[name] = id;
}

int
ControllerDef::get_button_count() const
{
  return buttons.size();
}

int
ControllerDef::get_axis_count() const
{
  return axes.size();
}

int
ControllerDef::get_keyboard_count() const
{
  return 1;
}

std::string
ControllerDef::button_id2name(int id) const
{
  for(std::map<std::string, int>::const_iterator i = buttons.begin(); i != buttons.end(); ++i)
    {
      if (i->second == id)
        return i->first;
    }
  throw std::runtime_error("Unknown button id");
}

int
ControllerDef::button_name2id(const std::string& name) const
{
  std::map<std::string, int>::const_iterator i = buttons.find(name);
  if (i == buttons.end())
    throw std::runtime_error("Unknown button name: " + name);

  return i->second;
}

std::string
ControllerDef::axis_id2name(int id) const
{
  for(std::map<std::string, int>::const_iterator i = axes.begin(); i != axes.end(); ++i)
    {
      if (i->second == id)
        return i->first;
    }
  throw std::runtime_error("Unknown axis id");
}

int 
ControllerDef::axis_name2id(const std::string& name) const
{
  std::map<std::string, int>::const_iterator i = axes.find(name);
  if (i == axes.end())
    throw std::runtime_error("Unknown axis name: " + name);

  return i->second;
}

/* EOF */
