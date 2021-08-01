
#include "core.h"

#include <Spectre2D/core.h>

#include <stack>

std::stack<sp_Error*> error_stack;

void sp_init()
{
	try
	{
		sp::init();
	}
	catch (const sp::Error& err)
	{
		sp::Error* error = new sp::Error(err);
		error_stack.push((sp_Error*)error);
	}
}

void sp_finish()
{
	try
	{
		sp::finish();

		while (!error_stack.empty())
		{
			delete ((sp::Error*)error_stack.top());
			error_stack.pop();
		}
	}
	catch (const sp::Error& err)
	{
		sp::Error* error = new sp::Error(err);
		error_stack.push((sp_Error*)error);
	}
}

bool sp_is_big_endian()
{
	return sp::BIG_ENDIAN;
}

uint16_t sp_get_error_code(const sp_Error* error)
{
	return ((const sp::Error*)error)->code;
}

uint32_t sp_get_error_description(const sp_Error* error, char* dest)
{
	strcpy_s(dest, ((const sp::Error*)error)->description.size(), ((const sp::Error*)error)->description.c_str());

	return ((const sp::Error*)error)->description.size();
}

void sp_set_error_code(sp_Error* error, uint16_t code)
{
	((sp::Error*)error)->code = code;
}

void sp_set_error_description(sp_Error* error, const char* description)
{
	((sp::Error*)error)->description = description;
}

sp_Error* sp_create_error(uint16_t code, const char* description)
{
	sp_Error* error = (sp_Error*)(new sp::Error{ code, description });
	return error;
}

sp_Error* sp_get_last_error()
{
	return error_stack.top();
}

void sp_push_error(sp_Error* error)
{
	sp_Error* newerr = (sp_Error*)new sp::Error(*((sp::Error*)error));

	error_stack.push(newerr);
}
