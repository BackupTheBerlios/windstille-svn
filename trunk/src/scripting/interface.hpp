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

void add_caption(int alignment, const std::string& text);
void end_caption();

void set_view(float x, float y);
void set_camera_active(bool active);
void set_controller_help_active(bool active);

// dialog alignment constants
static const int VCENTER = 0x00;
static const int LEFT    = 0x01;
static const int RIGHT   = 0x02;
static const int HCENTER = 0x00;
static const int TOP     = 0x10;
static const int BOTTOM  = 0x20;

void dialog_show(int alignment, const std::string& character, const std::string& portrait, const std::string& text);
void wait_for_dialog(HSQUIRRELVM vm) __suspend;

void conversation_add(const std::string& text);
void conversation_show();
int  conversation_get_selection();
void wait_for_conversation(HSQUIRRELVM v) __suspend;

void add_objective(const std::string& name, const std::string& text);
void objective_complete(const std::string& name);
bool is_objective_given(const std::string& name);
bool is_objective_complete(const std::string& name);

bool run_before(HSQUIRRELVM vm);

// for testing
void save_state(HSQUIRRELVM v, const std::string& filename);
void load_state(HSQUIRRELVM v, const std::string& filename);

void list_objects();

void set_debug(bool t);
bool get_debug();

float get_game_speed();
void  set_game_speed(float v);

//Waits the specified time in seconds.
void wait(HSQUIRRELVM vm, float time) __suspend;

int display(HSQUIRRELVM) __custom;
int println(HSQUIRRELVM) __custom;

void set_console_font(const std::string& font, int size);
void set_gamma(float g);
void set_gamma_rgb(float r, float g, float b);
}

#endif

