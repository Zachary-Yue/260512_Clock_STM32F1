
#include "GrainInEar.h"

#include "Star.h"

/* 《芒种》- 音阙诗听 */

static const u8 M1[] = {
    C5, P1,       REST, P1_2,   C5, P1_4,    C5, P1_4,
    REST, P1_4,   C5, P1_4,    C5, P1_2,    C5, P1_2,
    C5, P1_2,

    D5, P1,       REST, P1_2,   D5, P1_4,    D5, P1_4,
    REST, P1_4,   D5, P1_4,    D5, P1_2,    D5, P1_2,
    D5, P1_2,

    G5, P1,       REST, P1_2,   G5, P1_4,    G5, P1_4,
    REST, P1_4,   G5, P1_4,    G5, P1_2,    G5, P1_2,
    E5, P1_2,

    F5, P1 + P1_2, E5, P1_4,   F5, P1_4,    G5, P1,
    B5, P1
};

static const u8 M2[] = {
    G5, P1_4, E5, P1_4, E5, P1_4, G5, P1_4,
    E5, P1_4, E5, P1_4, G5, P1_4, E5, P1_4,
    E5, P1_4, G5, P1_4, E5, P1_4, E5, P1_4,
    G5, P1_4, E5, P1_4, G5, P1_4, A5, P1_4,

    C6, P1_4, C6, P1_4, G5, P1_4, C6, P1_4,
    C6, P1_4, G5, P1_4, C6, P1_4, C6, P1_4,
    G5, P1_4, C6, P1_4, C6, P1_4, G5, P1_4,
    C6, P1_4, G5, P1_4, C6, P1_4, E6, P1_4,

    D6, P1_4, D6, P1_4, G5, P1_4, E6, P1_4,
    E6, P1_4, G5, P1_4, D6, P1_4, D6, P1_4,
    G5, P1_4, E6, P1_4, E6, P1_4, G5, P1_4,
    D6, P1_4, G5, P1_4, E6, P1_4, G5, P1_4,

    D6, P1_4, C6, P1_4, B5, P1_4, A5, P1_4,
    B5, P1_4, A5, P1_4, G5, P1_4, D5, P1_4,
};

static const u8 M3[] = {
    G5, P1_4, D5, P1_4, B4, P1_4, A4, P1_4,
    B4, P1_2, B4, P1_4, D5, P1_4,
    C5, P1_4, REST, P1_4, G5, P1_4, G5, P1_8, G5, P1_8,
};

static const u8 M4[] = {
    E5, P1_4, REST, P1_4, G5, P1_4, G5, P1_8, G5, P1_8,
};

static const u8 M5[] = {
    F5, P1_4, REST, P1_4, G5, P1_4, G5, P1_8, G5, P1_8,
};

static const u8 M6[] = {
    C6, P1_2, G5, P1_2, C6, P1_2, G5, P1_2,
    D6, P1_2, G5, P1_2, E6, P1_2, G5, P1_2,
    F6, P1_2, G5, P1_2, G6, P1_2, G5, P1_2,
    F6, P3_16, G6, P1_8, F6, P1_8, E6, P3_16,
    F6, P1_8, E6, P1_8, D6, P3_16, E6, P1_8,
    D6, P1_8, C6, P1_4 + P1_16, REST, P1_4,
};

static const u8 M7[] = {
    B4, P2,
};

static const melody_t melodies[] = {
    { M1, sizeof(M1) / sizeof(u8) },
    { M2, sizeof(M2) / sizeof(u8) },
    { M3, sizeof(M3) / sizeof(u8) },
    { M4, sizeof(M4) / sizeof(u8) },
    { M4, sizeof(M4) / sizeof(u8) },
    { M4, sizeof(M4) / sizeof(u8) },
    { M5, sizeof(M5) / sizeof(u8) },
    { M5, sizeof(M5) / sizeof(u8) },
    { M5, sizeof(M5) / sizeof(u8) },
    { M5, sizeof(M5) / sizeof(u8) },
    { M6, sizeof(M6) / sizeof(u8) },
    { M2, sizeof(M2) / sizeof(u8) },
    { M7, sizeof(M7) / sizeof(u8) },
};

static const music_t music1 = {
    .melodies = melodies,
    .melody_num = sizeof(melodies) / sizeof(melody_t),
};

const music_3v_song_t m3v_GrainInEar = {
    .name = "GrainInEar",
    .bpm = 112, // 请根据原曲实际BPM调整
    .m1 = &music1,
    .m2 = NULL,
    .m3 = NULL,
};
