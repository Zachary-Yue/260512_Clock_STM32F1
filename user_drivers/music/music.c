
#include "music.h"
#include "sys_err.h"
#include <string.h>
#include <ctype.h>

/**
 * @brief Initialize music control struct with given config, set music to NULL,
 *        period_ptr and note_ptr to 0, note_time and note_time_cnt to 0, state to Music_STOP.
 * 
 * @param ctrl music control struct pointer
 * @param config music config struct pointer
 */
void music_ctrl_init(music_ctrl_t *ctrl, music_ctrl_cfg_t *config)
{
    CHECK_FALSE_RET_VOID(ctrl && config);
    CHECK_FALSE_RET_VOID(config->opr.set_note);

    ctrl->music = NULL;
    ctrl->melody_ptr = 0;
    ctrl->note_ptr = 0;
    ctrl->note_time = 0;
    ctrl->note_time_cnt = 0;
    ctrl->state = Music_STOP;
    ctrl->opr = config->opr;

    ctrl->opr.set_note(REST); // play a rest note to stop sound immediately
}

#define curr_melody(ctrl) ((ctrl)->music->melodies[(ctrl)->melody_ptr])
#define curr_note(ctrl) (curr_melody(ctrl).melody[(ctrl)->note_ptr])
#define curr_dur(ctrl) (curr_melody(ctrl).melody[(ctrl)->note_ptr + 1])

/**
 * @brief Music task, should be called periodically to update music state and play notes.
 * 
 * @param ctrl music control struct pointer
 */
void music_task(music_ctrl_t *ctrl)
{
    CHECK_FALSE_RET_VOID(ctrl);
    CHECK_FALSE_RET_VOID(ctrl->state == Music_PLAY && ctrl->music);

    if (ctrl->note_time_cnt == ctrl->note_time - 1 && ctrl->note_time > 2 * TIME_TO_TICK_FACTOR) {
        // play a rest note for at least 2 ticks to separate notes
        ctrl->opr.set_note(REST);
    }
    else if (ctrl->note_time_cnt >= ctrl->note_time) {
        // time to play next note
        ctrl->note_time_cnt = 0;
        
        while (true) {
            if (ctrl->note_ptr + 1 >= curr_melody(ctrl).melody_size) {
                // no more note in this melody, move to next melody
                ctrl->melody_ptr++;
                ctrl->note_ptr = 0;
                if (ctrl->melody_ptr >= ctrl->music->melody_num) {
                    // no more melody, stop music
                    ctrl->state = Music_STOP;
                    // play a rest note to stop sound immediately
                    ctrl->opr.set_note(REST);
                    return;
                }
            } else {
                // play this note
                ctrl->opr.set_note(curr_note(ctrl));
                // calculate time for this note
                ctrl->note_time = curr_dur(ctrl) * TIME_TO_TICK_FACTOR;
                // move to next note
                ctrl->note_ptr += 2;
                break;
            }
        }
    }

    ctrl->note_time_cnt++;
}

/**
 * @brief Play given music, if music is NULL, stop current music.
 * 
 * @param ctrl music control struct pointer
 * @param music music to play
 * @return true if play successfully, false if ctrl is NULL or music is NULL
 * @return false if ctrl is NULL or music is NULL
 */
bool music_play(music_ctrl_t *ctrl, const music_t *music)
{
    CHECK_FALSE_RET(ctrl && music, false);
    ctrl->note_ptr = 0;
    ctrl->melody_ptr = 0;
    ctrl->note_time = 0;
    ctrl->note_time_cnt = 0;
    ctrl->music = music;
    ctrl->state = Music_PLAY;
    return true;
}

/**
 * @brief Stop current music, set state to Music_STOP and play a rest note to stop sound immediately.
 * 
 * @param ctrl music control struct pointer
 * @return true if stop successfully, false if ctrl is NULL
 * @return false if ctrl is NULL
 */
bool music_stop(music_ctrl_t *ctrl)
{
    CHECK_FALSE_RET(ctrl, false);
    ctrl->opr.set_note(REST);
    ctrl->state = Music_STOP;
    return true;
}

/**
 * @brief Pause current music, set state to Music_PAUSE.
 * 
 * @param ctrl music control struct pointer
 * @return true if pause successfully, false if ctrl is NULL or current state is not Music_PLAY
 * @return false if ctrl is NULL or current state is not Music_PLAY
 */
bool music_pause(music_ctrl_t *ctrl)
{
    CHECK_FALSE_RET(ctrl, false);
    CHECK_FALSE_RET(ctrl->state == Music_PLAY, false);
    ctrl->opr.set_note(REST);
    ctrl->state = Music_PAUSE;
    return true;
}

/**
 * @brief Resume current music, set state to Music_PLAY.
 * 
 * @param ctrl music control struct pointer
 * @return true if resume successfully, false if ctrl is NULL or current state is not Music_PAUSE
 * @return false if ctrl is NULL or current state is not Music_PAUSE
 */
bool music_resume(music_ctrl_t *ctrl)
{
    CHECK_FALSE_RET(ctrl, false);
    CHECK_FALSE_RET(ctrl->state == Music_PAUSE, false);
    ctrl->state = Music_PLAY;
    return true;
}
