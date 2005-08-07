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

#include "display/display.hpp"
#include "math/rect.hpp"
#include "controller_def.hpp"
#include "fonts.hpp"
#include "input/input_manager_sdl.hpp"
#include "input_configurator.hpp"

InputConfigurator::InputConfigurator()
  : area(Rectf(120, 120,
               Display::get_width()  - 220, 
               Display::get_height() - 220), 
         false)
{
  out << "Input Configurator\n"
      << "------------------\n"
      << "Press Escape to ignore a setting\n"
      << std::endl; 

  area.set_text(out.str());

  area.set_font(Fonts::ttffont);

  add_configure_item(ConfigureItem::CONFIGURE_BUTTON, PDA_BUTTON);
  add_configure_item(ConfigureItem::CONFIGURE_BUTTON, TERTIARY_BUTTON);
  add_configure_item(ConfigureItem::CONFIGURE_BUTTON, SECONDARY_BUTTON);
  add_configure_item(ConfigureItem::CONFIGURE_BUTTON, PRIMARY_BUTTON);

  add_configure_item(ConfigureItem::CONFIGURE_AXIS, Y_AXIS);
  add_configure_item(ConfigureItem::CONFIGURE_AXIS, X_AXIS);


  add_configure_item(ConfigureItem::CONFIGURE_BUTTON, AIM_BUTTON);

  print_item();
}

InputConfigurator::~InputConfigurator()
{
}

void
InputConfigurator::add_configure_item(ConfigureItem::Mode mode, int event_id)
{
  ConfigureItem item;
  
  item.mode     = mode;
  item.event_id = event_id;

  items.push_back(item);

}
  
void
InputConfigurator::draw()
{
  Rectf rect(100, 100,
             Display::get_width()  - 200, 
             Display::get_height() - 200);

  Display::fill_rounded_rect(rect, 16.0f, Color(0.3f, 0.3f, 0.5f, 0.5f));
  Display::draw_rounded_rect(rect, 16.0f, Color(1.0f, 1.0f, 1.0f, 0.5f)); 
  area.draw();
}

void
InputConfigurator::update(float delta)
{
  
}

void
InputConfigurator::print_item()
{
  ControllerDef def;

  if (!items.empty())
    {
      const ConfigureItem& item = items.back();
      
      if (item.mode == ConfigureItem::CONFIGURE_AXIS)
        {
          out << "Configuring " << def.axis_id2name(item.event_id) << ": " << std::endl;
        }
      else if (item.mode == ConfigureItem::CONFIGURE_BUTTON)
        {
          out << "Configuring " << def.button_id2name(item.event_id) << ": " << std::endl;
        }
    }

  area.set_text(out.str());
}

void
InputConfigurator::next_item()
{
  ControllerDef def;

  out << std::endl;
  items.pop_back();

  print_item();

  area.set_text(out.str());
}

void
InputConfigurator::on_event(const SDL_Event& event)
{
  if (items.empty())
    return; 

  switch(event.type)
    {        
    case SDL_MOUSEMOTION:
      // event.motion:      
      break;

    case SDL_MOUSEBUTTONDOWN:
      // event.button
      break;

    case SDL_MOUSEBUTTONUP:
      // event.button
      break;

    case SDL_JOYAXISMOTION:
      if (items.back().mode == ConfigureItem::CONFIGURE_AXIS)
        {
          InputManagerSDL::current()->bind_joystick_axis(items.back().event_id, event.jaxis.which, event.jaxis.axis);
          out << "(joystick-axis (device " << event.jaxis.which << ")\n"
              << "               (axis   " << event.jaxis.axis << "))" << std::endl;
          next_item();
        }
      break;

    case SDL_JOYBALLMOTION:
      // event.jball
      break;
          
    case SDL_JOYHATMOTION:
      // event.jhat
      break;
          
    case SDL_JOYBUTTONUP:
      break;

    case SDL_JOYBUTTONDOWN:
      if (items.back().mode == ConfigureItem::CONFIGURE_BUTTON)
        {
          InputManagerSDL::current()->bind_joystick_button(items.back().event_id, event.jbutton.which, event.jbutton.button);
          out << "(joystick-button (device " << event.jbutton.which << ")\n"
              << "                 (button " << event.jbutton.button << "))" << std::endl;
          next_item();
        }
      else if (items.back().mode == ConfigureItem::CONFIGURE_AXIS)
        {
          // ignore
        }
      break;

    case SDL_KEYUP:
      break;

    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE)
        {
          next_item();
        }
      else
        {
          if (items.back().mode == ConfigureItem::CONFIGURE_BUTTON)
            {
              InputManagerSDL::current()->bind_keyboard_button(items.back().event_id, event.key.keysym.sym);
              out << "(keyboard-button (key " << InputManagerSDL::current()->keyid_to_string(event.key.keysym.sym) << "))" << std::endl;
              next_item();
            }
          else if (items.back().mode == ConfigureItem::CONFIGURE_AXIS)
            {
              if (wait_for_plus && minus.type == SDL_KEYDOWN)
                {
                  out << "(keyboard-axis (minus " << InputManagerSDL::current()->keyid_to_string(minus.key.keysym.sym) << ") "
                      << "(plus  " << InputManagerSDL::current()->keyid_to_string(event.key.keysym.sym) << "))" << std::endl;
                  InputManagerSDL::current()->bind_keyboard_axis(items.back().event_id, minus.key.keysym.sym, event.key.keysym.sym);
                  next_item();
                  wait_for_plus = false;
                }
              else if (!wait_for_plus)
                {
                  out << "Print key for other direction" << std::endl;
                  area.set_text(out.str());
                  minus = event;
                  wait_for_plus = true;
                }
            }
        }
      break;
    }
}

/* EOF */
