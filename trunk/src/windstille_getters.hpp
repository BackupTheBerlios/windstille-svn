#ifndef __WINDSTILLE_GETTERS_HPP__
#define __WINDSTILLE_GETTERS_HPP__

#include "math/vector.hpp"

class Color;

namespace lisp
{

bool property_get(const lisp::Lisp* lisp, Color& col);
bool property_get(const lisp::Lisp* lisp, Vector& vec);

}

#endif

