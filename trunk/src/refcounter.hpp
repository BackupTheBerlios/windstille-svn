#ifndef __REFCOUNTER_HPP__
#define __REFCOUNTER_HPP__

#include <assert.h>

/**
 * A base class that provides reference counting facilities
 */
class RefCounter
{
public:
  RefCounter()
    : refcount(0)
  { }

  /** increases reference count */
  void ref()
  {
    refcount++;
  }
  /** decreases reference count. Destroys the object if the reference count
   * reaches 0
   */
  void unref()
  {
    refcount--;
    if(refcount <= 0) {
      delete this;
      return;
    }
  }

protected:
  virtual ~RefCounter()
  {
    assert(refcount == 0);
  }

private:
  int refcount;
};

#endif

