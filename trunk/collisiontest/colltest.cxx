#include "colltest.hxx"
#include "collision.hxx"
#include <assert.h>

SweepResult simple_sweep_1d(float a, float aw, float av,
			    float b, float bw, float bv)
{
  SweepResult res;
  // Normalize the calculation so that only A moves and B stands still
  float v = av - bv;

  if (v > 0)
    {
      res.t0 = (b - (a + aw)) / v;
      res.t1 = (b + bw - a) / v;
      res.state = SweepResult::COL_AT;

      assert(res.t0 <= res.t1);
    }
  else if (v < 0)
    {
      res.t0 = (b + bw - a) / v;
      res.t1 = (b - (a + aw)) / v;
      res.state = SweepResult::COL_AT;

      assert(res.t0 <= res.t1);
    }
  else // (v == 0)
    {
      if ((a + aw) < b || (a > b + bw))
        res.state = SweepResult::COL_NEVER;
      else
        res.state = SweepResult::COL_ALWAYS;
    }
  return res;
}



bool overlap(float a,float aw,float b,float bw)
{
  float a2=a+aw;
  float b2=b+bw;
  return ((a>=b && a<b2) || (a2>=b && a2<b2) || (b>=a && b<a2) || (b2>=a && b2<=a2));
}

void collideRectRect(InstantCollisionResult &result,const CollRect &a,const CollRect &b)
{
  result.collision=overlap(a.x_pos(),a.width(),b.x_pos(),b.width()) &&  overlap(a.x_pos(),a.width(),b.x_pos(),b.width());
  if(result.collision)
    {
      // FIXME: calculate unstuck direction and depth

      
    }
}

void collideRectTri(InstantCollisionResult &result,const CollRect &a,const CollTri &b)
{
}
void collideTriTri(InstantCollisionResult &result,const CollTri &a,const CollTri &b)
{
}
