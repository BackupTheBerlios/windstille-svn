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
#include "controller_def.hpp"
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
  Uint8* keystate = SDL_GetKeyState(0);

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
            if (event.key.state)
              add_axis_event(X_AXIS, -1.0);
            else if (!keystate[SDLK_RIGHT])
              add_axis_event(X_AXIS, 0);
            break;

          case SDLK_RIGHT:
            if (event.key.state)
              add_axis_event(X_AXIS, 1.0);
            else if (!keystate[SDLK_LEFT])
              add_axis_event(X_AXIS, 0);
            break;

          case SDLK_UP:
            if (event.key.state)
              add_axis_event(Y_AXIS, -1.0);
            else
              add_axis_event(Y_AXIS, 0);
            break;

          case SDLK_DOWN:
            if (event.key.state)
              add_axis_event(Y_AXIS, 1.0);
            else
              add_axis_event(Y_AXIS, 0);
            break;

          case SDLK_a:
            add_button_event(TERTIARY_BUTTON, event.key.state);
            break;

          case SDLK_s:
            add_button_event(PRIMARY_BUTTON, event.key.state);
            break;

          case SDLK_d:
            add_button_event(SECONDARY_BUTTON, event.key.state);
            break;

          case SDLK_w:
            add_button_event(PDA_BUTTON, event.key.state);
            break;

          case SDLK_LSHIFT:
            add_button_event(AIM_BUTTON, event.key.state);
            break;

          case SDLK_PAUSE:
          case SDLK_p:
            add_button_event(PAUSE_BUTTON, event.key.state);
            break;
           
          default:
            break;
          }
      }
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
      if (event.jaxis.axis == 0) 
        {
          if (event.jaxis.value < -DEAD_ZONE)
            {
              add_axis_event(X_AXIS, event.jaxis.value/32768.0f);
            }
          else if (event.jaxis.value > DEAD_ZONE)
            {
              add_axis_event(X_AXIS, event.jaxis.value/32767.0f);
            }
          else
            {
              add_axis_event(X_AXIS, 0);
            }
        }
      else if (event.jaxis.axis == 1) 
        {
          if (event.jaxis.value < -DEAD_ZONE)
            {
              add_axis_event(Y_AXIS, event.jaxis.value/32768.0f);
            }
          else if (event.jaxis.value > DEAD_ZONE)
            {
              add_axis_event(Y_AXIS, event.jaxis.value/32767.0f);
            }
          else
            {
              add_axis_event(Y_AXIS, 0);
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
          add_button_event(PDA_BUTTON, event.jbutton.state); 
        }
      else if (event.jbutton.button == 1)
        {
          add_button_event(TERTIARY_BUTTON, event.jbutton.state); 
        }
      else if (event.jbutton.button == 2)
        {
          add_button_event(SECONDARY_BUTTON, event.jbutton.state);
        }
      else if (event.jbutton.button == 3)
        {
          add_button_event(PRIMARY_BUTTON, event.jbutton.state);
        }
      else if (event.jbutton.button == 7)
        {
          add_button_event(AIM_BUTTON, event.jbutton.state);
        }
      else if (event.jbutton.button == 9)
        {
          add_button_event(PAUSE_BUTTON, event.jbutton.state);
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
