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

#include <ClanLib/Display/display.h>
#include <iostream>
#include "fonts.hpp"
#include "game_session.hpp"
#include "input/controller.hpp"
#include "input/input_manager.hpp"
#include "dialog_manager.hpp"
#include "script_manager.hpp"
#include "text_area.hpp"
#include "display/display.hpp"
#include "gameconfig.hpp"

DialogManager* DialogManager::current_ = 0;

DialogManager::DialogManager()
{
  current_ = this;
  text_area = 0;
}

DialogManager::~DialogManager()
{
  delete text_area;
}

void
DialogManager::add_dialog(int alignment_, const std::string& portrait_, const std::string& text_)
{
  progress  = 0;
  delay     = 0.0;
  alignment = alignment_;
  portrait  = Sprite(portrait_);
  text      = text_;

  static const int dialog_width = 600;
  static const int outer_border_x = 20;
  static const int outer_border_y = 20;
  static const int portrait_border_x = 10;
  static const int portrait_border_y = 10;
  static const int text_border_x = 10;
  static const int text_border_y = 10;
  static const int portrait_width = 180;
  static const int portrait_height = 192;

  CL_Point pos(0,0);
  if(alignment & LEFT) {
    pos.x = outer_border_x;
  } else if(alignment & RIGHT) {
    pos.x = config->screen_width - dialog_width - outer_border_x;
  } else {
    pos.x = (config->screen_width - dialog_width) / 2;
  }
      
  int text_width
    = dialog_width - portrait_height - portrait_border_x*2 - text_border_x;
  Rect text_rect = Rect(CL_Point(pos.x + portrait_width + portrait_border_x*2, 0),
                        CL_Size(500, 200)); // FIXME: use real bounding box calc
  
  text_rect.bottom = text_rect.top + text_rect.get_height();
  text_rect.top    = pos.y + text_border_y;

  int dialog_height = std::max(portrait_height + portrait_border_y*2,
                               text_rect.get_height() + text_border_y*2);

  if(alignment & TOP) {
    pos.y = outer_border_y;
  } else if(alignment & BOTTOM) {
    pos.y = config->screen_height - dialog_height - outer_border_y;
  } else {
    pos.y = (config->screen_height - dialog_height) / 2;
  }

  text_rect.bottom = text_rect.top + text_rect.get_height();
  text_rect.top = pos.y + text_border_y;

  CL_Size dialog_size(dialog_width, dialog_height);


  delete text_area;
  text_area = new TextArea(Rect(CL_Point(text_rect.left, text_rect.top + Fonts::ttfdialog->get_height()),
                                   CL_Size(text_width, 200)));
  text_area->set_font(Fonts::ttfdialog);
  text_area->set_text(text);
}

void
DialogManager::draw()
{
  static const int outer_border_x = 20;
  static const int outer_border_y = 20;
  static const int portrait_border_x = 10;
  static const int portrait_border_y = 10;
  static const int dialog_width = 600;
  static const int portrait_height = 192;
  static const int text_border_y = 10;

  int dialog_height = std::max(portrait_height + portrait_border_y*2,
                               int(text_area->get_rect().get_height()
                                   + text_border_y * 2.0f));

  CL_Point pos(0,0);
  if(alignment & LEFT) {
    pos.x = outer_border_x;
  } else if(alignment & RIGHT) {
    pos.x = config->screen_width - dialog_width - outer_border_x;
  } else {
    pos.x = (config->screen_width - dialog_width) / 2;
  }

  if(alignment & TOP) {
    pos.y = outer_border_y;
  } else if(alignment & BOTTOM) {
    pos.y = config->screen_height - dialog_height - outer_border_y;
  } else {
    pos.y = (config->screen_height - dialog_height) / 2;
  }

  Rectf rect(pos.x,
             pos.y,
             pos.x + dialog_width,
             pos.y + 200);

  VDisplay::fill_rounded_rect(rect, 16.0f,
                              Color(0, 0, 0.3f, 0.5f));
  VDisplay::draw_rounded_rect(rect, 16.0f,
                              Color(0.6f, 1.0f, 1.0f, 0.8f));

  portrait.draw(Vector(pos.x + portrait_border_x,
                       pos.y + portrait_border_y));
  
  text_area->draw();
}

void
DialogManager::update(float delta)
{
  text_area->update(delta);

  delay += delta;
  if (InputManager::get_controller().get_button_state(FIRE_BUTTON) && delay > 0.2 && progress * text_speed < text.size())
    progress = int(text.size()) / text_speed;
  else
    progress += delta;

  InputEventLst events = InputManager::get_controller().get_events();
	
  for (InputEventLst::iterator i = events.begin(); i != events.end(); ++i)
    {
      if ((*i).type == BUTTON_EVENT)
        {
          if ((*i).button.name == FIRE_BUTTON && (*i).button.down == true
              && int(progress * text_speed) > int(text.size()))
            {
              GameSession::current()->set_game_state();
              script_manager->fire_wakeup_event(ScriptManager::DIALOG_CLOSED);
            } 
        }
    }
}

/* EOF */
