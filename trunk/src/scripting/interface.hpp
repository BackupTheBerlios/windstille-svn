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

void play_sound(const std::string& soundfile);

// alignment constants
static const int VCENTER = 0x00;
static const int LEFT    = 0x01;
static const int RIGHT   = 0x02;
static const int HCENTER = 0x00;
static const int TOP     = 0x10;
static const int BOTTOM  = 0x20;

void set_dialog(int alignment, const std::string& portrait,
                const std::string& text);

void show_dialog(float fadein_time);

void hide_dialog(float fadeout_time);

/** @SUSPEND@
 * Waits the specified time in seconds.
 */
void wait(HSQUIRRELVM vm, float time);

/** @SUSPEND@
 * Waits till currently opened dialog has closed.
 */
void wait_for_dialog(HSQUIRRELVM vm);

}

#endif

