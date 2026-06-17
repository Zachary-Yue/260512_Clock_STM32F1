
#include "Rainbow.h"

/* 《Rainbow》- 刘浩存、王安宇 */

/*--------------- Main Melody - Field 6 -----------*/

static const u8 main_m1[] =
{
    REST, P2 + P1 + P1_2, C6, P1_2, C6, P1 + P1_2, E6, P1_2 + P4,

    REST, P1 + P1_2, C6, P1_2, B5, P1 + P1_2, E6, P1_2 + P1, D6, P1 + P2,

    REST, P1 + P1_2, C6, P1_2, C6, P1 + P1_2, E6, P2,

    REST, P1_2,
};

static const u8 main_m2[] =
{
    D6, P1, C6, P1_2, C6, P1, G5, P1, F5, P1 + P2 + P1_2, REST, P1,
};

static const u8 main_m3[] =
{
    D6, P1, C6, P1_2, C6, P1_2 + P1, B5, P1_2,
    C6, P1_2 + P1 + P2, REST, P1,
};

static const u8 main_m4[] =
{
    REST, P1, G5, P1, B5, P1, C6, P1,

    C6, P1, REST, P1, C6, P1, REST, P1,

    C6, P1_2, C6, P1_2, C6, P1_2, D6, P1_2, REST, P1_2, E6, P1, D6, P1_2,

    C6, P1, REST, P1, C6, P1, REST, P1,

    G6, P1_2, F6, P1_2, E6, P1_2, D6, P1_2, REST, P1_2, E6, P1,
    C6, P1_2, C6, P1 + P2, REST, P2 + P1_2,

    F6, P1, E6, P1, D6, P1_2, D6, P4,

    REST, P4,
};

static const u8 main_m5[] =
{
    REST, P2, G5, P1_2, G5, P1, E6, P1_2,
};

static const u8 main_m6[] =
{
    E6, P1_2, D6, P1, D6, P1, C6, P1, D6, P1_2 + P1, REST, P1,

    G5, P1_2, G5, P1, F6, P1, E6, P1, E6, P1, D6, P1,
    E6, P1_2 + P2, REST, P2,

    E6, P1_2, D6, P1, D6, P1, C6, P1, C6, P1_2 + P2, REST, P2,

    C6, P1_2, C6, P1_2, C6, P1_2, C6, P1, B5, P1, C6, P1_2,
};

static const u8 main_m7[] =
{
    C6, P1, REST, P1, G5, P1_2, G5, P1, E6, P1_2,
};

static const u8 main_m8[] =
{
    C6, P1 + P2, REST, P1,
};

static const u8 main_m9[] =
{
    C6, P1_2, B5, P1, A5, P1, G5, P1, D6, P1_2 + P2, REST, P2,

    C6, P1_2, D6, P1, E6, P1, C6, P1, B5, P1_2 + P2 + P1,
    REST, P1 + P2, B5, P1, C6, P4, REST, P1,

    B5, P1_2, C6, P1_2, B5, P1_2, C6, P1_2, B5, P1, G5, P1_2,
    F5, P1_2, F5, P2, REST, P2,

    B5, P1_2, C6, P1_2, B5, P1_2, C6, P1_2, B5, P1, G5, P1_2,
    F5, P1_2, F5, P4, REST, P4,
};

static const melody_t main_melodies[] =
{
    {main_m1, sizeof(main_m1) / sizeof(u8)},
    {main_m2, sizeof(main_m2) / sizeof(u8)},
    {main_m1, sizeof(main_m1) / sizeof(u8)},
    {main_m3, sizeof(main_m3) / sizeof(u8)},
    {main_m4, sizeof(main_m4) / sizeof(u8)},
    {main_m5, sizeof(main_m5) / sizeof(u8)},
    {main_m6, sizeof(main_m6) / sizeof(u8)},
    {main_m7, sizeof(main_m7) / sizeof(u8)},
    {main_m6, sizeof(main_m6) / sizeof(u8)},
    {main_m8, sizeof(main_m8) / sizeof(u8)},
    {main_m9, sizeof(main_m9) / sizeof(u8)},
    {main_m5, sizeof(main_m5) / sizeof(u8)},
    {main_m6, sizeof(main_m6) / sizeof(u8)},
    {main_m7, sizeof(main_m7) / sizeof(u8)},
    {main_m6, sizeof(main_m6) / sizeof(u8)},
    {main_m8, sizeof(main_m8) / sizeof(u8)},
};

static const music_t music_rainbow_main =
{
    .melodies = main_melodies,
    .melody_num = sizeof(main_melodies) / sizeof(melody_t),
};

/*----------------- Chord P1 -------------*/

static const u8 c1_m1[] =
{
    REST, P8, REST, P8, REST, P8, REST, P8, REST, P8, REST, P8,
    REST, P8, REST, P8, REST, P8, REST, P8, REST, P4,
};

static const u8 c1_m2[] =
{
    REST, P8, REST, P4, REST, P2, C7, P1, C7, P1_8, D7, P1_8,
    E7, P1_8, F7, P1_8, G7, P1_8, A7, P1_8, B7, P1_8,
};

static const u8 c1_m2_1[] =
{
    C8, P1_8 + P1 + P2, REST, P1,
};

static const u8 c1_m2_2[] =
{
    C8, P1_8 + P4,
};

static const u8 c1_m3[] =
{
    REST, P2 + P1_2,

    C7, P1_2 + P1, D7, P3, REST, P1 + P2 + P1_2,
    B6, P1_2 + P1, C7, P3, C8, P3 + P1_2,

    B7, P1_2, C8, P1_2, G7, P1_2, F7, P2, REST, P1,

    E7, P1_2, F7, P1_2, G7, P1, G6, P1, E7, P1, F7, P1, E7, P3, REST, P1,

    REST, P2 + P1_2, G7, P1, F7, P1_4, E7, P1_4, D7, P2, REST, P2,

    G7, P2 + P1_2, D8, P1 + P1_2,

    C8, P4,

    REST, P2, C8, P1_2, G7, P1_2, E7, P1_2, C7, P1_2,
    F7, P2, E7, P1_2, F7, P1_2 + P1,

    G7, P2, F7, P1_2, G7, P1_2 + P1, E7, P4,
};

static const u8 c1_m4[] =
{
    // 当大雨模糊
    REST, P1 + P1_2, E7, P1, D7, P1, C7, P1_2,

    G7, P1 + P1_2, F7, P1_4, E7, P1_4, D7, P2,

    REST, P2 + P1_2, A7, P1_2 + P1, G7, P3, D8, P3,
    A7, P1_2, G7, P1_2, E7, P1_2, D7, P1_2,

    C7, P3, C8, P3, A7, P1_2, G7, P1_2, E7, P1_2, C7, P1_2,

    F7, P4, REST, P2, F7, P1_2, E7, P1_2, D7, P1_2, C7, P1_2,

    F7, P2, G7, P2, Gs7, P4,
};

static const u8 c1_m6_1[] =
{
    C8, P2 + P1_2,

    B7, P1_2, C8, P1_2, G7, P1_2,
};

static const u8 c1_m6_1_1[] =
{
    E7, P1_4, D7, P1_4, C7, P1_2, E7, P1_2, F7, P1_2,

    F7, P1_4, E7, P1_4, D7, P1_2, E7, P1_2, F7, P1_2,
};

static const u8 c1_m6_2[] =
{
    G7, P2, REST, P2 + P1_2, E8, P1, D8, P1, C8, P1_2 + P1,

    D8, P3 + P1_2, G7, P1_2 + P1, C8, P2, B7, P1,

    C8, P4, REST, P1 + P1_2,

    B7, P1_4, C8, P1_4,

    C8, P1_4, B7, P1_4, A7, P1_4, G7, P1_4, A7, P1_4, G7, P1_4,
    D7, P1_4, C7, P1_4,

    F7, P4, G7, P4,
};

static const melody_t c1_melodies[] =
{
    {c1_m1, sizeof(c1_m1) / sizeof(u8)},
    {c1_m2, sizeof(c1_m2) / sizeof(u8)},
    {c1_m2_1, sizeof(c1_m2_1) / sizeof(u8)},
    {c1_m3, sizeof(c1_m3) / sizeof(u8)},
    {c1_m4, sizeof(c1_m4) / sizeof(u8)},
    {c1_m2, sizeof(c1_m2) / sizeof(u8)},
    {c1_m2_2, sizeof(c1_m2_2) / sizeof(u8)},
    {c1_m6_1, sizeof(c1_m6_1) / sizeof(u8)},
    {c1_m6_1_1, sizeof(c1_m6_1_1) / sizeof(u8)},
    {c1_m6_1_1, sizeof(c1_m6_1_1) / sizeof(u8)},
    {c1_m6_2, sizeof(c1_m6_2) / sizeof(u8)},
};

static const music_t music_rainbow_c1 =
{
    .melodies = c1_melodies,
    .melody_num = sizeof(c1_melodies) / sizeof(melody_t),
};

const music_3v_song_t m3v_Rainbow =
{
    .name = "Rainbow",
    .bpm = 148,
    .m1 = &music_rainbow_main,
    .m2 = &music_rainbow_c1,
    .m3 = NULL,
};
