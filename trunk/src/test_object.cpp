#include "test_object.hpp"

TestObject::TestObject()
{
  sprite = new Sprite3D("3dsprites/heroken.wsprite");
}

TestObject::~TestObject()
{
  delete sprite;
}

void
TestObject::draw(SceneContext& context)
{
  sprite->draw(context);
}

void
TestObject::update(float )
{
}

