%module windstille

%include "std_string.i"
%include "std_vector.i"
// %template(Std_vector_int) std::vector<int>;
// %template(Std_vector_ObjMapObject) std::vector<ObjMapObject>;

%{
#include "scripting/game.hxx"
%}

%include "scripting/game.hxx"
%include "entity.hxx"

// EOF //
