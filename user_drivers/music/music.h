
#pragma once

#include "main.h"

#define TIME_TO_TICK_FACTOR (5) // 一个四分音符对应 64/4=16 个时钟周期，再乘以 5 就是任务经历的 tick 数

// 音符，不可相加
typedef enum Pitch_t
{
        bD4, D4, bE4, E4, F4, bG4, G4, bA4, A4, bB4, B4,
    C5, bD5, D5, bE5, E5, F5, bG5, G5, bA5, A5, bB5, B5,
    C6, bD6, D6, bE6, E6, F6, bG6, G6, bA6, A6, bB6, B6,
    C7, bD7, D7, bE7, E7, F7, bG7, G7, bA7, A7, bB7, B7,
    C8, bD8, D8, bE8, E8, F8, bG8, G8, bA8, A8, bB8, B8,
    C9, bD9, D9, bE9, E9, F9, bG9, G9, bA9, A9, bB9, B9,
    REST,
} Pitch_t;

// 音符时长记号，可以相加。
// P1 表示 1 拍（（四分音符），P2 表示 2 拍（（二分音符），P1_2 表示 1/2 拍（八分音符）。以此类推
typedef enum Pulse_t
{
    P1_16  = (u8)1,     // 六十四分音符
    P1_8   = (u8)2,     // 三十二分音符
    P3_16  = (u8)3,     // 附点三十二分音符
    P1_4   = (u8)4,     // 十六分音符
    P3_8   = (u8)6,     // 附点十六分音符
    P1_2   = (u8)8,     // 八分音符
    P3_4   = (u8)12,    // 附点八分音符
    P1     = (u8)16,    // 四分音符
    P3_2   = (u8)24,    // 附点四分音符
    P2     = (u8)32,    // 二分音符
    P3     = (u8)48,    // 附点二分音符
    P4     = (u8)64,    // 全音符
    P8     = (u8)128,   // 二倍全音符
    PMAX   = (u8)255,   // 不能再大了
} Pulse_t;

typedef struct melody_t {
    const u8 *melody;
    const u16 melody_size; // 论 byte 的每段音乐的长度，正常是音符数量的两倍
} melody_t;

typedef struct music_t {
    const melody_t *melodies;
    const u16 melody_num; // 有几段 melody
} music_t;

typedef enum music_state_t
{
    Music_STOP,
    Music_PAUSE,
    Music_PLAY
} music_state_t;

typedef struct music_opr_t {
    void (*set_note)(u8 pitch);
} music_opr_t;

typedef struct music_ctrl_t {
    const music_t *music;
    music_opr_t opr;
    music_state_t state;
    u16 melody_ptr;
    u16 note_ptr;
    u16 note_time;
    u16 note_time_cnt;
} music_ctrl_t;

typedef struct music_ctrl_cfg_t {
    music_opr_t opr;
} music_ctrl_cfg_t;

void music_ctrl_init(music_ctrl_t *ctrl, music_ctrl_cfg_t *config);
void music_task(music_ctrl_t *ctrl);

bool music_play(music_ctrl_t *ctrl, const music_t *music);
bool music_stop(music_ctrl_t *ctrl);
bool music_pause(music_ctrl_t *ctrl);
bool music_resume(music_ctrl_t *ctrl);
