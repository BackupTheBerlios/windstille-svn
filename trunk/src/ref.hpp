#ifndef __REF_HPP__
#define __REF_HPP__

template<typename T>
class Ref
{
public:
  Ref(T* object)
    : object(object)
  {
    if(object)
      object->ref();
  }
  ~Ref()
  {
    if(object)
      object->unref();
  }

  void operator= (T* object)
  {
    if(object)
      object->ref();
    if(this->object)
      this->object->unref();
    this->object = object;
  }

  T* operator ->() const
  {
    return object;
  }

  T& operator* () const
  {
    return *object;
  }

  T* get() const
  {
    return object;
  }

private:
  T* object;
};

#endif

