#include "util.h"

void add_function(lua_State* L, lua_CFunction func, const char* name)
{
	lua_pushcfunction(L, func);
	lua_setfield(L, -2, name);
}