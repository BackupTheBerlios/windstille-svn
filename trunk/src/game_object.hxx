//  $Id: game_object.hxx,v 1.2 2003/08/12 08:24:41 grumbel Exp $
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

#ifndef GAME_OBJECT_HXX
#define GAME_OBJECT_HXX

#include "display/scene_context.hxx"

class Sector;

class GameObject
{
private:
  static Sector* world;
  bool remove_;
protected:
  void remove () { remove_= true; } 
public:
  bool is_removable () { return remove_; } 
  static void set_world (Sector* w) { world = w; }
  
  Sector* get_world () { return world; }

  GameObject() : remove_ (false) {}

  virtual void draw (SceneContext& sc) =0;
  virtual void update (float) =0;
};

#endif

/* EOF */
