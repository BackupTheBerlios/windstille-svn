#include "wrapper.interface.hpp"
#include "game_objects.hpp"

namespace Scripting
{

const std::string&
GameObject::get_name() const
{
  return object->get_name();
}

void
GameObject::remove()
{
  object->remove();
}

void
FlashingSign::enable()
{
  obj()->enable();
}

void
FlashingSign::disable()
{
  obj()->disable();
}

void
TestObject::set_sprite(const std::string& filename)
{
  obj()->set_sprite(filename);
}

void
TestObject::set_action(const std::string& animation)
{
  obj()->set_action(animation);
}

void
TestObject::set_pos(float x, float y)
{
  obj()->set_pos(Vector(x, y));
}

void
TestObject::set_vflip(bool vflip)
{
  obj()->set_vflip(vflip);
}

void
Player::start_listening()
{
  obj()->start_listening();
}

void
Player::stop_listening()
{
  obj()->stop_listening();
}

}

