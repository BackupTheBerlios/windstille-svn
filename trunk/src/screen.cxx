//  $Id: screen.cxx,v 1.3 2003/10/10 21:06:22 grumbel Exp $
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

#include "windstille_main.hxx"
#include "delta_manager.hxx"
#include "screen.hxx"
#include "fonts.hxx"
#include "sound/sound_manager.hpp"

namespace Windstille {

Screen::Screen()
  : show_fps(true), frames(0)
{
  slot = CL_Keyboard::sig_key_down().connect(this, &Screen::key_down);
}

void 
Screen::display()
{
  do_pause = false;
  do_quit  = false;

  on_startup();

  DeltaManager delta_manager;
  
  while (!do_quit)
    {
      draw();
      float delta = delta_manager.getset ();
      float step = 10/1000.0f;
      if (show_fps)
        draw_fps(delta);
      CL_Display::flip();
      
      ++frames;
      
      while (delta > step)
        {
          update(step);
          delta -= step;
        }
      // FIXME: non constant delta isn't a good idea
      update(delta);
      
      // update(0.020f);

      sound_manager->update();
      CL_System::keep_alive ();
      //CL_System::sleep (1);
    }

  on_shutdown();
}

void 
Screen::draw_fps(float delta)
{
  static float time_counter = 0;
  static int fps_counter = 0;
  static int fps_save = 0;

  time_counter += delta;
  ++fps_counter;

  if (time_counter > 1)
  {
    fps_save = fps_counter;
    time_counter = 0;
    fps_counter = 0;
  }
  
  char output[20];
  sprintf(output, "FPS: %d", fps_save);
  
  Fonts::copyright.set_alpha(1.0f);
  Fonts::copyright.set_alignment(origin_bottom_left);
  Fonts::copyright.draw(CL_Display::get_width() - 100, 30, output);
}

void
Screen::key_down(const CL_InputEvent& event)
{
  switch (event.id)
    {
    case CL_KEY_F10:
        show_fps = !show_fps;
      break;
    case CL_KEY_F11:
      if (CL_Display::is_fullscreen())
        CL_Display::set_windowed();
      else
        CL_Display::set_fullscreen(WindstilleMain::screen_width,
                                   WindstilleMain::screen_height,
                                   32);
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
