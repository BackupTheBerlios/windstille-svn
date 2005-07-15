//  $Id: game_object.hpp,v 1.2 2003/08/12 08:24:41 grumbel Exp $
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

#include <string>
#include "display/scene_context.hpp"
#include "refcounter.hpp"

class Sector;

class GameObject : public RefCounter
{
private:
  static Sector* world;
  
protected:
  bool removable;

  /**
   * name of the GameObject. Note: You should not change it anymore once the
   * object has been added to a Sector
   */
  std::string name;

  /** If a object is 'active = false' it will neither be drawn or updated */
  bool active;
public:
  GameObject() : removable(false), active(true) {}
  virtual ~GameObject() {}


  void remove()
  {
    removable = true;
  }
  bool is_removable() const
  {
    return removable;
  }

  const std::string& get_name() const
  {
    return name;
  }

  void set_active(bool a) { active = a; }
  bool is_active() const { return active; }

  virtual void draw (SceneContext& context) = 0;
  virtual void update (float elapsed_time) = 0;
    
  static void set_world (Sector* w) { world = w; }
  Sector* get_world () const
  { return world; }
};

#endif

/* EOF */
