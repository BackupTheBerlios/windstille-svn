//  $Id: windstille_menu.cxx,v 1.11 2003/11/13 12:59:42 grumbel Exp $
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

#include <iostream>
#include <ClanLib/Display/display.h>
#include <ClanLib/Display/display_window.h>
#include <ClanLib/Display/keys.h>
#include <ClanLib/Core/System/system.h>
#include <ClanLib/Display/keyboard.h>
#include "globals.hpp"
#include "fonts.hpp"
#include "input/controller.hpp"
#include "windstille_menu.hpp"
#include "game_session.hpp"
#include "sound/sound_manager.hpp"
#include "input/input_manager.hpp"
#include "tinygettext/gettext.hpp"

WindstilleMenu::WindstilleMenu()
  : background("menu_background", resources),
    windstille("logo_large", resources)
{
  if (debug) std::cout << "Starting Menu" << std::endl;
  current_choice = 0;
  windstille.set_alignment(origin_center);
  passed_time = 0;
  
  sound_manager->play_music("music/jingle.ogg");
}

WindstilleMenu::~WindstilleMenu()
{
  
}

void
WindstilleMenu::update(float delta)
{
  InputManager::update(delta);

  passed_time += delta;

  background.update(delta);
  windstille.update(delta);

  if (CL_Keyboard::get_keycode(CL_KEY_ESCAPE))
    game_main_state = QUIT_GAME;

  InputEventLst events = InputManager::get_controller().get_events();

  for (InputEventLst::iterator i = events.begin();
       i != events.end(); ++i)
    {
      if ((*i).type == BUTTON_EVENT)
        {
          if ((*i).button.name == FIRE_BUTTON && (*i).button.down == true)
            {
              if (current_choice == 1)// QUIT
                {
                  sound_manager->stop_music();
                  fadeout();                  
                  game_main_state = QUIT_GAME;
                  break;
                }
              else if (current_choice == 0) // start game
                {
                  InputManager::clear();
                  sound_manager->stop_music();
                  fadeout();
                  game_main_state = LOAD_GAME_SESSION;
                  break;
                }
            }
          else if ((*i).button.name == UP_BUTTON && (*i).button.down == true)
            {
              current_choice -= 1;
              passed_time = 0;
            }
          else if ((*i).button.name == DOWN_BUTTON && (*i).button.down == true)
            {
              current_choice += 1;
              passed_time = 0;
            }
        }
    }

  while (CL_Keyboard::get_keycode(CL_KEY_ESCAPE))
    CL_System::keep_alive();

  if (current_choice < 0)
    {
      current_choice = 1;
    }
  else if (current_choice > 1 )
    current_choice = 0;
}

void
WindstilleMenu::draw()
{
  //std::cout << "Draw... " << std::endl;
  background.draw(0,0);

  if (0) // ugly wooble
    {
      windstille.set_color(
                           sin(passed_time*3.141f)*.2f + .8f,
                           sin(passed_time*3.141f)*.2f + .8f,
                           sin(passed_time*3.141f)*.2f + .8f
                           );
      windstille.set_scale(cos(passed_time*3.141f)*.05f + .95f,
                           cos(passed_time*3.141f)*.05f + .95f);
    }

  windstille.draw(CL_Display::get_width()/2, 145);
  
  Fonts::menu.set_alignment(origin_bottom_center);
  Fonts::menu_h.set_alignment(origin_bottom_center);

  Fonts::menu_h.set_alpha(cos(passed_time*3.141f)*.4f + .6f);

  Fonts::menu.draw(580, 330, _("[Start Game]"));
  if (current_choice == 0)
    {
      Fonts::menu_h.draw(580, 330, _("[Start Game]"));
    }

  Fonts::menu.draw(580, 440, _("[Quit]"));
  if (current_choice == 1)
    {      
      Fonts::menu_h.draw(580, 440, _("[Quit]"));
    }

  std::string copyright = "Windstille " PACKAGE_VERSION "\n";
  copyright += _("Copyright (c) 2003 Ingo Ruhnke <grumbel@gmx.de>\n"
                 "This game comes with ABSOLUTELY NO WARRANTY. "
                 "This is free software, and you are welcome\n"
                 "to redistribute it under certain conditions; "
                 "see the file COPYING for details.\n");

  Fonts::copyright.set_alignment(origin_bottom_left);
  Fonts::copyright.draw(15, CL_Display::get_height() - 10, copyright.c_str());
}

void
WindstilleMenu::fadeout()
{
  int alpha = 0;
  while (alpha <= 255)
    {
      background.draw(0,0);

      windstille.draw(CL_Display::get_width()/2,
                      145);
      CL_Display::fill_rect(CL_Rect(0, 0, 
                                    CL_Display::get_width(), CL_Display::get_height()),
                            CL_Color(0,0,0, std::min(alpha, 255)));
      CL_Display::flip(0);
      sound_manager->update();
      CL_System::keep_alive();
      CL_System::sleep(50);
      alpha += 15;
    }
}

/* EOF */
