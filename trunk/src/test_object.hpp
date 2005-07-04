#ifndef __TEST_OBJECT_HPP__
#define __TEST_OBJECT_HPP__

#include "game_object.hpp"
#include "sprite3d/sprite3d.hpp"
#include "math/vector.hpp"

class TestObject : public GameObject
{
public:
  TestObject();
  virtual ~TestObject();

  void draw(SceneContext& context);
  void update(float elapsed_time);

  void set_sprite(const std::string& filename);
  void set_action(const std::string& action);
  void set_vflip(bool vflip);
  void set_pos(const Vector& pos);

private:
  Sprite3D* sprite;
  Vector pos;
};

#endif

