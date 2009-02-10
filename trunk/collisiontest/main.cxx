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
#include <ClanLib/core.h>
#include <ClanLib/gl.h>
#include "collision.hxx"

int main()
{
  CL_SetupCore    core_setup;
  CL_SetupGL      gl_setup;
  CL_SetupDisplay display_setup;

  // Init Display
  CL_DisplayWindow window("Physic Test", 800, 600);
  
  // Init Physic Engine
  CollisionEngine physics;

  bool create_object = false;
  int x_pos;
  int y_pos;

  PhysicalObject *o1,*o2,*o3,*o4,*o5;
  // ground
  physics.add_object(o1=new PhysicalObject(32, 600-32, 800-32, 32));
  physics.add_object(o2=new PhysicalObject(0, 0, 32, 600));
  physics.add_object(o3=new PhysicalObject(800-32, 0, 32, 600));
  physics.add_object(o4=new PhysicalObject(32, 0, 800-32, 32));
  physics.add_object(o5=new PhysicalObject(400, 300, 100, 100));
  o1->set_movable(false);
  o2->set_movable(false);
  o3->set_movable(false);
  o4->set_movable(false);
  o5->set_movable(false);

  Elevator *e=new Elevator;
  e->insertPoint(CL_Vector2(200,500));
  e->insertPoint(CL_Vector2(200,400));
  physics.add_object(e);

  Elevator *e2=new Elevator;
  e2->insertPoint(CL_Vector2(500,530));
  e2->insertPoint(CL_Vector2(700,530));
  physics.add_object(e2);

  Player *player=new Player(100,100,32,64);
  physics.add_object(player);
  

  bool jump=false;

  // Run Main Loop
  while(!CL_Keyboard::get_keycode(CL_KEY_ESCAPE))
    {
      CL_Display::clear();

      physics.update(0.033f);
      //      physics.update(0.033f);
      //for(int i=0;i<1000;i++)
	physics.draw();

      CL_Display::flip();

      CL_System::keep_alive();

      if (CL_Mouse::get_keycode(CL_MOUSE_LEFT) && !create_object)
        {
          create_object = true;
          x_pos = CL_Mouse::get_x();
          y_pos = CL_Mouse::get_y();
        }
      else if (create_object && !CL_Mouse::get_keycode(CL_MOUSE_LEFT))
        {
          create_object = false;

          int n_x_pos = CL_Mouse::get_x();
          int n_y_pos = CL_Mouse::get_y();

          CollisionObject& obj = *physics.add_object(new PhysicalObject(x_pos, y_pos, 16, 32));
          obj.set_movement (CL_Vector2 ( x_pos - n_x_pos, y_pos - n_y_pos));
        }

      if(CL_Keyboard::get_keycode(CL_KEY_RIGHT))
	{
	  player->set_movement(CL_Vector2(15,player->get_movement().y));
	}
      else if(CL_Keyboard::get_keycode(CL_KEY_LEFT))
	{
	  player->set_movement(CL_Vector2(-15,player->get_movement().y));
	}
      else
	player->set_movement(CL_Vector2(0,player->get_movement().y));
      if(CL_Keyboard::get_keycode(CL_KEY_UP))
	{
	  if(player->on_ground() && !jump)
	    {
	      player->set_movement(CL_Vector2(player->get_movement().x,-40));
	      player->detachElevator();
	      jump=true;
	    }
	}
      else
	jump=false;
    }

  // Deinit Collision Engine

  // Deinit Display
}

/* EOF */
