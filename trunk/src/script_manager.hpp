#ifndef __SCRIPT_INTERPRETER_H__
#define __SCRIPT_INTERPRETER_H__

#include <vector>
#include <list>
#include <map>
#include <squirrel.h>
#include <iostream>
#include "timer.hpp"
#include "scripting/wrapper.hpp"
#include "scripting/wrapper_util.hpp"

/**
 * This class is responsible for managing all running squirrel threads
 * (called coroutines by others)
 * It contains functions for loading and starting script, keeps a list of
 * suspended scripts and receives wakeup events for them
 */
class ScriptManager
{
public:
  ScriptManager();
  ~ScriptManager();

  void update();

  void run_script_file(const std::string& filename);
  void run_script(const std::string& string, const std::string& sourcename);
  void run_script(std::istream& in, const std::string& sourcename);

  HSQUIRRELVM get_vm() const
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

