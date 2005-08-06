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

#include <math.h>
#include <iostream>
#include "display/display.hpp"
#include "windstille_main.hpp"
#include "screen.hpp"
#include "fonts.hpp"
#include "globals.hpp"
#include "console.hpp"
#include "gameconfig.hpp"
#include "event_manager.hpp"
#include "input/input_manager.hpp"
#include "input/input_manager_sdl.hpp"
#include "sound/sound_manager.hpp"

namespace Windstille {

Screen::Screen()
  : frames(0),
    time_counter(0),
    fps_counter(0),
    fps_save(0),
    overlap_delta(0)
{
  ticks = SDL_GetTicks();
}

Screen::~Screen()
{
}

void 
Screen::display()
{
  /// Amount of time the world moves forward each update(), this is
  /// independed of the number of frames and always constant
  static const float step = 10/1000.0f;

  Uint32 now = SDL_GetTicks();
  float delta = static_cast<float>(now - ticks) / 1000.0f;
  ticks = now;
  
  /*only used in debug. Limits page flipping so one can see what the framerate
  is like without it being clipped by vsync*/  
  static int last_flip_time = 0;

  ++frames;
 
  float time_delta = delta + overlap_delta;
  while (time_delta > step)
    {
      console.update(step);
      update(step);
      InputManager::clear();
  
      time_delta -= step;
    }

  overlap_delta = time_delta;

  sound_manager->update();

  {
    draw();
    
    if (config->show_fps)
      draw_fps(delta);
 
    console.draw();
  }

  if (!debug || now - last_flip_time > 20)
    {
      SDL_GL_SwapBuffers();
      last_flip_time = now;
    }
  ++frames;

  poll_events();
}

void 
Screen::draw_fps(float delta)
{
  time_counter += delta;
  ++fps_counter;

  if(time_counter > 1) {
    fps_save = fps_counter;
    time_counter = fmodf(time_counter, 1.0);
    fps_counter = 0;
  }
  
  char output[20];
  snprintf(output, sizeof(output), "FPS: %d", fps_save);
  
  Fonts::ttffont->draw(Display::get_width() - 100, 30, output);
}

void
Screen::poll_events()
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
                  handle_event(event);
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
          handle_event(event);
          break;
      }
    }
}

} // namespace Windstille

/* EOF */
