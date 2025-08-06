/**
 * bloc.h | Dynamic arrays
 */

#ifndef _BLOC_H
# define _BLOC_H

# define	BLOC_DECL(_type) \
	typedef struct _type##_bloc { uint32_t size; uint32_t count; _type *bloc; } _type##s

# define	BLOC_FOREACH(_type, _it, _bloc) \
	for (_type *_it = _bloc.bloc; _it < _bloc.bloc + _bloc.count; ++_it)

# define	BLOC_MAX_SIZE	65536

#endif // _BLOC_H

#define BLOC_IMPLEMENTATION
#if defined(BLOC_IMPLEMENTATION)

# define	BLOC_REALLOC(_bloc) \
	do { \
		if (_bloc.size << 1 > BLOC_MAX_SIZE) \
			abort(); \
		if (_bloc.count + 1 == _bloc.size) \
			_bloc.bloc = realloc(_bloc.bloc, _bloc.size << 1); \
	} while (0)

# define	BLOC_APPEND(_bloc, _e) \
	do { \
		BLOC_REALLOC(_bloc); \
		_bloc.bloc[_bloc.count++] = e; \
	} while (0);

# define	BLOCK_POP(_bloc, _n) \
	do { \
		if (_n >= _bloc.count) \
			_bloc.count = 0; \
		else \
			_bloc.count -= n; \
	} while (0);

#endif // BLOC_IMPLEMENTATION
