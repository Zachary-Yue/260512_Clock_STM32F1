
#pragma once

#include "main.h"
#include "music.h"

typedef enum music_3v_state_t
{
    MUSIC_3V_STOP,
    MUSIC_3V_PLAY,
    MUSIC_3V_PAUSE,
} music_3v_state_t;

typedef enum music_3v_start_status_t
{
    MUSIC_3V_Start_OK,
    MUSIC_3V_Start_ERROR_NUM,
    MUSIC_3V_Start_ERROR_PARAM,
} music_3v_start_status_t;

/**
 * @brief Music list elements, provide information of a multi-melody music.
 * 
 */
typedef struct music_3v_song_t
{
    const music_t *m1;  // melody 1
    const music_t *m2;  // melody 2
    const music_t *m3;  // melody 3
    const char *name;   // music information
    u8 bpm;             // speed of music
} music_3v_song_t;

typedef struct music_3v_ctrl_t
{
    music_ctrl_t ctrl[3];          // music play control x3
    void (*set_bpm)(u8 bpm);       // function to set bpm of music
    const music_3v_song_t **list;  // music list
    u8 list_size;                  // number of songs
    u8 pnum;                       // position of current music
    u8 state;                      // music state
} music_3v_ctrl_t;

typedef struct music_3v_ctrl_cfg_t {
    struct {
        void (*set_note)(u8 pitch);
    } ctrl_opr[3];
    // music list of 3-voices music
    void (*set_bpm)(u8 bpm);       // function to set bpm of music
    const music_3v_song_t **list;
    // number of songs in list
    u8 list_size;
} music_3v_ctrl_cfg_t;


/*---------------------------------------- Exported Functions --------------------------------------*/

bool music_3v_init(music_3v_ctrl_t *ctrl, music_3v_ctrl_cfg_t *cfg);
void music_3v_task(music_3v_ctrl_t *ctrl);

music_3v_start_status_t music_3v_start(music_3v_ctrl_t *ctrl, u8 number);
bool music_3v_stop(music_3v_ctrl_t *ctrl);
bool music_3v_pause(music_3v_ctrl_t *ctrl);
bool music_3v_resume(music_3v_ctrl_t *ctrl);
