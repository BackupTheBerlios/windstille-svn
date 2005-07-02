#ifndef __SPAWN_POINT_HPP__
#define __SPAWN_POINT_HPP__

#include <string>
#include <ClanLib/Core/Math/point.h>
#include "lisp/lisp.hpp"

class SpawnPoint
{
public:
  SpawnPoint(const lisp::Lisp* lisp);

  std::string name;
  CL_Pointf pos;
};

#endif

