#ifndef __SCRIPT_INTERPRETER_H__
#define __SCRIPT_INTERPRETER_H__

#include <vector>
#include <squirrel.h>
#include <iostream>
#include "timer.hpp"

class ScriptManager
{
public:
  ScriptManager();
  ~ScriptManager();

  void update();

  void run_script(std::istream& in, const std::string& sourcename = "");
  
  void expose_object(void* object, const std::string& name,
                     const std::string& type);

  void set_wakeup_time(float seconds);

  HSQUIRRELVM create_coroutine();

private:
  struct WaitingVM {
    HSQUIRRELVM vm;
    float wakeup_time;
  };
  std::vector<WaitingVM> waiting_vms;
  
  HSQUIRRELVM v;
};

extern ScriptManager* script_manager;

#endif

