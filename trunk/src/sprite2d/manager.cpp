#include <config.h>

#include "sprite2d/manager.hpp"
#include "sprite2d/data.hpp"
#include "sprite2d/sprite.hpp"

sprite2d::Manager* sprite2d_manager = 0;

namespace sprite2d
{

Manager::Manager()
{
}

Manager::~Manager()
{
  for(Datas::iterator i = datas.begin(); i != datas.end(); ++i)
    delete i->second;
}

const Data*
Manager::create_data(const std::string& filename)
{
  Datas::iterator i = datas.find(filename);
  if(i != datas.end())
    {
      return i->second;
    }
  else
    {  
      Data* data = new Data(filename);
      datas.insert(std::make_pair(filename, data));
      return data;
    }
}

}
