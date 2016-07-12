#ifndef TYPES_H
#define TYPES_H

/** @file include/types.h Basic definitions and data types. */

#define ENUM_FLAG_OPERATORS(x) \
constexpr inline x  operator |  (const x a, const x b) { return (x)(((int)a) | ((int)b)); } \
inline x& operator |= (x &a, x b) { return (x&)(((int&)a) |= ((int)b)); } \
constexpr inline x  operator &  (const x a, const x b) { return (x)(((int)a) & ((int)b)); } \
inline x& operator &= (x &a, x b) { return (x&)(((int&)a) &= ((int)b)); } \
constexpr inline x  operator ~  (const x a) { return (x)(~((int)a)); } \
constexpr inline x  operator ^  (const x a, const x b) { return (x)(((int)a) ^ ((int)b)); } \
inline x& operator ^= (x &a, x b) { return (x&)(((int&)a) ^= ((int)b)); } \

#define ENUM_MATH_OPERATORS(x) \
inline x& operator ++ (x& a) { a = (x)((int)a + 1); return a; } \
inline x  operator ++ (x& a, int) { x b = a; a = (x)((int)a + 1); return b; } \
constexpr inline x  operator +  (const x a, const x b) { return (x)(((int)a) + ((int)b)); } \
constexpr inline x  operator +  (const int a, const x b) { return (x)((a) + ((int)b)); } \
constexpr inline x  operator +  (const x a, const int b) { return (x)(((int)a) + (b)); } \
inline x& operator += (x &a, x b) { return (x&)(((int&)a) += ((int)b)); } \
inline x& operator += (x &a, int b) { return (x&)(((int&)a) += (b)); } \
inline x& operator -- (x& a) { a = (x)((int)a - 1); return a; } \
inline x  operator -- (x& a, int) { x b = a; a = (x)((int)a - 1); return b; } \
constexpr inline x  operator -  (const x a, const x b) { return (x)(((int)a) - ((int)b)); } \
constexpr inline x  operator -  (const int a, const x b) { return (x)((a) - ((int)b)); } \
constexpr inline x  operator -  (const x a, const int b) { return (x)(((int)a) - (b)); } \
inline x& operator -= (x &a, x b) { return (x&)(((int&)a) -= ((int)b)); } \
inline x& operator -= (x &a, int b) { return (x&)(((int&)a) -= (b)); }

#define GCC_PACKED
#define GCC_UNUSED
#define PACK

#define BIT_S8  int8
#define BIT_S16 int16
#define BIT_S32 int32
#define BIT_U8  uint8
#define BIT_U16 uint16
#define BIT_U32 uint32

#define UNUSED(x) (void)x;

#define MSVC_PACKED_BEGIN __pragma(pack(push, 1))
#define MSVC_PACKED_END __pragma(pack(pop))
#pragma warning(disable:4102) /* unreferenced label */
#pragma warning(disable:4996) /* deprecated functions */

/* GCC warns about redundant redeclartion; to avoid it, we add __LINE__ to
 *  the variable name. This of course doesn't avoid most warnings, but
 *  should at least avoid most.
 * We need a second indirection to resolve __LINE__ correctly. */
#define __assert_compile(expr, line) extern const int __ct_assert_##line##__[1 - 2 * !(expr)] GCC_UNUSED
#define _assert_compile(expr, line) __assert_compile(expr, line)
#define assert_compile(expr) _assert_compile(expr, __LINE__)

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned int uint32;
typedef signed int int32;
typedef unsigned int uint;

assert_compile(sizeof(uint8 ) == 1);
assert_compile(sizeof( int8 ) == 1);
assert_compile(sizeof(uint16) == 2);
assert_compile(sizeof( int16) == 2);
assert_compile(sizeof(uint32) == 4);
assert_compile(sizeof( int32) == 4);

/**
 * bits 0 to 7 are the offset in the tile.
 * bits 8 to 13 are the position on the map.
 * bits 14 and 15 are never used (or should never be used).
 */
typedef struct tile32
{
	uint16 x;
	uint16 y;
} tile32;

enum IndexType
{
	IT_NONE = 0,
	IT_TILE = 1,
	IT_UNIT = 2,
	IT_STRUCTURE = 3
};

#endif /* TYPES_H */
