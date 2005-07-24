//  $Id: trigger.cxx,v 1.3 2003/09/21 17:34:00 grumbel Exp $
// 
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#include <list>
#include "sector.hpp"
#include "trigger.hpp"
#include "player.hpp"
#include "script_manager.hpp"
#include "lisp/lisp.hpp"
#include "lisp/properties.hpp"

Trigger::Trigger(const lisp::Lisp* lisp)
  : triggered(false), one_time_trigger(false)
{
  using namespace lisp;
  float x = -1;
  float y = -1;
  float width = -1;
  float height = -1;
  
  Properties props(lisp);
  props.get("x", x);
  props.get("y", y);
  props.get("width", width);
  props.get("height", height);
  props.get("script", script);
  props.get("one_time_trigger", one_time_trigger);
  props.print_unused_warnings("trigger");

  if(x < 0 || y < 0 || width < 0 || height < 0)
    throw std::runtime_error("Invalid or missing area in Trigger object");
 
  area.left = x;
  area.top = y;
  area.right = area.left + width;
  area.bottom = area.top + height;
}

Trigger::~Trigger()
{
}

void
Trigger::draw (SceneContext& )
{
}

void
Trigger::update (float )
{
  //FIXME use proper collision detection
  Player* player = Player::current();
  if(!area.is_inside(CL_Pointf(player->get_pos().x,
                                  player->get_pos().y))) {
    last_trigger = false;
    return;
  }
  
  if(triggered && one_time_trigger)
    return;

  if(last_trigger == false) {
    triggered = true;
    try {
      script_manager->run_script(script, "TriggerObject");
    } catch(std::exception& e) {
      std::cerr << "Couldn't run trigger-script: " << e.what() << "\n";
    }
  }
  last_trigger = true;
}

/* EOF */
