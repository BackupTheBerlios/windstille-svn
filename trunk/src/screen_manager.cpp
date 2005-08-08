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
#include "game_session.hpp"
#include "input/input_manager_sdl.hpp"
#include "display/display.hpp"
#include "globals.hpp"
#include "screen.hpp"
#include "fonts.hpp"
#include "gameconfig.hpp"
#include "input/input_manager.hpp"
#include "sound/sound_manager.hpp"
#include "screen_manager.hpp"

ScreenManager screen_manager; 

ScreenManager::ScreenManager()
  : time_counter(0),
    frame_counter(0),
    last_fps(0),
    overlap_delta(0),
    do_quit(false)
{
  screen = 0;
  ticks = 0;
}

ScreenManager::~ScreenManager()
{
}

void
ScreenManager::run()
{
  do_quit = false;

  ticks = SDL_GetTicks();

  while (!do_quit)
    {
      /// Amount of time the world moves forward each update(), this is
      /// independed of the number of frames and always constant
      static const float step = 10/1000.0f;

      Uint32 now = SDL_GetTicks();
      float delta = static_cast<float>(now - ticks) / 1000.0f + overlap_delta;
      ticks = now;

      time_counter  += delta;

      while (delta > step)
        {
          console.update(step);
          screen->update(step, InputManager::get_controller());
          InputManager::clear();
  
          delta -= step;
        }

      overlap_delta = delta;

      sound_manager->update();

      screen->draw();
      
      console.draw();

      if (config->show_fps)
        draw_fps();

      SDL_GL_SwapBuffers();
      frame_counter += 1;

      poll_events();
    }
}

void
ScreenManager::poll_events()
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
    {
      switch(event.type)
        {
        case SDL_QUIT:
          // FIXME: This should be a bit more gentle, but will do for now
          std::cout << "Ctrl-c or Window-close pressed, game is going to quit" << std::endl;
          quit();
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
                  if (!console.is_active())
                    screen->handle_event(event);
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

        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEMOTION:
        case SDL_JOYAXISMOTION:
        case SDL_JOYBALLMOTION:
        case SDL_JOYHATMOTION:
        case SDL_JOYBUTTONUP:
        case SDL_JOYBUTTONDOWN:
          if (InputManagerSDL::current())
            InputManagerSDL::current()->on_event(event);
          break;
        
        default:
          screen->handle_event(event);
          break;
      }
    }
}

void 
ScreenManager::draw_fps()
{
  if(time_counter > 1) 
    {
      last_fps = int(frame_counter / time_counter);

      time_counter  = fmodf(time_counter, 1.0);
      frame_counter = 0;
    }
  
  char output[20];
  snprintf(output, sizeof(output), "FPS: %d", last_fps);
  Fonts::ttffont->draw(Display::get_width() - 100, 30, output);
}

void
ScreenManager::set_screen(Screen* s)
{
  delete screen;
  screen = s;
}

void
ScreenManager::quit()
{
  do_quit = true;
}

/* EOF */
