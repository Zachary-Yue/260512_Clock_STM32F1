
#include "music_3v.h"
#include "sys_err.h"

/**
 * @brief Read next note from current period in ctrl->music, update ctrl->note_name, ctrl->oct_num,
 * 
 * @param ctrl music control struct pointer
 */
void music_3v_task(music_3v_ctrl_t *ctrl)
{
    CHECK_FALSE_RET_VOID(ctrl);
    CHECK_FALSE_RET_VOID(ctrl->list);
    CHECK_FALSE_RET_VOID(ctrl->state == MUSIC_3V_PLAY);

    music_task(&ctrl->ctrl[0]);
    music_task(&ctrl->ctrl[1]);
    music_task(&ctrl->ctrl[2]);

    // Determine the condition of termination
    if (ctrl->ctrl[0].state == Music_STOP &&
        ctrl->ctrl[1].state == Music_STOP &&
        ctrl->ctrl[2].state == Music_STOP)
    {
        ctrl->state = MUSIC_3V_STOP;
    }
}

/**
 * @brief Initialize music control struct with given config, set music to NULL,
 * 
 * @param ctrl music control struct pointer
 * @param cfg music control config struct pointer
 * @return true if initialized successfully, false if ctrl or cfg is NULL, or cfg is invalid
 * @return false if ctrl or cfg is NULL, or cfg is invalid
 */
bool music_3v_init(music_3v_ctrl_t *ctrl, music_3v_ctrl_cfg_t *cfg)
{
    CHECK_FALSE_RET(cfg && ctrl, false);
    CHECK_FALSE_RET(cfg->list && cfg->list_size > 0, false);
    CHECK_FALSE_RET(cfg->ctrl_opr[0].set_note && cfg->ctrl_opr[1].set_note && cfg->ctrl_opr[2].set_note, false);
    CHECK_FALSE_RET(cfg->set_bpm, false);

    ctrl->list = cfg->list;
    ctrl->list_size = cfg->list_size;
    ctrl->pnum = 0;
    ctrl->state = MUSIC_3V_STOP;
    music_ctrl_cfg_t cfg1;
    for (u8 i = 0; i < 3; i++) {
        cfg1.opr.set_note = cfg->ctrl_opr[i].set_note;
        music_ctrl_init(&ctrl->ctrl[i], &cfg1);
    }
    ctrl->set_bpm = cfg->set_bpm;
    return true;
}

/**
 * @brief Start playing music with given number, if number is invalid, return error status, if music is playing, return busy status.
 * 
 * @param ctrl music control struct pointer
 * @param number music number to play, should be between 1 and ctrl->list_size
 * @return music_3v_start_status_t status of starting music
 */
music_3v_start_status_t music_3v_start(music_3v_ctrl_t *ctrl, u8 number)
{
    CHECK_FALSE_RET(ctrl, MUSIC_3V_Start_ERROR_PARAM);
    CHECK_FALSE_RET(ctrl->list, MUSIC_3V_Start_ERROR_PARAM);
    CHECK_FALSE_RET(number > 0 && number <= ctrl->list_size, MUSIC_3V_Start_ERROR_NUM);

    ctrl->pnum = number - 1;
    const music_3v_song_t *music = ctrl->list[ctrl->pnum];
    CHECK_FALSE_RET(music->m1 || music->m2 || music->m3, MUSIC_3V_Start_ERROR_PARAM);
    ctrl->set_bpm(music->bpm);
    music_stop(&ctrl->ctrl[0]);
    music_stop(&ctrl->ctrl[1]);
    music_stop(&ctrl->ctrl[2]);
    if (music->m1) music_play(&ctrl->ctrl[0], music->m1);
    if (music->m2) music_play(&ctrl->ctrl[1], music->m2);
    if (music->m3) music_play(&ctrl->ctrl[2], music->m3);
    ctrl->state = MUSIC_3V_PLAY;
    return MUSIC_3V_Start_OK;
}

/**
 * @brief Stop current music
 * 
 * @param ctrl music control struct pointer
 * @return true if stop successfully, false if ctrl is NULL
 * @return false if ctrl is NULL
 */
bool music_3v_stop(music_3v_ctrl_t *ctrl)
{
    CHECK_FALSE_RET(ctrl, false);
    music_stop(&ctrl->ctrl[0]);
    music_stop(&ctrl->ctrl[1]);
    music_stop(&ctrl->ctrl[2]);
    ctrl->state = MUSIC_3V_STOP;
    return true;
}

/**
 * @brief Pause current music
 * 
 * @param ctrl music control struct pointer
 * @return true if pause successfully, false if ctrl is NULL
 * @return false if ctrl is NULL
 */
bool music_3v_pause(music_3v_ctrl_t *ctrl)
{
    CHECK_FALSE_RET(ctrl, false);
    CHECK_FALSE_RET(ctrl->state == MUSIC_3V_PLAY, false);
    music_pause(&ctrl->ctrl[0]);
    music_pause(&ctrl->ctrl[1]);
    music_pause(&ctrl->ctrl[2]);
    ctrl->state = MUSIC_3V_PAUSE;
    return true;
}

/**
 * @brief Resume current music
 * 
 * @param ctrl music control struct pointer
 * @return true if resume successfully, false if ctrl is NULL
 * @return false if ctrl is NULL
 */
bool music_3v_resume(music_3v_ctrl_t *ctrl)
{
    CHECK_FALSE_RET(ctrl, false);
    CHECK_FALSE_RET(ctrl->state == MUSIC_3V_PAUSE, false);
    music_resume(&ctrl->ctrl[0]);
    music_resume(&ctrl->ctrl[1]);
    music_resume(&ctrl->ctrl[2]);
    ctrl->state = MUSIC_3V_PLAY;
    return true;
}
