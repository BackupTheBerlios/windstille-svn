/*
 * Copyright (c) 2005 by the Windstille team. All rights reserved.
 *
 * collision.hxx
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

#include "collision.hpp"
#include "collision_test.hpp"
#include "collision_object.hpp"

#include <assert.h>

#include <ClanLib/display.h>

/***********************************************************************
 * CollisionResult
 ***********************************************************************/

CollisionData CollisionData::merge(const CollisionData &r)
{
  if (state==COLLISION)
    {
      if (( r.state==COLLISION && r.col_time<col_time ) || r.state==STUCK )
	*this=r;
    }
  else if (state == NONE)
    *this=r;

  return *this;
}


/***********************************************************************
 * CollPrimitive
 ***********************************************************************/

CollPrimitive::CollPrimitive(CollisionObject *object_):object(object_)
{
  assert (object_);
}

CollPrimitive::~CollPrimitive()
{
}

inline float CollPrimitive::x_velocity() const
{
  return object->get_movement ().x;
}
inline float CollPrimitive::y_velocity() const
{
  return object->get_movement ().y;
}

CL_Vector2 CollPrimitive::get_velocity() const
{
  return object->get_movement();
}

/***********************************************************************
 * CollRect
 ***********************************************************************/

CollRect::CollRect(const CL_Rectf &r_,CollisionObject *object_):
  CollPrimitive(object_),rect(r_)
  {
  }

CollPrimitive *CollRect::clone(CollisionObject *o) const
{
  return new CollRect(rect,o);
}


float CollRect::x_pos() const
{
  return rect.left+object->get_pos ().x;
}

float CollRect::y_pos() const
{
  return rect.top+object->get_pos ().y;
}

float CollRect::width() const
{
  return rect.get_width ();
}
float CollRect::height() const
{
  return rect.get_height ();
}

inline std::ostream& operator<<(std::ostream& out, const CollRect &b)
{
  out<<"("<<b.x_pos()<<","<<b.y_pos()<<","<<b.width()<<","<<b.height()<<","<<b.object->get_movement().x<<","<<b.object->get_movement().y<<")";
  return out;
}

CL_Vector2 CollRect::get_vector0() const
{
  return CL_Vector2 (x_pos (),y_pos ());
}
CL_Vector2 CollRect::get_vector1() const
{
  return CL_Vector2 (x_pos ()+width (),y_pos ());
}
CL_Vector2 CollRect::get_vector2() const
{
  return CL_Vector2 (x_pos (),y_pos ()+height ());
}
CL_Vector2 CollRect::get_vector3() const
{
  return CL_Vector2 (x_pos ()+width (),y_pos ()+height ());
}

void CollRect::drawCollision() const
{
  CL_Gradient g (CL_Color (255, 255, 255),CL_Color (255, 255, 255),CL_Color (255, 255, 255),CL_Color (255, 255, 255));

  CL_Vector2 v=object->get_pos ();
  CL_Rectf r=rect;
  r+=CL_Pointf (v.x,v.y);

  CL_Display::fill_rect (r,g);
  
  CL_Display::draw_rect (r,
			 CL_Color(155, 155, 155));        
  
  CL_Display::draw_line (r.left + r.get_width ()/2,
			 r.top  + r.get_height ()/2,
			 r.left + r.get_width ()/2 + object->get_movement ().x,
			 r.top  + r.get_height ()/2 + object->get_movement ().y,
			 CL_Color (255, 0, 255));

}

/***********************************************************************
 * CollTri
 ***********************************************************************/

CollTri::CollTri(CollisionObject *object_):
  CollPrimitive (object_)
{
}

CollTri::CollTri(const CL_Vector2 &base_, float dx_, float dy_,CollisionObject *object_):
  CollPrimitive (object_),
  base(base_),
  dx(dx_),
  dy(dy_)
{
}

CollPrimitive *CollTri::clone(CollisionObject *o) const
{
  return new CollTri (base,dx,dy,o);
}

float CollTri::width() const
{
  return fabs (dx);
}
float CollTri::height() const
{
  return fabs (dy);
}

float CollTri::x_pos() const
{
  return std::min (base.x, base.x + dx) + object->get_pos ().x;
}
float CollTri::y_pos() const
{
  return std::min(base.y, base.y + dy) + object->get_pos ().y;
}

CL_Vector2 CollTri::normal() const
{
  CL_Vector2 v(dy,dx);
  v/=v.length ();
  return v;
}

CL_Vector2 CollTri::get_vector0() const
{
  return CL_Vector2 (x_pos (),y_pos ());
}
CL_Vector2 CollTri::get_vector1() const
{
  return CL_Vector2 (x_pos ()+dx,y_pos ());
}
CL_Vector2 CollTri::get_vector2() const
{
  return CL_Vector2 (x_pos (),y_pos ()+dy);
}

void CollTri::drawCollision() const
{
  CL_Vector2 act_pos = object->get_pos ()+base;
  CL_Gradient g(CL_Color(255, 255, 255),CL_Color(255, 255, 255),CL_Color(255, 255, 255),CL_Color(255, 255, 255));

  CL_Display::fill_triangle(act_pos.x   , act_pos.y,
			    act_pos.x+dx, act_pos.y,
			    act_pos.x   , act_pos.y+dy,g);
  
  CL_Display::draw_triangle(act_pos.x   , act_pos.y,
			    act_pos.x+dx, act_pos.y,
			    act_pos.x   , act_pos.y+dy,
			    CL_Color(155, 155, 155));        
  
  CL_Display::draw_line(act_pos.x + dx/4,
			act_pos.y + dy/4,
			act_pos.x + dx/4 + object->get_movement().x,
			act_pos.y + dy/4 + object->get_movement().y,
			CL_Color(255, 0, 255));
}


/***********************************************************************
 * helpers
 ***********************************************************************/



// LEFT means b1 is left of b2
CollisionData collideBB(CollRect &b1,CollRect &b2,float delta)
{
  SweepResult result0 = simple_sweep_1d(b1.x_pos(), b1.width(), b1.x_velocity(),
					b2.x_pos(), b2.width(), b2.x_velocity());
  SweepResult result1 = simple_sweep_1d(b1.y_pos(), b1.height(), b1.y_velocity(),
					b2.y_pos(), b2.height(), b2.y_velocity());

  CollisionData result;
  result.delta=delta;

  if(result0.collision(delta) && result1.collision(delta))
    {
      if(result0.always() && result1.always())
	result.state=CollisionData::STUCK;
      else
	{
	  if(result0.begin(delta)<result1.begin(delta))
	    {
	      // x direction prior
	      if(b1.x_pos()<b2.x_pos())
		{
		  result.state=CollisionData::COLLISION;
		  result.direction=CL_Vector2(-1,0);
		}
	      else
		{
		  result.state=CollisionData::COLLISION;
		  result.direction=CL_Vector2(1,0);
		}
	      result.col_time=result0.t0;
	    }
	  else
	    {
	      // x direction prior
	      if(b1.y_pos()<b2.y_pos())
		{
		  result.state=CollisionData::COLLISION;
		  result.direction=CL_Vector2(0,-1);
		}
	      else
		{
		  result.state=CollisionData::COLLISION;
		  result.direction=CL_Vector2(0,1);
		}
	      result.col_time=result1.t0;
	    }
	}
    }
  return result;
}

CollisionData collideBT(CollRect &b1,CollTri &b2,float delta)
{
  // get normal of the triangle's diagonal
  CL_Vector2 normal=b2.normal();

  // get triangle's coordinates along this normal
  float b2a=b2.get_vector0().dot(normal);
  float b2b=b2.get_vector1().dot(normal);
  
  float b2max=std::max(b2a,b2b);
  float b2min=std::min(b2a,b2b);

  // get box's coordinates

  float b1a=b1.get_vector0().dot(normal);
  float b1b=b1.get_vector1().dot(normal);
  float b1c=b1.get_vector2().dot(normal);
  float b1d=b1.get_vector3().dot(normal);

  float b1max=std::max(std::max(b1a,b1b),std::max(b1c,b1d));
  float b1min=std::min(std::min(b1a,b1b),std::min(b1c,b1d));

  // get velocity

  float b1vel=b1.get_velocity().dot(normal);
  float b2vel=b2.get_velocity().dot(normal);

  SweepResult result0 = simple_sweep_1d(b1.x_pos(), b1.width(), b1.x_velocity(),
					b2.x_pos(), b2.width(), b2.x_velocity());
  SweepResult result1 = simple_sweep_1d(b1.y_pos(), b1.height(), b1.y_velocity(),
					b2.y_pos(), b2.height(), b2.y_velocity());

  SweepResult result2 = simple_sweep_1d(b1min, b1max-b1min, b1vel,
					b2min, b2max-b2min, b2vel);

  CollisionData result;

  if(result0.collision(delta) && result1.collision(delta) && result2.collision(delta))
    {
      if(result0.always() && result1.always() && result2.always())
	result.state=CollisionData::STUCK;
      else
	{
	  if(result0.begin(delta)<result1.begin(delta))// && result0.begin(delta)<result2.begin(delta))
	    {
	      // x direction prior
	      if(b1.x_pos()<b2.x_pos())
		{
		  result.state=CollisionData::COLLISION;
		  result.direction=CL_Vector2(-1,0);
		}
	      else
		{
		  result.state=CollisionData::COLLISION;
		  result.direction=CL_Vector2(1,0);
		}
	      result.col_time=result0.t0;
	    }
	  else
	    {
	      // y direction prior
	      if(b1.y_pos()<b2.y_pos())
		{
		  result.state=CollisionData::COLLISION;
		  result.direction=CL_Vector2(0,1);
		}
	      
	      else
		{
		  result.state=CollisionData::COLLISION;
		  result.direction=CL_Vector2(0,-1);
		}
	      result.col_time=result1.t0;
	    }
	}
    }
  return result;
}

CollisionData collide(CollPrimitive *o1,CollPrimitive *o2,float delta)
{
  CollRect *b1=dynamic_cast<CollRect*>(o1);
  CollRect *b2=dynamic_cast<CollRect*>(o2);
  if(b1 && b2)
    {
      return collideBB(*b1,*b2,delta);
    }
  else if(b1)
    return collideBT(*b1,*dynamic_cast<CollTri*>(o2),delta);
  else if(b2)
    return collideBT(*b2,*dynamic_cast<CollTri*>(o1),delta);
  //  else
  //    return collide(dynamic_cast<CollTri*>(o1),dynamic_cast<CollTri*>(o2));
  
  return CollisionData();
}

CollisionData collide(CollisionObject &a,CollisionObject &b,float delta)
{
  CollisionData r;

  //  if(!(a.movable or b.movable))
  //    return r;

  bool first=true;
  std::list<CollPrimitive*>::iterator i=a.colliders.begin();
  for(;i!=a.colliders.end();i++)
    {
      std::list<CollPrimitive*>::iterator j=b.colliders.begin();
      for(;j!=b.colliders.end();j++)
	{
	  if(first)
	    {
	      r=collide(*i,*j,delta);
	      first=false;
	    }
	  else
	    r.merge(collide(*i,*j,delta));
	}
    }
  return r;
}






