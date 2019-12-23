/**
 * @file gpio.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-12-09
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef __GPIO_H
#define __GPIO_H


//GPIO defines
#define GREEN_LEDS_PORT     (GPIOC)
#define GLEDS_GPIO_0        (GPIO_PIN_3)
#define GLEDS_GPIO_1        (GPIO_PIN_4)
#define GLEDS_GPIO_2        (GPIO_PIN_5)
#define GLEDS_GPIO_3        (GPIO_PIN_6)
#define GLEDS_GPIO_4        (GPIO_PIN_7)
#define GLEDS_GPIO_MASK     ( GLEDS_GPIO_0 | GLEDS_GPIO_1 | GLEDS_GPIO_2 | GLEDS_GPIO_3 | GLEDS_GPIO_4 )
#define GLEDS_MAX_NUM       ((uint16_t) 20)
//RGB LEDs
#define RGB_LEDS_PORT       (GPIOD)
#define RGB_LEDS_PIN        (GPIO_PIN_3)
#define RGB_LEDS_PWR_PORT   (GPIOB)
#define RGB_LEDS_PWR_PIN    (GPIO_PIN_5)
//Light sensor
#define LS_POWER_EN_PORT    (GPIOB)
#define LS_POWER_EN_PIN     (GPIO_PIN_4)
#define LS_OUTPUT_PORT      (GPIOD)
#define LS_OUTPUT_PIN       (GPIO_PIN_2)
#define LS_ADC_CHANNEL      (ADC1_CHANNEL_3)
#define LS_SCHMIT_TRIGGER   (ADC1_SCHMITTTRIG_CHANNEL3)
//Button
#define BUTTON_PORT         (GPIOD)
#define BUTTON_PIN          (GPIO_PIN_6)
//Power managment
#define BOOST_EN_PORT       (GPIOD)
#define BOOST_EN_PIN        (GPIO_PIN_5)

//Macroses
#define GPIO_GET(port,pin)              ( port->IDR & ((uint8_t)(pin)) )
#define GPIO_SET(port,pin)              { port->ODR |= (uint8_t)(pin); }
#define GPIO_RESET(port,pin)            { port->ODR &= (uint8_t)(~pin); }
#define GPIO_CONTROL(port,pin,state)    { if (state) { GPIO_SET(port,pin); } \
                                          else { GPIO_RESET(port,pin); } }
#define GPIO_TO_HIZ(port,pin,state)     { if (state) { port->DDR &= (uint8_t)(~pin); } \
                                          else { port->DDR |= (uint8_t)(pin); } }
#endif //__GPIO_H