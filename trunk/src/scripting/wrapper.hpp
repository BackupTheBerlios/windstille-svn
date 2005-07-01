/**
 * WARNING: This file is automatically generated from:
 *  'src/scripting/wrapper.interface.hpp'
 * DO NOT CHANGE
 */
#ifndef __windstille_WRAPPER_H__
#define __windstille_WRAPPER_H__

#include "wrapper_util.hpp"

extern WrappedFunction windstille_global_functions[];
extern WrappedClass windstille_classes[];
extern WrappedConstant<int> windstille_int_constants[];
extern WrappedConstant<float> windstille_float_constants[];
extern WrappedConstant<const char*> windstille_string_constants[];

static inline void register_windstille_wrapper(HSQUIRRELVM v)
{
    register_functions(v, windstille_global_functions);
    register_classes(v, windstille_classes);
    register_constants(v, windstille_int_constants);
    register_constants(v, windstille_float_constants);
    register_constants(v, windstille_string_constants);
}

#endif

