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

using namespace Scripting;

static int play_music_wrapper(HSQUIRRELVM v)
{
  const char* arg0;
  sq_getstring(v, 2, &arg0);
  
  Scripting::play_music(arg0);
  
  return 0;
}

static int play_sound_wrapper(HSQUIRRELVM v)
{
  const char* arg0;
  sq_getstring(v, 2, &arg0);
  
  Scripting::play_sound(arg0);
  
  return 0;
}

static int create_dialog_wrapper(HSQUIRRELVM v)
{
  const char* arg0;
  sq_getstring(v, 2, &arg0);
  const char* arg1;
  sq_getstring(v, 3, &arg1);
  
  Scripting::create_dialog(arg0, arg1);
  
  return 0;
}

static int clear_dialog_wrapper(HSQUIRRELVM v)
{
  (void) v;
  
  Scripting::clear_dialog();
  
  return 0;
}

static int show_dialog_wrapper(HSQUIRRELVM v)
{
  (void) v;
  
  Scripting::show_dialog();
  
  return 0;
}

WrappedFunction supertux_global_functions[] = {
  { "play_music", &play_music_wrapper },
  { "play_sound", &play_sound_wrapper },
  { "create_dialog", &create_dialog_wrapper },
  { "clear_dialog", &clear_dialog_wrapper },
  { "show_dialog", &show_dialog_wrapper },
  { 0, 0 }
};

WrappedClass supertux_classes[] = {
  { 0, 0 }
};

