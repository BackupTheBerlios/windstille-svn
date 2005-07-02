#ifndef __FLASHING_SIGN_HPP__
#define __FLASHING_SIGN_HPP__

#include <ClanLib/Display/sprite.h>
#include <ClanLib/Core/Math/point.h>
#include "game_object.hpp"
#include "lisp/lisp.hpp"

class FlashingSign : public GameObject
{
private:
  CL_Sprite sprite;
  CL_Pointf pos;

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

