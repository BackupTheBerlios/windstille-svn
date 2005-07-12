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
#include "util.hpp"
#include "lisp/list_iterator.hpp"

#include <exception>

Character::Character(const lisp::Lisp* lisp)
  : Entity(0, 0), already_talked(false)
{
  lisp::ListIterator iter(lisp);
  while(iter.next()) {
    if(iter.item() == "name") {
      name = iter.value().get_string();
    } else if(iter.item() == "x") {
      pos.x = iter.value().get_float();
    } else if(iter.item() == "y") {
      pos.y = iter.value().get_float();
      std::cerr << "Skipping unknown attribute '" 
                << iter.item() << "' in Character\n";
    }
  }
  
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
  
  if (collided && !already_talked)
    {
      already_talked = true;
      Scripting::add_dialog(Dialog::TOP | Dialog::RIGHT, "human/portrait");
	  
      //first add standard dialog functions
      std::string filename = ::datadir + "scripts/" + "dialog.nut";
      std::string script;
      file_to_string(filename, script);
      
      //then add this character's dialog script
      filename = basename(GameSession::current()->get_filename());
      filename.erase(filename.find('.'));
      filename = ::datadir + "scripts/" + filename + "/" + name + ".nut";
      file_to_string(filename, script);
    
      try 
        {
          script_manager->run_script(script, name);
        } catch (std::exception e) {
          Console::current()->add(e.what());
        }
    }
  if (!collided)
    already_talked = false;
    
  sprite->update(delta);
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

