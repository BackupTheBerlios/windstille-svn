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
#include "input_manager_sdl.hpp"

InputManagerSDL* InputManagerSDL::current_ = 0;

#define DEAD_ZONE 4096

InputManagerSDL::InputManagerSDL()
{
  current_ = this;
  
  int num_joysticks = SDL_NumJoysticks();

  if (num_joysticks > 0)
    /*SDL_Joystick* joy =*/ SDL_JoystickOpen(0);
}

InputManagerSDL::~InputManagerSDL()
{
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


        switch (event.key.keysym.sym)
          {
          case SDLK_LEFT:
            add_button_event(LEFT_BUTTON, event.key.state);
            break;

          case SDLK_RIGHT:
            add_button_event(RIGHT_BUTTON, event.key.state);
            break;

          case SDLK_UP:
            add_button_event(UP_BUTTON, event.key.state);
            break;

          case SDLK_DOWN:
            add_button_event(DOWN_BUTTON, event.key.state);
            break;

          case SDLK_a:
            add_button_event(RUN_BUTTON, event.key.state);
            break;

          case SDLK_d:
            add_button_event(FIRE_BUTTON, event.key.state);
            break;

          case SDLK_s:
            add_button_event(JUMP_BUTTON, event.key.state);
            break;

          case SDLK_w:
            add_button_event(USE_BUTTON, event.key.state);
            break;
            
          default:
            break;
          }
      }
      break;

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
      if (event.jaxis.axis == 0) 
        {
          if (event.jaxis.value < -DEAD_ZONE)
            {
              add_button_event(LEFT_BUTTON, true);   
              add_button_event(RIGHT_BUTTON, false);
            }
          else if (event.jaxis.value > DEAD_ZONE)
            {
              add_button_event(LEFT_BUTTON, false); 
              add_button_event(RIGHT_BUTTON, true); 
            }
          else
            {
              add_button_event(LEFT_BUTTON, false);
              add_button_event(RIGHT_BUTTON, false);
            }
        }
      else if (event.jaxis.axis == 1) 
        {
          if (event.jaxis.value < -DEAD_ZONE)
            {
              add_button_event(UP_BUTTON, true);   
              add_button_event(DOWN_BUTTON, false);
            }
          else if (event.jaxis.value > DEAD_ZONE)
            {
              add_button_event(UP_BUTTON, false); 
              add_button_event(DOWN_BUTTON, true); 
            }
          else
            {
              add_button_event(UP_BUTTON, false);
              add_button_event(DOWN_BUTTON, false);
            }
        }
      break;

    case SDL_JOYBALLMOTION:
      // event.jball
      break;
          
    case SDL_JOYHATMOTION:
      // event.jhat
      break;
          
    case SDL_JOYBUTTONUP:
    case SDL_JOYBUTTONDOWN:
      if (event.jbutton.button == 0)
        {
          add_button_event(FIRE_BUTTON, event.jbutton.state); 
        }
      else if (event.jbutton.button == 1)
        {
          add_button_event(USE_BUTTON, event.jbutton.state); 
        }
      else if (event.jbutton.button == 2)
        {
          add_button_event(JUMP_BUTTON, event.jbutton.state);
        }
      else if (event.jbutton.button == 3)
        {
          add_button_event(RUN_BUTTON, event.jbutton.state);
        }
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

/* EOF */
