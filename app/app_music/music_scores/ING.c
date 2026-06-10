
#include "ING.h"

/*----------------- Main ------------------*/

static const u8 main_m1[] =
{
    E6, P1, C6, P1_2, REST, P1_2, G6, P1_2 + P1_4, F6, P1_4,
    E6, P1_2 + P1_4, F6, P1_4,

    E6, P2, D6, P1, REST, P1,
};

static const u8 main_m1_2[] =
{
    E6, P1, C6, P1_2, REST, P1_2,

    G6, P1_2 + P1_4, F6, P1_4, E6, P1_2 + P1_4, F6, P1_4,

    E6, P2, D6, P1, REST, P1,

    D6, P1_2 + P1_4, E6, P1_4, F6, P1_2 + P1_4, G6, P1_4,

    A6, P1_2, REST, P1_4, G6, P1_4, F6, P1_2 + P1_4, E6, P1_4,

    E6, P2, D6, P1, REST, P1,
};

/* 然后重复 1 */

static const u8 main_m4[] =
{
    D6, P1_2 + P1_4, E6, P1_4, F6, P1_2 + P1_4, G6, P1_4,

    A6, P1_2, REST, P1_4, G6, P1_4, A6, P1_2 + P1_4, B6, P1_4,

    C7, P1, REST, P1 + P2,
};
// 装饰音（第2、3拍，刮奏 G7~C8~HE7, C8~HE7~HH5.

/* 重复 1 2 */

static const u8 main_m1_var[] =
{
    E6, P1, C6, P1_2, REST, P1_2, G6, P1_2 + P1_4, F6, P1_4,
    E6, P1_2 + P1_4, F6, P1_4,

    E6, P2, D6, P2,
};

static const u8 main_m8[] =
{
    D6, P1_2 + P1_4, E6, P1_4, F6, P1_2 + P1_4, G6, P1_4,

    A6, P1_2 + P1_4, G6, P1_4, A6, P1_2 + P1_4, B6, P1_4,

    C7, P2 + P1 + P1_2, REST, P1_2,
};

static const u8 main_m9_12[] =
{
    C6, P1, C6, P1_2 + P1_4, E6, P1_4, D6, P1, C6, P1,

    B5, P1, B5, P1_2 + P1_4, D6, P1_4, C6, P1, B5, P1,

    A5, P1, A5, P1_2 + P1_4, B5, P1_4, C6, P1, A5, P1,

    B5, P1, B5, P1_2 + P1_4, C6, P1_4, D6, P1, G5, P1,

    E6, P1, E6, P1_2 + P1_4, G6, P1_4, D6, P1, C6, P1,

    D6, P1, D6, P1, G5, P1, B5, P1,

    F6, P1, F6, P1_2 + P1_4, D6, P1_4, G5, P1, G6, P1,

    E6, P4 - P1_2, REST, P1_2,
};

/* 重复 1 2 1 4 */
/* End */

static const melody_t main_melodies[] =
{
    {main_m1_2, sizeof(main_m1_2) / sizeof(u8)},
    {main_m1, sizeof(main_m1) / sizeof(u8)},
    {main_m4, sizeof(main_m4) / sizeof(u8)},

    {main_m1_2, sizeof(main_m1_2) / sizeof(u8)},
    {main_m1_var, sizeof(main_m1_var) / sizeof(u8)},
    {main_m8, sizeof(main_m8) / sizeof(u8)},

    {main_m9_12, sizeof(main_m9_12) / sizeof(u8)},

    {main_m1_2, sizeof(main_m1_2) / sizeof(u8)},
    {main_m1, sizeof(main_m1) / sizeof(u8)},
    {main_m4, sizeof(main_m4) / sizeof(u8)},
};

static const music_t music_ing_main =
{
    .melodies = main_melodies,
    .melody_num = sizeof(main_melodies) / sizeof(melody_t),
};

/*----------------- C1 ------------------*/

static const u8 c1_m1[] =
{
    C5, P1_2, REST, P1_2, E5, P1_4, G5, P1_4, REST, P1_2,
    C5, P1_2, REST, P1_2, E5, P1_4, G5, P1_4, REST, P1_2,
    B4, P1_2, REST, P1_2, F5, P1_4, G5, P1_4, REST, P1_2,
    B4, P1_2, REST, P1_2, D5, P1_4, G5, P1_4, REST, P1_2,
};

static const u8 c1_m2[] =
{
    A4, P1_2, REST, P1_2, C5, P1_4, F5, P1_4, REST, P1_2,
    A4, P1_2, REST, P1_2, C5, P1_4, F5, P1_4, REST, P1_2,
    C5, P1_2, REST, P1_2, E5, P1_4, G5, P1_4, REST, P1_2,
    C5, P1_2, REST, P1_2, E5, P1_4, G5, P1_4, REST, P1_2,
};

static const u8 c1_m4[] =
{
    A4, P1_2, REST, P1_2, C5, P1_4, F5, P1_4, REST, P1_2,
    G4, P1_2, REST, P1_2, B4, P1_4, F5, P1_4, REST, P1_2,
    REST, P1, G6, P1_8, A6, P1_8, B6, P1_8, C7, P1_8, E7, P1_8, REST, P3_8,
    C7, P1_8, D7, P1_8, E7, P1_8, F7, P1_8, G7, P1_8, REST, P3_8,
    REST, P1,
};

static const u8 c1_m5_8[] =
{
    C5, P1_2, REST, P1_2, E5, P1_4, G5, P1_4, REST, P1_2,
    C5, P1_2, REST, P1_2, E5, P1_4, G5, P1_4, REST, P1_2,

    G4, P1_2, REST, P1_2, D5, P1_4, F5, P1_4, REST, P1_2,
    G4, P1_2, REST, P1_2, B4, P1_4, E5, P1_4, REST, P1_2,

    D4, P1_2, REST, P1_2, F4, P3_16, A4, P3_16, C5, P3_16, REST, P1_4 + P3_16,

    F4, P1_2, REST, P1_2, F4, P3_16, A4, P3_16, D5, P3_16, REST, P1_4 + P3_16,

    G4, P1_2, REST, P1_2, D4, P1_4, G4, P1_4, REST, P1_2,
    E4, P1_4, A4, P1_4, REST, P1_2, D4, P1_4, B4, P1_4, REST, P1_2,

    C5, P1_2, REST, P1_2, E5, P1_4, G5, P1_4, REST, P1_2,
    C5, P1_2, REST, P1_2, E5, P1_4, G5, P1_4, REST, P1_2,

    G4, P1_2, REST, P1_2, B4, P1_4, D5, P1_4, REST, P1_2,
    G4, P1_2, REST, P1_2, D5, P1_4, F5, P1_4, REST, P1_2,

    D4, P1_2, REST, P1_2, F4, P1_4, C5, P1_4, REST, P1_2,
    G4, P1_2, REST, P1_2, B4, P1_4, F5, P1_4, REST, P1_2,

    C5, P1_2, REST, P1_2, C5, P1_4, E5, P1_4, REST, P1_2,
    B4, P1_4, D5, P1_4, REST, P1_2, C5, P1_4, F5, P1_4, REST, P1_2,
};

static const u8 c1_m9_12[] =
{
    E4, P1, C5, P1, E5, P2,

    G5, P1, G5, P1, G4, P2,

    D4, P1, F4, P1, A4, P1, C5, P1,

    B4, P1, G4, P1, A4, P1, B4, P1,

    E4, P1_2, G4, P1_2, C5, P1_2, G4, P1_2,
    E5, P1_2, G4, P1_2, C5, P1_2, G4, P1_2,

    B4, P1_2, D4, P1_2, G4, P1_2, D4, P1_2,
    D4, P1_2, G4, P1_2, A4, P1_2, B4, P1_2,

    A5, P1_2, C6, P1_2, D6, P1_2, A5, P1_2,
    G5, P1_2, B5, P1_2, D6, P1_2, G5, P1_2,

    E5, P1_2, G5, P1_2, C6, P1_2, E6, P1_2,
    G5, P3_16, A5, P3_16, B5, P3_16, C6, P3_16, D6, P3_16, E6, P3_16,
    REST, P1_2 + P3_8,
};

static const u8 c1_m14[] =
{
    A5, P1_2, REST, P1_2, C6, P1_4, F6, P1_4, REST, P1_2,
    A5, P1_2, REST, P1_2, C6, P1_4, F6, P1_4, REST, P1_2,
    G5, P1_2, REST, P1_2, C6, P1_4, F6, P1_4, REST, P1_2,
    G5, P1_2, REST, P1_2, B5, P1_4, G6, P1_4, REST, P1_2,
};

static const melody_t c1_melodies[] =
{
    {c1_m1, sizeof(c1_m1) / sizeof(u8)},
    {c1_m2, sizeof(c1_m2) / sizeof(u8)},
    {c1_m1, sizeof(c1_m1) / sizeof(u8)},
    {c1_m4, sizeof(c1_m4) / sizeof(u8)},
    {c1_m5_8, sizeof(c1_m5_8) / sizeof(u8)},
    {c1_m9_12, sizeof(c1_m9_12) / sizeof(u8)},
    {c1_m1, sizeof(c1_m1) / sizeof(u8)},
    {c1_m14, sizeof(c1_m14) / sizeof(u8)},
    {c1_m1, sizeof(c1_m1) / sizeof(u8)},
};

static const music_t music_ing_c1 =
{
    .melodies = c1_melodies,
    .melody_num = sizeof(c1_melodies) / sizeof(melody_t),
};

/*----------------- C2 ------------------*/

static const u8 c2_m1[] =
{
    REST, P8, REST, P8, REST, P8, REST, P8,

    E5, P1, REST, P1, G5, P1_2 + P1_4, F5, P1_4,
    E5, P1_2 + P1_4, F5, P1_4,

    E5, P1 + P1_2, REST, P2 + P1_2,

    REST, P1, F5, P1_2 + P1_4, G5, P1_4, A5, P1_2, REST, P1_4,
    G5, P1_4, F5, P1_2 + P1_4, E5, P1_4,

    E5, P2, REST, P2,

    E5, P1, REST, P1, G5, P1_2 + P1_4, F5, P1_4,
    E5, P1_2 + P1_4, F5, P1_4,

    E5, P2, REST, P2,

    REST, P2, A6, P1_2 + P1_4, G6, P1_4, A6, P1_2 + P1_4, B6, P1_4,

    C7, P2 + P1 + P1_2, REST, P1_2,

    C6, P1 + P1_2 + P1_4, E6, P1_4, D6, P1, E6, P1,
    G6, P4 - P1_4, REST, P1_4,

    A6, P2, C7, P1, A6, P1, G6, P1, REST, P1,

    A6, P1, B6, P1, C7, P2, E7, P2, G7, P2,

    A7, P1, G7, P1, F7, P2, B6, P2, C7, P4 - P1_2, REST, P1_2,
};

static const melody_t c2_melodies[] =
{
    {c2_m1, sizeof(c2_m1) / sizeof(u8)},
};

static const music_t music_ing_c2 =
{
    .melodies = c2_melodies,
    .melody_num = sizeof(c2_melodies) / sizeof(melody_t),
};

/*----------------- Song ------------------*/

const music_3v_song_t m3v_ING =
{
    .name = "ING",
    .bpm = 132,
    .m1 = &music_ing_main,
    .m2 = &music_ing_c1,
    .m3 = &music_ing_c2,
};
