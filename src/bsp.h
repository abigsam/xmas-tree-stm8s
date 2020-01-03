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

//Macroses
#define RGB_LEDS_PWR(s)         { GPIO_CONTROL(RGB_LEDS_PWR_PORT, RGB_LEDS_PWR_PIN, s); }   //COntrol power for RGB LEDs
#define LIGHT_SENSOR_PWR(s)     { GPIO_CONTROL(LS_POWER_EN_PORT, LS_POWER_EN_PIN, s); }     //COntrol power for light sensor
#define BOOST_PWR(s)            { GPIO_CONTROL(BOOST_EN_PORT, BOOST_EN_PIN, s); }           //Control boost converter
#define RGB_LEDS_WRITE(b)       { GPIO_CONTROL(RGB_LEDS_PORT, RGB_LEDS_PIN, b); }           //Write 0 or 1 to RGB LEDs (connected to the chain)
#define BUTTON_READ()           ( !GPIO_GET(BUTTON_PORT, BUTTON_PIN) )                      //Read button state


// BSP functiones
void bsp_init(void);
void bsp_sleep(void);
void bsp_wakeup(void);
void green_led(FunctionalState led_st, uint16_t num);
void test_blink(void);

void tim4_upd_irq(void);

void configure_rgb(uint8_t led_num, uint8_t red, uint8_t green, uint8_t blue);
void send_rgb(void);

void test_rgb(void);

#endif //__BSP_H