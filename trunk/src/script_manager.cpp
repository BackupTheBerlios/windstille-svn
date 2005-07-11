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

#include "console.hpp"
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
  SquirrelWrapper::register_windstille_wrapper(v);
}

ScriptManager::~ScriptManager()
{
  for(WaitingVMs::iterator i = waiting_vms.begin();
      i != waiting_vms.end(); ++i) {
    sq_release(v, &(i->vm_obj));
  }
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
  HSQUIRRELVM vm = sq_newthread(v, 1024);
  if(vm == 0)
    throw SquirrelError(v, "Couldn't create new VM");

  // retrieve reference to thread from stack and increase refcounter
  HSQOBJECT vm_obj;
  sq_resetobject(&vm_obj);
  if(sq_getstackobj(v, -1, &vm_obj) < 0)
    throw SquirrelError(v, "Couldn't get coroutine vm from stack");
  sq_addref(v, &vm_obj);
  sq_pop(v, 1);
  
  if(sq_compile(vm, squirrel_read_char, &in, sourcename.c_str(), true) < 0)
    throw SquirrelError(vm, "Couldn't parse script");

  sq_pushroottable(vm);
  if(sq_call(vm, 1, false) < 0)
    throw SquirrelError(vm, "Couldn't start script");

  handle_suspends(vm, vm_obj);
}

bool
ScriptManager::handle_suspends(HSQUIRRELVM vm, HSQOBJECT vm_obj)
{
  if(sq_getvmstate(vm) == SQ_VMSTATE_SUSPENDED) {
    bool found = false;
    for(WaitingVMs::iterator i = waiting_vms.begin();
        i != waiting_vms.end(); ++i) {
      if(i->vm == vm) {
        // make sure vm_obj is assigned (it will not be assigned directly after
        // the sq_call call in run_script)
        i->vm_obj = vm_obj;
        found = true;
        break;
      }
    }
    if(!found) {
      std::cerr << "Warning: Script suspended but not in wakeup list!\n";
      sq_release(v, &vm_obj);
      return true;
    }
  } else {
    sq_release(v, &vm_obj);
    return true;
  }

  return false;
}

void
ScriptManager::update()
{
  for(WaitingVMs::iterator i = waiting_vms.begin(); i != waiting_vms.end(); ) {
    WaitingVM& waiting_vm = *i;
    
    if(waiting_vm.wakeup_time > 0 && game_time >= waiting_vm.wakeup_time) {
      waiting_vm.waiting_for_events = 0;
      try {
        if(sq_wakeupvm(waiting_vm.vm, false, false) < 0) {
          throw SquirrelError(waiting_vm.vm, "Couldn't resume script");
        }
      } catch(std::exception& e) {
        sq_release(v, &waiting_vm.vm_obj);
        std::cerr << "Problem executing script: " << e.what() << "\n";
        i = waiting_vms.erase(i);
        continue;
      }
      bool stopped = handle_suspends(waiting_vm.vm, waiting_vm.vm_obj);

      // remove (but check that the script hasn't set a new wakeup_time)
      if(stopped)
        i = waiting_vms.erase(i);
    } else {
      ++i;
    }
  }
}

void
ScriptManager::set_wakeup_event(HSQUIRRELVM vm, WakeupEvent event, float time)
{
  // search if the VM is already in the wakeup list and update it
  for(WaitingVMs::iterator i = waiting_vms.begin();
      i != waiting_vms.end(); ++i) {
    WaitingVM& waiting_vm = *i;
    if(waiting_vm.vm == vm) {
      if(time < 0) {
        waiting_vm.wakeup_time = -1;
      } else {
        waiting_vm.wakeup_time = game_time + time;
      }
      waiting_vm.waiting_for_events |= event;
      return;
    }
  }

  // create a new entry
  WaitingVM waiting_vm;
  waiting_vm.vm = vm;
  if(time < 0) {
    waiting_vm.wakeup_time = -1;
  } else {
    waiting_vm.wakeup_time = game_time + time;
  }
  waiting_vm.waiting_for_events = event;
  waiting_vms.push_back(waiting_vm);
}

void
ScriptManager::remove_object(const std::string& name)
{
  sq_pushroottable(v);
  sq_pushstring(v, name.c_str(), -1);
  if(sq_deleteslot(v, -2, SQFalse) < 0) {
    std::ostringstream msg;
    msg << "Couldn't remove squirrel object '" << name << "'";
    throw SquirrelError(v, msg.str());
  }
  sq_pop(v, 1);
}

void
ScriptManager::fire_wakeup_event(WakeupEvent event)
{
  for(WaitingVMs::iterator i = waiting_vms.begin();
      i != waiting_vms.end(); ++i) {
    WaitingVM& vm = *i;
    if(vm.waiting_for_events & event) {
      vm.wakeup_time = game_time;
    }
  }
}

