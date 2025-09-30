/**
 * utils.c
 */

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include <B_arena.h>

#define	B_ARENA_CAP	134217728

static String	_bstring_arena			= NULL;
static Size		_bstring_off			= 0;
static char		_bstring_buffer[2048]	= {0};

StringC
B_arena_string_alloc(StringC new)
{
	if (!_bstring_arena)
		_bstring_arena = malloc(B_ARENA_CAP);
	assert(_bstring_arena && "string arena allocation failed.");

	if (_bstring_off == 0)
	{
		memset(_bstring_arena, 0, B_ARENA_CAP);
		_bstring_arena[_bstring_off++] = 0;
	}

	Size	len = strlen(new);

	assert(_bstring_off + len <= B_ARENA_CAP && "arena full, aborting.");

	String	start = _bstring_arena + _bstring_off;

	memcpy(start, new, len);
	_bstring_off += len;
	_bstring_arena[_bstring_off++] = 0;

	return (start);
}

Size
B_arena_save(void)
{
	return (_bstring_off);
}

void
B_arena_restore(Size off)
{
	if (off == 0)
		return ;
	_bstring_off = off;
}

void
B_arena_erase(Size size)
{
	if (size >= _bstring_off)
	{
		_bstring_off = 0;
		return ;
	}
	_bstring_off -= size;
}

void
B_arena_free(void)
{
	free(_bstring_arena);
}

StringC
B_arena_string(StringC fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf((char *)_bstring_buffer, sizeof(_bstring_buffer), fmt, ap);
    va_end(ap);

    return (B_arena_string_alloc(_bstring_buffer));
}
