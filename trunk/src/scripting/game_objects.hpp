#ifndef __GAMEOBJECTS_HPP__
#define __GAMEOBJECTS_HPP__

#ifndef SCRIPTING_API
#include "game_object.hpp"
#include "flashing_sign.hpp"
#include "ref.hpp"

typedef GameObject _GameObject;
typedef FlashingSign _FlashingSign;
#endif

namespace Scripting
{

class GameObject
{
#ifndef SCRIPTING_API
protected:
  Ref<_GameObject> object;
  
public:
  GameObject(_GameObject* _object)
    : object(_object)
  { }
  virtual ~GameObject()
  {}
#endif
  
public:
  const std::string& get_name() const;
};

class FlashingSign : public GameObject
{
#ifndef SCRIPTING_API
public:
  FlashingSign(_FlashingSign* _object)
    : GameObject(_object)
  { }
  virtual ~FlashingSign() {}  

  _FlashingSign* obj() const
  {
    return reinterpret_cast<_FlashingSign*> (object.get());
  }
#endif

public:
  void enable();
  void disable();
};

}

#endif

