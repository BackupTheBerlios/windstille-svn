#ifndef HEADER_COLLISION_HXX
#define HEADER_COLLISION_HXX

#include <iostream>
#include <list>
#include <vector>

#include <ClanLib/Core/Math/vector2.h>
#include <ClanLib/Core/Math/rect.h>

using namespace std;

class CollisionObject;
class CollRect;

struct CollPrimitive
{
protected:
  CollisionObject *object;
public:
  CollPrimitive(CollisionObject *object_);
  virtual ~CollPrimitive();
  virtual CollPrimitive *clone(CollisionObject *o) const=0;

  virtual float x_pos() const=0;
  virtual float y_pos() const=0;
  virtual float width() const=0;
  virtual float height() const=0;

  float x_velocity() const;
  float y_velocity() const;
  CL_Vector2 get_velocity() const;

  virtual void drawCollision() const=0;

  friend std::ostream& operator<<(std::ostream& out, const CollRect &b);

};


class CollRect:public CollPrimitive
{
  CL_Rectf rect;

public:
  CollRect(const CL_Rectf &r_,CollisionObject *object_);

  virtual CollPrimitive *clone(CollisionObject *o) const;
  float x_pos() const;
  float y_pos() const;
  float width() const;
  float height() const;

  CL_Vector2 get_vector0() const;
  CL_Vector2 get_vector1() const;
  CL_Vector2 get_vector2() const;
  CL_Vector2 get_vector3() const;

  virtual void drawCollision() const;
};

inline std::ostream& operator<<(std::ostream& out, const CollRect &b);

class CollTri:public CollPrimitive
{
  CL_Vector2 base;

  float dx;
  float dy;

public:
  CollTri(CollisionObject *object_);
  CollTri(const CL_Vector2 &base_, float w_, float h_,CollisionObject *object_);
  virtual CollPrimitive *clone(CollisionObject *o) const;

  float x_pos() const;
  float y_pos() const;
  float width() const;
  float height() const;

  // normal - normalized!
  CL_Vector2 normal() const;

  CL_Vector2 get_vector0() const;
  CL_Vector2 get_vector1() const;
  CL_Vector2 get_vector2() const;

  virtual void drawCollision() const;
};

struct CollisionData
{
  enum State {NONE,STUCK,COLLISION};
  // points into direction from where the other object came
  CL_Vector2 direction;
  
  State state;

  // size of time frame
  float delta; 

  // time of collision
  float col_time;

  CollisionData()
  {
    state=NONE;
    col_time=0;
  }

  CollisionData merge(const CollisionData &r);

  CollisionData invert() const
  {
    CollisionData r(*this);
    r.direction*=-1;

    return r;
  }

  
};
inline std::ostream &operator<<(std::ostream &o,const CollisionData &r)
{
  o<<"("<<r.direction.x<<","<<r.direction.y<<","<<r.col_time<<")";
  return o;
}

class CollisionEngine;

class CollisionObject
{

public:
  /*  CollisionObject(const CollisionObject &o);
  CollisionObject(float x_,float y_,float w_,float h_);


  CL_Vector2 get_impulse() const;
  void set_impulse_x(float i);
  void set_impulse_y(float i);

  void hard_collision(const CollisionData &result, float delta);
  void soft_collision(const CollisionData &result, CL_Vector2 impulse_, float delta_);

  friend CollisionData collide(CollisionObject &a,CollisionObject &b,float delta);
  
  void drawCollision();
*/

  CollisionObject();
  virtual ~CollisionObject();

  CL_Vector2 get_pos() const
  {
    if(parent != 0)
      return parent->get_pos() + position;
    
    return position;
  }

  CL_Vector2 get_movement() const
  {
    if(parent != 0)
      return parent->get_movement() + movement;

    return movement;
  }

  void set_movement(const CL_Vector2 &m);

  //  void attach(CollisionObject &o);
  //  void detach(CollisionObject &o);


  // this function is for preparing impulse collisions
  // you have to calculate the resulting impulse for both object before changing,
  // so this is called for both objects first and then collision() for both.
  //
  // this isn't really fast, because everything's done twice, maybe someone has a better idea?
  virtual void prepare_collision(const CollisionData &data, CollisionObject &other)
  {
  }
  virtual void collision(const CollisionData& data, CollisionObject& other) = 0; 

  virtual void move(float delta);

  void insertCollPrimitive(CollPrimitive *primitive);

  // debugging helpers
  void drawCollision();

  // this functions support unstucking, which needs to be done, when more than 2 object stack over one another
  // should this object be unstuck ??
  virtual bool unstuck() const=0;
  // is this object movable ?
  virtual bool unstuck_movable() const=0;
  
  // is this object movable

protected:
  /// only rectangular objects for now
  CL_Rectf bbox;
 
  /// position of the object
  CL_Vector2 position;
  /// movement till next frame
  CL_Vector2 movement;

  CollisionObject* parent;
  std::vector<CollisionObject*> children;


private:
  std::list<CollPrimitive*> colliders;
  CollisionEngine *coll_engine;

  friend class CollisionEngine;
  friend CollisionData collide(CollisionObject &a,CollisionObject &b,float delta);
};



class CollisionEngine
{
  typedef std::list<CollisionObject*> Objects;

public:

  CollisionEngine();
  ~CollisionEngine();

  void draw();
  void update(float delta);
  void update(CollisionObject& obj, float delta);
  void collision(CollisionObject& a, CollisionObject& b, const CollisionData &result, float delta);

  CollisionObject* add_object(CollisionObject *obj);

  float get_min_velocity() const;
  float get_friction() const;
  CL_Vector2 get_graphity() const;

private:
  Objects objects;

  float friction;
  float unstuck_velocity;

  float x_acceleration;
  float y_acceleration;

  float minimum_velocity;

  void unstuck(CollisionObject& a, CollisionObject& b, float delta);

};

class PhysicalObject:public CollisionObject
{
public:
  PhysicalObject(int x_,int y_,int w_, int h_);


  virtual void prepare_collision(const CollisionData &data, CollisionObject &other);

  virtual void collision(const CollisionData& data, CollisionObject& other); 

  void set_movable(bool m);

  virtual bool unstuck() const;
  virtual bool unstuck_movable() const;

  virtual void move(float delta);

protected:

  CL_Vector2 get_impulse() const;
  void set_impulse_x(float i);
  void set_impulse_y(float i);

  CL_Vector2 coll_impulse;

  float mass;

  bool movable;

  float friction;
  float adhesion;
};

class Player:public PhysicalObject
{
  int lastTouch;
  bool onGround;
public:
  Player(int x_,int y_,int w_, int h_);

  virtual void collision(const CollisionData& data, CollisionObject& other); 

  void set_movement(const CL_Vector2 &v);
  virtual void move(float delta);

  bool on_ground() const;

  void detachElevator();
};

class Elevator:public CollisionObject
{
  std::vector<CL_Vector2> points;
  float position;
  float speed;
public:
  Elevator();
  void move(float delta);

  virtual void collision(const CollisionData& data, CollisionObject& other);

  virtual bool unstuck() const;
  virtual bool unstuck_movable() const;


  void insertPoint(const CL_Vector2 &p);
};


#endif
