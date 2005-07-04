#ifndef __SPRITE3D_MANAGER_HPP__
#define __SPRITE3D_MANAGER_HPP__

#include <string>
#include <map>

class Sprite3D;
class Sprite3DData;

class Sprite3DManager
{
public:
  Sprite3DManager();
  ~Sprite3DManager();

  Sprite3D* create(const std::string& filename);

private:
  typedef std::map<std::string, Sprite3DData*> Datas;
  Datas datas;
};

extern Sprite3DManager* sprite3d_manager;

#endif

