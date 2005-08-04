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

#ifndef HEADER_INPUT_MANAGER_CUSTOM_HXX
#define HEADER_INPUT_MANAGER_CUSTOM_HXX

#include <ClanLib/Display/input_device.h>
#include <ClanLib/Display/input_event.h>
#include "input_event.hpp"
#include "input_button.hpp"
#include "input_axis.hpp"
#include "input_keyboard.hpp"
#include "input_manager_impl.hpp"
#include "lisp/lisp.hpp"

/** */
class InputManagerCustom : public InputManagerImpl
{
private:
  std::vector<CL_Slot> slots;

  typedef std::vector<InputAxis*>     Axes;
  typedef std::vector<InputButton*>   Buttons;
  typedef std::vector<InputKeyboard*> Keyboards;

  Axes      axes;
  Buttons   buttons;
  Keyboards keyboards;

public:
  InputManagerCustom(const lisp::Lisp* lisp);
  
  void update(float delta);

  void on_button_up(int name);
  void on_button_down(int name);
  void on_axis_move(float pos, int name);
  void on_key(KeyboardEvent::KeyType key_type, int code);

private:
  void init(const lisp::Lisp* lisp);

  InputManagerCustom (const InputManagerCustom&);
  InputManagerCustom& operator= (const InputManagerCustom&);
};

#endif

/* EOF */
