#include <config.h>

#include "flashing_sign.hpp"

#include <math.h>
#include "lisp/properties.hpp"
#include "windstille_getters.hpp"
#include "timer.hpp"
#include "globals.hpp"
#include "sprite2d/manager.hpp"

FlashingSign::FlashingSign(const lisp::Lisp* lisp)
  : pos(0, 0), enabled(false)
{
  std::string spritename;
  flashspeed = 5;

  lisp::Properties props(lisp);
  props.get("sprite", spritename);
  props.get("pos", pos);
  props.get("flashspeed", flashspeed);
  props.get("name", name);
  props.get("enabled", enabled);
  props.print_unused_warnings("flahing-sign");

  if(spritename == "")
    throw std::runtime_error("No sprite name specified in FlashingSign");
  
  sprite = sprite2d_manager->create(spritename);
  flashdelta = game_time;
}

FlashingSign::~FlashingSign()
{
  delete sprite;
}

void
FlashingSign::draw(SceneContext& sc)
{
  if(!enabled)
    return;
  
  float time = game_time - flashdelta;
  
  if(static_cast<int>(time/flashspeed) % 2 == 0) {
    float alpha = fmodf(time, flashspeed) / flashspeed;
    // fade on
    sprite->set_alpha(alpha);
  } else {
    float alpha = 1.0 - (fmodf(time, flashspeed) / flashspeed);
    // fade off
    sprite->set_alpha(alpha);
  }
  sprite->draw(sc, pos, 10.0f);
}

void
FlashingSign::update(float elapsed_time)
{
  sprite->update(elapsed_time);
}

void
FlashingSign::enable()
{
  enabled = true;
  flashdelta = game_time;
}

void
FlashingSign::disable()
{
  enabled = false;
}
