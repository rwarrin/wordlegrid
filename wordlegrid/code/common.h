#ifndef COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef s32 b32;

#define true 1
#define false 0

#define Assert(Condition) do { if(!(Condition)) { *(u32 *)0 = 0; } } while(0)
#define InvalidCodePath Assert(!"InvalidCodePath")

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)

inline void
ZeroSize_(void *Memory, u32 Size)
{
    u8 *Ptr = (u8 *)Memory;
    while(Size--)
    {
        *Ptr++ = 0;
    }
}

#define COMMON_H
#endif
