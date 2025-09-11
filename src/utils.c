/**
 * utils.c
 */

#include <b.h>
#include <string.h>

#define	BSTRING_ARENA_CAP	134217728

static String	_bstring_arena			= NULL;
static Size		_bstring_off			= 0;

char			_bstring_buffer[2048]	= {0};

StringC
B_arena_string(StringC new)
{
	if (!_bstring_arena)
		_bstring_arena = malloc(BSTRING_ARENA_CAP);
	if (!_bstring_arena)
		B_error(ERROR_ALLOC, "%s: allocation failed.", __func__);

	if (_bstring_off == 0)
	{
		memset(_bstring_arena, 0, BSTRING_ARENA_CAP);
		_bstring_arena[_bstring_off++] = 0;
	}

	Size	len = strlen(new);

	if (_bstring_off + len > BSTRING_ARENA_CAP)
		B_error(ERROR_ALLOC, "%s: arena full, reconsider allocations...", __func__);

	String	start = _bstring_arena + _bstring_off;

	memcpy(start, new, len);
	_bstring_off += len;
	_bstring_arena[_bstring_off++] = 0;

	return (start);
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

StringC
B_asprintf(StringC fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf((char *)_bstring_buffer, sizeof(_bstring_buffer), fmt, ap);
    va_end(ap);

    return (B_arena_string(_bstring_buffer));
}
