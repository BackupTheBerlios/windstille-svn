#ifndef __REFCOUNTER_HPP__
#define __REFCOUNTER_HPP__

#include <assert.h>

class RefCounter
{
public:
  RefCounter()
    : refcount(0)
  { }
  virtual ~RefCounter()
  {
    assert(refcount == 0);
  }

  void ref()
  {
    refcount++;
  }
  void unref()
  {
    refcount--;
    if(refcount <= 0) {
      delete this;
      return;
    }
  }

private:
  int refcount;
};

#endif

