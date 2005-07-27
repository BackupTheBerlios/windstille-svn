#include <config.h>

#include "test_object.hpp"
#include "sprite3d/manager.hpp"
#include "lisp/properties.hpp"
#include "windstille_getters.hpp"

TestObject::TestObject(const lisp::Lisp* lisp)
{
  using namespace lisp;
  pos = Vector(0, 0);
  std::string spritename;

  Properties props(lisp);
  props.get("sprite", spritename);
  props.get("pos", pos);
  props.get("name", name);
  props.print_unused_warnings("testobject");

  if(spritename == "")
    throw std::runtime_error("No sprite name specified in Box");
  sprite = sprite3d_manager->create(spritename);
}

TestObject::~TestObject()
{
  delete sprite;
}

void
TestObject::draw(SceneContext& context)
{
  sprite->draw(context, pos, 100);
}

void
TestObject::update(float elapsed_time)
{
  sprite->update(elapsed_time);
}

void
TestObject::set_sprite(const std::string& filename)
{
  try {
    sprite3d::Sprite* newsprite = sprite3d_manager->create(filename);
    delete sprite;
    sprite = newsprite;
  } catch(std::exception& e) {
    std::cerr << "Couldn't change sprite to '" << filename << "': " 
              << e.what() << "\n";
  }
}

void
TestObject::set_action(const std::string& action)
{
  try {
    sprite->set_action(action);
  } catch(std::exception& e) {
    std::cerr << "Couldn't change action to '" << action << "': "
              << e.what() << "\n";
  }
}

void
TestObject::set_pos(const Vector& pos)
{
  this->pos = pos;
}

void
TestObject::set_vflip(bool vflip)
{
  sprite->set_rot(vflip);
}

