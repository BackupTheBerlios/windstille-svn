#include <config.h>

#include <stdexcept>
#include <sstream>
#include "wrapper_util.hpp"

void print_squirrel_stack(HSQUIRRELVM v)
{
    printf("--------------------------------------------------------------\n");
    int count = sq_gettop(v);
    for(int i = 1; i <= count; ++i) {
        printf("%d: ",i);
        switch(sq_gettype(v, i))
        {
            case OT_NULL:
                printf("null");        
                break;
            case OT_INTEGER: {
                int val;
                sq_getinteger(v, i, &val);
                printf("integer (%d)", val);
                break;
            }
            case OT_FLOAT: {
                float val;
                sq_getfloat(v, i, &val);
                printf("float (%f)", val);
                break;
            }
            case OT_STRING: {
                const char* val;
                sq_getstring(v, i, &val);
                printf("string (%s)", val);
                break;    
            }
            case OT_TABLE:
                printf("table");
                break;
            case OT_ARRAY:
                printf("array");
                break;
            case OT_USERDATA:
                printf("userdata");
                break;
            case OT_CLOSURE:        
                printf("closure(function)");    
                break;
            case OT_NATIVECLOSURE:
                printf("native closure(C function)");
                break;
            case OT_GENERATOR:
                printf("generator");
                break;
            case OT_USERPOINTER:
                printf("userpointer");
                break;
            case OT_THREAD:
                printf("thread");
                break;
            case OT_CLASS:
                printf("class");
                break;
            case OT_INSTANCE:
                printf("instance");
                break;
            default:
                printf("unknown?!?");
                break;
        }
        printf("\n");
    }
    printf("--------------------------------------------------------------\n");
}

//----------------------------------------------------------------------------

SquirrelError::SquirrelError(HSQUIRRELVM v, const std::string& message) throw()
{
  std::ostringstream msg;
  msg << "Squirrel error: " << message << " (";
  const char* lasterr;
  sq_getlasterror(v);
  if(sq_gettype(v, -1) != OT_STRING) {
    lasterr = "no error info";
  } else {
    sq_getstring(v, -1, &lasterr);
  }
  sq_pop(v, 1);
  msg << lasterr << ")";
  this->message = msg.str();
}

SquirrelError::~SquirrelError() throw()
{}

const char*
SquirrelError::what() const throw()
{
  return message.c_str();
}
