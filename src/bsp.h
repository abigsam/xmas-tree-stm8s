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

//GPIO macroses
#define RGB_LEDS_PWR(s)         { GPIO_CONTROL(RGB_LEDS_PWR_PORT, RGB_LEDS_PWR_PIN, s); }
#define LIGHT_SENSOR_PWR(s)     { GPIO_CONTROL(LS_POWER_EN_PORT, LS_POWER_EN_PIN, s); }
#define BOOST_PWR(s)            { GPIO_CONTROL(BOOST_EN_PORT, BOOST_EN_PIN, s); }
#define RGP_LEDS_WRITE(b)       { GPIO_CONTROL(RGB_LEDS_PORT, RGB_LEDS_PIN, b); }
#define BUTTON_READ()           ( !GPIO_GET(BUTTON_PORT, BUTTON_PIN) )

// BSP functiones
void bsp_init(void);




#endif //__BSP_H