//  $Id: dialog_manager.cxx,v 1.3 2003/09/30 16:42:26 grumbel Exp $
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

#include <ClanLib/Display/display.h>
#include <iostream>
#include "fonts.hpp"
#include "game_session.hpp"
#include "input/controller.hpp"
#include "input/input_manager.hpp"
#include "dialog_manager.hpp"
#include "script_manager.hpp"
#include "gameconfig.hpp"

DialogManager* DialogManager::current_ = 0;

Dialog::Dialog(int alignment, const std::string& portrait,
               const std::string& text)
  : portrait(portrait, resources), text(text), alignment(alignment)
{
}

DialogManager::DialogManager()
{
  current_ = this;
  current_dialog = -1;
  current_choice = 0;
}

void
DialogManager::add_dialog(int alignment, const std::string& portrait,
                          const std::string& text)
{
  dialogs.push_back(Dialog(alignment, portrait, text));
  current_dialog = dialogs.size()-1;
}

void
DialogManager::add_answer(const std::string& text, const std::string& script)
{
  Dialog& dialog = dialogs[current_dialog];
  dialog.answers.push_back(std::make_pair(text, script));
}

void
DialogManager::draw()
{
  if (current_dialog != -1)
    {
      static const int dialog_width = 600;
      static const int outer_border_x = 20;
      static const int outer_border_y = 20;
      static const int portrait_border_x = 10;
      static const int portrait_border_y = 10;
      static const int text_border_x = 10;
      static const int text_border_y = 10;
      static const int portrait_width = 180;
      static const int portrait_height = 192;

      Dialog& dialog = dialogs[current_dialog];

      CL_Point pos;
      if(dialog.alignment & Dialog::LEFT) {
        pos.x = outer_border_x;
      } else if(dialog.alignment & Dialog::RIGHT) {
        pos.x = config->screen_width - dialog_width - outer_border_x;
      } else {
        pos.x = (config->screen_width - dialog_width) / 2;
      }

      int text_width
        = dialog_width - portrait_height - portrait_border_x*2 - text_border_x;
      CL_Rect text_rect = Fonts::dialog.bounding_rect(
        CL_Rect(CL_Point(pos.x + portrait_width + portrait_border_x*2, 0),
                CL_Size(text_width, 600)), dialog.text);

      int dialog_height = std::max(portrait_height + portrait_border_y*2,
                                   text_rect.get_height() + text_border_y*2);

      if(dialog.answers.size() > 0) 
          dialog_height += 80;

      if(dialog.alignment & Dialog::TOP) {
        pos.y = outer_border_y;
      } else if(dialog.alignment & Dialog::BOTTOM) {
        pos.y = config->screen_height - dialog_height - outer_border_y;
      } else {
        pos.y = (config->screen_height - dialog_height) / 2;
      }

      text_rect.bottom = text_rect.top + text_rect.get_height();
      text_rect.top = pos.y + text_border_y;

      CL_Size dialog_size(dialog_width, dialog_height);
      
      CL_Display::fill_rect(CL_Rect(pos, dialog_size), 
                            CL_Gradient(CL_Color(0,0,0,228),
                                        CL_Color(0,0,0,228),
                                        CL_Color(0,0,0,128),
                                        CL_Color(0,0,0,128)));
      CL_Display::draw_rect(CL_Rect(pos, dialog_size),
                            CL_Color(255,255,255, 80));
      
      CL_Display::flush();
      
      dialog.portrait.draw(pos.x + portrait_border_x,
                           pos.y + portrait_border_y);
      Fonts::dialog.set_alignment(origin_top_left);
      Fonts::dialog_h.set_alignment(origin_top_left);

      Fonts::dialog.draw(text_rect, dialog.text);

      Fonts::dialog.set_alignment(origin_top_center);
      Fonts::dialog_h.set_alignment(origin_top_center);

      if (dialog.answers.size() > 0)
        {
          int w = (dialog_width/dialog.answers.size());
          for(Dialogs::size_type i = 0; i < dialog.answers.size(); ++i)
            {
              if (int(i) == current_choice)
                Fonts::dialog_h.draw(pos.x + i*w + w/2,
                                     pos.y + text_rect.get_height() 
                                        + text_border_y*2 + 20,
                                     "[" + dialog.answers[i].first + "]");
              else
                Fonts::dialog.draw(pos.x + i*w + w/2,
                                   pos.y + text_rect.get_height() 
                                        + text_border_y*2 + 20,
                                   dialog.answers[i].first);
            }
        }
    }
}

void
DialogManager::update(float )
{
  if (current_dialog != -1)
    {
      InputEventLst events = InputManager::get_controller().get_events();

      for (InputEventLst::iterator i = events.begin(); i != events.end(); ++i)
        {
          if ((*i).type == BUTTON_EVENT)
            {
          if ((*i).button.name == FIRE_BUTTON && (*i).button.down == true)
            {
              GameSession::current()->set_game_state();
              script_manager->fire_wakeup_event(ScriptManager::DIALOG_CLOSED);
              if (dialogs[current_dialog].answers.size() > 0) {
                script_manager->run_script(
                  dialogs[current_dialog].answers[current_choice].second);
              }
            }
          else if ((*i).button.name == LEFT_BUTTON && (*i).button.down == true)
            {
              current_choice -= 1;
            }
          else if ((*i).button.name == RIGHT_BUTTON && (*i).button.down == true)
            {
              current_choice += 1;
            }
            }
        }

      if (current_choice < 0)
        current_choice = 0;
      else if (current_choice >= int(dialogs[current_dialog].answers.size()))
        current_choice = int(dialogs[current_dialog].answers.size()) - 1;
    }
  else
    {
      std::cout << "DialogManager: No dialog available" << std::endl;
      GameSession::current()->set_game_state();
    }
}

void
DialogManager::clear()
{
  current_dialog = 0;
  current_choice = 0;
  dialogs.clear();
}

/* EOF */
