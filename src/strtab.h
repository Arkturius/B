/**
 * String table helpers.
 */

#ifndef _STRTAB_H
# define _STRTAB_H

# include <stdlib.h>
# include <string.h>

# include <types.h>

# define	STRTAB_SIZE_MIN 2048
# define	STRTAB_SIZE_MAX 65536

# define	TO_HDR(_s)		(((StrtabHdr *)(_s)) - 1)
# define	FROM_HDR(_s)	(Strtab)(((StrtabHdr *)(_s)) + 1)

typedef struct strtab_hdr	StrtabHdr;
typedef char				*Strtab;

struct strtab_hdr
{
	String	cursor;
	u32		size;
	u32		cap;
};

Strtab
strtab_init(Size s);

void
strtab_destroy(Strtab tab);

StringIdx
strtab_append(Strtab tab, String str);

String
strtab_get(Strtab tab, StringIdx idx);

#endif // _STRTAB_H

#if defined(STRTAB_IMPLEMENTATION)

Strtab
strtab_init(Size s)
{
	if (s < STRTAB_SIZE_MIN)
		s = STRTAB_SIZE_MIN;
	if (s > STRTAB_SIZE_MAX)
		return (NULL);

	StrtabHdr	*ptr = (StrtabHdr *)malloc(sizeof(StrtabHdr) + s);

	if (!ptr)
		return (NULL);

	memset(ptr, 0, sizeof(StrtabHdr) + s);
	ptr->cursor = FROM_HDR(ptr) + 1;
	ptr->size = 1;
	ptr->cap = s;

	return FROM_HDR(ptr);
}

void
strtab_destroy(Strtab tab)
{
	if (tab)
		free(TO_HDR(tab));
}

StringIdx
strtab_append(Strtab tab, String str)
{
	StrtabHdr	*hdr = TO_HDR(tab);
	u64			len = strlen(str);

	if (hdr->cursor - tab + len >= hdr->cap)
		return (0);

	StringIdx	idx = hdr->size;
	char		*ptr = (char *)hdr->cursor;
	
	memcpy(ptr, str, len);
	ptr += len;
	*ptr++ = 0;
	hdr->size += len + 1;
	hdr->cursor = ptr;
	
	return (idx);
}

String
strtab_get(Strtab tab, StringIdx idx)
{
	return (String)(tab + idx);
}

#endif
