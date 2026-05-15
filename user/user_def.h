/**
 * @file user_def.h
 * @author Zach (2063408575@qq.com)
 * 
 * @brief 一些 defines 和 typedefs。此文件被 main.h 包含。
 * 
 * @version 0.1
 * @date 2025-07-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

/*-------------------------------- Basic -------------------------------*/

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

#define Bit_RESET       ((u32)(0))
#define Bit_SET         ((u32)(1))

#ifndef max
    #define max(a, b)       ((a) > (b) ? (a) : (b))
#endif
#ifndef min
    #define min(a, b)       ((a) < (b) ? (a) : (b))
#endif

#define BIT(n)          ((u32)(1U << (n)))

/*-------------------------------- Compiler -------------------------------*/

#ifdef __GNUC__
    #define UNLIKELY(x)     __builtin_expect(!!(x), 0)
    #define LIKELY(x)       __builtin_expect(!!(x), 1)
#else
    #define UNLIKELY(x)     (x)
    #define LIKELY(x)       (x)
#endif

#ifndef IS_ISR_CONTEXT
#define IS_ISR_CONTEXT() (__get_IPSR() != 0U)
#endif // !IS_ISR_CONTEXT

/*-------------------------------- Extension -------------------------------*/

#define USER_UNUSED(x)  (void)(x)

#define begins_with_str(str, cstr) (strncmp((char*)(str), (cstr), sizeof(cstr) - 1) == 0)
#define lenof_cstr(cstr) (sizeof(cstr) - 1)

#ifdef __cplusplus
} // extern "C"
#endif
