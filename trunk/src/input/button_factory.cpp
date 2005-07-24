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

#include <ClanLib/Display/joystick.h>
#include <ClanLib/Display/keyboard.h>
#include "windstille_error.hpp"
#include "input_button.hpp"
#include "input_axis.hpp"
#include "input_button_input_device.hpp"
#include "axis_factory.hpp"
#include "axis_button.hpp"
#include "multi_button.hpp"
#include "button_factory.hpp"
#include "lisp/lisp.hpp"
#include "lisp/properties.hpp"

InputButton* 
ButtonFactory::create(const lisp::Lisp* lisp)
{
  if(lisp->get_type() != lisp::Lisp::TYPE_LIST)
    throw std::runtime_error("expected list in axis argument");
  if(lisp->get_list_size() == 0)
    throw std::runtime_error("List needs at least 1 argument in axis");
  const lisp::Lisp* namel = lisp->get_list_elem(0);
  if(namel->get_type() != lisp::Lisp::TYPE_SYMBOL)
    throw std::runtime_error("axis needs symbol as first argument");
 
  std::string name = namel->get_symbol();

  if(name == "joystick_button") {
    return create_joystick_button(lisp);
  } else if(name == "keyboard-button") {
    return create_keyboard_button(lisp);
  } else if(name == "axis-button") {
    return create_axis_button(lisp);
  } else if(name == "multi-button") {
    return create_multi_button(lisp);
  }
  
  std::ostringstream msg;
  msg << "Unknown button type '" << name << "' found";
  throw std::runtime_error(msg.str());
}

InputButton*
ButtonFactory::create_axis_button(const lisp::Lisp* lisp)
{
  if(lisp->get_list_size() != 3)
    throw std::runtime_error("axis_button needs exactly 2 arguments");
  
  InputAxis* axis = AxisFactory::create(lisp->get_list_elem(1));
  bool top = lisp->get_list_elem(2)->get_bool();
  
  return new AxisButton(axis, top);
}

InputButton*
ButtonFactory::create_joystick_button(const lisp::Lisp* lisp)
{
  if(lisp->get_list_size() != 3)
    throw std::runtime_error("joystick_button needs exactly 2 arguments");

  int device_num = lisp->get_list_elem(1)->get_int();
  int button_num = lisp->get_list_elem(2)->get_int();
  
  if (device_num >= 0 && device_num < CL_Joystick::get_device_count())
    return new InputButtonInputDevice(CL_Joystick::get_device(device_num), button_num);
  else
    throw WindstilleError("Error: ButtonFactory::create_joystick_button: device out of range");
}

InputButton*
ButtonFactory::create_keyboard_button(const lisp::Lisp* lisp)
{
  if(lisp->get_list_size() != 2)
    throw std::runtime_error("keyboard_button needs exactly 1 argument");
  std::string key_str = lisp->get_list_elem(1)->get_string();
  int key_num         = CL_Keyboard::get_device().string_to_keyid(key_str);

  // FIXME: No error checking
  return new InputButtonInputDevice(CL_Keyboard::get_device(), key_num);
}

InputButton*
ButtonFactory::create_multi_button(const lisp::Lisp* lisp)
{
  MultiButton* button = new MultiButton();
 
  lisp::Properties props(lisp);
  lisp::PropertyIterator<const lisp::Lisp*> iter = props.get_iter();
  while(iter.next()) {
    button->add(create(*iter));
  }
  
  return button;
}

/* EOF */
