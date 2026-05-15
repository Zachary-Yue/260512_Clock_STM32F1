
#include "Star.h"

static const u8 m1[] = {
    C5, P1, C5, P1, G5, P1, G5, P1, A5, P1, A5, P1, G5, P2,
    F5, P1, F5, P1, E5, P1, E5, P1, D5, P1, D5, P1, C5, P2,
};

static const u8 m2[] = {
    G5, P1, G5, P1, F5, P1, F5, P1, E5, P1, E5, P1, D5, P2,
};

static const melody_t melodies[] = {
    { m1, sizeof(m1) / sizeof(u8) },
    { m2, sizeof(m2) / sizeof(u8) },
    { m2, sizeof(m2) / sizeof(u8) },
    { m1, sizeof(m1) / sizeof(u8) },
};

static const music_t music1 = {
    .melodies = melodies,
    .melody_num = sizeof(melodies) / sizeof(melody_t),
};

const music_3v_song_t m3v_twinkle_star = {
    .name = "Star",
    .bpm = 120,
    .m1 = &music1,
    .m2 = NULL,
    .m3 = NULL,
};
