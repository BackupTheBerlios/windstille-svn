#ifndef __SPAWN_OBJECT_HPP__
#define __SPAWN_OBJECT_HPP__

#ifndef SCRIPTING_API
#include <squirrel.h>
#endif

namespace Scripting
{

/**
 * Spawn object. Parameters:
 *    name:    string with name of object
 *    table:   table that is parsed to get object properties
 *
 * Example: spawn_object("FlashingSign", { name="blup", x = 2.0, y = 5.0, sprite="arrows/red" });
 */
int spawn_object(HSQUIRRELVM v) __custom;

}

#endif

