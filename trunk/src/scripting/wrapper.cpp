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

static int GameObject_remove_wrapper(HSQUIRRELVM v)
{
  Scripting::GameObject* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  
  _this->remove();
  
  return 0;
}

static int GameObject_set_active_wrapper(HSQUIRRELVM v)
{
  Scripting::GameObject* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  SQBool arg0;
  sq_getbool(v, 2, &arg0);
  
  _this->set_active(arg0);
  
  return 0;
}

static int GameObject_set_parent_wrapper(HSQUIRRELVM v)
{
  Scripting::GameObject* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  const char* arg0;
  sq_getstring(v, 2, &arg0);
  
  _this->set_parent(arg0);
  
  return 0;
}

static int TestObject_release_hook(SQUserPointer ptr, int )
{
  Scripting::TestObject* _this = reinterpret_cast<Scripting::TestObject*> (ptr);
  delete _this;
  return 0;
}

void create_squirrel_instance(HSQUIRRELVM v, Scripting::TestObject* object, bool setup_releasehook)
{
  sq_pushstring(v, "TestObject", -1);
  if(sq_get(v, -2) < 0) {
    std::ostringstream msg;
    msg << "Couldn't resolved squirrel type 'TestObject'";
    throw SquirrelError(v, msg.str());
  }

  if(sq_createinstance(v, -1) < 0 || sq_setinstanceup(v, -1, object) < 0) {
    std::ostringstream msg;
    msg << "Couldn't setup squirrel instance for object of type 'TestObject'";
    throw SquirrelError(v, msg.str());
  }
  sq_remove(v, -2);

  if(setup_releasehook) {
    sq_setreleasehook(v, -1, TestObject_release_hook);
  }
}

static int TestObject_set_sprite_wrapper(HSQUIRRELVM v)
{
  Scripting::TestObject* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  const char* arg0;
  sq_getstring(v, 2, &arg0);
  
  _this->set_sprite(arg0);
  
  return 0;
}

static int TestObject_set_action_wrapper(HSQUIRRELVM v)
{
  Scripting::TestObject* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  const char* arg0;
  sq_getstring(v, 2, &arg0);
  
  _this->set_action(arg0);
  
  return 0;
}

static int TestObject_set_pos_wrapper(HSQUIRRELVM v)
{
  Scripting::TestObject* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  float arg0;
  sq_getfloat(v, 2, &arg0);
  float arg1;
  sq_getfloat(v, 3, &arg1);
  
  _this->set_pos(arg0, arg1);
  
  return 0;
}

static int TestObject_set_vflip_wrapper(HSQUIRRELVM v)
{
  Scripting::TestObject* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  SQBool arg0;
  sq_getbool(v, 2, &arg0);
  
  _this->set_vflip(arg0);
  
  return 0;
}

static int TestObject_attach_wrapper(HSQUIRRELVM v)
{
  Scripting::TestObject* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  const char* arg0;
  sq_getstring(v, 2, &arg0);
  const char* arg1;
  sq_getstring(v, 3, &arg1);
  
  _this->attach(arg0, arg1);
  
  return 0;
}

static int Player_release_hook(SQUserPointer ptr, int )
{
  Scripting::Player* _this = reinterpret_cast<Scripting::Player*> (ptr);
  delete _this;
  return 0;
}

void create_squirrel_instance(HSQUIRRELVM v, Scripting::Player* object, bool setup_releasehook)
{
  sq_pushstring(v, "Player", -1);
  if(sq_get(v, -2) < 0) {
    std::ostringstream msg;
    msg << "Couldn't resolved squirrel type 'Player'";
    throw SquirrelError(v, msg.str());
  }

  if(sq_createinstance(v, -1) < 0 || sq_setinstanceup(v, -1, object) < 0) {
    std::ostringstream msg;
    msg << "Couldn't setup squirrel instance for object of type 'Player'";
    throw SquirrelError(v, msg.str());
  }
  sq_remove(v, -2);

  if(setup_releasehook) {
    sq_setreleasehook(v, -1, Player_release_hook);
  }
}

static int Player_start_listening_wrapper(HSQUIRRELVM v)
{
  Scripting::Player* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  
  _this->start_listening();
  
  return 0;
}

static int Player_stop_listening_wrapper(HSQUIRRELVM v)
{
  Scripting::Player* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  
  _this->stop_listening();
  
  return 0;
}

static int ScriptableObject_release_hook(SQUserPointer ptr, int )
{
  Scripting::ScriptableObject* _this = reinterpret_cast<Scripting::ScriptableObject*> (ptr);
  delete _this;
  return 0;
}

void create_squirrel_instance(HSQUIRRELVM v, Scripting::ScriptableObject* object, bool setup_releasehook)
{
  sq_pushstring(v, "ScriptableObject", -1);
  if(sq_get(v, -2) < 0) {
    std::ostringstream msg;
    msg << "Couldn't resolved squirrel type 'ScriptableObject'";
    throw SquirrelError(v, msg.str());
  }

  if(sq_createinstance(v, -1) < 0 || sq_setinstanceup(v, -1, object) < 0) {
    std::ostringstream msg;
    msg << "Couldn't setup squirrel instance for object of type 'ScriptableObject'";
    throw SquirrelError(v, msg.str());
  }
  sq_remove(v, -2);

  if(setup_releasehook) {
    sq_setreleasehook(v, -1, ScriptableObject_release_hook);
  }
}

static int ScriptableObject_move_to_wrapper(HSQUIRRELVM v)
{
  Scripting::ScriptableObject* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  float arg0;
  sq_getfloat(v, 2, &arg0);
  float arg1;
  sq_getfloat(v, 3, &arg1);
  float arg2;
  sq_getfloat(v, 4, &arg2);
  float arg3;
  sq_getfloat(v, 5, &arg3);
  
  _this->move_to(arg0, arg1, arg2, arg3);
  
  return 0;
}

static int ScriptableObject_start_flash_wrapper(HSQUIRRELVM v)
{
  Scripting::ScriptableObject* _this;
  sq_getinstanceup(v, 1, (SQUserPointer*) &_this, 0);
  float arg0;
  sq_getfloat(v, 2, &arg0);
  
  _this->start_flash(arg0);
  
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

static int dialog_show_wrapper(HSQUIRRELVM v)
{
  int arg0;
  sq_getinteger(v, 2, &arg0);
  const char* arg1;
  sq_getstring(v, 3, &arg1);
  const char* arg2;
  sq_getstring(v, 4, &arg2);
  
  Scripting::dialog_show(arg0, arg1, arg2);
  
  return 0;
}

static int wait_for_dialog_wrapper(HSQUIRRELVM v)
{
  HSQUIRRELVM arg0 = v;
  
  Scripting::wait_for_dialog(arg0);
  
  return sq_suspendvm(v);
}

static int conversation_add_wrapper(HSQUIRRELVM v)
{
  const char* arg0;
  sq_getstring(v, 2, &arg0);
  
  Scripting::conversation_add(arg0);
  
  return 0;
}

static int conversation_show_wrapper(HSQUIRRELVM v)
{
  (void) v;
  
  Scripting::conversation_show();
  
  return 0;
}

static int conversation_get_selection_wrapper(HSQUIRRELVM v)
{
  
  int return_value = Scripting::conversation_get_selection();
  
  sq_pushinteger(v, return_value);
  return 1;
}

static int wait_for_conversation_wrapper(HSQUIRRELVM v)
{
  HSQUIRRELVM arg0 = v;
  
  Scripting::wait_for_conversation(arg0);
  
  return sq_suspendvm(v);
}

static int run_before_wrapper(HSQUIRRELVM v)
{
  HSQUIRRELVM arg0 = v;
  
  bool return_value = Scripting::run_before(arg0);
  
  sq_pushbool(v, return_value);
  return 1;
}

static int save_state_wrapper(HSQUIRRELVM v)
{
  HSQUIRRELVM arg0 = v;
  const char* arg1;
  sq_getstring(v, 2, &arg1);
  
  Scripting::save_state(arg0, arg1);
  
  return 0;
}

static int load_state_wrapper(HSQUIRRELVM v)
{
  HSQUIRRELVM arg0 = v;
  const char* arg1;
  sq_getstring(v, 2, &arg1);
  
  Scripting::load_state(arg0, arg1);
  
  return 0;
}

static int list_objects_wrapper(HSQUIRRELVM v)
{
  (void) v;
  
  Scripting::list_objects();
  
  return 0;
}

static int set_debug_wrapper(HSQUIRRELVM v)
{
  SQBool arg0;
  sq_getbool(v, 2, &arg0);
  
  Scripting::set_debug(arg0);
  
  return 0;
}

static int get_debug_wrapper(HSQUIRRELVM v)
{
  
  bool return_value = Scripting::get_debug();
  
  sq_pushbool(v, return_value);
  return 1;
}

static int get_game_speed_wrapper(HSQUIRRELVM v)
{
  
  float return_value = Scripting::get_game_speed();
  
  sq_pushfloat(v, return_value);
  return 1;
}

static int set_game_speed_wrapper(HSQUIRRELVM v)
{
  float arg0;
  sq_getfloat(v, 2, &arg0);
  
  Scripting::set_game_speed(arg0);
  
  return 0;
}

static int get_text_speed_wrapper(HSQUIRRELVM v)
{
  
  float return_value = Scripting::get_text_speed();
  
  sq_pushfloat(v, return_value);
  return 1;
}

static int set_text_speed_wrapper(HSQUIRRELVM v)
{
  float arg0;
  sq_getfloat(v, 2, &arg0);
  
  Scripting::set_text_speed(arg0);
  
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

static int display_wrapper(HSQUIRRELVM v)
{
  return Scripting::display(v);
}

static int println_wrapper(HSQUIRRELVM v)
{
  return Scripting::println(v);
}

static int set_console_font_wrapper(HSQUIRRELVM v)
{
  const char* arg0;
  sq_getstring(v, 2, &arg0);
  int arg1;
  sq_getinteger(v, 3, &arg1);
  
  Scripting::set_console_font(arg0, arg1);
  
  return 0;
}

static int spawn_object_wrapper(HSQUIRRELVM v)
{
  return Scripting::spawn_object(v);
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

  sq_pushstring(v, "dialog_show", -1);
  sq_newclosure(v, &dialog_show_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'dialog_show'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "wait_for_dialog", -1);
  sq_newclosure(v, &wait_for_dialog_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'wait_for_dialog'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "conversation_add", -1);
  sq_newclosure(v, &conversation_add_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'conversation_add'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "conversation_show", -1);
  sq_newclosure(v, &conversation_show_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'conversation_show'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "conversation_get_selection", -1);
  sq_newclosure(v, &conversation_get_selection_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'conversation_get_selection'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "wait_for_conversation", -1);
  sq_newclosure(v, &wait_for_conversation_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'wait_for_conversation'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "run_before", -1);
  sq_newclosure(v, &run_before_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'run_before'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "save_state", -1);
  sq_newclosure(v, &save_state_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'save_state'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "load_state", -1);
  sq_newclosure(v, &load_state_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'load_state'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "list_objects", -1);
  sq_newclosure(v, &list_objects_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'list_objects'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "set_debug", -1);
  sq_newclosure(v, &set_debug_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'set_debug'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "get_debug", -1);
  sq_newclosure(v, &get_debug_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'get_debug'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "get_game_speed", -1);
  sq_newclosure(v, &get_game_speed_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'get_game_speed'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "set_game_speed", -1);
  sq_newclosure(v, &set_game_speed_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'set_game_speed'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "get_text_speed", -1);
  sq_newclosure(v, &get_text_speed_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'get_text_speed'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "set_text_speed", -1);
  sq_newclosure(v, &set_text_speed_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'set_text_speed'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "wait", -1);
  sq_newclosure(v, &wait_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'wait'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "display", -1);
  sq_newclosure(v, &display_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'display'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "println", -1);
  sq_newclosure(v, &println_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'println'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "set_console_font", -1);
  sq_newclosure(v, &set_console_font_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'set_console_font'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "spawn_object", -1);
  sq_newclosure(v, &spawn_object_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'spawn_object'";
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

  sq_pushstring(v, "remove", -1);
  sq_newclosure(v, &GameObject_remove_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'remove'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "set_active", -1);
  sq_newclosure(v, &GameObject_set_active_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'set_active'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "set_parent", -1);
  sq_newclosure(v, &GameObject_set_parent_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'set_parent'";
    throw SquirrelError(v, msg.str());
  }

  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register class'GameObject'";
    throw SquirrelError(v, msg.str());
  }

  // Register class TestObject
  sq_pushstring(v, "TestObject", -1);
  sq_pushstring(v, "GameObject", -1);
  sq_get(v, -3);
  if(sq_newclass(v, SQTrue) < 0) {
    std::ostringstream msg;
    msg << "Couldn't create new class 'TestObject'";
    throw SquirrelError(v, msg.str());
  }
  sq_pushstring(v, "set_sprite", -1);
  sq_newclosure(v, &TestObject_set_sprite_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'set_sprite'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "set_action", -1);
  sq_newclosure(v, &TestObject_set_action_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'set_action'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "set_pos", -1);
  sq_newclosure(v, &TestObject_set_pos_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'set_pos'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "set_vflip", -1);
  sq_newclosure(v, &TestObject_set_vflip_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'set_vflip'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "attach", -1);
  sq_newclosure(v, &TestObject_attach_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'attach'";
    throw SquirrelError(v, msg.str());
  }

  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register class'TestObject'";
    throw SquirrelError(v, msg.str());
  }

  // Register class Player
  sq_pushstring(v, "Player", -1);
  sq_pushstring(v, "GameObject", -1);
  sq_get(v, -3);
  if(sq_newclass(v, SQTrue) < 0) {
    std::ostringstream msg;
    msg << "Couldn't create new class 'Player'";
    throw SquirrelError(v, msg.str());
  }
  sq_pushstring(v, "start_listening", -1);
  sq_newclosure(v, &Player_start_listening_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'start_listening'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "stop_listening", -1);
  sq_newclosure(v, &Player_stop_listening_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'stop_listening'";
    throw SquirrelError(v, msg.str());
  }

  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register class'Player'";
    throw SquirrelError(v, msg.str());
  }

  // Register class ScriptableObject
  sq_pushstring(v, "ScriptableObject", -1);
  sq_pushstring(v, "GameObject", -1);
  sq_get(v, -3);
  if(sq_newclass(v, SQTrue) < 0) {
    std::ostringstream msg;
    msg << "Couldn't create new class 'ScriptableObject'";
    throw SquirrelError(v, msg.str());
  }
  sq_pushstring(v, "move_to", -1);
  sq_newclosure(v, &ScriptableObject_move_to_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'move_to'";
    throw SquirrelError(v, msg.str());
  }

  sq_pushstring(v, "start_flash", -1);
  sq_newclosure(v, &ScriptableObject_start_flash_wrapper, 0);
  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register function'start_flash'";
    throw SquirrelError(v, msg.str());
  }

  if(sq_createslot(v, -3) < 0) {
    std::ostringstream msg;
    msg << "Couldn't register class'ScriptableObject'";
    throw SquirrelError(v, msg.str());
  }

  sq_pop(v, 1);
}

}

