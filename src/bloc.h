/**
 * bloc.h | Dynamic arrays
 */

#ifndef _BLOC_H
# define _BLOC_H

# define	BLOC_DECL(_type) \
	typedef struct _type##_bloc \
    { \
        uint32_t size; \
        uint32_t count; \
        _type *bloc; \
    } _type##s

# define    BLOC_COUNT(_bloc)   (_bloc.count)

# define	BLOC_FOREACH(_type, _it, _bloc) \
	for (_type *_it = _bloc.bloc; _it < _bloc.bloc + _bloc.count; ++_it)

# define    BLOC_INDEX(_bloc, _e)   ((_e) - (_bloc.bloc))

# define	BLOC_MIN_SIZE	8
# define	BLOC_MAX_SIZE	65536

#endif // _BLOC_H

#define BLOC_IMPLEMENTATION
#if defined(BLOC_IMPLEMENTATION)

# include <stdlib.h>
# include <stdint.h>

# define	BLOC_REALLOC(_bloc) \
	do { \
		if ((_bloc.size << 1) > BLOC_MAX_SIZE) \
			abort(); \
        uint32_t new_size = 0; \
		if (_bloc.size == 0) \
            new_size = BLOC_MIN_SIZE; \
		else if ( _bloc.count + 1 == _bloc.size) \
            new_size = (_bloc.size << 1); \
        if (new_size) \
        { \
            _bloc.bloc = realloc(_bloc.bloc, new_size * sizeof(_bloc.bloc[0])); \
            _bloc.size = new_size; \
        } \
	} while (0)

# define    BLOC_SWAPLAST(_bloc, _i) \
    do { \
        uint32_t _elem_size = sizeof *_bloc.bloc; \
        memcpy(&_bloc.bloc[_i], &_bloc.bloc[BLOC_COUNT(_bloc)], _elem_size); \
    } while (0)

# define	BLOC_DESTROY(_bloc) \
	do { \
		free(_bloc.bloc); \
	} while (0)

# define	BLOC_APPEND(_bloc, _e) \
	do { \
		BLOC_REALLOC(_bloc); \
		_bloc.bloc[_bloc.count++] = _e; \
	} while (0)

# define	BLOC_POP(_bloc, _n) \
	do { \
		if (_n >= _bloc.count) \
			_bloc.count = 0; \
		else \
			_bloc.count -= n; \
	} while (0)

# define    BLOC_DELETE(_bloc, _i) \
    do { \
        if (i >= BLOC_COUNT(_bloc)) \
            break ; \
        BLOC_SWAPLAST(_bloc, _i); \
        _bloc.count -= 1; \
    } while (0)

/**
 * Bitset implementation
 */

typedef uint64_t    BitSet;

# define    BS_BIT(_bit)        (1 << _bit)

# define    BS_SET(_bs, _bit)   ((_bs) |= BS_BIT((_bit)))
# define    BS_DEL(_bs, _bit)   ((_bs) &= ~BS_BIT((_bit)))
# define    BS_CMP(_bs, _bit)   ((_bs) & BS_BIT((_bit)))
# define    BS_CLR(_bs)         ((_bs) = 0)
# define    BS_INV(_bs)         (~(_bs))

# define    SWAP(_t, _a, _b) \
    do { \
        _t _tmp = _a; \
        _a = _b; \
        _b = _tmp; \
    } while (0)
        

#endif // BLOC_IMPLEMENTATION
