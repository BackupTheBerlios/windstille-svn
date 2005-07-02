#include "spawnpoint.hpp"

#include <iostream>
#include <stdexcept>
#include "lisp/list_iterator.hpp"

SpawnPoint::SpawnPoint(const lisp::Lisp* lisp)
{
  lisp::ListIterator iter(lisp);
  while(iter.next()) {
    if(iter.item() == "name")
      name = iter.value().get_string();
    else if(iter.item() == "x")
      pos.x = iter.value().get_float();
    else if(iter.item() == "y")
      pos.y = iter.value().get_float();
    else {
      std::cerr << "Skipping unknown tag '" << iter.item() 
                << "' in SpawnPoint.\n";
    }
  }

  if(name == "")
    throw std::runtime_error("No name specified for spawnpoint");
}
