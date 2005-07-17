#include <config.h>

#include "sprite3d/manager.hpp"
#include "sprite3d/data.hpp"
#include "sprite3d/sprite.hpp"

sprite3d::Manager* sprite3d_manager = 0;

namespace sprite3d
{

Manager::Manager()
{
}

Manager::~Manager()
{
  for(Datas::iterator i = datas.begin(); i != datas.end(); ++i)
    delete i->second;
}

Sprite*
Manager::create(const std::string& filename)
{
  Datas::iterator i = datas.find(filename);
  if(i != datas.end())
    return new Sprite(i->second);
  
  Data* data = new Data(filename);
  datas.insert(std::make_pair(filename, data));
  return new Sprite(data);
}

}
