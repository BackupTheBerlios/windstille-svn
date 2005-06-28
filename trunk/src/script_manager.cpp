#include "script_manager.hpp"

#include <stdarg.h>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <sqstdio.h>
#include <sqstdaux.h>
#include <sqstdblob.h>
#include <sqstdsystem.h>
#include <sqstdmath.h>
#include <sqstdstring.h>

#include "console.hxx"
#include "timer.hpp"
#include "scripting/wrapper.hpp"
#include "scripting/wrapper_util.hpp"

ScriptManager* script_manager = 0;

static void printfunc(HSQUIRRELVM, const char* str, ...)
{
  char buf[4096];
  va_list arglist; 
  va_start(arglist, str); 
  vsprintf(buf, str, arglist);
  Console::current()->add(buf);
  puts(buf);
  va_end(arglist); 
}

ScriptManager::ScriptManager()
{
  v = sq_open(1024);
  if(v == 0)
    throw std::runtime_error("Couldn't initialize squirrel vm");

  // register default error handlers
  sqstd_seterrorhandlers(v);
  // register squirrel libs
  sq_pushroottable(v);
  if(sqstd_register_bloblib(v) < 0)
    throw SquirrelError(v, "Couldn't register blob lib");
  if(sqstd_register_iolib(v) < 0)
    throw SquirrelError(v, "Couldn't register io lib");
  if(sqstd_register_systemlib(v) < 0)
    throw SquirrelError(v, "Couldn't register system lib");
  if(sqstd_register_mathlib(v) < 0)
    throw SquirrelError(v, "Couldn't register math lib");
  if(sqstd_register_stringlib(v) < 0)
    throw SquirrelError(v, "Couldn't register string lib");

  // register print function
  sq_setprintfunc(v, printfunc);
  
  // register windstille API
  register_functions(v, windstille_global_functions);
  register_classes(v, windstille_classes);
}

ScriptManager::~ScriptManager()
{
  sq_close(v);
}

static SQInteger squirrel_read_char(SQUserPointer file)
{
  std::istream* in = reinterpret_cast<std::istream*> (file);
  char c = in->get();
  if(in->eof())
    return 0;    
  return c;
}

void
ScriptManager::run_script(const std::string& script,
        const std::string& sourcename)
{
  std::istringstream stream(script);
  run_script(stream, sourcename);
}

void
ScriptManager::run_script(std::istream& in, const std::string& sourcename)
{
  HSQUIRRELVM vm = create_coroutine();
  
  printf("Stackbefore:\n");
  print_squirrel_stack(vm);
  if(sq_compile(vm, squirrel_read_char, &in, sourcename.c_str(), true) < 0)
    throw SquirrelError(vm, "Couldn't parse script");
 
  sq_push(vm, -2);
  if(sq_call(vm, 1, false) < 0)
    throw SquirrelError(vm, "Couldn't start script");
  cleanup_coroutine(vm);

  printf("After:\n");
  print_squirrel_stack(vm);
}

void
ScriptManager::expose_object(void* object, const std::string& name,
                                 const std::string& type)
{
  // part1 of registration of the instance in the root table
  sq_pushroottable(v);
  sq_pushstring(v, name.c_str(), -1);

  // resolve class name
  sq_pushroottable(v);
  sq_pushstring(v, type.c_str(), -1);
  if(sq_get(v, -2) < 0) {
    std::ostringstream msg;
    msg << "Couldn't resolve squirrel type '" << type << "'.";
    throw std::runtime_error(msg.str());
  }
  sq_remove(v, -2); // remove roottable
  
  // create an instance and set pointer to c++ object
  if(sq_createinstance(v, -1) < 0 || sq_setinstanceup(v, -1, object)) {
    std::ostringstream msg;
    msg << "Couldn't setup squirrel instance for object '"
        << name << "' of type '" << type << "'.";
    throw SquirrelError(v, msg.str());
  }
  
  sq_remove(v, -2); // remove class from stack
  
  // part2 of registration of the instance in the root table
  if(sq_createslot(v, -3) < 0)
    throw SquirrelError(v, "Couldn't register object in squirrel root table");    sq_pop(v, 1);
}

void
ScriptManager::cleanup_coroutine(HSQUIRRELVM vm)
{
  if(sq_getvmstate(vm) != SQ_VMSTATE_SUSPENDED) {
    // FIXME
    // sq_close(vm);
  } else {
    bool found = false;
    for(WaitingVMs::iterator i = waiting_vms.begin();
        i != waiting_vms.end(); ++i) {
      if(i->vm == vm) {
        found = true;
        break;
      }
    }
    if(!found) {
      std::cerr << "Warning: Script suspended but not in wakeup list!\n";
      // FIXME
      //sq_close(vm);
    }  
  }
}

void
ScriptManager::update()
{
  for(WaitingVMs::iterator i = waiting_vms.begin(); i != waiting_vms.end(); ) {
    WaitingVM& waiting_vm = *i;
    
    if(waiting_vm.wakeup_time > game_time) {
      try {
        if(sq_wakeupvm(waiting_vm.vm, false, false) < 0) {
          throw SquirrelError(waiting_vm.vm, "Couldn't resume script");
        }

        cleanup_coroutine(waiting_vm.vm);
      } catch(std::exception& e) {
        std::cerr << "Problem executing script: " << e.what() << "\n";
      }

      // remove (but check that the script hasn't set a new wakeup_time)
      if(waiting_vm.wakeup_time > game_time)
        i = waiting_vms.erase(i);
    } else {
      ++i;
    }
  }
}

void
ScriptManager::set_wakeup_time(HSQUIRRELVM vm, float time)
{
  // search if the VM is already in the wakeup list and update it
  for(WaitingVMs::iterator i = waiting_vms.begin();
      i != waiting_vms.end(); ++i) {
    WaitingVM& waiting_vm = *i;
    if(waiting_vm.vm == vm) {
      waiting_vm.wakeup_time = game_time + time;
      return;
    }
  }

  // create a new entry
  WaitingVM waiting_vm;
  waiting_vm.vm = vm;
  waiting_vm.wakeup_time = game_time + time;
}


HSQUIRRELVM
ScriptManager::create_coroutine()
{
  HSQUIRRELVM vm = sq_newthread(v, 1024);
  if(vm == 0)
    throw SquirrelError(v, "Couldn't create new VM");

  return vm;
}
