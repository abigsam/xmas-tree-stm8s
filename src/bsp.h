/**
 * @file bsp.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-12-09
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef __BSP_H
#define __BSP_H

#include "stm8s.h"
#include "pt.h"
#include "gpio.h"

//Defines
#define GREEN_LEDS_NUM          ( GLEDS_NUM ) //Number of green LEDs
//#define __USE_RGB_DIRECT_CONTROL /* Uncomment to activate RGB direct control macroses */
#define PRESSED_COUNT           (18u)
#define UNPRESSED_COUNT         (180u)
#define SHORT_PRESS             ((uint16_t)PRESSED_COUNT * 6)
#define LONG_PRESS              ((((uint16_t)PRESSED_COUNT) << 2) * 5)
#define TIM4_TICK_MAX           (0xffffu)

//Macroses
#define RGB_LEDS_PWR(s)         { GPIO_CONTROL(RGB_LEDS_PWR_PORT, RGB_LEDS_PWR_PIN, s); }   //COntrol power for RGB LEDs
#define LIGHT_SENSOR_PWR(s)     { GPIO_CONTROL(LS_POWER_EN_PORT, LS_POWER_EN_PIN, s); }     //COntrol power for light sensor
#define BOOST_PWR(s)            { GPIO_CONTROL(BOOST_EN_PORT, BOOST_EN_PIN, s); }           //Control boost converter
#define RGB_LEDS_WRITE(b)       { GPIO_CONTROL(RGB_LEDS_PORT, RGB_LEDS_PIN, b); }           //Write 0 or 1 to RGB LEDs (connected to the chain)
#define BUTTON_IS_PRESSED()     ( !GPIO_GET(BUTTON_PORT, BUTTON_PIN) )                      //Check if button is pressed
#define BUTTON_IRQ_EN()         { BUTTON_PORT->CR2 |= (uint8_t)(BUTTON_PIN); }
#define BUTTON_IRQ_DIS()        { BUTTON_PORT->CR2 &= (uint8_t)(~BUTTON_PIN); }

//Structures
struct timer {
    uint8_t sign;
    uint16_t start, interval;
};

// BSP functiones
void bsp_init(void);
//Green LEDs control
void green_led(uint16_t num, FunctionalState led_st);
void green_led_all(FunctionalState led_st);
void test_blink(void);
//RGB LEDs control
void configure_rgb(uint8_t led_num, uint8_t red, uint8_t green, uint8_t blue);
void send_rgb(void);
void test_rgb(void);
//Timer
void timer_set(struct timer *t, uint16_t interval);
// { t->interval = interval; t->start = clock_time(); }
uint8_t timer_expired(struct timer *t);
// { return (int)(clock_time() - t->start) >= (int)t->interval; }
//Aux
void tim4_upd_irq(void);


#endif //__BSP_H