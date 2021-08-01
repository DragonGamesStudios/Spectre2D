#include "core.h"

#include <iostream>

#include <lua.hpp>

void load_functions(lua_State* L)
{
	lua_newtable(L);

	load_core(L);

	lua_setglobal(L, "sp");
}

int main(int argc, char* argv[])
{
	sp::init();

	if (argc > 1)
	{
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);

		load_functions(L);

		try
		{
			if (!luaL_dofile(L, argv[1]))
			{
				std::cout << lua_tostring(L, -1) << std::endl;
				lua_pop(L, 1);
			}
		}
		catch (const sp::Error& err)
		{
			std::cout << "Error " << err.code << "; Description: " << err.description << std::endl;
		}
	}
	else
	{
		// Display no game screen
	}

	sp::finish();
}