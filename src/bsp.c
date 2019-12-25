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

#include "gpio.h"
#include "bsp.h"

//Macroses

//Conatant array for control green LEDs using "charlieplexing"
const uint8_t GLEDS_ARRAY[2u][GLEDS_MAX_NUM] = {
    { 0x08, 0x10, 0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0x80, 0x08, 0x20, 0x80, 0x10, 0x40, 0x08, 0x40, 0x08, 0x20,  0x10, 0x80 }, //GPIO out state
    { 0x18, 0x18, 0x30, 0x30, 0x60, 0x60, 0xc0, 0xc0, 0x88, 0x88, 0xa0, 0xa0, 0x50, 0x50, 0x48, 0x48, 0x28, 0x28,  0x90, 0x90 }  //GPIO HiZ state
};


/**
 * @brief Init all hardware
 * 
 */
void bsp_init(void)
{
    //Init GPIOs
    GPIO_Init(GREEN_LEDS_PORT, GLEDS_GPIO_MASK, GPIO_MODE_IN_FL_NO_IT);
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

/**
 * @brief Enable/disable green LED
 * 
 * @param led_st    LED state, can be one of:
 *                  DISABLE, ENABLE
 * @param num       Number of LED, can be in range
 *                  0..(GLEDS_MAX_NUM-1)
 */
void green_led(FunctionalState led_st, uint16_t num)
{
    static uint8_t ddr = 0u, odr = 0u, cr1 = 0u;

    ddr = GREEN_LEDS_PORT->DDR & ~GLEDS_GPIO_MASK;
    odr = GREEN_LEDS_PORT->ODR & ~GLEDS_GPIO_MASK;
    cr1 = GREEN_LEDS_PORT->CR1 & ~GLEDS_GPIO_MASK;

    if (led_st != DISABLE && (num < GLEDS_MAX_NUM)) {
        odr |= GLEDS_ARRAY[0][num]; //GPIO states
        ddr |= GLEDS_ARRAY[1][num]; //GPIO HiZ
        cr1 |= GLEDS_ARRAY[1][num]; //GPIO to push-pull
    }

    GREEN_LEDS_PORT->DDR = ddr;
    GREEN_LEDS_PORT->ODR = odr;
    GREEN_LEDS_PORT->CR1 = cr1;
}



