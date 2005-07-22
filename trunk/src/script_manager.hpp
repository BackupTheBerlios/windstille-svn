#ifndef __SCRIPT_INTERPRETER_H__
#define __SCRIPT_INTERPRETER_H__

#include <vector>
#include <list>
#include <squirrel.h>
#include <iostream>
#include "timer.hpp"
#include "scripting/wrapper.hpp"
#include "scripting/wrapper_util.hpp"

class ScriptManager
{
public:
  ScriptManager();
  ~ScriptManager();

  void update();

  void run_script_file(const std::string& filename);
  void run_script(const std::string& string, const std::string& sourcename);
  void run_script(std::istream& in, const std::string& sourcename);

  template<typename T>
  void expose_object(T* object, const std::string& name, bool free)
  {
    // part1 of registration of the instance in the root table
    sq_pushroottable(v);
    sq_pushstring(v, name.c_str(), -1);

    sq_pushroottable(v);
    SquirrelWrapper::create_squirrel_instance(v, object, free);
    sq_remove(v, -2);
    
    // register instance in root table
    if(sq_createslot(v, -3) < 0) {
      std::ostringstream msg;
      msg << "Couldn't register object '" << name << "' in squirrel root table";
      throw SquirrelError(v, msg.str());
    }

    sq_pop(v, 1);
  }
  void remove_object(const std::string& name);

  HSQUIRRELVM get_vm()
  {
    return v;
  }

  enum WakeupEvent {
    TIME                = (1 << 0),
    DIALOG_CLOSED       = (1 << 1),
    CONVERSATION_CLOSED = (1 << 2),
  };                  

  void set_wakeup_event(HSQUIRRELVM vm, WakeupEvent event, float timeout = -1);
  void fire_wakeup_event(WakeupEvent event);
  
  bool run_before(HSQUIRRELVM vm);

private:
  class SquirrelVM
  {
  public:
    SquirrelVM(const std::string& arg_name, HSQUIRRELVM arg_vm, HSQOBJECT arg_obj);
    std::string name;
    HSQUIRRELVM vm;
    HSQOBJECT vm_obj;
    float wakeup_time;
    int waiting_for_events;
  };
  
  typedef std::list<SquirrelVM> SquirrelVMs;
  SquirrelVMs squirrel_vms;
  std::map<std::string, bool> already_run_scripts;

  HSQUIRRELVM v;
};

extern ScriptManager* script_manager;

#endif

