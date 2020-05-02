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

//Macroses
#define RGB_LEDS_PWR(s)         { GPIO_CONTROL(RGB_LEDS_PWR_PORT, RGB_LEDS_PWR_PIN, s); }   //COntrol power for RGB LEDs
#define LIGHT_SENSOR_PWR(s)     { GPIO_CONTROL(LS_POWER_EN_PORT, LS_POWER_EN_PIN, s); }     //COntrol power for light sensor
#define BOOST_PWR(s)            { GPIO_CONTROL(BOOST_EN_PORT, BOOST_EN_PIN, s); }           //Control boost converter
#define RGB_LEDS_WRITE(b)       { GPIO_CONTROL(RGB_LEDS_PORT, RGB_LEDS_PIN, b); }           //Write 0 or 1 to RGB LEDs (connected to the chain)
#define BUTTON_READ()           ( !GPIO_GET(BUTTON_PORT, BUTTON_PIN) )                      //Read button state

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
//Aux
void tim4_upd_irq(void);

#endif //__BSP_H