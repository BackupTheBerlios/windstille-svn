#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#ifndef SCRIPTING_API
#include <string>
#include <squirrel.h>
#endif

namespace Scripting
{

void set_sector(const std::string& filename);

void play_music(const std::string& musicfile);

void stop_music(float fadetime);

void play_sound(const std::string& soundfile);

// dialog alignment constants
static const int VCENTER = 0x00;
static const int LEFT    = 0x01;
static const int RIGHT   = 0x02;
static const int HCENTER = 0x00;
static const int TOP     = 0x10;
static const int BOTTOM  = 0x20;

void dialog_show(int alignment, const std::string& portrait, const std::string& text);
void wait_for_dialog(HSQUIRRELVM vm) __suspend;

void conversation_add(const std::string& text);
void conversation_show();
int  conversation_get_selection();
void wait_for_conversation(HSQUIRRELVM v) __suspend;

bool run_before(HSQUIRRELVM vm);

// for testing
void save_state(HSQUIRRELVM v, const std::string& filename);
void load_state(HSQUIRRELVM v, const std::string& filename);

void activate_object(const std::string& name, bool active);
void list_objects();

void set_debug(bool t);
bool get_debug();

float get_game_speed();
void  set_game_speed(float v);

float get_text_speed();
void  set_text_speed(float v);

//Waits the specified time in seconds.
void wait(HSQUIRRELVM vm, float time) __suspend;

int display(HSQUIRRELVM) __custom;
int println(HSQUIRRELVM) __custom;

}

#endif

