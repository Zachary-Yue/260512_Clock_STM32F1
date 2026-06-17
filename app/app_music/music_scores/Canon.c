/**
 * @file Canon.c
 * @author Zach (zachary-yue@qq.com)
 * 
 * @brief 1HH = C8, 1H = C7, 1 = C6, 1L = C5, 1LL = C4
 * 
 * @version 0.1
 * @date 2026-05-17
 * 
 */
#include "Canon.h"

/* Canon in D */

/*---------------------------- Main ----------------------------*/

static const u8 main_1[] = {
    // 1
    E6, P2, D6, P2, C6, P2, B5, P2, A5, P2, G5, P2, A5, P2, B5, P2,

    // 5
    G6, P2, G6, P2, E6, P2, E6, P2, C6, P2, C6, P2, C6, P2, D6, P2,

    // 9
    G6, P2, G6, P1, F6, P1, C7, P2, B6, P1, G6, P1,
    A6, P2, G6, P1, E6, P1, F6, P2, D6, P1, F6, P1,

    // 13
    C7, P1_2, B6, P1_2, C7, P1_2, E6, P1_2, G6, P1, B6, P1,
    C7, P1, E7, P1, G7, P1_2, E7, P1_2, G7, P1_2, A7, P1_2,
    F7, P1_2, E7, P1_2, D7, P1_2, F7, P1_2, E7, P1_2, D7, P1_2, C7, P1_2, B6, P1_2,
    A6, P1_2, F6, P1_2, C7, P1, B6, P1_2, G6, P1_2, C7, P1_2, B6, P1_2,

    // 17
    C7, P1_2, B6, P1_2, C7, P1_2, E6, P1_2, G6, P1, B6, P1,
    C7, P1, E7, P1, G7, P1_2, E7, P1_2, G7, P1_2, A7, P1_2,
    F7, P1_2, E7, P1_2, D7, P1_2, F7, P1_2, E7, P1_2, D7, P1_2, C7, P1_2, B6, P1_2,
    A6, P1_2, G6, P1_2, F6, P1, C7, P3_4, G6, P1_4, B6, P1_2, D7, P1_4, G6, P1_4,

    // 21
    E7, P3_4, G6, P1_4, E7, P1_4, D7, P1_4, C7, P1_4, D7, P1_4 + P3_4, E7, P1_4, F7, P1_4, E7, P1_4, D7, P1_4, E7, P1_4,
    C7, P3_4, C7, P1_4 + P1_2, B6, P1_4, C7, P1_4, B6, P1_2, G6, P1_4, E6, P1_4 + P1_2, G6, P1_2,
    A6, P1, B6, P1_2, C7, P1_2, G6, P1, E6, P1_2, G6, P1_2,
    F6, P1, F6, P1_4, A6, P1_4, C7, P1_2, C7, P3_4, B6, P1_4, B6, P1_4, C7, P1_4, D7, P1_4, G6, P1_4,

    // 25
    E7, P3_4, G6, P1_4, E7, P1_4, D7, P1_4, C7, P1_4, D7, P1_4 + P3_4, E7, P1_4, F7, P1_4, E7, P1_4, D7, P1_4, E7, P1_4,
    C7, P3_4, C7, P1_4 + P1_2, B6, P1_4, C7, P1_4, B6, P1_2, G6, P1_4, E6, P1_4 + P1_2, G6, P1_2,
    A6, P1, B6, P1_2, C7, P1_2, G6, P1, E6, P1_2, G6, P1_2,
    F6, P1, F6, P1_4, A6, P1_4, C7, P1_2, C7, P3_4, B6, P1_4, B6, P1_4, C7, P1_4, D7, P1_4, G6, P1_4,
};

static const u8 main_2[] = {
    // 29
    G7, P1_2, E7, P1_4, F7, P1_4, G7, P1_2, E7, P1_4, F7, P1_4, G7, P1_4, G6, P1_4, A6, P1_4, B6, P1_4, C7, P1_4, D7, P1_4, E7, P1_4, F7, P1_4,
    E7, P1_2, C7, P1_4, D7, P1_4, E7, P1_2, E6, P1_4, F6, P1_4, G6, P1_4, A6, P1_4, G6, P1_4, F6, P1_4, G6, P1_4, C7, P1_4, B6, P1_4, C7, P1_4,
    A6, P1_2, C7, P1_4, B6, P1_4, A6, P1_2, G6, P1_4, F6, P1_4, G6, P1_4, F6, P1_4, E6, P1_4, F6, P1_4, G6, P1_4, A6, P1_4, B6, P1_4, C7, P1_4,
    A6, P1_2, C7, P1_4, B6, P1_4, C7, P1_2, B6, P1_4, C7, P1_4, B6, P1_4, A6, P1_4, B6, P1_4, C7, P1_4, D7, P1_4, E7, P1_4, F7, P1_4, G7, P1_4,
};

static const u8 main_3[] = {
    // 37
    E7, P1_2, C7, P1_4, D7, P1_4, E7, P1_2, D7, P1_4, C7, P1_4, D7, P1_4, B6, P1_4, C7, P1_4, D7, P1_4, E7, P1_4, D7, P1_4, C7, P1_4, B7, P1_4,
    C7, P1_2, A6, P1_4, B6, P1_4, C7, P1_2, C6, P1_4, D6, P1_4, E6, P1_4, F6, P1_4, E6, P1_4, D6, P1_4, E6, P1_4, C7, P1_4, B6, P1_4, C7, P1_4,
    A6, P1_2, C7, P1_4, B6, P1_4, A6, P1_2, G6, P1_4, F6, P1_4, G6, P1_4, F6, P1_4, E6, P1_4, F6, P1_4, G6, P1_4, A6, P1_4, B6, P1_4, C7, P1_4,
    A6, P1_2, C7, P1_4, B6, P1_4, C7, P1_2, B6, P1_4, A6, P1_4, B6, P1_4, C7, P1_4, D7, P1_4, C7, P1_4, B6, P1_4, C7, P1_4, A6, P1_4, B6, P1_4,
};

static const u8 main_4[] = {
    // 41
    C7, P4, REST, P4, REST, P8,

    // 45
    REST, P1_2, E7, P1_4, F7, P1_4, G7, P1_2, E7, P1_2, REST, P1_2, D7, P1_4, E7, P1_4, F7, P1_2, D7, P1_2,
    REST, P1_2, C7, P1_4, D7, P1_4, E7, P1_2, C7, P1_2, REST, P1_2, E7, P1_4, D7, P1_4, C7, P1_2, B6, P1_2,
    REST, P1_2, A6, P1_4, B6, P1_4, C7, P1_2, A6, P1_2, REST, P1_2, G6, P1_4, A6, P1_4, C7, P1_2, G6, P1_2,
    REST, P1_2, A6, P1_4, B6, P1_4, C7, P1_2, B6, P1_4, A6, P1_4,
    REST, P1_4, G6, P1_4, A6, P1_4, B6, P1_4, D7, P1_2, B6, P1_2,

    // 49
    REST, P1_2, E7, P1_4, F7, P1_4, G7, P1_2, E7, P1_2, REST, P1_2, D7, P1_4, E7, P1_4, F7, P1_2, D7, P1_2,
    REST, P1_4, A6, P1_4, C7, P1_4, D7, P1_4, E7, P1_2, C7, P1_2, REST, P1_2, G7, P1_4, F7, P1_4, E7, P1_2, G7, P1_2,
    A7, P1_2, A7, P1_4, G7, P1_4, F7, P1_2, A7, P1_2, G7, P1_2, G7, P1_4, F7, P1_4, E7, P1_2, G7, P1_2,
    A7, P1_4, G7, P1_4, F7, P1_4, A7, P1_4,  G7, P1_4, F7, P1_4, A7, P1_4, G7, P1_4,
    B7, P1_4, A7, P1_4, G7, P1_2,  B6, P1_4, C7, P1_4, D7, P1_4, G6, P1_4,
};

static const u8 main_5[] = {
    // 53
    G6, P1,  E7, P1_4, D7, P1_4, C7, P1_4, D7, P1_4 + P3_4,
    E7, P1_4+P1_4, F7, P1_4, D7, P1_4, E7, P1_4,
    D7, P3_4, C7, P1_4 + P1_2, B6, P1_4, C7, P1_4,
    E6, P1_4, G6, P1_4, B6, P1_4, C7, P1_4,
    E7, P1_4, G7, P1_4, B7, P1_4, C8, P1_4,

    B7, P1_4, A7, P1_4, G7, P1_4, F7, P1_4,
    G7, P1_4, F7, P1_4, E7, P1_4, D7, P1_4,
    E7, P1_4, D7, P1_4, C7, P1_4, B6, P1_4,
    C7, P1_4, B6, P1_4, A6, P1_4, G6, P1_4,

    A6, P1_4, G6, P1_4, F6, P1_4, G6, P1_4,
    A6, P1_4, F6, P1_4, A6, P1_4, C7, P1_4,
    B6, P1_4, A6, P1_4, G6, P1_4, C7, P1_4,
    C7, P1_4, G6, P1_4, D7, P1_4, G6, P1_4,
};

static const u8 main_5_1[] = {
    // 57
    C7, P1, E7, P1_2, F7, P1_2, G7, P1_2, A7, P1_2, G7, P1_2, F7, P1_2,
    C7, P1, C7, P1_2, D7, P1_2, E7, P1_2, F7, P1_2, E7, P1_2, D7, P1_2,
    F6, P1, A6, P1_4, B6, P1_4, C7, P1_4, C7, P1_4,
    C7, P1_4, C7, P1_2, C7, P1_4, C7, P1_4, C7, P1_4, B6, P1_4, C7, P1_4,
    F6, P1, F6, P1_4, A6, P1_4, C7, P1_4, D7, P1_4,
    C7, P1_4, B6, P1_2, B6, P1_4, B6, P1_4, C7, P1_4, D7, P1_4, G6, P1_4,
};

static const u8 main_6[] = {
    // 65
    E7, P2, D7, P2, C7, P2, B6, P2, A6, P2, G6, P2, A6, P2, B6, P2,
};

static const u8 main_7[] = {
    // 69
    E6, P4,
};


static const melody_t main[] = {
    {main_1, sizeof(main_1)},
    {main_2, sizeof(main_2)},
    {main_2, sizeof(main_2)},
    {main_3, sizeof(main_3)},
    {main_4, sizeof(main_4)},
    {main_5, sizeof(main_5)},
    {main_5_1, sizeof(main_5_1)},
    {main_5_1, sizeof(main_5_1)},
    {main_6, sizeof(main_6)},
    {main_6, sizeof(main_6)},
    {main_7, sizeof(main_7)},
};

static const music_t music_main = {
    main,
    sizeof(main) / sizeof(melody_t),
};

/*---------------------------- C1 ----------------------------*/

static const u8 c1_1[] = {
    // 1
    C5, P2, G4, P2, A4, P2, E4, P2,  F4, P2, G4, P2, F4, P2, G4, P2,
    C5, P2, G4, P2, A4, P2, E4, P2,  F4, P2, C5, P2, F4, P2, G4, P2,
};

static const u8 c1_2[] = {
    // 9
    C5, P1_2, G5, P1_2, C6, P1_2, E6, P1_2,  B4, P1_2, D5, P1_2, G5, P1_2, B5, P1_2,
};

static const u8 c1_3[] = {
    // 10
    A4, P1_2, E5, P1_2, A5, P1_2, C6, P1_2,  E4, P1_2, E5, P1_2, G5, P1_2, B5, P1_2,
    F4, P1_2, C5, P1_2, F5, P1_2, A5, P1_2,  C5, P1_2, E5, P1_2, G5, P1_2, C6, P1_2,
    F4, P1_2, C5, P1_2, F5, P1_2, A5, P1_2,  G4, P1_2, D5, P1_2, G5, P1_2, B5, P1_2,
};

static const u8 c1_4[] = {
    // 13
    C5, P1_2, E5, P1_2, G5, P1_2, C6, P1_2,  G4, P1_2, D5, P1_2, G5, P1_2, B5, P1_2,
};

static const u8 c1_2_1[] = {
    // 21
    C5, P1_2, E5, P1_2, G5, P1_2, C6, P1_2,  B4, P1_2, D5, P1_2, G5, P1_2, B5, P1_2,
};

static const u8 c1_5[] = {
    // 29
    C5, P1_2, G5, P1_2, C6, P1,  G4, P1_2, D5, P1_2, B5, P1,
    A4, P1_2, E5, P1_2, C6, P1,  E4, P1_2, E5, P1_2, G5, P1,
    F4, P1_2, C5, P1_2, A5, P1,  C5, P1_2, G5, P1_2, C6, P1,
    F4, P1_2, C5, P1_2, A5, P1,  F4, P1_2, D5, P1_2, B5, P1,
};

static const u8 c1_6[] = {
    // 25
    C5, P4,
};


static const melody_t c1[] = {
    {c1_1, sizeof(c1_1)},

    {c1_2, sizeof(c1_2)},
    {c1_3, sizeof(c1_3)},
    {c1_4, sizeof(c1_4)},
    {c1_3, sizeof(c1_3)},
    {c1_4, sizeof(c1_4)},
    {c1_3, sizeof(c1_3)},
    {c1_2_1, sizeof(c1_2_1)},
    {c1_3, sizeof(c1_3)},

    {c1_4, sizeof(c1_4)}, // 25
    {c1_3, sizeof(c1_3)},
    {c1_5, sizeof(c1_5)}, // 29
    {c1_5, sizeof(c1_5)}, // 33
    {c1_5, sizeof(c1_5)}, // 37

    {c1_2, sizeof(c1_2)}, // 41
    {c1_3, sizeof(c1_3)},
    {c1_4, sizeof(c1_4)}, // 45
    {c1_3, sizeof(c1_3)},
    {c1_4, sizeof(c1_4)}, // 49
    {c1_3, sizeof(c1_3)},
    {c1_4, sizeof(c1_4)}, // 53
    {c1_3, sizeof(c1_3)},
    {c1_4, sizeof(c1_4)}, // 57
    {c1_3, sizeof(c1_3)},
    {c1_4, sizeof(c1_4)}, // 61
    {c1_3, sizeof(c1_3)},
    {c1_2, sizeof(c1_2)}, // 65
    {c1_3, sizeof(c1_3)},
    {c1_2, sizeof(c1_2)}, // 69
    {c1_3, sizeof(c1_3)},

    {c1_6, sizeof(c1_6)}, // 73
};

static const music_t music_c1 = {
    .melodies = c1,
    .melody_num = sizeof(c1) / sizeof(melody_t),
};

/*---------------------------- C2 ----------------------------*/

static const u8 c2_1[] = {
    // 1
    REST, P8, REST, P8,
    // 5
    E7, P2, D7, P2, C7, P2, B6, P2, A6, P2, G6, P2, A6, P2, B6, P2,
    E7, P2, D7, P2, REST, P8, A6, P2, G6, P2,
    // 13
    REST, P8, REST, P8, REST, P8, REST, P8,
    // 21
    REST, P8, REST, P4, A6, P1, REST, P3,
    // 25
    REST, P8, REST, P4, A6, P1, REST, P3,
    // 29
    REST, P8, REST, P8, REST, P8, REST, P8,
    REST, P8, REST, P8, REST, P8, REST, P8,
    REST, P8, REST, P8, REST, P8, REST, P8,
    // 53
    E7, P1, REST, P3 + P4, REST, P8,
    // 57
    E7, P1, REST, P3, E7, P1, REST, P3, C7, P1, REST, P3, A6, P1, REST, P3,
    // 61
    E7, P1, REST, P3, E7, P1, REST, P3, C7, P1, REST, P3, A6, P1, REST, P3,
    // 65
    REST, P8, REST, P8, REST, P8, REST, P8,
    // 73
    C7, P4,
};

static const melody_t c2[] = {
    {c2_1, sizeof(c2_1)},
};

static const music_t music_c2 = {
    .melodies = c2,
    .melody_num = sizeof(c2) / sizeof(melody_t),
};

/*---------------------------- Music ----------------------------*/

const music_3v_song_t m3v_CANON = {
    .name = "Canon",
    .bpm = 86,
    .m1 = &music_main,
    .m2 = &music_c1,
    .m3 = &music_c2,
};
