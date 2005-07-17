#include "wrapper.interface.hpp"
#include "spawn_object.hpp"

#include <iostream>
#include "lisp/lisp.hpp"
#include "wrapper_util.hpp"
#include "sector.hpp"

namespace Scripting
{

using namespace lisp;

lisp::Lisp* table_to_lisp(HSQUIRRELVM v, int table_idx)
{
  using namespace lisp;
  Lisp* lisp = 0;

  // offset because of sq_pushnull
  if(table_idx < 0)
    table_idx--;

  // iterate table
  Lisp* cur = 0;
  sq_pushnull(v);
  while(SQ_SUCCEEDED(sq_next(v, table_idx))) {
    if(cur == 0) {
      lisp = new Lisp(Lisp::TYPE_CONS);
      cur = lisp;
    } else {
      cur->v.cons.cdr = new Lisp(Lisp::TYPE_CONS);
      cur = cur->v.cons.cdr;
    }
    
    // key is -2, value -1 now
    if(sq_gettype(v, -2) != OT_STRING) {
      std::cerr << "Table contains a non string key\n";
      continue;
    }
    const char* key;
    sq_getstring(v, -2, &key);

    Lisp* val = 0;
    switch(sq_gettype(v, -1)) {
      case OT_INTEGER:
        val = new Lisp(Lisp::TYPE_INTEGER);
        sq_getinteger(v, -1, &val->v.integer);
        break;
      case OT_FLOAT:
        val = new Lisp(Lisp::TYPE_REAL);
        sq_getfloat(v, -1, &val->v.real);
        break;
      case OT_STRING: {
        const char* str;
        sq_getstring(v, -1, &str);
        val = new Lisp(Lisp::TYPE_STRING, str);
        break;
      }
      case OT_BOOL: {
        SQBool boolean;
        sq_getbool(v, -1, &boolean);
        val = new Lisp(Lisp::TYPE_BOOLEAN);
        val->v.boolean = boolean;
      }
      case OT_TABLE:
        val = table_to_lisp(v, -1);
        break;
      case OT_ARRAY:
        // TODO...
      default:
        std::cerr << "Unsupported value type in table\n";
        break;
    }
    if(val == 0)
      continue;

    Lisp* node = new Lisp(Lisp::TYPE_CONS);
    cur->v.cons.car = node;

    node->v.cons.car = new Lisp(Lisp::TYPE_SYMBOL, key);
    if(val->get_type() == Lisp::TYPE_CONS) {
      node->v.cons.cdr = val;
    } else {
      node->v.cons.cdr = new Lisp(Lisp::TYPE_CONS);
      node->v.cons.cdr->v.cons.car = val;
      node->v.cons.cdr->v.cons.cdr = 0;
    }
    
    // pop table key and value
    sq_pop(v, 2);
  }
  if(cur != 0)
    cur->v.cons.cdr = 0;
  // pop iterator
  sq_pop(v, 1);

  return lisp;
}

int spawn_object(HSQUIRRELVM v)
{
  const char* objname;
  sq_getstring(v, 2, &objname);

  // FIXME: who delete's the lisp?
  lisp::Lisp* lisp = table_to_lisp(v, 3);
  if(lisp == 0) {
    // FIXME: this should never get called, instead Lisp should handle
    // the empty list (nil) properly, instead of just using NULL
    // pointer
    std::cerr << "Invalid or empty table specified for spawn_object\n";
    return 0;
  }
  try {
    Sector::current()->add_object(objname, lisp);
  } catch(std::exception& e) {
    std::cerr << "Error parsing object in spawn_object: " << e.what()
      << "\n";
  }

  return 0;
}

}
