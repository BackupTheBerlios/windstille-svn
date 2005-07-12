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

#ifndef HEADER_ENTITY_HXX
#define HEADER_ENTITY_HXX

#include <ClanLib/Core/Math/cl_vector.h>
#include <ClanLib/Display/sprite.h>
#include "game_object.hpp"
#include "collision/collision_object.hpp"

/** A GameObject which has a position and some other properties which
    are shared among all/most things in the world */
class Entity : public GameObject, public CollisionObject
{
private:
  bool mover_active;
  CL_Vector target_pos;
  
  /** Reference to the parent object, must not be deleted */
  Entity* parent;

  CL_Signal_v0 done;

protected:
  CL_Sprite sprite;

public:
  Entity();
  virtual ~Entity();
  
  /** Bind the entity to a parent, causing all movement to be affected
      by the parent entity
  
      @param recalc_pos recalculates the current entities position
      into the local space of parent_
  */
  void bind(Entity* parent_, bool recalc_pos = true);

  /** Unbinds an entity */
  void unbind(bool recalc_pos = true);
  
  CL_Vector get_pos() const;

  void set_pos(float x, float y);
  void move_to(float x, float y);

  void draw(SceneContext& gc);
  virtual void update(float delta) = 0;

  CL_Signal_v0& sig_done() { return done; }
private:
  Entity (const Entity&);
  Entity& operator= (const Entity&);

protected:
    void move(float delta);
  //    CL_Vector pos;
};

#endif

/* EOF */
