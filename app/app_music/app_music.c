
#include "app_music.h"
#include "tim.h"
#include "freq_table.h"
#include "sys_err.h"
#include "music_scores/music_scores.h"

#define TAG "MUSIC"

#define BEAT_TIM  (TIM1)
#define BUZZ1_TIM (TIM2)
#define BUZZ2_TIM (TIM3)
#define BUZZ3_TIM (TIM4)
#define TIMx_CCMR1_OC1M_MASK        (BIT(6) | BIT(5) | BIT(4))
#define TIMx_OC_MODE_FORCED_ACTIVE  (0x5U << 4)
#define TIMx_OC_MODE_TOGGLE         (0x3U << 4)

#define Tim_OC1_ForceHigh(tim) MODIFY_REG((tim)->CCMR1, TIMx_CCMR1_OC1M_MASK, TIMx_OC_MODE_FORCED_ACTIVE)
#define Tim_OC1_Toggle(tim) MODIFY_REG((tim)->CCMR1, TIMx_CCMR1_OC1M_MASK, TIMx_OC_MODE_TOGGLE)

#define Get_ARR_bpm(sysclk, bpm) ( (u16) ((u32)(sysclk) \
    / (LL_TIM_GetPrescaler(BEAT_TIM) + 1) \
    * 60 / (P1 * TIME_TO_TICK_FACTOR * (bpm)) - 1))
#define Set_Beat_TIM_BPM(sysclk, bpm) LL_TIM_SetAutoReload(BEAT_TIM, Get_ARR_bpm(sysclk, bpm))

music_3v_ctrl_t ctrl;

static void set_bpm(u8 bpm)
{
    LL_RCC_ClocksTypeDef sysclks;
    LL_RCC_GetSystemClocksFreq(&sysclks);
    Set_Beat_TIM_BPM(sysclks.SYSCLK_Frequency, bpm);
}

static void buzz1_set_note(u8 pitch)
{
    if (pitch == REST) {
        // Force output oc channel at high level
        Tim_OC1_ForceHigh(BUZZ1_TIM);
    }
    else {
        LL_TIM_SetAutoReload(BUZZ1_TIM, Get_Freq(pitch));
        // Toggle output oc channel
        Tim_OC1_Toggle(BUZZ1_TIM);
    }
}

static void buzz2_set_note(u8 pitch)
{
    if (pitch == REST) {
        // Force output oc channel at high level
        Tim_OC1_ForceHigh(BUZZ2_TIM);
    }
    else {
        LL_TIM_SetAutoReload(BUZZ2_TIM, Get_Freq(pitch));
        // Toggle output oc channel
        Tim_OC1_Toggle(BUZZ2_TIM);
    }
}

static void buzz3_set_note(u8 pitch)
{
    if (pitch == REST) {
        // Force output oc channel at high level
        Tim_OC1_ForceHigh(BUZZ3_TIM);
    }
    else {
        LL_TIM_SetAutoReload(BUZZ3_TIM, Get_Freq(pitch));
        // Toggle output oc channel
        Tim_OC1_Toggle(BUZZ3_TIM);
    }
}

static const music_3v_song_t *music_3v_list[] = {
    &m3v_HeadToFutureTogether,
    &m3v_ING,
    &m3v_Rainbow,
    &m3v_ShanhuHai,
    &m3v_GrainInEar,
    &m3v_music_06,
    &m3v_twinkle_star,
    &m3v_CANON,
};

void app_music_init(void)
{
    LOGI(TAG, "Initializing music app...");
    music_3v_ctrl_cfg_t cfg = {
        .ctrl_opr[0].set_note = buzz1_set_note,
        .ctrl_opr[1].set_note = buzz2_set_note,
        .ctrl_opr[2].set_note = buzz3_set_note,
        .list = music_3v_list,
        .list_size = sizeof(music_3v_list) / sizeof(music_3v_song_t*),
        .set_bpm = set_bpm,
    };
    if (music_3v_init(&ctrl, &cfg)) {
        LL_TIM_CC_EnableChannel(BUZZ1_TIM, LL_TIM_CHANNEL_CH1);
        LL_TIM_CC_EnableChannel(BUZZ2_TIM, LL_TIM_CHANNEL_CH1);
        LL_TIM_CC_EnableChannel(BUZZ3_TIM, LL_TIM_CHANNEL_CH1);
        LL_TIM_EnableCounter(BUZZ1_TIM);
        LL_TIM_EnableCounter(BUZZ2_TIM);
        LL_TIM_EnableCounter(BUZZ3_TIM);

        LL_TIM_ClearFlag_UPDATE(BEAT_TIM);
        LL_TIM_EnableIT_UPDATE(BEAT_TIM);
        LL_TIM_EnableCounter(BEAT_TIM);
        LOGI(TAG, "Music app initialized successfully");
    }
    else {
        LOGE(TAG, "Failed to initialize music app");
    }
}
