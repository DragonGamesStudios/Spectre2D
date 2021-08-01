#include "..\include\Spectre2D\String.h"
#include <map>

namespace sp
{
	StringUTF8::StringUTF8()
		: std::string()
	{
	}

	StringUTF8::StringUTF8(const char* _Ptr)
		: std::string(_Ptr)
	{
	}

	StringUTF8::StringUTF8(const std::string& s)
	{
		std::string::operator=(s);
	}

}