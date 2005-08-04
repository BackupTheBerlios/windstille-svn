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

#include <stdlib.h>
#include <SDL.h>
#include <iostream>
#include "console.hpp"
#include "globals.hpp"
#include "gameconfig.hpp"
#include "display/display.hpp"
#include "input/input_manager_sdl.hpp"
#include "event_manager.hpp"

EventManager* EventManager::instance_ = 0;

EventManager::EventManager()
{
  
}

EventManager::~EventManager()
{
  
}

EventManager*
EventManager::instance()
{
  if (instance_)
    return instance_;
  else
    return (instance_ = new EventManager());
}

void
EventManager::update()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
    {
      switch(event.type)
        {
        case SDL_QUIT:
          // FIXME: This should be a bit more gentle, but will do for now
          std::cout << "Ctrl-c or Window-close pressed, game is going to quit" << std::endl;
          exit(EXIT_SUCCESS);
          break;
          
        case SDL_ACTIVEEVENT:
          // event.active
          break;
          
        case SDL_VIDEORESIZE:
          // event.resize
          break;
              
        case SDL_VIDEOEXPOSE:
          // event.expose
          break;
                
        case SDL_USEREVENT:
          // event.user
          break;
                    
        case SDL_SYSWMEVENT:
          // event.syswm
          break;
          break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
          if (event.key.state)
            {
              switch (event.key.keysym.sym)
                {
                case SDLK_c:
                  if (debug) {
                    collision_debug = !collision_debug;
                    console << "Collision Debugging " << (collision_debug ? "enabled" : "disabled") << std::endl;
                  }
                  break;

                case SDLK_F10:
                  config->show_fps = ! (config->show_fps);
                  break;
              
                case SDLK_F11:
                  config->use_fullscreen = ! (config->use_fullscreen);
                  Display::set_fullscreen(config->use_fullscreen);
                  break;
              
                case SDLK_F12:
                  // FIXME: Implement me for SDL
                  {
                    std::string filename = "screenshot.png";
                    std::cout << "Saving screenshot *NOT* to: " << filename << std::endl;
                  }
                  break;
              
                default:
                  break;
                }
            }
              
          if (!console.is_active() && event.key.state && event.key.keysym.sym == SDLK_F1)
            {
              console.activate();
            }
          else
            {
              if (InputManagerSDL::current())
                InputManagerSDL::current()->on_event(event);
            }
          break;

        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_JOYAXISMOTION:
        case SDL_JOYBALLMOTION:
        case SDL_JOYHATMOTION:
        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
          if (InputManagerSDL::current())
            InputManagerSDL::current()->on_event(event);
          break;
        }
    }

}

/* EOF */
