#include "flashing_sign.hpp"

#include <math.h>
#include "lisp/list_iterator.hpp"
#include "timer.hpp"
#include "globals.hpp"
#include "sprite2d/manager.hpp"

FlashingSign::FlashingSign(const lisp::Lisp* lisp)
  : pos(0, 0, 10.0), enabled(false)
{
  std::string spritename;
  flashspeed = 5;

  lisp::ListIterator iter(lisp);
  while(iter.next()) {
    if(iter.item() == "sprite") {
      spritename = iter.value().get_string();
    } else if(iter.item() == "x") {
      pos.x = iter.value().get_float();
    } else if(iter.item() == "y") {
      pos.y = iter.value().get_float();
    } else if(iter.item() == "z") {
      pos.z = iter.value().get_float();
    } else if(iter.item() == "flashspeed") {
      flashspeed = iter.value().get_float();
    } else if(iter.item() == "name") {
      name = iter.value().get_string();
    } else if(iter.item() == "enabled") {
      enabled = iter.value().get_bool();
    } else {
      std::cerr << "Skipping unknown attribute '" 
                << iter.item() << "' in FlashingSign\n";
    }
  }

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
  sprite->draw(sc, pos);
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
