#ifndef __TEST_OBJECT_HPP__
#define __TEST_OBJECT_HPP__

#include "game_object.hpp"
#include "sprite3d.hpp"

class TestObject : public GameObject
{
public:
  TestObject();
  virtual ~TestObject();

  void draw(SceneContext& context);
  void update(float elapsed_time);

private:
  Sprite3D* sprite;
};

#endif

