#include <config.h>

#include "lisp/lisp.hpp"
#include "lisp/getters.hpp"
#include "windstille_getters.hpp"
#include "math/vector.hpp"
#include "color.hpp"

namespace lisp
{

bool property_get(const lisp::Lisp* lisp, Color& color)
{
  std::vector<float> col;
  if(property_get(lisp, col) == false)
    return false;
  if(col.size() == 3)
    color = Color(col[0], col[1], col[2]);
  else if(col.size() == 4)
    color = Color(col[0], col[1], col[2], col[3]);
  else
    return false;
  
  return true;
}

bool property_get(const lisp::Lisp* lisp, Vector& vec)
{
  size_t size = lisp->get_list_size();
  if(size != 4 && size != 3)
    return false;
  if(get(lisp->get_list_elem(1), vec.x) == false)
    return false;
  if(get(lisp->get_list_elem(2), vec.y) == false)
    return false;
  return true;
}

}
