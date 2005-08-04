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

#include <iostream>
#include <ClanLib/Display/keyboard.h>
#include <ClanLib/Display/joystick.h>
#include <ClanLib/Display/display_iostream.h>
#include <ClanLib/Display/keys.h>

#include "lisp/lisp.hpp"
#include "lisp/properties.hpp"
#include "controller_def.hpp"
#include "input_button_input_device.hpp"
#include "input_axis_input_device.hpp"
#include "input_keyboard_input_device.hpp"
#include "button_factory.hpp"
#include "axis_factory.hpp"

#include "input_manager_custom.hpp"

InputManagerCustom::InputManagerCustom(const lisp::Lisp* lisp)
{
  if (0) // FIXME: input disabled, change this to SDL
    {
      init(lisp);

      for (int i = 0; i < (int)buttons.size(); ++i)
        {
          if (buttons[i])
            {
              slots.push_back(buttons[i]->on_key_down().connect(this, &InputManagerCustom::on_button_down, i));
              slots.push_back(buttons[i]->on_key_up().connect  (this, &InputManagerCustom::on_button_up,   i));
            }
          else
            {
              std::cout << "# Warrning: Button '" << ControllerDef::button_id2name(i)
                        << "' not configured and will not be usable" << std::endl;
            }
        }

      for (int i = 0; i < (int)axes.size(); ++i)
        {
          if (axes[i])
            {
              slots.push_back(axes[i]->on_move().connect(this, &InputManagerCustom::on_axis_move, i));
            }
          else
            {
              std::cout << "# Warrning: Axis '" << ControllerDef::axis_id2name(i)
                        << "' not configured and will not be usable" << std::endl;
            }
        }

      for (int i = 0; i < (int)keyboards.size(); ++i)
        {
          if (keyboards[i])
            {
              slots.push_back(keyboards[i]->on_key().connect(this, &InputManagerCustom::on_key));
            }
          else
            {
              std::cout << "# Warrning: Keyboard not configured" << std::endl;
            }
        }
    }
}

void 
InputManagerCustom::init(const lisp::Lisp* lisp)
{
  buttons.resize(ControllerDef::get_button_count());
  axes.resize(ControllerDef::get_axis_count());
  keyboards.resize(ControllerDef::get_keyboard_count());

  lisp::Properties props(lisp);
  lisp::PropertyIterator<const lisp::Lisp*> iter = props.get_iter();
  while(iter.next()) {
    std::string name = iter.item();
    const lisp::Lisp* lisp = (*iter)->get_list_elem(1);

    int id = ControllerDef::button_name2id(name);
    if (id != -1)
      {
        buttons[id] = ButtonFactory::create(lisp);
      }
    else if (name == "keyboard")
      {
        keyboards[0] = new InputKeyboardInputDevice(CL_Keyboard::get_device());
      }
    else
      {
        id = ControllerDef::axis_name2id(name);
        if (id != -1)
          {
            axes[id] = AxisFactory::create(lisp);
          }
        else
          {
            std::cout << "# Warning: InputManagerCustom::init: Error unknown tag: " << std::endl;
          }
      }
  }
}  

void
InputManagerCustom::on_axis_move(float pos, int name)
{
  add_axis_event(name, pos);
  controller.set_axis_state(name, pos);
}

void
InputManagerCustom::on_button_up(int name)
{
  add_button_event(name, false);
  controller.set_button_state(name, false);
}

void
InputManagerCustom::on_button_down(int name)
{
  add_button_event(name, true);
  controller.set_button_state(name, true);
}

void
InputManagerCustom::on_key(KeyboardEvent::KeyType key_type, int code)
{
  add_keyboard_event(0, key_type, code);
}

void
InputManagerCustom::update(float delta)
{
  for (int i = 0; i < (int)buttons.size(); ++i)
    {
      if (buttons[i])
        buttons[i]->update(delta);
    }

  for (int i = 0; i < (int)axes.size(); ++i)
    {
      if (axes[i])
        axes[i]->update(delta);
    }

  for (int i = 0; i < (int)keyboards.size(); ++i)
    {
      if (keyboards[i])
        keyboards[i]->update(delta);
    }
}

/* EOF */
