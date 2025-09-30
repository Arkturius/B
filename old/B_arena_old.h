/**
 * b_arena.h
 */

#if !defined (_B_ARENA_H)
# define _B_ARENA_H

# include <types.h>

__attribute__((format(printf, 1, 2))) StringC
B_arena_string(StringC fmt, ...);

Size
B_arena_save(void);

void
B_arena_restore(Size off);

void
B_arena_erase(Size size);

void
B_arena_free(void);

#endif // _B_ARENA_H
