#include "test_object.hpp"
#include "sprite3d/sprite3d_manager.hpp"

TestObject::TestObject()
{
  sprite = sprite3d_manager->create("3dsprites/heroken.wsprite");
  pos = Vector(12*32, 26*32, 100);
  name = "TEST";
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
TestObject::update(float )
{
}

void
TestObject::set_sprite(const std::string& filename)
{
  try {
    Sprite3D* newsprite = sprite3d_manager->create(filename);
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
  sprite->set_vflip(vflip);
}

