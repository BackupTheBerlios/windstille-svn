#ifndef __FLASHING_SIGN_HPP__
#define __FLASHING_SIGN_HPP__

#include "game_object.hpp"
#include "lisp/lisp.hpp"
#include "math/vector.hpp"
#include "sprite2d/sprite.hpp"

class FlashingSign : public GameObject
{
private:
  sprite2d::Sprite* sprite;
  Vector pos;

  bool enabled;
  float flashspeed;
  float flashdelta;

public:
  FlashingSign(const lisp::Lisp* lisp);
  virtual ~FlashingSign();

  virtual void draw(SceneContext& sc);
  virtual void update(float elapsed_time);

  void enable();
  void disable();
};

#endif

