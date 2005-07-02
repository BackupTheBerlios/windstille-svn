#include "game_objects.hpp"

namespace Scripting
{

const std::string&
GameObject::get_name() const
{
  return object->get_name();
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

}

