#ifndef __TEST_OBJECT_HPP__
#define __TEST_OBJECT_HPP__

#include <vector>
#include "game_object.hpp"
#include "sprite3d/sprite.hpp"
#include "math/vector.hpp"
#include "lisp/lisp.hpp"

class TestObject : public GameObject
{
public:
  TestObject(const lisp::Lisp* lisp);
  virtual ~TestObject();

  void draw(SceneContext& context);
  void update(float elapsed_time);

  void set_sprite(const std::string& filename);
  void set_action(const std::string& action);
  void set_vflip(bool vflip);
  void set_pos(const Vector& pos);
  void attach(const std::string& spritename,
		      const std::string& attachement_point);

private:
  sprite3d::Sprite sprite;
  struct AttachedSprite {
    sprite3d::Sprite sprite;
	sprite3d::PointID attachpoint;
  };
  std::vector<AttachedSprite> attached_sprites;
  Vector pos;
};

#endif

