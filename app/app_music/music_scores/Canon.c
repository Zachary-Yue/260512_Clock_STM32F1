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

/*---------------------------- Main ----------------------------*/

static const u8 main_1[] = {
    E6, P2, D6, P2, C6, P2, B5, P2, A5, P2, G5, P2, A5, P2, B5, P2,
    G6, P2, G6, P2, E6, P2, E6, P2, C6, P2, C6, P2, C6, P2, D6, P2,
    G6, P2, G6, P1, F6, P1, C7, P2, B6, P1, G6, P1,
    A6, P2, G6, P1, E6, P1, F6, P2, D6, P1, F6, P1,
    C7, P1_2, B6, P1_2, C7, P1_2, E6, P1_2, G6, P1, B6, P1,
    C7, P1, E7, P1, G7, P1_2, E7, P1_2, G7, P1_2, A7, P1_2,
    F7, P1_2, E7, P1_2, D7, P1_2, F7, P1_2, E7, P1_2, D7, P1_2, C7, P1_2, B6, P1_2,
    A6, P1_2, F6, P1_2, C7, P1, B6, P1_2, G6, P1_2, C7, P1_2, B6, P1_2,
    C7, P1_2, B6, P1_2, C7, P1_2, E6, P1_2, G6, P1, B6, P1,
    C7, P1, E7, P1, G7, P1_2, E7, P1_2, G7, P1_2, A7, P1_2,
    F7, P1_2, E7, P1_2, D7, P1_2, F7, P1_2, E7, P1_2, D7, P1_2, C7, P1_2, B6, P1_2,
    A6, P1_2, F6, P1_2, C7, P1, B6, P1_2, G6, P1_2, C7, P1_2, B6, P1_2,
    C7, P1_2, B6, P1_2, C7, P1_2, E6, P1_2, G6, P1, B6, P1,
    C7, P1, E7, P1_2, C7, P1_2, G7, P1_2, E7, P1_2, G7, P1_2, A7, P1_2,
    F7, P1_2, E7, P1_2, D7, P1_2, F7, P1_2, E7, P1_2, D7, P1_2, C7, P1_2, B6, P1_2,
    A6, P1_2, G6, P1_2, F6, P1, C7, P3_4, B6, P1_4, B6, P1_4, C7, P1_4, D7, P1_4, G6, P1_4,
    E7, P3_4, G6, P1_4, E7, P1_4, D7, P1_4, C7, P1_4, D7, P1_4 + P3_4, E7, P1_4, F7, P1_4, E7, P1_4, D7, P1_4, E7, P1_4,
    C7, P3_4, C7, P1_4 + P1_2, B6, P1_4, C7, P1_4, B6, P1_2, G6, P1_4, E6, P1_4 + P1_2, G6, P1_2,
    A6, P1, B6, P1_2, C7, P1_2, G6, P1, E6, P1_2, G6, P1_2,
    F6, P1 + P1_4, A6, P1_4, C7, P1_2 + P3_4, B6, P1_4 + P1_4, C7, P1_4, D7, P1_4, G6, P1_4,
};

static const u8 main_2[] = {
    G7, P1_2, E7, P1_4, F7, P1_4, G7, P1_2, E7, P1_4, F7, P1_4, G7, P1_4, B6, P1_4, A6, P1_4, B6, P1_4, C7, P1_4, D7, P1_4, E7, P1_4, F7, P1_4,
    E7, P1_2, C7, P1_4, D7, P1_4, E7, P1_2, E6, P1_4, F6, P1_4, G6, P1_4, A6, P1_4, G6, P1_4, F6, P1_4, G6, P1_4, C7, P1_4, B6, P1_4, C7, P1_4,
    A6, P1_2, C7, P1_4, B6, P1_4, A6, P1_2, G6, P1_4, F6, P1_4, G6, P1_4, F6, P1_4, E6, P1_4, F6, P1_4, G6, P1_4, A6, P1_4, B6, P1_4, C7, P1_4,
    A6, P1_2, C7, P1_4, B6, P1_4, C7, P1_2, B6, P1_4, C7, P1_4, B6, P1_4, A6, P1_4, B6, P1_4, C7, P1_4, D7, P1_4, E7, P1_4, F7, P1_4, G7, P1_4,
};


static const melody_t main[] = {
    {main_1, sizeof(main_1)},
    {main_2, sizeof(main_2)},
    {main_2, sizeof(main_2)},
};

static const music_t music_main = {
    main,
    sizeof(main) / sizeof(melody_t),
};

/*---------------------------- C1 ----------------------------*/

/*---------------------------- C2 ----------------------------*/

/*---------------------------- Music ----------------------------*/

const music_3v_song_t m3v_CANON = {
    .name = "Canon",
    .bpm = 120,
    .m1 = &music_main,
    .m2 = NULL,
    .m3 = NULL,
};
