//  $Id$
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
#include <config.h>

#include "character.hpp"
#include "sector.hpp"
#include "game_session.hpp"
#include "scripting/wrapper.interface.hpp"
#include "dialog_manager.hpp"
#include "script_manager.hpp"
#include "physfs/physfs_stream.hpp"
#include "console.hpp"
#include "sprite3d/manager.hpp"
#include "lisp/properties.hpp"
#include "util.hpp"
#include "windstille_getters.hpp"

#include <exception>

Character::Character(const lisp::Lisp* lisp)
{
  set_useable(true);
  use_verb = "Talk";

  lisp::Properties props(lisp);
  props.get("name", name);
  props.get("pos", pos);
  props.print_unused_warnings("character");
  
  sprite = Sprite3D("3dsprites/heroken.wsprite");
  sprite.set_action("Stand");
}

Character::~Character()
{
}

void
Character::update(float delta)
{   
  sprite.update(delta);
}

void
Character::draw (SceneContext& gc)
{
  sprite.draw(gc, pos, 100);
}

void
Character::use()
{
  //Scripting::add_dialog(Dialog::TOP | Dialog::RIGHT, "portrait");

  std::string filename = basename(GameSession::current()->get_filename());
  filename.erase(filename.find('.'));
  filename = "scripts/" + filename + "/" + name + ".nut";

  try 
    {
      script_manager->run_script_file(filename);
    } catch (std::exception e) {
      console << e.what() << std::endl;
    }
}

/* EOF */
