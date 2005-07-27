#ifndef __GAMEOBJECTS_HPP__
#define __GAMEOBJECTS_HPP__

#ifndef SCRIPTING_API
#include "game_object.hpp"
#include "test_object.hpp"
#include "player.hpp"
#include "picture_entity.hpp"
#include "ref.hpp"

typedef GameObject _GameObject;
typedef TestObject _TestObject;
typedef Player _Player;
typedef PictureEntity _PictureEntity;
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
  {}
  virtual ~GameObject()
  {}
#endif
  
public:
  const std::string& get_name() const;
  void remove();
};

class TestObject : public GameObject
{
#ifndef SCRIPTING_API
public:
  TestObject(_TestObject* _object)
    : GameObject(_object)
  {}
  virtual ~TestObject()
  {}

  _TestObject* obj() const
  {
    return reinterpret_cast<_TestObject*> (object.get());
  }
#endif

public:
  void set_sprite(const std::string& filename);
  void set_action(const std::string& action);
  void set_pos(float x, float y);
  void set_vflip(bool vflip);
};

class Player : public GameObject
{
#ifndef SCRIPTING_API
public:
  Player(_Player* _player)
    : GameObject(_player)
  {}
  virtual ~Player()
  {}

  _Player* obj() const
  {
    return reinterpret_cast<_Player*> (object.get());
  }
#endif

public:
  void start_listening();
  void stop_listening();
};

class PictureEntity : public GameObject
{
#ifndef SCRIPTING_API
public:
  PictureEntity(_PictureEntity* _object)
    : GameObject(_object)
  {}
  virtual ~PictureEntity()
  {}

  _PictureEntity* obj() const
  {
    return reinterpret_cast<_PictureEntity*> (object.get());
  }
#endif

public:
  void move_to(float x, float y, float speed);
  void show(bool visible);
};
 
}

#endif

