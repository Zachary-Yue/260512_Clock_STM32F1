
#include "ShanhuHai.h"

/*----------------------------------- Main1 ------------------------------------*/

static const u8 main1_m1[] =
{
    REST, P1_2, C6, P1_2, C6, P1_2, C6, P1_2, C6, P1_2, B5, P1_2,
    A5, P1_2, B5, P1_2,

    A5, P1_4, G5, P1_4, G5, P1_2 + P1, REST, P2,

    REST, P1_2, C6, P1_2, C6, P1_2, C6, P1_2, C6, P1_2, D6, P1_2,
    D6, P1_2, G5, P1_2,

    F6, P1_2, E6, P1_2 + P1, REST, P1, E6, P1_2, G6, P1_2,

    G6, P1_4, C6, P1_4, C6, P1 + P1_2, REST, P1, C6, P1_2, E6, P1_2,

    E6, P1_4, A5, P1_4, A5, P1_2 + P1, REST, P1, A5, P1_2, C6, P1_2,

    D6, P1 + P1_2, E6, P1_2, F6, P1 + P1_2, E6, P1_4, D6, P1_4,

    D6, P3,

    REST, P1 + P4,

    REST, P1 + P1_2, G5, P1_2, D6, P1_2, E6, P1_4, E6, P1_4 + P1,

    REST, P1_2, E6, P1_2, E6, P1_2, E6, P1_2, E6, P1_2, D6, P1_2,
    C6, P1_2, D6, P1_2,

    D6, P1_2, F6, P1_4, E6, P1_4 + P1, REST, P2,

    REST, P1, C6, P1_2, D6, P1_2, E6, P1_2, G6, P1_2 + P1,

    REST, P1, C6, P1_2, D6, P1_2, E6, P1_2, C6, P1_2, G5, P1_2,
    C6, P1_2,

    E6, P1 + P1_2, F6, P1_2, G6, P1 + P1_2, E6, P1_2,

    F6, P2, REST, P1, G5, P1_2, D6, P1_2,

    D6, P1_2, E6, P1, REST, P1_2, E6, P1_2, F6, P1_2, F6, P1_2,
    C6, P1_2,

    A6, P1_2, G6, P1, REST, P1_2, G6, P1_2, A6, P1_2, B6, P1_2,
    D7, P1_2,

    D7, P1_2, C7, P1, REST, P1_2, C7, P1_2, B6, P1_2, A6, P1_2,
    B6, P1_2,

    A6, P1_2, G6, P1 + P1_2, REST, P1, G6, P1_2, B6, P1_2,

    B6, P1_2, C7, P1, REST, P1_2, C7, P1_2, B6, P1_2, A6, P1_2,
    B6, P1_2,

    A6, P1_2, G6, P1 + P1_2, REST, P1, E6, P1_2, A6, P1_2,

    A6, P1_4, C6, P1_4, C6, P1, REST, P1_2, A6, P1_2, G6, P1_2,
    F6, P1_2, A6, P1_2,

    A6, P1, REST, P1_2, G6, P1_2, G6, P1_2, REST, P1_2, G5, P1_2,
    D6, P1_2,

    D6, P1_2, E6, P1, REST, P1_2, E6, P1_2, F6, P1_2, F6, P1_2,
    C6, P1_2,

    A6, P1_2, G6, P1, REST, P1_2, G6, P1_2, A6, P1_2, B6, P1_2,
    D7, P1_2,

    D7, P1_2, C7, P1, REST, P1_2, C7, P1_2, B6, P1_2, A6, P1_2,
    B6, P1_2,

    A6, P1_2, G6, P1_2 + P1, REST, P1, G6, P1_2, B6, P1_2,

    B6, P1_2, C7, P1, REST, P1_2, C7, P1_2, B6, P1_2, A6, P1_2,
    B6, P1_2,

    A6, P1_2, G6, P1_2 + P1, REST, P1, E6, P1_2, A6, P1_2,

    A6, P1_4, C6, P1_4, C6, P1, REST, P1_2, A6, P1_2, G6, P1_2,
    F6, P1_2, E6, P1_2,

    E6, P1_4, D6, P1_2, D6, P1_4 + P1, E6, P1_2, D6, P1_2, C6, P1_2,
    D6, P1_2,

    D6, P1_4, C6, P1_2, C6, P1_4 + P2, REST, P1,
};

static const melody_t main1_melodies[] =
{
    { main1_m1, sizeof(main1_m1) / sizeof(u8) },
};

static const music_t music_shanhu_hai_main1 =
{
    .melodies = main1_melodies,
    .melody_num = sizeof(main1_melodies) / sizeof(melody_t),
};

/*----------------------------------- Main2 ------------------------------------*/

static const u8 main2_m1[] =
{
    REST, P8, REST, P8, REST, P8, REST, P8,

    REST, P1_2, C7, P1_2, C7, P1_2, C7, P1_2, C7, P1_2, B6, P1_2,
    A6, P1_2, B6, P1_2,

    A6, P1_2, G6, P1_2 + P1, REST, P2,

    REST, P1_2, C7, P1_2, C7, P1_2, C7, P1_2, C7, P1_2, B6, P1_2,
    A6, P1_2, B6, P1_2,

    B6, P1_2, C7, P1_4, C7, P1_4 + P1, REST, P1, C7, P1_2, B6, P1_2,

    B6, P1_2, C7, P1_2 + P1, REST, P1, C7, P1_2, B6, P1_2,

    B6, P1_2, C7, P1_4, C7, P1_4 + P1, REST, P1, G6, P1_2, C7, P1_2,

    C7, P1 + P1_2, D7, P1_2, E7, P1 + P1_2, C7, P1_2,

    D7, P2, REST, P2,

    REST, P1, C7, P1, C7, P1_2, D7, P1_2, D7, P1_2, C7, P1_2,

    C7, P1_2, B6, P1_2 + P1, B6, P1_2, C7, P1_2, D7, P1_2, F7, P1_2,

    F7, P1_2, E7, P1_2 + P1, E7, P1_2, D7, P1_2, C7, P1_2, D7, P1_2,

    C7, P1_2, B6, P1_2 + P1, REST, P1, D7, P1,

    C7, P1_2, E7, P1 + P1_2, E7, P1_2, D7, P1_2, C7, P1_2, D7, P1_2,

    C7, P1_2, B6, P1, G6, P1_2, B6, P1_2, C7, P1_4, C7, P1_4 + P1,

    REST, P1, E6, P1_2, A6, P1_2, A6, P1_2, B6, P1_2, C7, P1_2,
    D7, P1_2,

    C7, P1 + P1_2, B6, P1_2, B6, P1_2, REST, P1_2, G6, P1_2, D7, P1_2,

    D7, P1_2, C7, P1 + P1_2, C7, P1_2, D7, P1_2, D7, P1_2, C7, P1_2,

    C7, P1_2, B6, P1_2 + P1, B6, P1_2, C7, P1_2, D7, P1_2, F7, P1_2,

    F7, P1_2, E7, P1_2 + P1, E7, P1_2, D7, P1_2, C7, P1_2, D7, P1_2,

    C7, P1_2, B6, P1 + P1_2, REST, P1, D7, P1_2, D7, P1_2,

    D7, P1_2, E7, P1 + P1_2, E7, P1_2, D7, P1_2, C7, P1_2, D7, P1_2,

    C7, P1_2, B6, P1, G6, P1_2, B6, P1_2, C7, P1_4, C7, P1_4 + P1,

    REST, P1, E6, P1_2, A6, P1_2, A6, P1_2, B6, P1_2, C7, P1_2,
    D7, P1_2,

    C7, P1_4, B6, P1_2, B6, P1_4 + P1, E7, P1_2, D7, P1_2, C7, P1_2,
    D7, P1_2,

    D7, P1_4, C7, P1_2, C7, P1_4 + P2, REST, P1,
};

static const melody_t main2_melodies[] =
{
    { main2_m1, sizeof(main2_m1) / sizeof(u8) },
};

static const music_t music_shanhu_hai_main2 =
{
    .melodies = main2_melodies,
    .melody_num = sizeof(main2_melodies) / sizeof(melody_t),
};

const music_3v_song_t m3v_ShanhuHai =
{
    .name = "ShanhuHai",
    .bpm = 80,
    .m1 = &music_shanhu_hai_main1,
    .m2 = &music_shanhu_hai_main2,
    .m3 = NULL,
};
