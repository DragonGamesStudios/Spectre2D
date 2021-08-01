#pragma once

#include <lua.hpp>

void add_function(lua_State* L, lua_CFunction func, const char* name);