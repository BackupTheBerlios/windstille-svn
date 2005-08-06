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

#include <iostream>
#include <vector>
#include "controller_def.hpp"
#include "input_manager_sdl.hpp"

InputManagerSDL* InputManagerSDL::current_ = 0;

#define DEAD_ZONE 4096

struct JoystickButtonBinding
{
  int event;
  int device;
  int button;
};

struct JoystickAxisBinding
{
  int event;
  int device;
  int axis;
};

struct KeyboardButtonBinding
{
  int event;
  SDLKey key;
};

struct KeyboardAxisBinding
{
  int event;
  SDLKey plus;
  SDLKey minus;
};

class InputManagerSDLImpl
{
public:
  std::vector<JoystickButtonBinding> joystick_button_bindings;
  std::vector<JoystickAxisBinding>   joystick_axis_bindings;

  std::vector<KeyboardButtonBinding> keyboard_button_bindings;
  std::vector<KeyboardAxisBinding>   keyboard_axis_bindings;
};

InputManagerSDL::InputManagerSDL()
  : impl(new InputManagerSDLImpl)
{
  current_ = this;
  
  int num_joysticks = SDL_NumJoysticks();

  if (num_joysticks > 0)
    /*SDL_Joystick* joy =*/ SDL_JoystickOpen(0);

  //SDLK_LAST

  bind_joystick_button(PDA_BUTTON,       0, 0);
  bind_joystick_button(TERTIARY_BUTTON,  0, 1);
  bind_joystick_button(SECONDARY_BUTTON, 0, 2);
  bind_joystick_button(PRIMARY_BUTTON,   0, 3);
  bind_joystick_button(AIM_BUTTON,       0, 7);
  bind_joystick_button(PAUSE_BUTTON,     0, 9);

  bind_joystick_axis(X_AXIS, 0, 0);
  bind_joystick_axis(Y_AXIS, 0, 1);

  if (1)
    {
      bind_keyboard_button(PDA_BUTTON,       SDLK_w);
      bind_keyboard_button(TERTIARY_BUTTON,  SDLK_a);
      bind_keyboard_button(SECONDARY_BUTTON, SDLK_d);
      bind_keyboard_button(PRIMARY_BUTTON,   SDLK_s);
      bind_keyboard_button(AIM_BUTTON,       SDLK_LSHIFT);
      bind_keyboard_button(PAUSE_BUTTON,     SDLK_p);
    }
  else
    {
      bind_keyboard_button(PDA_BUTTON,       SDLK_COMMA);
      bind_keyboard_button(TERTIARY_BUTTON,  SDLK_a);
      bind_keyboard_button(SECONDARY_BUTTON, SDLK_e);
      bind_keyboard_button(PRIMARY_BUTTON,   SDLK_o);
      bind_keyboard_button(AIM_BUTTON,       SDLK_LSHIFT);
      bind_keyboard_button(PAUSE_BUTTON,     SDLK_p);
    }

  bind_keyboard_axis(X_AXIS, SDLK_LEFT, SDLK_RIGHT);
  bind_keyboard_axis(Y_AXIS, SDLK_UP, SDLK_DOWN);
}

InputManagerSDL::~InputManagerSDL()
{
}

void
InputManagerSDL::on_key_event(const SDL_KeyboardEvent& event)
{
  for (std::vector<KeyboardButtonBinding>::const_iterator i = impl->keyboard_button_bindings.begin();
       i != impl->keyboard_button_bindings.end();
       ++i)
    {
      if (event.keysym.sym == i->key)
        {
          add_button_event(i->event, event.state);
        }
    }

  Uint8* keystate = SDL_GetKeyState(0);

  for (std::vector<KeyboardAxisBinding>::const_iterator i = impl->keyboard_axis_bindings.begin();
       i != impl->keyboard_axis_bindings.end();
       ++i)
    {
      if (event.keysym.sym == i->minus)
        {
          if (event.state)
            add_axis_event(i->event, -1.0f);
          else if (!keystate[i->plus])
            add_axis_event(i->event, 0.0f);
        }
      else if (event.keysym.sym == i->plus)
        {
          if (event.state)
            add_axis_event(i->event, 1.0f);
          else if (!keystate[i->minus])
            add_axis_event(i->event, 0.0f);
        }
    }
}

void
InputManagerSDL::on_joy_button_event(const SDL_JoyButtonEvent& button)
{
  for (std::vector<JoystickButtonBinding>::const_iterator i = impl->joystick_button_bindings.begin();
       i != impl->joystick_button_bindings.end();
       ++i)
    {
      if (button.which  == i->device &&
          button.button == i->button)
        {
          add_button_event(i->event, button.state);
        }
    }
}

void
InputManagerSDL::on_joy_axis_event(const SDL_JoyAxisEvent& event)
{
  for (std::vector<JoystickAxisBinding>::const_iterator i = impl->joystick_axis_bindings.begin();
       i != impl->joystick_axis_bindings.end();
       ++i)
    {
      if (event.which  == i->device &&
          event.axis   == i->axis)
        {
          if (event.value < -DEAD_ZONE)
            {
              add_axis_event(i->event, event.value/32768.0f);
            }
          else if (event.value > DEAD_ZONE)
            {
              add_axis_event(i->event, event.value/32767.0f);
            }
          else
            {
              add_axis_event(i->event, 0);
            }
        }
    }
}

void
InputManagerSDL::on_event(const SDL_Event& event)
{
  switch(event.type)
    {        
    case SDL_KEYUP:
    case SDL_KEYDOWN:
      {
        if (event.key.state)
          {
            if ((event.key.keysym.unicode > 0 && event.key.keysym.unicode < 128)
                && (isgraph(event.key.keysym.unicode) || event.key.keysym.unicode == ' '))
              {
                add_keyboard_event(0, KeyboardEvent::LETTER, event.key.keysym.unicode);
              }
            else
              {
                add_keyboard_event(0, KeyboardEvent::SPECIAL, event.key.keysym.sym);
              }
          }
      }
      on_key_event(event.key);
      break;

    case SDL_MOUSEMOTION:
      // event.motion:      break;

    case SDL_MOUSEBUTTONDOWN:
      // event.button
      break;

    case SDL_MOUSEBUTTONUP:
      // event.button
      break;

    case SDL_JOYAXISMOTION:
      on_joy_axis_event(event.jaxis);
      break;

    case SDL_JOYBALLMOTION:
      // event.jball
      break;
          
    case SDL_JOYHATMOTION:
      // event.jhat
      break;
          
    case SDL_JOYBUTTONUP:
    case SDL_JOYBUTTONDOWN:
      on_joy_button_event(event.jbutton);
      break;

    default:
      std::cout << "InputManagerSDL: unknown event" << std::endl;
      break;
    }
}

void
InputManagerSDL::update(float delta)
{
}

void
InputManagerSDL::bind_joystick_hat_axis(int event, int device, int axis)
{
}

void
InputManagerSDL::bind_joystick_axis(int event, int device, int axis)
{
  JoystickAxisBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.axis   = axis;

  impl->joystick_axis_bindings.push_back(binding);
}

void
InputManagerSDL::bind_joystick_button(int event, int device, int button)
{
  JoystickButtonBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.button = button;

  impl->joystick_button_bindings.push_back(binding);
}

void
InputManagerSDL::bind_keyboard_button(int event, SDLKey key)
{
  KeyboardButtonBinding binding;
  
  binding.event = event;
  binding.key   = key;

  impl->keyboard_button_bindings.push_back(binding);
}

void
InputManagerSDL::bind_keyboard_axis(int event, SDLKey minus, SDLKey plus)
{
  KeyboardAxisBinding binding;
  
  binding.event = event;
  binding.minus = minus;
  binding.plus  = plus;

  impl->keyboard_axis_bindings.push_back(binding);
}

/* EOF */
