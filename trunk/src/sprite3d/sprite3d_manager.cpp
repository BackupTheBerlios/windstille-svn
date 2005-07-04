#include "sprite3d_manager.hpp"
#include "sprite3d_data.hpp"
#include "sprite3d.hpp"

Sprite3DManager* sprite3d_manager = 0;

Sprite3DManager::Sprite3DManager()
{
}

Sprite3DManager::~Sprite3DManager()
{
  for(Datas::iterator i = datas.begin(); i != datas.end(); ++i)
    delete i->second;
}

Sprite3D*
Sprite3DManager::create(const std::string& filename)
{
  Datas::iterator i = datas.find(filename);
  if(i == datas.end()) {
    Sprite3DData* data = new Sprite3DData(filename);
    datas.insert(std::make_pair(filename, data));
    return new Sprite3D(data);
  }

  return new Sprite3D(i->second);
}

