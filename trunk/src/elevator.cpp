/*  $Id$
**   __      __.__            .___        __  .__.__  .__
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "sector.hpp"
#include "collision/collision_engine.hpp"
#include "elevator.hpp"

Elevator::Elevator(const lisp::Lisp* lisp)
{
  pos.x = 1312;
  pos.y =  832;
  size  = CL_Size(128, 64);

  colobject = new CollisionObject(CL_Rectf(CL_Pointf(0,0), size));
  Sector::current()->get_collision_engine()->add(colobject);
  colobject->set_pos(pos);
}

Elevator::~Elevator()
{
  Sector::current()->get_collision_engine()->remove(colobject);
  delete colobject;
}

void
Elevator::draw(SceneContext& sc)
{
  
}

void
Elevator::update(float delta)
{
  
}

/* EOF */
