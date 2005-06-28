#ifndef __SCRIPT_INTERPRETER_H__
#define __SCRIPT_INTERPRETER_H__

#include <vector>
#include <list>
#include <squirrel.h>
#include <iostream>
#include "timer.hpp"

class ScriptManager
{
public:
  ScriptManager();
  ~ScriptManager();

  void update();

  void run_script(const std::string& script, const std::string& sourcename = "");
  void run_script(std::istream& in, const std::string& sourcename = "");
  
  void expose_object(void* object, const std::string& name,
                     const std::string& type);

  HSQUIRRELVM create_coroutine();

  void set_wakeup_time(HSQUIRRELVM vm, float time);

private:
  void cleanup_coroutine(HSQUIRRELVM vm);
  
  struct WaitingVM {
    HSQUIRRELVM vm;
    float wakeup_time;
  };
  typedef std::list<WaitingVM> WaitingVMs;
  WaitingVMs waiting_vms;
  
  HSQUIRRELVM v;
};

extern ScriptManager* script_manager;

#endif

