/**
 * @file HeadToFutureTogether.c
 * @author Zach (2063408575@qq.com)
 * 
 * @brief 《一起去未来吧》- 刘浩存
 * 
 * @version 0.1
 * @date 2025-07-05
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "HeadToFutureTogether.h"

/*------------------------ 旋律层 ---------------------*/

static const u8 m1[] =
{
    REST, P1, B5, P1_2, C6, P1_2, C6, P1, B5, P1_2, C6, P1_2,

    B5, P1, G5, P1, REST, P1, E5, P1_2, G5, P1_2,

    A5, P1, G5, P1_2, G5, P1_2 + P1, F5, P1_2, G5, P1_2 + P1,

    C5, P1, C5, P1_2, C5, P1_2, D5, P1_2, C5, P1_2,
};

static const u8 m2[] =
{
    E5, P2, REST, P1, G5, P1_2, D5, P2,

    REST, P1, D5, P1, C5, P1_2,

    D5, P1_2, C5, P2, REST, P1 + P1_2,
};

static const u8 m3[] =
{
    REST, P4,
};

static const u8 m4[] =
{
    E5, P2, REST, P1, G5, P1_2, A5, P1,

    G5, P1, REST, P1, G5, P1, C5, P1_2,

    D5, P1_2, C5, P2, REST, P1_2 + P1,
};

static const u8 m5[] =
{
    REST, P1_2, C6, P1_2, C6, P1_2, C6, P1_2,
    C6, P1_2, C6, P1_2, D6, P1_2, C6, P1_2,

    E6, P1, E6, P1, E6, P1_2, D6, P1_2, C6, P1_2, D6, P1_2 + P1,

    REST, P1, C6, P1_2, C6, P1_2, D6, P1_2, C6, P1_2,
};

static const u8 m6[] =
{
    D6, P1, D6, P1, D6, P1, D6, P1_2, D6, P1,

    C6, P1, REST, P1_2, C6, P1_2, C6, P1_2, D6, P1_2, C6, P1_2,

    E6, P1, E6, P1, E6, P1, G6, P1_2, D6, P1_2 + P1,

    REST, P1_2, C6, P1_2, D6, P1, C6, P1_2, D6, P1_2 + P1,

    D6, P1_2, C6, P1_2 + P2,
};

static const u8 m7[] =
{
    REST, P1_2, C6, P1_2, C6, P1_2, C6, P1_2,
    C6, P1_2, C6, P1_2, D6, P1_2, C6, P1_2,

    E6, P1, E6, P1, E6, P1, G6, P1_2, A6, P1,

    G6, P1, REST, P1_2, D6, P1_2, D6, P1_2, D6, P1_2, C6, P1_2,
};

static const u8 m8[] =
{
    REST, P1 + P1_2, G5, P1_2, C6, P1, A5, P1_2,

    G5, P1, C6, P2 + P1_2, REST, P1,
};

static const u8 m9[] =
{
    B5, P1, B5, P1, B5, P1, A5, P1_2,

    B5, P1, C6, P1_2 + P1, REST, P2,

    C6, P1, C6, P1, C6, P1, D6, P1_2,

    C6, P1_2 + P2, REST, P2,

    E6, P1, D6, P1, D6, P1, E6, P1_2,

    C6, P1_2 + P2, REST, P2,

    C6, P1, C6, P1, C6, P1, G5, P1_2,

    C6, P1_2 + P2, REST, P2,
};

static const u8 m10[] =
{
    REST, P8, REST, P8, REST, P8,
};

static const u8 m11[] =
{
    REST, P1 + P1_2, G5, P1_2, C6, P1, A5, P1,

    G5, P1, C6, P2 + P1,
};

/*----------------------- 音乐层 --------------------*/

static const melody_t melodies[] =
{
    {m1, sizeof(m1) / sizeof(u8)},
    {m2, sizeof(m2) / sizeof(u8)},
    {m3, sizeof(m3) / sizeof(u8)},
    {m1, sizeof(m1) / sizeof(u8)},
    {m4, sizeof(m4) / sizeof(u8)},

    {m5, sizeof(m5) / sizeof(u8)},
    {m6, sizeof(m6) / sizeof(u8)},
    {m7, sizeof(m7) / sizeof(u8)},
    {m6, sizeof(m6) / sizeof(u8)},
    {m8, sizeof(m8) / sizeof(u8)},

    {m10, sizeof(m10) / sizeof(u8)},
    {m3, sizeof(m3) / sizeof(u8)},

    {m1, sizeof(m1) / sizeof(u8)},
    {m2, sizeof(m2) / sizeof(u8)},

    {m5, sizeof(m5) / sizeof(u8)},
    {m6, sizeof(m6) / sizeof(u8)},
    {m7, sizeof(m7) / sizeof(u8)},
    {m6, sizeof(m6) / sizeof(u8)},
    {m8, sizeof(m8) / sizeof(u8)},
    {m9, sizeof(m9) / sizeof(u8)},

    {m10, sizeof(m10) / sizeof(u8)},

    {m5, sizeof(m5) / sizeof(u8)},
    {m6, sizeof(m6) / sizeof(u8)},
    {m7, sizeof(m7) / sizeof(u8)},
    {m6, sizeof(m6) / sizeof(u8)},
    {m11, sizeof(m11) / sizeof(u8)},
    {m3, sizeof(m3) / sizeof(u8)},
};

static const music_t music1 = {
    .melodies = melodies,
    .melody_num = sizeof(melodies) / sizeof(melody_t),
};

const music_3v_song_t m3v_HeadToFutureTogether =
{
    .name = "Head To Future Together",
    .bpm = 130,
    .m1 = &music1,
    .m2 = NULL,
    .m3 = NULL,
};
