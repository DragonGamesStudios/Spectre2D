#include "core.h"
#include "util.h"

void load_core(lua_State* L)
{
	lua_newtable(L);

	lua_pushboolean(L, sp::BIG_ENDIAN);
	lua_setfield(L, -2, "BIG_ENDIAN");

	lua_setfield(L, -2, "core");
}