/**
 * WARNING: This file is automatically generated from:
 *  'src/scripting/wrapper.interface.hpp'
 * DO NOT CHANGE
 */
#include <config.h>

#include <new>
#include <assert.h>
#include <string>
#include <squirrel.h>
#include "wrapper_util.hpp"
#include "wrapper.interface.hpp"

namespace SquirrelWrapper
{

static int GameObject_release_hook(SQUserPointer ptr, int )
{
  Scripting::GameObject* _this = reinterpret_cast<Scripting::GameObject*> (ptr);
  delete _this;
  return 0;
}

void create_squirrel_instance(HSQUIRRELVM v, Scripting::GameObject* object, bool setup_releasehook)
{
  sq_pushstring(v, "GameObject", -1);
  if(sq_get(v, -2) < 0) {
    std::ostringstream msg;
    msg << "Couldn't resolved squirrel type 'GameObject'";
    throw SquirrelError(v, msg.str());
  }

  if(sq_createinstance(v, -1) < 0 || sq_setinstanceup(v, -1, object) < 0) {
    std::ostringstream msg;
    msg << "Couldn't setup squirrel instance for object of type 'GameObject'";
    throw SquirrelError(v, msg.str());
  }
  sq_remove(v, -2);

  if(setup_releasehook) {
    sq_setreleasehook(v, -1, GameObject_release_hook);
  }
}
static int GameObject_get_name_wrapper(HSQUIRRELVM v)
{
  Scripting::GameObject* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  
  const std::string& return_value = _this->get_name();
  
  sq_pushstring(v, return_value.c_str(), return_value.size());
  return 1;
}

static int FlashingSign_release_hook(SQUserPointer ptr, int )
{
  Scripting::FlashingSign* _this = reinterpret_cast<Scripting::FlashingSign*> (ptr);
  delete _this;
  return 0;
}

void create_squirrel_instance(HSQUIRRELVM v, Scripting::FlashingSign* object, bool setup_releasehook)
{
  sq_pushstring(v, "FlashingSign", -1);
  if(sq_get(v, -2) < 0) {
    std::ostringstream msg;
    msg << "Couldn't resolved squirrel type 'FlashingSign'";
    throw SquirrelError(v, msg.str());
  }

  if(sq_createinstance(v, -1) < 0 || sq_setinstanceup(v, -1, object) < 0) {
    std::ostringstream msg;
    msg << "Couldn't setup squirrel instance for object of type 'FlashingSign'";
    throw SquirrelError(v, msg.str());
  }
  sq_remove(v, -2);

  if(setup_releasehook) {
    sq_setreleasehook(v, -1, FlashingSign_release_hook);
  }
}
static int FlashingSign_enable_wrapper(HSQUIRRELVM v)
{
  Scripting::FlashingSign* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  
  _this->enable();
  
  return 0;
}

static int FlashingSign_disable_wrapper(HSQUIRRELVM v)
{
  Scripting::FlashingSign* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  
  _this->disable();
  
  return 0;
}

static int set_sector_wrapper(HSQUIRRELVM v)
{
  const char* arg0;
  sq_getstring(v, 2, &arg0);
  
  Scripting::set_sector(arg0);
  
  return 0;
}

static int play_music_wrapper(HSQUIRRELVM v)
{
  const char* arg0;
  sq_getstring(v, 2, &arg0);
  
  Scripting::play_music(arg0);
  
  return 0;
}

static int stop_music_wrapper(HSQUIRRELVM v)
{
  float arg0;
  sq_getfloat(v, 2, &arg0);
  
  Scripting::stop_music(arg0);
  
  return 0;
}

static int play_sound_wrapper(HSQUIRRELVM v)
{
  const char* arg0;
  sq_getstring(v, 2, &arg0);
  
  Scripting::play_sound(arg0);
  
  return 0;
}

static int set_dialog_wrapper(HSQUIRRELVM v)
{
  int arg0;
  sq_getinteger(v, 2, &arg0);
  const char* arg1;
  sq_getstring(v, 3, &arg1);
  const char* arg2;
  sq_getstring(v, 4, &arg2);
  
  Scripting::set_dialog(arg0, arg1, arg2);
  
  return 0;
}

static int show_dialog_wrapper(HSQUIRRELVM v)
{
  float arg0;
  sq_getfloat(v, 2, &arg0);
  
  Scripting::show_dialog(arg0);
  
  return 0;
}

static int hide_dialog_wrapper(HSQUIRRELVM v)
{
  float arg0;
  sq_getfloat(v, 2, &arg0);
  
  Scripting::hide_dialog(arg0);
  
  return 0;
}

static int wait_wrapper(HSQUIRRELVM v)
{
  HSQUIRRELVM arg0 = v;
  float arg1;
  sq_getfloat(v, 2, &arg1);
  
  Scripting::wait(arg0, arg1);
  
  return sq_suspendvm(v);
}

static int wait_for_dialog_wrapper(HSQUIRRELVM v)
{
  HSQUIRRELVM arg0 = v;
  
  Scripting::wait_for_dialog(arg0);
  
  return sq_suspendvm(v);
}

void register_windstille_wrapper(HSQUIRRELVM v)
{
  sq_pushroottable(v);
  sq_pushstring(v, "VCENTER", -1);
  sq_pushinteger(v, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register constant'VCENTER'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "LEFT", -1);
  sq_pushinteger(v, 1);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register constant'LEFT'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "RIGHT", -1);
  sq_pushinteger(v, 2);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register constant'RIGHT'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "HCENTER", -1);
  sq_pushinteger(v, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register constant'HCENTER'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "TOP", -1);
  sq_pushinteger(v, 16);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register constant'TOP'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "BOTTOM", -1);
  sq_pushinteger(v, 32);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register constant'BOTTOM'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "set_sector", -1);
  sq_newclosure(v, &set_sector_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'set_sector'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "play_music", -1);
  sq_newclosure(v, &play_music_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'play_music'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "stop_music", -1);
  sq_newclosure(v, &stop_music_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'stop_music'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "play_sound", -1);
  sq_newclosure(v, &play_sound_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'play_sound'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "set_dialog", -1);
  sq_newclosure(v, &set_dialog_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'set_dialog'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "show_dialog", -1);
  sq_newclosure(v, &show_dialog_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'show_dialog'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "hide_dialog", -1);
  sq_newclosure(v, &hide_dialog_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'hide_dialog'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "wait", -1);
  sq_newclosure(v, &wait_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'wait'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "wait_for_dialog", -1);
  sq_newclosure(v, &wait_for_dialog_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'wait_for_dialog'";
    throw SquirrelError(v, msg.str());
  }

  // Register class GameObject
  sq_pushstring(v, "GameObject", -1);
  if(sq_newclass(v, SQFalse) < 0) {
    std::ostringstream msg;
    msg << "Couldn't create new class 'GameObject'";
    throw SquirrelError(v, msg.str());
  }
  sq_pushstring(v, "get_name", -1);
  sq_newclosure(v, &GameObject_get_name_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'get_name'";
    throw SquirrelError(v, msg.str());
  }

  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register class'GameObject'";
    throw SquirrelError(v, msg.str());
  }

  // Register class FlashingSign
  sq_pushstring(v, "FlashingSign", -1);
  sq_pushstring(v, "GameObject", -1);
  sq_get(v, -3);
  if(sq_newclass(v, SQTrue) < 0) {
    std::ostringstream msg;
    msg << "Couldn't create new class 'FlashingSign'";
    throw SquirrelError(v, msg.str());
  }
  sq_pushstring(v, "enable", -1);
  sq_newclosure(v, &FlashingSign_enable_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'enable'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "disable", -1);
  sq_newclosure(v, &FlashingSign_disable_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'disable'";
    throw SquirrelError(v, msg.str());
  }

  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register class'FlashingSign'";
    throw SquirrelError(v, msg.str());
  }

  sq_pop(v, 1);
}

}

