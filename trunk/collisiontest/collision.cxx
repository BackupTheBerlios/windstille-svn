#include "collision.hxx"
#include "colltest.hxx"

#include <assert.h>

#include <ClanLib/display.h>

/***********************************************************************
 * CollisionResult
 ***********************************************************************/

CollisionData CollisionData::merge(const CollisionData &r)
{
  if(state==COLLISION)
    {
      if((r.state==COLLISION && r.col_time<col_time) || r.state==STUCK)
	*this=r;
    }
  else if(state==NONE)
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
  return object->get_movement().x;
}
inline float CollPrimitive::y_velocity() const
{
  return object->get_movement().y;
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
  return rect.left+object->get_pos().x;
}

float CollRect::y_pos() const
{
  return rect.top+object->get_pos().y;
}

float CollRect::width() const
{
  return rect.get_width();
}
float CollRect::height() const
{
  return rect.get_height();
}

inline std::ostream& operator<<(std::ostream& out, const CollRect &b)
{
  out<<"("<<b.x_pos()<<","<<b.y_pos()<<","<<b.width()<<","<<b.height()<<","<<b.object->get_movement().x<<","<<b.object->get_movement().y<<")";
  return out;
}

CL_Vector2 CollRect::get_vector0() const
{
  return CL_Vector2(x_pos(),y_pos());
}
CL_Vector2 CollRect::get_vector1() const
{
  return CL_Vector2(x_pos()+width(),y_pos());
}
CL_Vector2 CollRect::get_vector2() const
{
  return CL_Vector2(x_pos(),y_pos()+height());
}
CL_Vector2 CollRect::get_vector3() const
{
  return CL_Vector2(x_pos()+width(),y_pos()+height());
}

void CollRect::drawCollision() const
{
  CL_Gradient g(CL_Color(255, 255, 255),CL_Color(255, 255, 255),CL_Color(255, 255, 255),CL_Color(255, 255, 255));
  CL_Gradient g2(CL_Color(0, 255, 255),CL_Color(0, 255, 255),CL_Color(0, 255, 255),CL_Color(0, 255, 255));

  /*  if(object->collision)
    CL_Display::fill_rect(CL_Rectff(CL_Pointf(i->x_pos(), i->y_pos()),
				   CL_Sizef(i->width(), i->height())),g2);
				   else*/

  CL_Vector2 v=object->get_pos();
  CL_Rectf r=rect;
  r+=CL_Pointf(v.x,v.y);

  CL_Display::fill_rect(r,g);
  
  CL_Display::draw_rect(r,
			CL_Color(155, 155, 155));        
  
  CL_Display::draw_line(r.left + r.get_width()/2,
			r.top  + r.get_height()/2,
			r.left + r.get_width()/2 + object->get_movement().x,
			r.top  + r.get_height()/2 + object->get_movement().y,
			CL_Color(255, 0, 255));

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
  return new CollTri(base,dx,dy,o);
}

float CollTri::width() const
{
  return fabs(dx);
}
float CollTri::height() const
{
  return fabs(dy);
}

float CollTri::x_pos() const
{
  return std::min(base.x,base.x+dx)+object->get_pos().x;
}
float CollTri::y_pos() const
{
  return std::min(base.y,base.y+dy)+object->get_pos().y;
}

CL_Vector2 CollTri::normal() const
{
  CL_Vector2 v(dy,dx);
  v/=v.length();
  return v;
}

CL_Vector2 CollTri::get_vector0() const
{
  return CL_Vector2(x_pos(),y_pos());
}
CL_Vector2 CollTri::get_vector1() const
{
  return CL_Vector2(x_pos()+dx,y_pos());
}
CL_Vector2 CollTri::get_vector2() const
{
  return CL_Vector2(x_pos(),y_pos()+dy);
}

void CollTri::drawCollision() const
{
  CL_Vector2 act_pos=object->get_pos()+base;
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
 * CollisionObject
 ***********************************************************************/

CollisionObject::CollisionObject()
{
  parent=0;
}

CollisionObject::~CollisionObject()
{
}

void CollisionObject::insertCollPrimitive(CollPrimitive *primitive)
{
  colliders.push_back(primitive);
}



void CollisionObject::drawCollision()
{
  std::list<CollPrimitive*>::iterator j=colliders.begin();

  for(;j!=colliders.end();j++)
    {
      (*j)->drawCollision();
    }
}

void CollisionObject::move(float delta)
{
  //  position += get_movement() * delta;
  position += movement * delta;
  movement -= movement * coll_engine->get_friction() * delta;
  
  movement += coll_engine->get_graphity() * delta;
  

  if (fabsf(movement.x) < coll_engine->get_min_velocity())
    movement.x = 0.0f;

  if (fabsf(movement.y) < coll_engine->get_min_velocity())
    movement.y = 0.0f;

}

void 
CollisionObject::set_movement(const CL_Vector2 &m)
{
  movement=m;
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
  //  if(result0.t0 < 0 || result1.t0 < 0)
  //    result.before=true;
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








/***********************************************************************
 * Collision
 ***********************************************************************/

CollisionEngine::CollisionEngine()
{
  friction       = 0.01f;
  
  x_acceleration = 0.0f;
  y_acceleration = 5.0f;

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

  CL_Vector2 dir;

  if (left < right && left < top && left < bottom)
    {
      dir = CL_Vector2(std::min(left/2 + grace,add),0);
    }
  else if (right < left && right < top && right < bottom)
    {
      dir = CL_Vector2(-std::min(right/2 + grace,add),0);
    }
  else if (top < left && top < right && top < bottom)
    {
      dir = CL_Vector2( 0, std::min(top/2 + grace,add));
    }
  else // (bottom < left && bottom < right && bottom < top)
    {
      dir = CL_Vector2( 0, -std::min(bottom/2 + grace,add));
    }

  if (a.unstuck_movable())
    a.position -= dir;
      
  if (b.unstuck_movable())
    b.position += dir;
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
	      if(r.state!=CollisionData::NONE)//collision())
		{
		  collision(**i,**j,r,delta);
		  //		  i->collision=true;
		  //		  j->collision=true;

		  if(r.col_time>0)
		    moved=true;
		}
            }
        }
      
      //      if (i->movable && !moved)
      //      if ( !moved)
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
  obj.move(delta);
}

CollisionObject *
CollisionEngine::add_object(CollisionObject *obj)
{
  objects.push_back(obj);
  obj->coll_engine=this;

  //  objects.back().id = objects.size();

  return objects.back();
}


float CollisionEngine::get_min_velocity() const
{
  return minimum_velocity;
}
float CollisionEngine::get_friction() const
{
  return friction;
}
CL_Vector2 CollisionEngine::get_graphity() const
{
  return CL_Vector2(x_acceleration, y_acceleration);
}


/***********************************************************************
 * PhysicalObject
 ***********************************************************************/

PhysicalObject::PhysicalObject(int x_, int y_, int w_, int h_)
{
  mass = 1.0f;
  
  friction=0.6;
  adhesion=50.0;
  
  movement = CL_Vector2(0,0);
  bbox=CL_Rectf(0,0,w_,h_);
  position= CL_Vector2(x_,y_);

  movable=true;

  
  insertCollPrimitive(new CollRect(CL_Rectf(0,0,w_,h_),this));

  //  colliders.push_back(new CollRect(0,h_/2,w_,h_/2,this));
  //  colliders.push_back(new CollTri(0,h_/2,w_,-h_/2,this));
  

  //colliders.push_back(new CollRect(0,h_/2,w_/2,h_/2,this));
  //colliders.push_back(new CollTri(w_/2,0,w_/2,-h_/2,this));
  //colliders.push_back(new CollTri(w_/3,-h_/2,2*w_/3,-h_/2,this));

}

void PhysicalObject::prepare_collision(const CollisionData &data,CollisionObject &other)
{
  if(!movable)
    return;
  PhysicalObject *po=dynamic_cast<PhysicalObject*>(&other);
  if(!po)
    return;

  coll_impulse = get_impulse () + po->get_impulse ();
  coll_impulse*=0.5f;
}

void 
PhysicalObject::collision(const CollisionData &data,CollisionObject &other)
{
  PhysicalObject *po=dynamic_cast<PhysicalObject*>(&other);

  if(!movable)
    return;

  if(!po)
    {
      if(data.direction.x!=0)
	movement.x=0;
      else
	movement.y=0;
      return;
    }
  if(po->movable)
    {
      // soft collision with impulses
      
      float c_fric=friction*data.delta; // current friction influence
      
      if (data.direction.x==0)
	{
	  set_impulse_y (coll_impulse.y);
	  
	  // apply friction in x direction
	  set_impulse_x (coll_impulse.x * c_fric + get_impulse().x * (1.0f - c_fric));
	}
      else
	{
	  set_impulse_x (coll_impulse.x);
	  
	  // apply friction in y direction
	  set_impulse_y (coll_impulse.y * c_fric + get_impulse().y * (1.0f - c_fric));
	}
      
    }
  else
    {
      // hard collision with wall
      if(data.direction.x==0)
	{
	  if(data.direction.y==-1)
	    movement.y*=-0.5;
	  else
	    movement.y*=0.0f;
	  movement.x*=(1.0f-friction*data.delta);
	  if(fabs(movement.x)<data.delta*adhesion)
	    movement.x=0.0f;
	}
      if(data.direction.y==0)
	{
	  movement.x*=-0.7;
	  movement.y*=(1.0f-friction*data.delta);
	  if (fabs (movement.y) < data.delta * adhesion)
	    movement.y=0.0f;
	}
    }
}

CL_Vector2 
PhysicalObject::get_impulse() const
{
  return movement * mass;
}

void 
PhysicalObject::set_impulse_x(float i)
{
  movement.x = i / mass;
}

void 
PhysicalObject::set_impulse_y(float i)
{
  movement.y = i / mass;
}

void 
PhysicalObject::set_movable(bool m)
{
  movable=m;
}

bool 
PhysicalObject::unstuck() const
{
  return true;
}

bool 
PhysicalObject::unstuck_movable() const
{
  return movable;
}

void 
PhysicalObject::move(float delta)
{
  if(movable)
    CollisionObject::move(delta);
}

/***********************************************************************
 * Player
 ***********************************************************************/

Player::Player(int x_,int y_,int w_, int h_):
  PhysicalObject(x_,y_,w_,h_)
{
  mass=0.2;
  onGround=false;
}

void Player::collision(const CollisionData& data, CollisionObject& other)
{
  if(data.direction.y!=0)
    movement.y=0;

  Elevator *e=dynamic_cast<Elevator*>(&other);
  PhysicalObject *o=dynamic_cast<PhysicalObject*>(&other);
  if(e)
    {
      if(data.direction.y<0)
	{
	  if(!parent)
	    {
	      // attach
	      CL_Vector2 v=get_pos()-e->get_pos();
	      parent=e;
	      position=v;
	      lastTouch=0;
	    }
	}
    }
  else if(o)
    {
      if(!o->unstuck_movable())
	{
	  if(data.direction.y<0)
	    {
	      onGround=true;
	      lastTouch=0;
	    }
	}
    }
}

bool 
Player::on_ground() const
{
  return onGround || parent;
}


void Player::set_movement(const CL_Vector2 &v)
{
  movement=v;
}

void 
Player::move(float delta)
{
  lastTouch++;
  if(lastTouch>5)
    detachElevator();
  PhysicalObject::move(delta);
}

void 
Player::detachElevator()
{
  CL_Vector2 p=get_pos();
  parent=0;
  position=p;
  onGround=false;
}

/***********************************************************************
 * Elevator
 ***********************************************************************/
Elevator::Elevator()
{
  position=0;
  speed=0.05;

  insertCollPrimitive(new CollRect(CL_Rectf(0,0,60,8),this));
  bbox=CL_Rectf(0,0,60,8);
  //  mass=1000;
  //  movement=CL_Vector2(0,0);
}
void 
Elevator::move(float delta)
{
  position+=delta*speed;
  if(position>points.size())
    {
      position-=points.size();
    }

  int p=(int)position;
  int p2=p+1;
  if(p2>=points.size())
    p2=0;
  CL_Vector2 p0=points[p];
  CL_Vector2 p1=points[p2];

  float a=position-p;
  
  CollisionObject::position=p0*(1.0f-a)+p1*a;
}

bool 
Elevator::unstuck() const
{
  return true;
}
bool 
Elevator::unstuck_movable() const
{
  return false;
}

void 
Elevator::collision(const CollisionData& data, CollisionObject& other)
{
}

void 
Elevator::insertPoint(const CL_Vector2 &p)
{
  points.push_back(p);
}
