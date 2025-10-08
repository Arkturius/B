/**
* xlib - C utilities.
*/

#if !defined (_XLIB_H)
# define _XLIB_H

# include	<stdint.h>
# include	<stdio.h>
# include	<stdlib.h>
# include	<string.h>
# include	<unistd.h>
# include	<stdbool.h>
# include	<fcntl.h>

# if defined (XLIB_NO_PREFIX)
#	define	concat				x_concat
#	define	stringify			x_stringify

#	define	log					x_log	
#	define	warning				x_warning	
#	define	todo				x_todo
#	define	unreachable			x_unreachable

#   ifndef _noreturn
#	 define	_noreturn			x__noreturn
#   endif
#	define	_constructor		x__constructor
#	define	_destructor			x__destructor
#	define	_unused				x__unused

#	define	arr_count			x_arr_count
# 	define	arr_first			x_arr_first
# 	define	arr_last			x_arr_last
#	define	arr_nth				x_arr_nth
# 	define	arr_size			x_arr_size
# 	define	arr_index			x_arr_index

#	define	arr_append			x_arr_append
#	define	arr_pop				x_arr_pop
#	define	arr_delete			x_arr_delete

#	define	arr_realloc			x_arr_realloc
#	define	arr_reserve			x_arr_reserve
#	define	arr_destroy			x_arr_destroy
#	define	arr_foreach			x_arr_foreach
#	define	arr_foreach_rev		x_arr_foreach_rev
#	define	arr_map				x_arr_map
#	define	arr_map_custom		x_arr_map_custom
# endif

typedef uint64_t	u64;
typedef uint32_t	u32;
typedef uint16_t	u16;
typedef uint8_t		u8;

typedef int64_t		i64;
typedef int32_t		i32;
typedef int16_t		i16;
typedef int8_t		i8;

typedef	intptr_t	iptr;
typedef	uintptr_t	uptr;

typedef float		f32;
typedef double		f64;

typedef char        *String;
typedef char const	*StringC;

typedef u32			Size;
typedef i32         Offset;

/**
* Logging.
*/

# define	x_log_prefix			"[INFO]"
# define	x_warning_prefix		"[WARNING]"
# define	x_todo_prefix			"[TODO]"
# define	x_unreachable_prefix	"[UNREACHABLE]"

# define	x__noreturn				__attribute__((noreturn))
# define	x__destructor(...)		__attribute__(( destructor __VA_OPT__( (__VA_ARGS__) ) ))
# define	x__constructor(...)		__attribute__(( constructor __VA_OPT__( (__VA_ARGS__) ) ))
# define	x__unused				__attribute__((unused))

# define	_x_log(_pre, _s, ...)											\
																			\
	dprintf(2, "%s %s:%d: "_s"\n", _pre, __FILE__, __LINE__, ##__VA_ARGS__)

# define	x_todo(_s, ...)													\
																			\
	do {																	\
		_x_log(x_todo_prefix, _s, ##__VA_ARGS__);							\
		exit(1);															\
	} while (0);

# define	x_unreachable(_s, ...)											\
																			\
	do {																	\
		_x_log(x_unreachable_prefix, _s, ##__VA_ARGS__);					\
		exit(1);															\
	} while (0);

# define	x_log(_s, ...)		_x_log(x_log_prefix, _s, ##__VA_ARGS__)
# define	x_warning(_s, ...)	_x_log(x_warning_prefix, _s, ##__VA_ARGS__)

/**
* Macro utilities.
*/

# define    shift_args(ac, av)  (ac--, *av++)
# define    unused(_x)          (void)(_x)
# define	breakpoint			asm("int3")

# define	min(_a, _b)			((_a) < (_b) ? (_a) : (_b))
# define	max(_a, _b)			((_a) > (_b) ? (_a) : (_b))
# define	clamp(_e, _a, _b)	min(max(_a, _e), _b)
# define	array_len(_a)		(sizeof(_a) / sizeof((_a)[0]))

# define	x_parens			()
# define	x_rparen			)
# define	x_lparen			(

# define	_x_concat(_a, _b)	_a ## _b
# define	x_concat(_a, _b)	_x_concat(_a, _b)

# define	_x_stringify(_a)	#_a
# define	x_stringify(_a)		_x_stringify(_a)

# define	_x_expand(...)		_x_expand_1(_x_expand_1(_x_expand_1(_x_expand_1( __VA_ARGS__ ))))
# define	_x_expand_1(...)	_x_expand_2(_x_expand_2(_x_expand_2(_x_expand_2( __VA_ARGS__ ))))
# define	_x_expand_2(...)	_x_expand_3(_x_expand_3(_x_expand_3(_x_expand_3( __VA_ARGS__ ))))
# define	_x_expand_3(...)	_x_expand_4(_x_expand_4(_x_expand_4(_x_expand_4( __VA_ARGS__ ))))
# define	_x_expand_4(...)	__VA_ARGS__

/**
* Applies recursively _macro(ARG) for ARG in __VA_ARGS__
*/

# define	x_foreach(_macro, ...)											\
																			\
	__VA_OPT__(_x_expand(x_foreach_iter(_macro, ##__VA_ARGS__)))			\

# define	x_foreach_iter(_macro, _arg, ...)								\
																			\
	_macro(_arg)															\
	__VA_OPT__(x_foreach_again x_parens (_macro, ##__VA_ARGS__))			\

# define	x_foreach_again()	x_foreach_iter

/**
* Applies recursively _macro(_a1, ARG) for ARG in __VA_ARGS__
*/

# define	x_foreach_1(_macro, _a1, ...)									\
																			\
	__VA_OPT__(_x_expand(x_foreach_1_iter(_macro, _a1, ##__VA_ARGS__)))		\

# define	x_foreach_1_iter(_macro, _a1, _arg, ...)						\
																			\
	_macro(_a1, _arg)														\
	__VA_OPT__(x_foreach_1_again x_parens (_macro, _a1, ##__VA_ARGS__))		\

# define	x_foreach_1_again()	x_foreach_1_iter

/**
* Dynamic array macro implementation.
*/

# define	x_array(_type, _name)											\
																			\
	typedef struct _name##_arr												\
	{																		\
		u32		capacity;													\
		u32		count;														\
		_type	*items;														\
	} _name

# define	x_arr_count(_arr)		((_arr).count)
# define	x_arr_first(_arr)		((_arr).items)
# define	x_arr_last(_arr)		((_arr).items + x_arr_count(_arr) - 1)
# define	x_arr_nth(_arr, _n)		(x_arr_first((_arr)) + (_n))
# define	x_arr_size(_arr)		((_arr).capacity)
# define	x_arr_index(_arr, _e)	((_e) - ((_arr).items))

# define	x_subarray(_arr, _start, _count)								\
																			\
	(																		\
		{																	\
			u32 _safe_start = clamp((i32)_start, 0, (i32)x_arr_count(_arr));\
			u32 _max_count  = x_arr_count(_arr) - _safe_start;				\
			u32 _safe_count = clamp((i32)_count, 0, (i32)_max_count);		\
																			\
			__typeof__(_arr) _sub = {0};									\
			if (_safe_count > 0)											\
			{																\
				_sub.items = &(_arr).items[_safe_start];					\
				_sub.count = _safe_count;									\
				_sub.capacity = _safe_count;								\
			}																\
			_sub;															\
		}																	\
	)

# define	X_ARR_MIN_SIZE	(1 << 8)
# define	X_ARR_MAX_SIZE	(1 << 28)

# define	x_arr_foreach(_type, _it, _arr)									\
																			\
	for																		\
	(																		\
		_type *_it = x_arr_first(_arr);										\
		_it && _it <= x_arr_last(_arr);										\
		++_it																\
	)

# define	x_arr_foreach_rev(_type, _it, _arr)								\
																			\
	for																		\
	(																		\
		_type *_it = x_arr_last(_arr);										\
		x_arr_first(_arr) && _it >= x_arr_first(_arr);						\
		--_it																\
	)

# define	x_arr_realloc(_arr, _n)											\
																			\
	do {																	\
		if ((x_arr_size(_arr) << 1) > X_ARR_MAX_SIZE)						\
			abort();														\
		uint32_t new_capacity = 0;											\
		uint32_t wanted = (_n);												\
		if (x_arr_size(_arr) == 0)											\
			new_capacity = wanted ? wanted : X_ARR_MIN_SIZE;				\
		else if (x_arr_count(_arr) + 1 >= x_arr_size(_arr))					\
			new_capacity = ((_arr).capacity << 1);							\
		if (new_capacity)													\
		{																	\
			(_arr).items = realloc											\
			(																\
				x_arr_first(_arr),											\
				new_capacity * sizeof *x_arr_first(_arr)					\
			);																\
			x_arr_size(_arr) = new_capacity;								\
		}																	\
	} while (0)

# define	x_arr_reserve(_arr, _n)											\
																			\
	do {																	\
		x_arr_realloc(_arr, _n);											\
		x_arr_count(_arr) = 0;												\
	} while (0)

# define	x_arr_swaplast(_arr, _i)										\
																			\
	do {																	\
		uint32_t _elem_size = sizeof *x_arr_first(_arr);					\
		memcpy(&_arr.items[_i], x_arr_last(_arr), _elem_size);				\
	} while (0)

# define	x_arr_destroy(_arr)												\
																			\
	do {																	\
		free(x_arr_first(_arr));											\
		x_arr_first(_arr) = NULL;											\
		x_arr_count(_arr) = 0;												\
		x_arr_size(_arr) = 0;												\
	} while (0)

# define	x_arr_append(_arr, _e)											\
																			\
	do {																	\
		if (x_arr_count(_arr) + 1 > x_arr_size(_arr))						\
			x_arr_realloc(_arr, 0);											\
		(_arr).items[x_arr_count(_arr)++] = _e;								\
	} while (0)

# define	x_arr_pop(_arr, _n)												\
																			\
	do {																	\
		if (_n >= _arr.count)												\
			x_arr_count(_arr) = 0;											\
		else																\
			x_arr_count(_arr) -= _n;										\
	} while (0)

# define	x_arr_delete(_arr, _i)											\
																			\
	do {																	\
		if (_i >= x_arr_count(_arr))										\
			break ;															\
		x_arr_swaplast(_arr, _i);											\
		x_arr_count(_arr)--;												\
	} while (0)

# define	x_arr_map_custom(_type, _it, _arr, _f, _args)					\
																			\
	x_arr_foreach(_type, _it, _arr)											\
	{																		\
		_f _args;															\
	}

# define	x_arr_map(_type, _arr, _f)										\
																			\
	x_arr_map_custom(_type, _tmp, _arr, _f, (*_tmp))

/**
* Enum definition, with automatic stringification function
*/

# define	x_enum_prefix(_pre)												\
																			\
	_pre

# define	x_enum_members(...)												\
																			\
	__VA_ARGS__

# define	x_enum_name(_pre, _name)										\
																			\
	x_concat(_pre, x_concat(_, _name))

# define	x_enum_member_expand(_pre, _name, ...)							\
																			\
	x_enum_name(_pre, _name) __VA_OPT__( = __VA_ARGS__),

# define	x_enum_member(_pre, _name, ...)									\
																			\
	x_enum_member_expand(_pre, _name, ##__VA_ARGS__)

# define	x_enum_member_last(_pre)										\
																			\
	x_enum_member_expand(_pre, XENUM_LAST)

# define	x_enum_member_inner(_pre, _args)								\
																			\
	x_enum_member(_pre, x_enum_members _args)

# define	x_enum_member_list(_pre, ...)									\
																			\
	x_foreach_1(x_enum_member_inner, _pre, __VA_ARGS__)


# define	x_tostr_default													\
																			\
	default: return "UNKNOWN";

# define	x_tostr_case_expand(_pre, _name, ...)							\
																			\
	case x_enum_name(_pre, _name):											\
	return x_stringify(x_enum_name(_pre, _name));

# define	x_tostr_case(_pre, _name, ...)									\
																			\
	x_tostr_case_expand(_pre, _name, ##__VA_ARGS__)

# define	x_tostr_case_last(_pre)											\
																			\
	x_tostr_case_expand(_pre, XENUM_LAST)

# define	x_tostr_case_inner(_pre, _args)									\
																			\
	x_tostr_case(_pre, x_enum_members _args)

# define	x_tostr_case_list(_pre, ...)									\
																			\
	x_foreach_1(x_tostr_case_inner, _pre, __VA_ARGS__)


# define	x_enum_build(_type, _pre, ...)									\
																			\
	typedef enum x_enum_##_type												\
	{																		\
		x_enum_member_list(_pre, __VA_ARGS__)								\
		x_enum_member_last(_pre)											\
	}	_type;

# define	x_enum_tostr(_type, _pre, ...)									\
																			\
	static inline const char												\
	*x_concat(x_concat(x_, tostr_), _type)(_type x_tostr_arg)				\
	{																		\
		switch (x_tostr_arg)												\
		{																	\
			x_tostr_case_list(_pre, __VA_ARGS__)							\
			x_tostr_case_last(_pre)											\
			x_tostr_default													\
		}																	\
	}

# define	x_enum(_type, _prefix, ...)										\
																			\
	x_enum_build(_type, _prefix, __VA_ARGS__)								\
	x_enum_tostr(_type, _prefix, __VA_ARGS__)

#endif // _XLIB_H
