/**
 * @file bsp.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-12-09
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "bsp.h"


/**
 * @brief Init all hardware
 * 
 */
void bsp_init(void)
{
    //Init GPIOs
    GPIO_Init(GREEN_LEDS_PORT, GLEDS_GPIO_0, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GREEN_LEDS_PORT, GLEDS_GPIO_1, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GREEN_LEDS_PORT, GLEDS_GPIO_2, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GREEN_LEDS_PORT, GLEDS_GPIO_3, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(GREEN_LEDS_PORT, GLEDS_GPIO_4, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(RGB_LEDS_PORT, RGB_LEDS_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(RGB_LEDS_PWR_PORT, RGB_LEDS_PWR_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(LS_POWER_EN_PORT, LS_POWER_EN_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(LS_OUTPUT_PORT, LS_OUTPUT_PIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(BUTTON_PORT, BUTTON_PIN, GPIO_MODE_IN_FL_IT);
    GPIO_Init(BOOST_EN_PORT, BOOST_EN_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);

    //ADC input for light sensor
    ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
              LS_ADC_CHANNEL,
              ADC1_PRESSEL_FCPU_D10,
              ADC1_EXTTRIG_TIM,
              DISABLE,
              ADC1_ALIGN_RIGHT,
              LS_SCHMIT_TRIGGER,
              DISABLE);
    //
    //Configure clock, default using HSI (16 MHz)
    // MASTERCLK = HSI/8 (for timers and pheripherial)
    // CPUCLK = MASTERCLK/1
}



