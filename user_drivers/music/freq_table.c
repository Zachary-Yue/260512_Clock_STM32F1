
#include "freq_table.h"


#define FTIM_CLKIN (72000000.0f)
#define FTIM_AFTER_PSC (FTIM_CLKIN / (MUSIC_TIM_PSC + 1.0f))
#define ARRT(f) ((u16)((FTIM_AFTER_PSC / 2.0f / (f)) + 0.50f))

/**
 * @brief 频率 - ARR 重装值对照表。
 * 
 */
static const u16 FreqTab[] =
{
#if MUSIC_TIM_PSC >= 2
    ARRT(184.9972114f),     // F#3
    ARRT(195.997718f),      // G3
    ARRT(207.6523488f),     // G#3
    ARRT(220.0f),           // A3
    ARRT(233.0818808f),     // A#3
    ARRT(246.9416506f),     // B3

    ARRT(261.6255653f),     // C4
#endif
    ARRT(277.182631f),      // sC4
    ARRT(293.6647679f),     // D4
    ARRT(311.1269837f),     // sD4
    ARRT(329.6275569f),     // E4
    ARRT(349.2282314f),     // F4
    ARRT(369.9944227f),     // sF4
    ARRT(391.995436f),      // G4
    ARRT(415.3046976f),     // sG4
    ARRT(440.0f),           // A4
    ARRT(466.1637615f),     // sA4
    ARRT(493.8833013f),     // B4

    ARRT(523.2511306f),     // C5
    ARRT(554.365262f),      // 下面的依此类推
    ARRT(587.3295358f),
    ARRT(622.2539674f),
    ARRT(659.2551138f),
    ARRT(698.4564629f),
    ARRT(739.9888454f),
    ARRT(783.990872f),
    ARRT(830.6093952f),
    ARRT(880.0f),
    ARRT(932.327523f),
    ARRT(987.7666025f),

    ARRT(1046.502261f),     // C6
    ARRT(1108.730524f),
    ARRT(1174.659072f),
    ARRT(1244.507935f),
    ARRT(1318.510228f),
    ARRT(1396.912926f),
    ARRT(1479.977691f),
    ARRT(1567.981744f),
    ARRT(1661.21879f),
    ARRT(1760.0f),
    ARRT(1864.655046f),
    ARRT(1975.533205f),

    ARRT(2093.004522f),     // C7
    ARRT(2217.461048f),
    ARRT(2349.318143f),
    ARRT(2489.01587f),
    ARRT(2637.020455f),
    ARRT(2793.825851f),
    ARRT(2959.955382f),
    ARRT(3135.963488f),
    ARRT(3322.437581f),
    ARRT(3520.0f),
    ARRT(3729.310092f),
    ARRT(3951.06641f),

    ARRT(4186.009045f),     // C8
    ARRT(4434.922096f),
    ARRT(4698.636287f),
    ARRT(4978.03174f),
    ARRT(5274.040911f),
    ARRT(5587.651703f),
    ARRT(5919.910763f),
    ARRT(6271.926976f),
    ARRT(6644.875161f),
    ARRT(7040.0f),
    ARRT(7458.620184f),
    ARRT(7902.13282f),

    ARRT(8372.01809f),      // C9
    ARRT(8869.844191f),
    ARRT(9397.272573f),
    ARRT(9956.063479f),
    ARRT(10548.08182f),
    ARRT(11175.30341f),
    ARRT(11839.82153f),
    ARRT(12543.85395f),
    ARRT(13289.75032f),
    ARRT(14080.0f),
    ARRT(14917.24037f),
    ARRT(15804.26564f),
};

u16 Get_Freq(u8 index)
{
    if (index >= sizeof(FreqTab) / sizeof(u16)) return 0xffff;
    return FreqTab[index];
}
