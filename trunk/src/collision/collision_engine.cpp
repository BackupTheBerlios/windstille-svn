/*
 * Copyright (c) 2005 by the Windstille team. All rights reserved.
 *
 * collision_engine.hxx
 * by David Kamphausen (david.kamphausen@web.de)
 *    Ingo Ruhnke
 *
 * The "Windstille" project, including all files needed to compile it,
 * is free software; you can redistribute it and/or use it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.
 */

#include "collision_engine.hpp"

/***********************************************************************
 * Collision
 ***********************************************************************/

CollisionEngine::CollisionEngine()
{
  unstuck_velocity = 50.0f;
  minimum_velocity = .1f;
}

CollisionEngine::~CollisionEngine()
{
}

void
CollisionEngine::draw()
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      (*i)->drawCollision();
    }
}

void
CollisionEngine::collision(CollisionObject& a, CollisionObject& b, const CollisionData &result, float delta)
{
  CollisionData inv=result.invert();
  a.prepare_collision(result,b);
  b.prepare_collision(inv,a);

  a.collision(result,b);
  b.collision(inv,a);
}



void
CollisionEngine::unstuck(CollisionObject& a, CollisionObject& b, float delta)
{
  // The distance A needs to unstuck from B in the given direction
  float left   = fabsf(a.get_pos().x + a.bbox.get_width() - b.get_pos().x);
  float right  = fabsf(b.get_pos().x + b.bbox.get_width() - a.get_pos().x);
  float top    = fabsf(a.get_pos().y + a.bbox.get_height() - b.get_pos().y);
  float bottom = fabsf(b.get_pos().y + b.bbox.get_height() - a.get_pos().y);

  float grace =  0.05f;

  float add= unstuck_velocity * delta;

  add= 0.5;
  add=50;
  //  grace=0;

  CL_Vector dir;

  if (left < right && left < top && left < bottom)
    {
      dir = CL_Vector(std::min(left/2 + grace,add),0,0);
    }
  else if (right < left && right < top && right < bottom)
    {
      dir = CL_Vector(-std::min(right/2 + grace,add),0,0);
    }
  else if (top < left && top < right && top < bottom)
    {
      dir = CL_Vector( 0, std::min(top/2 + grace,add),0);
    }
  else // (bottom < left && bottom < right && bottom < top)
    {
      dir = CL_Vector( 0, -std::min(bottom/2 + grace,add),0);
    }

  if (a.unstuck_movable())
    a.pos -= dir;
      
  if (b.unstuck_movable())
    b.pos += dir;
}

void
CollisionEngine::update(float delta)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      bool moved=false;
      Objects::iterator j = i/*objects.begin()*/;
      j++;

      for(; j != objects.end(); ++j)
        {
          if (i != j)
            {
	      CollisionData r=collide(**i,**j,delta);
	      if(r.state!=CollisionData::NONE)
		{
		  collision(**i,**j,r,delta);

		  if(r.col_time>0)
		    moved=true;
		}
            }
        }
      
      update(**i, delta);
    }
  //  return;
  // check penetration and resolve
  bool penetration=true;
  int maxtries=15;
  while(penetration)
    {
      penetration=false;
      // FIXME: support this by some spatial container
      for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
	{
	  if(!(*i)->unstuck())
	    continue;
	  Objects::iterator j = i/*objects.begin()*/;
	  j++;
	  
	  for(; j != objects.end(); ++j)
	    {
	      if(!(*j)->unstuck())
		continue;
	      
	      if (i != j && ((*i)->unstuck_movable() || ((*j)->unstuck_movable())))
		{
		  CollisionData r=collide(**i,**j,delta/1000.0f);
		  if(r.state!=CollisionData::NONE)
		    {
		      //		      collision(**i,**j,r,delta);///30.0f);
		      penetration=true;
		      unstuck(**i,**j,delta/3.0f);
		    }
		}
	    }
	}
      maxtries--;
      if(maxtries==0)
	break;
    }
  //  cout<<(15-maxtries)<<endl;

}

void
CollisionEngine::update(CollisionObject& obj, float delta)
{
  obj.update(delta);
}

CollisionObject *
CollisionEngine::add(CollisionObject *obj)
{
  objects.push_back(obj);
  obj->coll_engine=this;

  return objects.back();
}

void 
CollisionEngine::remove(CollisionObject *obj)
{
  Objects::iterator i=std::find(objects.begin(),objects.end(),obj);
  if(i!=objects.end())
    objects.erase(i);
}

/* EOF */

