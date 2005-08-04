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

#include <ClanLib/display.h>
#include <ClanLib/Core/System/system.h>

#include <math.h>
#include "display/display.hpp"
#include "windstille_main.hpp"
#include "screen.hpp"
#include "fonts.hpp"
#include "globals.hpp"
#include "console.hpp"
#include "gameconfig.hpp"
#include "input/input_manager.hpp"
#include "sound/sound_manager.hpp"

namespace Windstille {

Screen::Screen()
  : frames(0),
    time_counter(0),
    fps_counter(0),
    fps_save(0),
    overlap_delta(0)
{
  ticks = CL_System::get_time();
}

Screen::~Screen()
{
  CL_Keyboard::sig_key_down().disconnect(slot);
}

void 
Screen::display()
{
  /// Amount of time the world moves forward each update(), this is
  /// independed of the number of frames and always constant
  static const float step = 10/1000.0f;

  slot = CL_Keyboard::sig_key_down().connect(this, &Screen::key_down);

  unsigned int now = CL_System::get_time();
  float delta = static_cast<float>(now - ticks) / 1000.0f;
  ticks = now;

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
    CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
    state.set_active(); 
    state.setup_2d(); 

    draw();
    
    if (config->show_fps)
      draw_fps(delta);
  
    console.draw();
  }

  CL_Display::flip(0);
  ++frames;
  
  CL_System::keep_alive ();
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
  
  Fonts::ttffont->draw(VDisplay::get_width() - 100, 30, output);
}

void
Screen::key_down(const CL_InputEvent& event)
{
  switch (event.id) 
    {
    case CL_KEY_F1:
      if (!console.is_active())
        console.activate();
      break;

    case CL_KEY_C:
      if(debug) {
        collision_debug = !collision_debug;
        console << "Collision Debugging " << (collision_debug ? "enabled" : "disabled") << std::endl;
      }
      break;
    case CL_KEY_F10:
      config->show_fps = ! (config->show_fps);
      break;
    case CL_KEY_F11:
      config->use_fullscreen = ! (config->use_fullscreen);
      
      if (config->use_fullscreen)
        CL_Display::set_fullscreen(config->screen_width,
                                 config->screen_height, 32);
      else
        CL_Display::set_windowed();
      break;
    case CL_KEY_F12:
      std::string filename = "screenshot.png";
      std::cout << "Saving screenshot to: " << filename << std::endl;
      CL_ProviderFactory::save(CL_Display::get_front_buffer(),
                               filename);
      break;
    }
}

} // namespace Windstille

/* EOF */
