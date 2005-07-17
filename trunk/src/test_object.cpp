#include "test_object.hpp"
#include "sprite3d/manager.hpp"
#include "lisp/list_iterator.hpp"

TestObject::TestObject(const lisp::Lisp* lisp)
{
  pos = Vector(0, 0, 100);
  std::string spritename;

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
    } else if(iter.item() == "name") {
      name = iter.value().get_string();
    } else {
      std::cerr << "Skipping unknown attribute '" 
                << iter.item() << "' in Box\n";
    }
  }

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
  sprite->draw(context, pos);
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

