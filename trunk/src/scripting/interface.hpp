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

void add_dialog(int alignment, const std::string& portrait);
void add_question(const std::string& text);
void add_answer(const std::string& answer);
int dialog_answer();
void end_dialog();
void show_dialog(HSQUIRRELVM vm, float fadein_time) __suspend;
void hide_dialog(float fadeout_time);

bool run_before(HSQUIRRELVM vm);

// for testing
void save_state(HSQUIRRELVM v, const std::string& filename);
void load_state(HSQUIRRELVM v, const std::string& filename);

void activate_object(const std::string& name, bool active);
void list_objects();

//Waits the specified time in seconds.
void wait(HSQUIRRELVM vm, float time) __suspend;
}

#endif

