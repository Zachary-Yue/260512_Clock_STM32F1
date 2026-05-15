
#include "light.h"
#include "debug.h"
#include "app_music.h"

#define TAG "light"

#define LED0_Pinout     PCout(13)
#define LED0_On()       (LED0_Pinout = Bit_RESET)
#define LED0_Off()      (LED0_Pinout = Bit_SET)
#define LED0_Toggle()   (LED0_Pinout ^= Bit_SET)

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
