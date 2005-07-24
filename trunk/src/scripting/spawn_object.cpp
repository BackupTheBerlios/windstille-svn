#include "wrapper.interface.hpp"
#include "spawn_object.hpp"

#include <iostream>
#include "lisp/lisp.hpp"
#include "wrapper_util.hpp"
#include "sector.hpp"

namespace Scripting
{

using namespace lisp;

lisp::Lisp* table_to_lisp(HSQUIRRELVM v, int table_idx,
                          std::vector<lisp::Lisp*>& entries)
{
  using namespace lisp;
  Lisp* lisp = 0;

  // offset because of sq_pushnull
  if(table_idx < 0)
    table_idx--;

  // iterate table
  sq_pushnull(v);
  while(SQ_SUCCEEDED(sq_next(v, table_idx))) {
    // key is -2, value -1 now
    if(sq_gettype(v, -2) != OT_STRING) {
      std::cerr << "Table contains a non string key\n";
      continue;
    }
    const char* key;
    sq_getstring(v, -2, &key);

    std::vector<Lisp*> childs;
    childs.push_back(new Lisp(Lisp::TYPE_SYMBOL, key));

    Lisp* val = 0;
    switch(sq_gettype(v, -1)) {
      case OT_INTEGER: {
        int val;
        sq_getinteger(v, -1, &val);
        childs.push_back(new Lisp(val));
        break;
      }
      case OT_FLOAT: {
        float val;
        sq_getfloat(v, -1, &val);
        childs.push_back(new Lisp(val));
        break;
      }
      case OT_STRING: {
        const char* str;
        sq_getstring(v, -1, &str);
        val = new Lisp(Lisp::TYPE_STRING, str);
        break;
      }
      case OT_BOOL: {
        SQBool boolean;
        sq_getbool(v, -1, &boolean);
        val = new Lisp((bool) boolean);
        break;
      }
      case OT_TABLE: {
        table_to_lisp(v, -1, childs);
        break;
      }
      case OT_ARRAY:
        // TODO...
      default:
        std::cerr << "Unsupported value type in table\n";
        break;
    }

    // pop table key and value
    sq_pop(v, 2);

    entries.push_back(new Lisp(childs));
  }
  // pop iterator
  sq_pop(v, 1);

  return lisp;
}

int spawn_object(HSQUIRRELVM v)
{
  using namespace lisp;
  
  const char* objname;
  sq_getstring(v, 2, &objname);

  std::vector<lisp::Lisp*> entries;
  entries.push_back(new Lisp(Lisp::TYPE_SYMBOL, objname));
  table_to_lisp(v, 3, entries);
  std::auto_ptr<Lisp> lisp (new Lisp(entries));
  try {
    Sector::current()->add_object(objname, lisp.get());
  } catch(std::exception& e) {
    std::cerr << "Error parsing object in spawn_object: " << e.what()
      << "\n";
  }

  return 0;
}

}
