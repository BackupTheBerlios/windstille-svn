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
#include "windstille_error.hxx"
#include "input_button.hxx"
#include "input_axis.hxx"
#include "input_button_input_device.hxx"
#include "lisp/list_iterator.hpp"
#include "lisp_util.hpp"
#include "axis_factory.hxx"
#include "axis_button.hxx"
#include "multi_button.hxx"
#include "button_factory.hxx"

InputButton* 
ButtonFactory::create(const lisp::Lisp* lisp)
{
  lisp::ListIterator iter(lisp);
  while(iter.next()) {
    if(iter.item() == "joystick-button") {
      return create_joystick_button(iter.lisp());
    } else if(iter.item() == "keyboard-button") {
      return create_keyboard_button(iter.lisp());
    } else if(iter.item() == "axis-button") {
      return create_axis_button(iter.lisp());
    } else if(iter.item() == "multi-button") {
      return create_multi_button(iter.lisp());
    } else {
      std::cerr << "Skipping unknown tag '" << iter.item() 
        << "' in controller file\n";
    }
  }
      
  return 0;
}

InputButton*
ButtonFactory::create_axis_button(const lisp::Lisp* lisp)
{
  InputAxis* axis = AxisFactory::create(lisp_get_list_nth(lisp, 0));
  bool top = lisp_get_list_nth(lisp, 1)->get_bool();
  
  return new AxisButton(axis, top);
}

InputButton*
ButtonFactory::create_joystick_button(const lisp::Lisp* lisp)
{
  int device_num = lisp_get_list_nth(lisp, 0)->get_int();
  int button_num = lisp_get_list_nth(lisp, 1)->get_int();
  
  if (device_num >= 0 && device_num < CL_Joystick::get_device_count())
    return new InputButtonInputDevice(CL_Joystick::get_device(device_num), button_num);
  else
    throw WindstilleError("Error: ButtonFactory::create_joystick_button: device out of range");
}

InputButton*
ButtonFactory::create_keyboard_button(const lisp::Lisp* lisp)
{
  std::string key_str = lisp->get_car()->get_string();
  int key_num         = CL_Keyboard::get_device().string_to_keyid(key_str);

  // FIXME: No error checking
  return new InputButtonInputDevice(CL_Keyboard::get_device(), key_num);
}

InputButton*
ButtonFactory::create_multi_button(const lisp::Lisp* lisp)
{
  MultiButton* button = new MultiButton();
 
  for( ; lisp != 0; lisp = lisp->get_cdr()) {
    button->add(create(lisp->get_car()));
  }
  
  return button;
}

/* EOF */
