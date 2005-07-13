#include "serialize.hpp"

#include <memory>
#include "lisp/lisp.hpp"
#include "lisp/parser.hpp"
#include "lisp/list_iterator.hpp"
#include "lisp/writer.hpp"

void load_squirrel_table(HSQUIRRELVM v, int table_idx, const lisp::Lisp* lisp)
{
  using namespace lisp;
  
  ListIterator iter(lisp);
  while(iter.next()) {
    sq_pushstring(v, iter.item().c_str(), iter.item().size());
    switch(iter.value().get_type()) {
      case Lisp::TYPE_CONS:
        sq_newtable(v);
        load_squirrel_table(v, sq_gettop(v), iter.lisp());
        break;
      case Lisp::TYPE_INTEGER:
        sq_pushinteger(v, iter.value().get_int());
        break;
      case Lisp::TYPE_REAL:
        sq_pushfloat(v, iter.value().get_float());
        break;
      case Lisp::TYPE_STRING:
        sq_pushstring(v, iter.value().get_string().c_str(), -1);
        break;
      case Lisp::TYPE_BOOLEAN:
        sq_pushbool(v, iter.value().get_bool());
        break;
      case Lisp::TYPE_SYMBOL:
        std::cerr << "Unexpected symbol in lisp file...";
        sq_pushnull(v);
        break;
      default:
        assert(false);
        break;
    }
    printf("Set: %s.\n", iter.item().c_str());
    if(table_idx < 0) {
      sq_createslot(v, table_idx - 2);
    } else {
      sq_createslot(v, table_idx);
    }
  }
}

void load_squirrel_table(HSQUIRRELVM v, int table_idx, const std::string& file)
{
  using namespace lisp;
  std::auto_ptr<Lisp> root (Parser::parse(file));

  const lisp::Lisp* table = root->get_lisp("squirrel-state");
  if(!table)
    throw std::runtime_error("Not a squirrel-state file");
  load_squirrel_table(v, table_idx, table);
}

void save_squirrel_table(HSQUIRRELVM v, int table_idx, lisp::Writer& writer)
{
  // offset because of sq_pushnull
  if(table_idx < 0)
    table_idx--;
  
  //iterator table
  sq_pushnull(v);
  while(SQ_SUCCEEDED(sq_next(v, table_idx))) {
    if(sq_gettype(v, -2) != OT_STRING) {
      std::cerr << "Table contains non-string key\n";
      continue;
    }
    const char* key;
    sq_getstring(v, -2, &key);

    switch(sq_gettype(v, -1)) {
      case OT_INTEGER: {
        int val;
        sq_getinteger(v, -1, &val);
        writer.write_int(key, val);
        break;
      }
      case OT_FLOAT: {
        float val;
        sq_getfloat(v, -1, &val);
        writer.write_float(key, val);
        break;
      }
      case OT_BOOL: {
        SQBool val;
        sq_getbool(v, -1, &val);
        writer.write_bool(key, val);
        break;
      }
      case OT_STRING: {
        const char* str;
        sq_getstring(v, -1, &str);
        writer.write_string(key, str);
        break;
      }
      case OT_TABLE: {
        writer.start_list(key);
        save_squirrel_table(v, -1, writer);
        writer.end_list(key);
        break;
      }
      case OT_CLOSURE:
        break; // ignore
      case OT_NATIVECLOSURE:
        break;
      default:
        std::cerr << "Can't serialize key '" << key << "' in table.\n";
        break;
    }
    sq_pop(v, 2);
  }
  sq_pop(v, 1);
}

void save_squirrel_table(HSQUIRRELVM v, int table_idx, const std::string& file)
{
  lisp::Writer writer(file);

  writer.start_list("squirrel-state");
  save_squirrel_table(v, table_idx, writer);
  writer.end_list("squirrel-state");
}
