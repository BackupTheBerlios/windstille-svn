/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005,2007 Ingo Ruhnke <grumbel@gmx.de>
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

#include <assert.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "wiimote.hpp"
#include "file_reader.hpp"
#include "controller_def.hpp"
#include "input_manager_sdl.hpp"

InputManagerSDL* InputManagerSDL::current_ = 0;

const int dead_zone =  8192;

class InputManagerSDLImpl
{
public:
  std::vector<JoystickButtonBinding>     joystick_button_bindings;
  std::vector<JoystickButtonAxisBinding> joystick_button_axis_bindings;
  std::vector<JoystickAxisBinding>       joystick_axis_bindings;

  std::vector<KeyboardButtonBinding> keyboard_button_bindings;
  std::vector<KeyboardAxisBinding>   keyboard_axis_bindings;

  std::vector<MouseButtonBinding>   mouse_button_bindings;

  std::vector<WiimoteButtonBinding> wiimote_button_bindings;
  std::vector<WiimoteAxisBinding>   wiimote_axis_bindings;

  std::vector<SDL_Joystick*> joysticks;

  std::map<std::string, SDLKey> keyidmapping;
};

std::string
InputManagerSDL::keyid_to_string(SDLKey id)
{
  return SDL_GetKeyName(id);
}

SDLKey
InputManagerSDL::string_to_keyid(const std::string& str)
{
  return impl->keyidmapping[str];
}

void
InputManagerSDL::ensure_open_joystick(int device)
{
  if (device >= int(impl->joysticks.size()))
    impl->joysticks.resize(device + 1, 0);

  if (!impl->joysticks[device])
    {
      if (SDL_Joystick* joystick = SDL_JoystickOpen(device))
        {
          impl->joysticks[device] = joystick;
        }
      else
        {
          std::cout << "InputManagerSDL: Couldn't open joystick device " << device << std::endl;
        }
    }
  
}

static bool has_suffix(const std::string& str, const std::string& suffix)
{
  if (str.length() >= suffix.length())
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
  else
    return false;
}

void
InputManagerSDL::load(const std::string& filename)
{
  FileReader reader = FileReader::parse(filename);

  std::cout << "InputManager: " << filename << std::endl;

  if (reader.get_name() != "windstille-controller") {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille-controller file";
    throw std::runtime_error(msg.str());
  }
  
  parse_config(reader);
}

void
InputManagerSDL::parse_config(FileReader& reader)
{
  std::vector<FileReader> sections = reader.get_sections();
  
  for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
    {
      if (has_suffix(i->get_name(), "-button"))
        {
          std::vector<FileReader> dev_sections = i->get_sections();
          for(std::vector<FileReader>::iterator j = dev_sections.begin(); j != dev_sections.end(); ++j)
            {
              if (j->get_name() == "joystick-button")
                {
                  int device = 0;
                  int button = 0;

                  j->get("device", device);
                  j->get("button", button);

                  bind_joystick_button(controller_description.get_definition(i->get_name()).id,
                                       device, button);
                }
              else if (j->get_name() == "wiimote-button")
                {
                  int device = 0;
                  int button = 0;

                  j->get("device", device);
                  j->get("button", button);

                  bind_wiimote_button(controller_description.get_definition(i->get_name()).id,
                                      device, button);
                }
              else if (j->get_name() == "keyboard-button")
                {
                  std::string key;

                  j->get("key", key);

                  bind_keyboard_button(controller_description.get_definition(i->get_name()).id,
                                       string_to_keyid(key));
                }
              else
                {
                  std::cout << "InputManagerSDL: Unknown tag: " << j->get_name() << std::endl;
                }
            }
        }
      else if (has_suffix(i->get_name(), "-axis"))
        {
          std::vector<FileReader> dev_sections = i->get_sections();
          for(std::vector<FileReader>::iterator j = dev_sections.begin(); j != dev_sections.end(); ++j)
            {
              if (j->get_name() == "joystick-axis")
                {
                  int  device = 0;
                  int  axis   = 0;
                  bool invert = false;

                  j->get("device", device);
                  j->get("axis",   axis);
                  j->get("invert", invert);

                  bind_joystick_axis(controller_description.get_definition(i->get_name()).id,
                                     device, axis, invert);
                }
              else if (j->get_name() == "keyboard-axis")
                {
                  std::string minus;
                  std::string plus;

                  j->get("minus", minus);
                  j->get("plus",  plus);

                  bind_keyboard_axis(controller_description.get_definition(i->get_name()).id, 
                                     string_to_keyid(minus), string_to_keyid(plus));
                }
              else if (j->get_name() == "wiimote-axis")
                {
                  int  device = 0;
                  int  axis   = 0;
                  
                  j->get("device", device);
                  j->get("axis",   axis);
                  
                  bind_wiimote_axis(controller_description.get_definition(i->get_name()).id,
                                    device, axis);
                }
              else
                {
                  std::cout << "InputManagerSDL: Unknown tag: " << j->get_name() << std::endl;
                }
            }

        }
    }
}

InputManagerSDL::InputManagerSDL()
  : impl(new InputManagerSDLImpl)
{
  current_ = this;

  for (int i = 0; i < SDLK_LAST; ++i) {
    char* key_name = SDL_GetKeyName(static_cast<SDLKey>(i));
    impl->keyidmapping[key_name] = static_cast<SDLKey>(i);
    // FIXME: Make the keynames somewhere user visible so that users can use them
    // std::cout << key_name << std::endl;
  }
  
  // FIXME: doesn't really belong here
  Wiimote::init();
  wiimote->connect();
}

InputManagerSDL::~InputManagerSDL()
{
}

void
InputManagerSDL::on_key_event(const SDL_KeyboardEvent& event)
{
  // Hardcoded defaults
  if (event.keysym.sym == SDLK_RETURN)
    {
      add_button_event(ENTER_BUTTON, event.state);
    }
  else if (event.keysym.sym == SDLK_ESCAPE)
    {
      add_button_event(ESCAPE_BUTTON, event.state);
    }
  else if (event.keysym.sym == SDLK_LEFT)
    {
      wiimote->set_led(1, 0);
      add_button_event(MENU_LEFT_BUTTON, event.state);
    }
  else if (event.keysym.sym == SDLK_RIGHT)
    {
      wiimote->set_led(2, 0);
      add_button_event(MENU_RIGHT_BUTTON, event.state);
    }
  else if (event.keysym.sym == SDLK_UP)
    {
      wiimote->set_led(1, 1);
      add_button_event(MENU_UP_BUTTON, event.state);
    }
  else if (event.keysym.sym == SDLK_DOWN)
    {
      wiimote->set_led(2, 1);
      add_button_event(MENU_DOWN_BUTTON, event.state);
    }

  // Dynamic bindings
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
            {
              add_axis_event(i->event, 1.0f);
            }
          else if (!keystate[i->minus])
            {
              add_axis_event(i->event, 0.0f);
            }
        }
    }
}

void
InputManagerSDL::on_mouse_button_event(const SDL_MouseButtonEvent& button)
{
  for (std::vector<MouseButtonBinding>::const_iterator i = impl->mouse_button_bindings.begin();
       i != impl->mouse_button_bindings.end();
       ++i)
    {
      if (button.button == i->button)
        {
          add_button_event(i->event, button.state);
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

  for (std::vector<JoystickButtonAxisBinding>::const_iterator i = impl->joystick_button_axis_bindings.begin();
       i != impl->joystick_button_axis_bindings.end();
       ++i)
    {
      if (button.which  == i->device)
        {
          if (button.button == i->minus)
            {
              add_axis_event(i->event, button.state ? -1.0f : 0.0f);
            }
          else if (button.button == i->plus)
            {
              add_axis_event(i->event, button.state ?  1.0f : 0.0f);
            }
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
          if (event.value < -dead_zone)
            {
              add_axis_event(i->event, event.value/(i->invert?-32768.0f:32768.0f));
            }
          else if (event.value > dead_zone)
            {
              add_axis_event(i->event, event.value/(i->invert?-32768.0f:32768.0f));
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
      // event.motion
      // FIXME: Hardcodes 0,1 values are not a good idea, need to bind the stuff like the rest
      if (0) std::cout << "mouse: " << event.motion.xrel << " " << event.motion.yrel << std::endl;
      add_ball_event(0, event.motion.xrel);
      add_ball_event(1, event.motion.yrel);
      break;

    case SDL_MOUSEBUTTONDOWN:
      on_mouse_button_event(event.button);
      break;

    case SDL_MOUSEBUTTONUP:
      on_mouse_button_event(event.button);
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
  if (wiimote->is_connected())
    {
      // Check for new events from the Wiimote
      std::vector<WiimoteEvent> events = wiimote->pop_events();
      for(std::vector<WiimoteEvent>::iterator i = events.begin(); i != events.end(); ++i)
        {
          WiimoteEvent& event = *i;
          if (event.type == WiimoteEvent::WIIMOTE_BUTTON_EVENT)
            {
              //std::cout << "WiimoteButton: " << event.button.button << " " << event.button.down << std::endl;

              for (std::vector<WiimoteButtonBinding>::const_iterator j = impl->wiimote_button_bindings.begin();
                   j != impl->wiimote_button_bindings.end();
                   ++j)
                {
                  if (event.button.device == j->device &&
                      event.button.button == j->button)
                    {
                      add_button_event(j->event, event.button.down);
                    }
                }
            }
          else if (event.type == WiimoteEvent::WIIMOTE_AXIS_EVENT)
            {
              //std::cout << "WiimoteAxis: " << event.axis.axis << " " << event.axis.pos << std::endl;

              for (std::vector<WiimoteAxisBinding>::const_iterator j = impl->wiimote_axis_bindings.begin();
                   j != impl->wiimote_axis_bindings.end();
                   ++j)
                {
                  if (event.axis.device == j->device &&
                      event.axis.axis == j->axis)
                    {
                      add_axis_event(j->event, event.axis.pos);
                    }
                }
            }
          else
            {
              assert(!"Never reached");
            }
        }
    }
}

void
InputManagerSDL::bind_mouse_button(int event, int device, int button)
{
  MouseButtonBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.button = button;

  impl->mouse_button_bindings.push_back(binding); 
}

void
InputManagerSDL::bind_joystick_hat_axis(int event, int device, int axis)
{
  
}

void
InputManagerSDL::bind_joystick_button_axis(int event, int device, int minus, int plus)
{
  ensure_open_joystick(device);

  JoystickButtonAxisBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.minus  = minus;
  binding.plus   = plus;

  impl->joystick_button_axis_bindings.push_back(binding); 
}

void
InputManagerSDL::bind_joystick_axis(int event, int device, int axis, bool invert)
{
  ensure_open_joystick(device);

  JoystickAxisBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.axis   = axis;
  binding.invert = invert;

  impl->joystick_axis_bindings.push_back(binding);
}

void
InputManagerSDL::bind_joystick_button(int event, int device, int button)
{
  ensure_open_joystick(device);

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

void
InputManagerSDL::bind_wiimote_button(int event, int device, int button)
{
  WiimoteButtonBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.button = button;

  impl->wiimote_button_bindings.push_back(binding);
}

void
InputManagerSDL::bind_wiimote_axis(int event, int device, int axis)
{
  WiimoteAxisBinding binding;

  binding.event  = event;
  binding.device = device;
  binding.axis   = axis;

  impl->wiimote_axis_bindings.push_back(binding);
}

void
InputManagerSDL::clear_bindings()
{
  impl->joystick_button_bindings.clear();
  impl->joystick_axis_bindings.clear();
  impl->joystick_button_axis_bindings.clear();
  
  impl->keyboard_button_bindings.clear();
  impl->keyboard_axis_bindings.clear();

  impl->mouse_button_bindings.clear();

  impl->wiimote_button_bindings.clear();
  impl->wiimote_axis_bindings.clear();
}

void
InputManagerSDL::add_axis_event  (int name, float pos)
{
  // Convert analog axis events into digital menu movements
  // FIXME: add key repeat
  float click_threshold = 0.5f;
  float release_threshold = 0.3f;

  // FIXME: need state info
  float old_pos = controller.get_axis_state(name);
  if (name == X_AXIS)
    {
      if (controller.get_button_state(MENU_LEFT_BUTTON) == 0 &&
          pos < -click_threshold && old_pos > -click_threshold) 
        {
          add_button_event(MENU_LEFT_BUTTON, 1);
        } 
      else if (controller.get_button_state(MENU_LEFT_BUTTON) == 1 &&
               old_pos < -release_threshold && pos > -release_threshold) 
        {
          add_button_event(MENU_LEFT_BUTTON, 0);
        } 
      
      else if (controller.get_button_state(MENU_RIGHT_BUTTON) == 0 &&
               pos > click_threshold && old_pos < click_threshold) 
        {
          add_button_event(MENU_RIGHT_BUTTON, 1);
        } 
      else  if (controller.get_button_state(MENU_RIGHT_BUTTON) == 1 &&
                old_pos > release_threshold && pos < release_threshold) 
        {
          add_button_event(MENU_RIGHT_BUTTON, 0);
        }
    }
  else if (name == Y_AXIS)
    {
      if (controller.get_button_state(MENU_UP_BUTTON) == 0 &&
          pos < -click_threshold && old_pos > -click_threshold) 
        {
          add_button_event(MENU_UP_BUTTON, 1);
        } 
      else if (controller.get_button_state(MENU_UP_BUTTON) == 1 &&
               old_pos < -release_threshold && pos > -release_threshold) 
        {
          add_button_event(MENU_UP_BUTTON, 0);
        }

      else  if (controller.get_button_state(MENU_DOWN_BUTTON) == 0 &&
                pos > click_threshold && old_pos < click_threshold) 
        {
          add_button_event(MENU_DOWN_BUTTON, 1);
        } 
      else  if (controller.get_button_state(MENU_DOWN_BUTTON) == 1 &&
                old_pos > release_threshold && pos < release_threshold) 
        {
          add_button_event(MENU_DOWN_BUTTON, 0);
        }
    }

  InputManagerImpl::add_axis_event(name, pos);
}

/* EOF */
