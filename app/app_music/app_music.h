
#include "music_3v.h"

extern music_3v_ctrl_t ctrl;

#define app_music_play(num) music_3v_start(&ctrl, num)
#define app_music_stop() music_3v_stop(&ctrl)
#define app_music_pause() music_3v_pause(&ctrl)
#define app_music_resume() music_3v_resume(&ctrl)

#define app_music_task() music_3v_task(&ctrl)

#define app_music_is_playing() (ctrl.state == MUSIC_3V_PLAY)

void app_music_init(void);
