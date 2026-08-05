
#include "light.h"
#include "debug.h"
#include "app_music.h"

#define TAG "light"

void light_task(void)
{
    // LOGI(TAG, "light task running...");
    if (app_music_is_playing()) {
        LED0_Toggle();
    }
    else {
        LED0_Off();
    }
}
