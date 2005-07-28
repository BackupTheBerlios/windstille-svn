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

#include "collision_test.hpp"
#include "collision_engine.hpp"
#include "globals.hpp"
#include "tile_map.hpp"

std::vector<Rectf> tilemap_collision_list(TileMap *tilemap, const Rectf &r);

/***********************************************************************
 * Collision
 ***********************************************************************/

CollisionEngine::CollisionEngine()
{
  unstuck_velocity = 50.0f;
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
  (void)delta;

  CollisionData inv=result.invert();

  a.collision(result, b);
  b.collision(inv, a);
}

void
CollisionEngine::unstuck(CollisionObject& a, CollisionObject& b, float delta)
{
  if (a.object_type == CollisionObject::RECTANGLE && b.object_type == CollisionObject::RECTANGLE)
    {
      unstuck_rect_rect (a, b, delta);
    }
  else
    {
      if (a.object_type == CollisionObject::RECTANGLE)
	unstuck_tilemap (b, a, delta);
      else
	unstuck_tilemap (a, b, delta);

    }
}

Vector unstuck_direction(const Rectf &a, const Rectf &b, float delta, float unstuck_velocity)
{
  // The distance A needs to unstuck from B in the given direction
  float left   = fabsf(a.right - b.left);
  float right  = fabsf(b.right - a.left);
  float top    = fabsf(a.bottom - b.top);
  float bottom = fabsf(b.bottom - a.top);

  float grace =  0.05f;

  float add = unstuck_velocity * delta;

  add = 0.5;
  add = 50;
  //  grace=0;

  Vector dir;

  if (left < right && left < top && left < bottom)
    {
      dir = Vector(std::min(left/2 + grace, add), 0);
    }
  else if (right < left && right < top && right < bottom)
    {
      dir = Vector(-std::min(right/2 + grace, add), 0);
    }
  else if (top < left && top < right && top < bottom)
    {
      dir = Vector( 0, std::min(top/2 + grace, add));
    }
  else // (bottom < left && bottom < right && bottom < top)
    {
      dir = Vector( 0, -std::min(bottom/2 + grace, add));
    }
  return dir;
}

void
CollisionEngine::unstuck_tilemap(CollisionObject& a, CollisionObject& b, float delta)
{
  Rectf rb = b.primitive;

  rb.left   += b.get_pos().x;
  rb.right  += b.get_pos().x;
  rb.top    += b.get_pos().y;
  rb.bottom += b.get_pos().y;

  // assume, that only one tile is penetrated
  std::vector<Rectf> rect_list= tilemap_collision_list (a.tilemap, rb);
  
  assert (b.object_type == CollisionObject::RECTANGLE);
  assert (a.object_type == CollisionObject::TILEMAP);
	
  if (rect_list.size() == 0)
    return;
  assert (rect_list.size()>=1);

  Vector dir = unstuck_direction (rect_list[0], rb, delta, unstuck_velocity);
  
  assert (b.unstuck_movable());
  b.pos += dir;

}

void
CollisionEngine::unstuck_rect_rect(CollisionObject& a, CollisionObject& b, float delta)
{
  Rectf ra = a.primitive;

  ra.left   += a.get_pos().x;
  ra.right  += a.get_pos().x;
  ra.top    += a.get_pos().y;
  ra.bottom += a.get_pos().y;

  Rectf rb = b.primitive;

  rb.left   += b.get_pos().x;
  rb.right  += b.get_pos().x;
  rb.top    += b.get_pos().y;
  rb.bottom += b.get_pos().y;

  Vector dir = unstuck_direction (ra, rb, delta, unstuck_velocity);

  if (a.unstuck_movable())
    a.pos -= dir;
      
  if (b.unstuck_movable())
    b.pos += dir;
}

void
CollisionEngine::update(float delta)
{
  if (objects.empty())
    return; 

  CollisionData col_data;
  float frame=delta;
  float min_time=frame;
  int max_tries=200;

  do
    {
      min_time=frame;
      
      for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
	{
	  for(Objects::iterator j = i + 1; j != objects.end(); ++j)
	    {
	      if (i != j)
		{
		  CollisionData r = collide(**i, **j, frame);
		  if(r.state!=CollisionData::NONE)
		    {
		      if (min_time > r.col_time && r.col_time>=0)
			{
			  r.a=*i;
			  r.b=*j;
			  col_data = r;
			  min_time = r.col_time;
			  if (min_time > 0.0005)
			    min_time -= 0.0005;
			}
		    }
		}
	    }
	}

      // move till first collision (or till end, when no collision occured)
      if(min_time>0)
	{
	  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
	    {
	      update(**i,min_time);
	    }
	}
      // report collision
      if (min_time < frame)
	{
	  collision (*col_data.a, *col_data.b, col_data, min_time);
	}

      frame-=min_time;
      min_time=0;


      // check tries
      --max_tries;
      if (max_tries == 0)
	std::cerr<<"Too much tries in collision detection"<<std::endl;

    }while (min_time < frame  && max_tries>0);

  //return; // uncomment, if you want no unstucking

  // check penetration and resolve
  bool penetration = true;
  int maxtries=15;
  while(penetration)
    {
      penetration=false;
      // FIXME: support this by some spatial container
      for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
	{
	  if(!(*i)->unstuck())
	    continue;
	  
          for(Objects::iterator j = i+1; j != objects.end(); ++j)
	    {
	      if(!(*j)->unstuck())
		continue;
	      
	      if (i != j && ((*i)->unstuck_movable() || ((*j)->unstuck_movable())))
		{
		  CollisionData r = collide(**i, **j, delta/1000.0f);
		  if(r.state!=CollisionData::NONE)
		    {
		      //		      collision(**i, **j, r, delta);///30.0f);
		      penetration=true;
		      unstuck(**i, **j, delta/3.0f);
		    }
		}
	    }
	}
      maxtries--;
      if(maxtries==0)
	break;
    }
}

void
CollisionEngine::update(CollisionObject& obj, float delta)
{
  obj.update(delta);
}

CollisionObject*
CollisionEngine::add(CollisionObject *obj)
{
  // FIXME: This might need commit_add/commit_remove stuff like in sector
  objects.push_back(obj);

  return objects.back();
}

void 
CollisionEngine::remove(CollisionObject *obj)
{
  // FIXME: This might need commit_add/commit_remove stuff like in sector
  Objects::iterator i = std::find(objects.begin(), objects.end(), obj);
  if(i != objects.end())
    objects.erase(i);
}

// LEFT means b1 is left of b2
CollisionData
CollisionEngine::collide(const Rectf& b1, const Rectf& b2,
                         const Vector& b1_v, const Vector& b2_v,
                         float delta)
{
  SweepResult result0 = simple_sweep_1d(b1.left, b1.get_width(),  b1_v.x,
					b2.left, b2.get_width(),  b2_v.x);
  SweepResult result1 = simple_sweep_1d(b1.top,  b1.get_height(), b1_v.y,
					b2.top,  b2.get_height(), b2_v.y);

  CollisionData result;
  result.delta = delta;

  if(result0.collision(delta) && result1.collision(delta))
    {
      if(result0.always() && result1.always())
	result.state = CollisionData::STUCK;
      else
	{
	  if(result0.begin(delta)<result1.begin(delta))
	    {
	      // x direction prior
	      if(b1.left < b2.left)
		{
		  result.state=CollisionData::COLLISION;
		  result.direction=Vector(-1, 0);
		}
	      else
		{
		  result.state=CollisionData::COLLISION;
		  result.direction=Vector(1, 0);
		}
	      result.col_time=result0.t0;
	    }
	  else
	    {
	      // x direction prior
	      if(b1.top < b2.top)
		{
		  result.state=CollisionData::COLLISION;
		  result.direction=Vector(0, -1);
		}
	      else
		{
		  result.state=CollisionData::COLLISION;
		  result.direction=Vector(0, 1);
		}
	      result.col_time=result1.t0;
	    }
	}
    }
  return result;
}

CollisionData
CollisionEngine::collide(CollisionObject& a, CollisionObject& b, float delta)
{
  if (a.object_type == CollisionObject::RECTANGLE && b.object_type == CollisionObject::RECTANGLE)
    {
      Rectf ra = a.primitive;
      Rectf rb = b.primitive;
      
      ra.left   += a.get_pos().x;
      ra.right  += a.get_pos().x;
      ra.top    += a.get_pos().y;
      ra.bottom += a.get_pos().y;
      
      rb.left   += b.get_pos().x;
      rb.right  += b.get_pos().x;
      rb.top    += b.get_pos().y;
      rb.bottom += b.get_pos().y;
      
      return collide(ra, rb,
		     a.get_velocity(), b.get_velocity(),
		     delta);
    }
  else
    {
      if (a.object_type == CollisionObject::RECTANGLE)
	return collide_tilemap (b, a, delta).invert();
      else
	return collide_tilemap (a, b, delta);
    }
}


int get_next_integer(float f, float direction)
{
  int result;

  if(direction < 0)
    {
      result = int (f);
      if (result == f)
	--result;
    }
  else
    {
      result = int (f);
      if (result <= f)
	++result;
    }
  return result;
}

bool tilemap_collision(TileMap *tilemap, const Rectf &r)
{
  int minx, maxx;
  int miny, maxy;
  int x, y;

  minx = (int)r.left   / TILE_SIZE;
  maxx = (int)r.right  / TILE_SIZE;
  miny = (int)r.top    / TILE_SIZE;
  maxy = (int)r.bottom / TILE_SIZE;

  for (y = miny; y <= maxy; ++y)
    for (x = minx; x <= maxx; ++x)
      {
	if(tilemap->is_ground (x * TILE_SIZE, y * TILE_SIZE ))
	  {
	    return true;
	  }
      }
  return false;
}

std::vector<Rectf> tilemap_collision_list(TileMap *tilemap, const Rectf &r)
{
  std::vector<Rectf> rect_list;
  int minx, maxx;
  int miny, maxy;
  int x, y;

  minx = (int)r.left   / TILE_SIZE;
  maxx = (int)r.right  / TILE_SIZE;
  miny = (int)r.top    / TILE_SIZE;
  maxy = (int)r.bottom / TILE_SIZE;

  for (y = miny; y <= maxy; ++y)
    for (x = minx; x <= maxx; ++x)
      {
	if(tilemap->is_ground (x * TILE_SIZE, y * TILE_SIZE ))
	  {
	    rect_list.push_back (Rectf (x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE));
	  }
      }
  return rect_list;
}

#define c_sign(x) ((x)<0?-1:((x)>0?1:0))

CollisionData
CollisionEngine::collide_tilemap(CollisionObject& a, CollisionObject& b, float delta)
{
  CollisionData result;

  assert(a.object_type == CollisionObject::TILEMAP);
  assert(b.object_type == CollisionObject::RECTANGLE);

  Vector vel = b.get_velocity() - a.get_velocity();

  if (vel.x == 0.0f && vel.y == 0.0f)
    return result;

  // The algorithm functions under following assumption, that
  // b is in the free at time == 0.
  // Then for the given frame delta, for each new grid collision is checked,
  // if a collision takes place.

  Rectf r = b.primitive;

  r.left   += b.get_pos().x;
  r.right  += b.get_pos().x;
  r.top    += b.get_pos().y;
  r.bottom += b.get_pos().y;

  // check, if stuck
  if (tilemap_collision (a.tilemap, r))
    {
      result.state=CollisionData::STUCK;
      result.col_time = 0;
      
      return result;
    }



  float time=0.0f;
  
  float *x, *y;         // current position
  int next_x, next_y;   // next grid position
  float tx, ty;         // next time, when grid is hit
  float ct=1.0f;        // collision_time

  // also check at time==0
  bool first_time=true;

  if (vel.x < 0)
    x = &r.left;
  else
    x = &r.right;

  if (vel.y < 0)
    y = &r.top;
  else
    y = &r.bottom;

  while (time < delta && ct >= 0.0f)
    {
      ct = -1.0f;

      if(first_time)
	{
	  next_x = ((int)(*x / TILE_SIZE)) * TILE_SIZE;
	  next_y = ((int)(*y / TILE_SIZE)) * TILE_SIZE;
	  first_time = false;
	}
      else
	{
	  next_x = get_next_integer ((*x / TILE_SIZE), vel.x) * TILE_SIZE;
	  next_y = get_next_integer ((*y / TILE_SIZE), vel.y) * TILE_SIZE;
	}

      if (vel.x != 0.0f)
	tx = (next_x - *x) / vel.x;
      else
	tx = 10000.0f;

      if (vel.y != 0.0f)
	ty = (next_y - *y) / vel.y;
      else
	ty = 10000.0f;

      if(tx<0)
	tx=0;
      if(ty<0)
	ty=0;

      if (tx < ty)
	{
	  if (time + tx < delta)
	    ct = tx;
	}
      else
	{
	  if (time + ty < delta)
	    ct = ty;
	}
      
      if (ct >= 0.0f)
	{
	  // move to next position
	  float dx, dy;

	  dx = ct * vel.x;
	  dy = ct * vel.y;

	  r.left   += dx;
	  r.right  += dx;
	  r.top    += dy;
	  r.bottom += dy;
	  time += ct;

	  // now shift one more pixel and check for collision with tilemap
	  Rectf tmp(r);
	  
	  if (tx < ty)
	    {
	      tmp.left  += c_sign(vel.x);
	      tmp.right += c_sign(vel.x);
	    }
	  else
	    {
	      tmp.top    += c_sign(vel.y);
	      tmp.bottom += c_sign(vel.y);
	    }

	  // check collision with tilemap

	  if (tilemap_collision (a.tilemap, tmp))
	    {
	      result.state=CollisionData::COLLISION;
	      result.col_time = time;
	      
	      if (tx < ty)
		result.direction=Vector(c_sign (vel.x), 0);
	      else
		result.direction=Vector(0, c_sign (vel.y));
	      return result;
	    }
	}
    }

  return result;
}

/* EOF */

