#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#ifndef SCRIPTING_API
#include <string>
#include <squirrel.h>
#endif

namespace Scripting
{

void play_music(const std::string& musicfile);

void play_sound(const std::string& soundfile);

void create_dialog(const std::string& portrait, const std::string& text);

void clear_dialog();

/** @SUSPEND@
 * Waits the specified time in seconds.
 */
void wait(HSQUIRRELVM vm, float time);

}

#endif

