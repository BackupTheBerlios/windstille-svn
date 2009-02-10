#ifndef HEADER_COLLTEST_HXX
#define HEADER_COLLTEST_HXX

#define MAXF 100000.0f

#include <iostream>
#include <ClanLib/Core/Math/vector2.h>

struct SweepResult
{
  enum CollisionState { COL_AT, COL_ALWAYS, COL_NEVER };
  CollisionState state;
  float t0,t1;

  SweepResult()
  {
    state=COL_NEVER;
  }

  bool collision(float delta) const
  {
    return state==COL_ALWAYS || (state==COL_AT && t1>0 && t0<delta);
  }

  float begin(float delta) const
  {
    if(state==COL_ALWAYS || state==COL_NEVER)
      return MAXF;

    if(t0>=0.0f && t0<delta)
      return t0;
    else
      return MAXF;
  }

  bool always() const
  {
    return state==COL_ALWAYS;
  }

};

inline std::ostream &operator<<(std::ostream &o,const SweepResult &r)
{
  o<<"("<<r.state<<","<<r.t0<<","<<r.t1<<")";
  return o;
}


SweepResult simple_sweep_1d(float a, float aw, float av,
			    float b, float bw, float bv);

class CollRect;
class CollTri;

struct InstantCollisionResult
{
  float depth;
  CL_Vector2 direction;
  bool collision;

  InstantCollisionResult()
  {
    collision=false;
  }
};

void collideRectRect(InstantCollisionResult &result,const CollRect &a,const CollRect &b);
void collideRectTri(InstantCollisionResult &result,const CollRect &a,const CollTri &b);
void collideTriTri(InstantCollisionResult &result,const CollTri &a,const CollTri &b);


#endif
