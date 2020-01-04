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

//Defines
/* For green LEDs */
#define TIM4_PRESCALLER     (TIM4_PRESCALER_32)
#define TIM4_PERIOD         ((uint8_t) 83)
//#define __USE_RGB_DIRECT_CONTROL /* Uncomment to activate RGB direct control macroses */

/* For RGB LEDs */
#define TIM2_PRESCALLER     (TIM2_PRESCALER_1)
#define TIM2_PERIOD         ((uint16_t) 63)
#define TIM2_T0H            ((uint16_t) 5)
#define TIM2_T1H            ((uint16_t) TIM2_PERIOD - TIM2_T0H)

//Macroses
#define TIM2CH2_TO_PWM()    { TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, TIM2_T1H, TIM2_OCPOLARITY_HIGH); }
#define TIM2CH2_TO_HIGH()   { TIM2_ForcedOC2Config(TIM2_FORCEDACTION_ACTIVE); }
#define TIM2CH2_TO_LOW()    { TIM2_ForcedOC2Config(TIM2_FORCEDACTION_INACTIVE); }

//Conatant array for control green LEDs using "charlieplexing"
const uint8_t GLEDS_ARRAY[2u][GLEDS_NUM] = {
    { 0x08, 0x10, 0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0x80, 0x08, 0x20, 0x80, 0x10, 0x40, 0x08, 0x40, 0x08, 0x20,  0x10, 0x80 }, //GPIO out state
    { 0x18, 0x18, 0x30, 0x30, 0x60, 0x60, 0xc0, 0xc0, 0x88, 0x88, 0xa0, 0xa0, 0x50, 0x50, 0x48, 0x48, 0x28, 0x28,  0x90, 0x90 }  //GPIO HiZ state
};


//Internal variables
static uint8_t gled_state[GLEDS_NUM] = { 0u };
uint8_t rgb_codes[RGB_LEDS_NUM*RGB_LED_BITS] = { TIM2_T0H };
uint8_t rgb_data_cnt = 0u;

//Internal functions
static void config_tim4(void);
static void config_tim2(void);
static void code_color(uint8_t *ptr, uint8_t *color);

/**
 * @brief Configure TIM4 for switching green LEDs.
 * Configure to make interrupt [200 Hz * 20 LEDs] ~= 6 kHz
 * 16 MHz / 32 * (83 + 1) ~= 5.9 kHz
 */
static void config_tim4(void)
{
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);
    TIM4_TimeBaseInit(TIM4_PRESCALLER, TIM4_PERIOD);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    TIM4_Cmd(ENABLE);
}


/**
 * @brief Configure TIM2 output compare channel 2 in PWM1
 * for control RGB LEDs.
 * Make interrupts: 16 MHz / 1 / (19 + 1) ~= 800 kHz.
 * For "1" PWM is 4, see TIM2_T0H.
 * For "0" PWM is (19-4), see TIM2_T1H.
 */
static void config_tim2(void)
{
    //Enable TIM2 clock
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
    TIM2_TimeBaseInit(TIM2_PRESCALLER, TIM2_PERIOD);
    // TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, TIM2_T1H, TIM2_OCPOLARITY_HIGH);
    TIM2CH2_TO_PWM();
    TIM2_OC2PreloadConfig(ENABLE);
    TIM2_GenerateEvent(TIM2_EVENTSOURCE_UPDATE);
    //Switch channel 2 to low state
    TIM2CH2_TO_LOW();
}

/**
 * @brief Code 8-bit color data with PWM values for TIM2
 * 
 * @param ptr       Pointer to byte array
 * @param color     Pointer to color 8-bit data
 */
static void code_color(uint8_t *ptr, uint8_t *color)
{
    for (uint8_t i = 0u; i < 8u; i++)
    {
        if ((*color) & ((uint8_t)1u << i)) {
            *(ptr + i) = TIM2_T1H;
        } else {
            *(ptr + i) = TIM2_T0H;
        }
    }
}


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
    // Configure clock: default using HSI (16 MHz)
    // MASTERCLK = HSI/8 (for timers and pheripherial)
    // CPUCLK = MASTERCLK/1
    // Use MASTERCLK = HSI/1
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

    //Config TIM4
    config_tim4();

    //Configure TIM2
    config_tim2();
    ITC_SetSoftwarePriority(ITC_IRQ_TIM2_OVF, ITC_PRIORITYLEVEL_2);

    for (uint8_t i = 0u; i < RGB_LEDS_NUM*RGB_LED_BITS; i++)
    {
        rgb_codes[i] = TIM2_T0H;
    }
}


/**
 * @brief Switch all into a sleep mode
 * 
 */
void bsp_sleep(void)
{

}


/**
 * @brief Run commands after exit from sleep mode
 * 
 */
void bsp_wakeup(void)
{

}


/**
 * @brief Enable/disable green LED
 * 
 * @param led_st    LED state, can be one of:
 *                  DISABLE, ENABLE
 * @param num       Number of LED, can be in range
 *                  0..(GLEDS_NUM-1)
 */
void green_led(FunctionalState led_st, uint16_t num)
{
    static uint8_t ddr = 0u, odr = 0u, cr1 = 0u;

    ddr = GREEN_LEDS_PORT->DDR & ~GLEDS_GPIO_MASK;
    odr = GREEN_LEDS_PORT->ODR & ~GLEDS_GPIO_MASK;
    cr1 = GREEN_LEDS_PORT->CR1 & ~GLEDS_GPIO_MASK;

    if (led_st != DISABLE && (num < GLEDS_NUM)) {
        odr |= GLEDS_ARRAY[0][num]; //GPIO states
        ddr |= GLEDS_ARRAY[1][num]; //GPIO HiZ
        cr1 |= GLEDS_ARRAY[1][num]; //GPIO to push-pull
    }

    GREEN_LEDS_PORT->DDR = ddr;
    GREEN_LEDS_PORT->ODR = odr;
    GREEN_LEDS_PORT->CR1 = cr1;
}


/**
 * @brief For testing green LEDs
 * 
 */
void test_blink(void)
{
    static uint8_t st = 0u;
    gled_state[5] = st ? GLEDS_ARRAY[0][5] : 0u;
    gled_state[9] = !st ? GLEDS_ARRAY[0][9] : 0u;
    st = st ? 0u : 1u;
}


/**
 * @brief Update IRQ for TIM4
 * 
 */
void tim4_upd_irq(void)
{
    static uint8_t led_cnt = 0u;
    static uint8_t ddr = 0u, odr = 0u, cr1 = 0u;

    odr = GREEN_LEDS_PORT->ODR & ~GLEDS_GPIO_MASK;
    ddr = GREEN_LEDS_PORT->DDR & ~GLEDS_GPIO_MASK;
    cr1 = GREEN_LEDS_PORT->CR1 & ~GLEDS_GPIO_MASK;

    // odr |= GLEDS_ARRAY[0][led_cnt]; //GPIO states
    odr |= gled_state[led_cnt]; //LED states
    ddr |= GLEDS_ARRAY[1][led_cnt]; //GPIO HiZ
    cr1 |= GLEDS_ARRAY[1][led_cnt]; //GPIO to push-pull

    GREEN_LEDS_PORT->ODR = odr;
    GREEN_LEDS_PORT->DDR = ddr;
    GREEN_LEDS_PORT->CR1 = cr1;

    led_cnt++;
    if ( (GLEDS_NUM - led_cnt) == 0u ) { led_cnt = 0u; }
}


/**
 * @brief Configure specified RGB LED with R, G, B parameters
 * Data transfered in order GRB (higher bit is first)
 * @param led_num   RGB LED number, can be in range 0...(RGB_LEDS_NUM-1)
 * @param red       Red component, 0...255
 * @param green     Green component, 0...255
 * @param blue      Blue component, 0...255
 */
void configure_rgb(uint8_t led_num, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t *ptr = rgb_codes;
    if (led_num < RGB_LEDS_NUM) {
        ptr += (RGB_LEDS_NUM - led_num - 1u)*RGB_LED_BITS; //First data for the first LED
        code_color(ptr, &blue);
        ptr += RGB_LED_BITS/3u;
        code_color(ptr, &red);
        ptr += RGB_LED_BITS/3u;
        code_color(ptr, &green);
    }
}


/**
 * @brief Send RGB data to the LEDs using TIM4
 * 
 */
void send_rgb(void)
{
    disableInterrupts();

    rgb_data_cnt = RGB_LEDS_NUM*RGB_LED_BITS - 1u;
    // rgb_data_cnt = RGB_LED_BITS - 1u;

    //Load first PWM value
    TIM2CH2_TO_PWM();
    TIM2->CCR2H = 0u;
    TIM2->CCR2L = *(rgb_codes + rgb_data_cnt);
    //Generate an event to copy PWM value
    //from the shadow register to the TIM2 register
    TIM2_GenerateEvent(TIM2_EVENTSOURCE_UPDATE);
    //Next value
    rgb_data_cnt--;

    //Load second PWM value and increment data counter
    TIM2->CCR2H = 0u;
    TIM2->CCR2L = *(rgb_codes + rgb_data_cnt);
    rgb_data_cnt--;
    //Enable TIM2 IRQ, run TIM2 and enable global IRQ
    TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
    TIM4_ITConfig(TIM4_IT_UPDATE, DISABLE);
    
    TIM2_Cmd(ENABLE);
    enableInterrupts();

    //Wait until transfer is ended
    while(rgb_data_cnt != 0u) {};

    //Disable TIM2
    disableInterrupts();
    TIM2CH2_TO_LOW();
    TIM2_Cmd(DISABLE);
    TIM2_ITConfig(TIM2_IT_UPDATE, DISABLE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    enableInterrupts();
}


#ifdef __USE_RGB_DIRECT_CONTROL
#define RGBPIN_SET()        { GPIO_SET(RGB_LEDS_PORT, RGB_LEDS_PIN); }
#define RGBPIN_RESET()      { GPIO_RESET(RGB_LEDS_PORT, RGB_LEDS_PIN); }
//
#define SEND_0()            { RGBPIN_SET(); \
                              nop(); nop(); nop(); nop(); \
                              nop(); nop(); \
                              RGBPIN_RESET(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                            }
//
#define SEND_1()            { RGBPIN_SET(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              nop(); nop(); nop(); nop(); nop(); nop(); nop(); \
                              RGBPIN_RESET(); \
                              nop(); nop(); nop(); nop(); \
                              nop(); nop(); \
                            }
//
#define SEND_TEST()         { /* Send green */ \
                              SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); \
                              /* Send red */ \
                              SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); \
                              /* Send blue */ \
                              SEND_0(); SEND_0(); SEND_1(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); \
                            }
//
#define SEND_TEST2()        { /* Send green */ \
                              SEND_0(); SEND_0(); SEND_1(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); \
                              /* Send red */ \
                              SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); \
                              /* Send blue */ \
                              SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); SEND_0(); \
                            }
//
#endif

void test_rgb(void)
{
#ifdef __USE_RGB_DIRECT_CONTROL
    uint16_t delay = 0u;
    GPIO_Init(RGB_LEDS_PORT, RGB_LEDS_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(RGB_LEDS_PWR_PORT, RGB_LEDS_PWR_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    //for(delay = 0u; delay < 30000u; delay++) {}
    RGB_LEDS_PWR(1);
    RGBPIN_RESET();
    for(delay = 0u; delay < 30000u; delay++) {}
    
    //LED #1
    
    //Send green
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_1();
    SEND_0();
    SEND_0();

    //Send red
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_0();

    //Send blue
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_0();
    SEND_0();
     
     
    //LED #2...#10
    SEND_TEST();
    SEND_TEST();
    // SEND_TEST();
    // SEND_TEST();
    // SEND_TEST();
    // SEND_TEST();
    // SEND_TEST();
    // SEND_TEST();
    // SEND_TEST2();
    
    RGBPIN_RESET();
    
    nop();
#endif
}