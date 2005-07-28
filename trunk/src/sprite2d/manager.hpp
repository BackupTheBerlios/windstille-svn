#ifndef __SPRITE2D_MANAGER_HPP__
#define __SPRITE2D_MANAGER_HPP__

#include <string>
#include <map>

namespace sprite2d
{
class Data;

class Manager
{
public:
  Manager();
  ~Manager();

  const Data* create_data(const std::string& filename);
  
private:
  typedef std::map<std::string, Data*> Datas;
  Datas datas;
};

}

extern sprite2d::Manager* sprite2d_manager;

#endif

