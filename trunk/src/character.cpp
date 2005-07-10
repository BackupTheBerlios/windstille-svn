//  $Id: animation_obj.hpp,v 1.3 2003/09/12 16:31:20 grumbel Exp $
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

#include "character.hpp"
#include "sector.hpp"
#include "game_session.hpp"
#include "player.hpp"
#include "scripting/wrapper.interface.hpp"
#include "dialog_manager.hpp"
#include "script_manager.hpp"
#include "physfs/physfs_stream.hpp"
#include "console.hpp"
#include "sprite3d/sprite3d_manager.hpp"

#include <exception>

Character::Character(const int x, const int y, const std::string& arg_name)
  : Entity(x, y), already_talked(false)
{
  name = arg_name;
  sprite = sprite3d_manager->create("3dsprites/heroken.wsprite");
  sprite->set_action("Stand");
}

Character::~Character()
{
  delete sprite;
}

void
Character::update(float delta)
{     
  (void) delta;
  
  Player* player = Sector::current()->get_player();
  
  bool collided = false;
  // FIXME: Should be handled by the collision system
  if (player->get_pos().x > pos.x - 20 && player->get_pos().x < pos.x + 20)
    collided = true;
  
  int state = Sector::current()->get_player()->get_movement_state();
  if (collided && state != Player::STAND_TO_LISTEN && state != Player::LISTEN && !already_talked)
    {
      already_talked = true;
      Scripting::add_dialog(Dialog::TOP | Dialog::RIGHT, "human/portrait");
      std::string level_name = GameSession::current()->get_filename();
      level_name = level_name.substr(level_name.rfind('/') + 1);
      level_name.erase(level_name.find('.'));
      std::string dialog_script = "scripts/" + level_name + "/" + name + ".nut";
      IFileStream f_stream(dialog_script.c_str());
      try 
        {
          script_manager->run_script(f_stream, name);
        } catch (std::exception e) {
          Console::current()->add(e.what());
        }
    }
  if (!collided)
    already_talked = false;
}

void
Character::draw (SceneContext& gc)
{
  sprite->draw(gc, pos);
}

void
Character::collision(const CollisionData& data, CollisionObject& other)
{
  (void) data;
  (void) other;
}

