#pragma once

#ifdef SPECTRE2DDLL_EXPORTS
#define SPECTRE_API __declspec(dllexport)
#else
#define SPECTRE_API __declspec(dllimport)
#endif

extern "C"
{
#include <stdint.h>

#define SPECTRE_OK 0
#define SPECTRE_NOT_INITIALIZED 1
#define SPECTRE_WINDOW_NOT_CREATED 2
#define SPECTRE_PLATFORM_ERROR 3
#define SPECTRE_INVALID_VALUE 4
#define SPECTRE_NO_ACTIVE_WINDOW 5
#define SPECTRE_UNKNOWN_ERROR 6
#define SPECTRE_INVALID_ENUM 7
#define SPECTRE_MONITOR_NOT_LINKED 8
#define SPECTRE_WINDOW_ACTIVE 9
#define SPECTRE_PIXEL_OUT_OF_BOUNDS 10
#define SPECTRE_UNSUPPORTED_FORMAT 11
#define SPECTRE_COULD_NOT_OPEN_FILE 12
#define SPECTRE_INVALID_FILE 13

	typedef struct sp_Error sp_Error;

	void SPECTRE_API sp_init();

	void SPECTRE_API sp_finish();

	bool SPECTRE_API sp_is_big_endian();

	uint16_t SPECTRE_API sp_get_error_code(const sp_Error* error);

	uint32_t SPECTRE_API sp_get_error_description(const sp_Error* error, char* dest);

	void SPECTRE_API sp_set_error_code(sp_Error* error, uint16_t code);

	void SPECTRE_API sp_set_error_description(sp_Error* error, const char* description);

	sp_Error SPECTRE_API * sp_create_error(uint16_t code, const char* description);

	sp_Error SPECTRE_API * sp_get_last_error();

	void SPECTRE_API sp_push_error(sp_Error* error);
}