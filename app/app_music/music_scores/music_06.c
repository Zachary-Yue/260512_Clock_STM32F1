
#include "music_06.h"

/* 不知名电子琴示范曲 */

/*------------------------ Main Melodies ------------------------*/

static const u8 main_m1[] =
{
    REST, P2, D6, P1, D6, P1, G6, P1, A6, P1,

    B6, P2, G6, P1_2, Fs6, P1_2,

    E6, P1 + P1_2, C7, P1_2, C7, P1,

    C7, P2, REST, P1,

    B6, P1 + P1_2, A6, P1_2, G6, P1,
};

static const u8 main_m2[] =
{
    G6, P1, Fs6, P1, G6, P1,
    A6, P2 + P1,
};

static const u8 main_m3[] =
{
    Fs6, P1, G6, P1, A6, P1,
    G6, P1 + P2,
};

static const melody_t main_melodies[] =
{
    {main_m1, sizeof(main_m1) / sizeof(u8)},
    {main_m2, sizeof(main_m2) / sizeof(u8)},
    {main_m1, sizeof(main_m1) / sizeof(u8)},
    {main_m3, sizeof(main_m3) / sizeof(u8)},
};

static const music_t music_06_main =
{
    .melodies = main_melodies,
    .melody_num = sizeof(main_melodies) / sizeof(melody_t),
};

/*--------------------- Chord 1 Melodies ---------------------*/

static const u8 c1_m0[] =
{
    REST, P1 + P2,
};

static const u8 c1_m1[] =
{
    G4, P1_2, B4, P1_2, D5, P1_2, B4, P1_2, D5, P1_2, B4, P1_2,
};

static const u8 c1_m2[] =
{
    C5, P1_2, E5, P1_2, G5, P1_2, E5, P1_2, G5, P1_2, E5, P1_2,
};

static const u8 c1_m3[] =
{
    A4, P1_2, Cs5, P1_2, E5, P1_2, Cs5, P1_2, E5, P1_2, Cs5, P1_2,
};

static const u8 c1_m4[] =
{
    D5, P1_2, Fs5, P1_2, A5, P1_2, Fs5, P1_2, A5, P1_2, Fs5, P1_2,
};

static const u8 c1_m5[] =
{
    D5, P1, REST, P1, D5, P1, D5, P1,
};

static const melody_t c1_melodies[] =
{
    {c1_m0, sizeof(c1_m0) / sizeof(u8)},

    {c1_m1, sizeof(c1_m1) / sizeof(u8)},
    {c1_m1, sizeof(c1_m1) / sizeof(u8)},
    {c1_m2, sizeof(c1_m2) / sizeof(u8)},
    {c1_m2, sizeof(c1_m2) / sizeof(u8)},

    {c1_m1, sizeof(c1_m1) / sizeof(u8)},
    {c1_m3, sizeof(c1_m3) / sizeof(u8)},
    {c1_m4, sizeof(c1_m4) / sizeof(u8)},
    {c1_m4, sizeof(c1_m4) / sizeof(u8)},

    {c1_m1, sizeof(c1_m1) / sizeof(u8)},
    {c1_m1, sizeof(c1_m1) / sizeof(u8)},
    {c1_m2, sizeof(c1_m2) / sizeof(u8)},
    {c1_m2, sizeof(c1_m2) / sizeof(u8)},

    {c1_m1, sizeof(c1_m1) / sizeof(u8)},
    {c1_m4, sizeof(c1_m4) / sizeof(u8)},
    {c1_m1, sizeof(c1_m1) / sizeof(u8)},
    {c1_m2, sizeof(c1_m2) / sizeof(u8)},

    {c1_m5, sizeof(c1_m5) / sizeof(u8)},
};

static const music_t music_06_chord1 =
{
    .melodies = c1_melodies,
    .melody_num = sizeof(c1_melodies) / sizeof(melody_t),
};

/*--------------------- Chord 2 Melodies ---------------------*/

static const u8 c2_m1[] =
{
    REST, P8, REST, P8, REST, P8,

    REST, P2, D6, P1,

    D6, P1, G6, P1, Fs6, P1,

    G6, P2, Fs6, P1,

    E6, P4 + P1, Fs6, P1,

    G6, P1 + P1_2, A6, P1_2, B6, P1,

    A6, P1, B6, P1, C7, P1,

    B6, P2 + P1, C7, P2 + P1, B6, P4,
};

static const melody_t c2_melodies[] =
{
    {c2_m1, sizeof(c2_m1) / sizeof(u8)},
};

static const music_t music_06_chord2 =
{
    .melodies = c2_melodies,
    .melody_num = sizeof(c2_melodies) / sizeof(melody_t),
};

const music_3v_song_t m3v_music_06 =
{
    .name = "Music_06",
    .bpm = 108,
    .m1 = &music_06_main,
    .m2 = &music_06_chord1,
    .m3 = &music_06_chord2,
};
