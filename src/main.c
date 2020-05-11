/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.3.0
  * @date    16-June-2017
  * @brief   Main program body
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "pt.h"
#include "pt-sem.h"

/* Private defines -----------------------------------------------------------*/
#define PROGRAMS_NUMBER         (2u)
#define LONG_PRESS_MS           (3000u)
#define SHORT_PRESS_MS          (100u)

/* Private variables ---------------------------------------------------------*/
extern uint16_t tim4_tick_ext;
static struct timer delay_timer;
static struct pt_sem disable_all_sem, restart_prog_sem;
static uint8_t prog_num = 0u;
static uint8_t system_enabled = 0u;
static struct pt led_programm_pt[2];
static struct pt button_check_pt;
static struct pt disable_all_pt;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static uint8_t PT_SEM_CHECK(struct pt_sem *s) { if (s->count > 0) {--(s)->count; return 1u;} else {return 0u;} }

#define LED_P1_DELAY_MS     (30u)
static PT_THREAD(led_programm_1(struct pt *pt))
{
  static uint8_t ledcnt = 0u;
  static FunctionalState state = ENABLE;
  PT_BEGIN(pt);
  for(;;) {
    PT_YIELD_UNTIL(pt, (1u == prog_num));
    if (PT_SEM_CHECK(&restart_prog_sem)) {
      ledcnt = 0u;
      state = ENABLE;
    }
    // green_led(14, ENABLE);
    green_led(ledcnt, state);
    if (ledcnt < GLEDS_NUM) {
      ledcnt++;
    } else {
      ledcnt = 0u;
      state = (ENABLE == state) ? DISABLE : ENABLE;
    }
    timer_set(&delay_timer, LED_P1_DELAY_MS);
    PT_WAIT_UNTIL(pt, timer_expired(&delay_timer));
  }
  PT_END(pt);
}

static PT_THREAD(led_programm_2(struct pt *pt))
{
  PT_BEGIN(pt);
  for(;;) {
    PT_YIELD_UNTIL(pt, (2u == prog_num));
    if (PT_SEM_CHECK(&restart_prog_sem)) {

    }
    green_led(15, ENABLE);
    green_led(1, ENABLE);
    green_led(2, ENABLE);
  }
  PT_END(pt);
}

enum fsm_state {
  ENABLE_DELAY = 0u,
  WAIT_PRESSED,
  WAIT_UNPRESSED,
  SHORT_PRESS_DET,
  LONG_PRESS_DET
};

static PT_THREAD(button_check(struct pt *pt))
{
  static uint16_t tstamp = 0u, delta = 0u;
  static enum fsm_state state = ENABLE_DELAY;
  static uint8_t prog_num_next = 0u;

  PT_BEGIN(pt);
  prog_num_next = 1u;
  for(;;) {
    if (0u == prog_num)
      prog_num = prog_num_next;
    
    //Restart FSM
    if (!system_enabled) {
      system_enabled = 1u;
      state = ENABLE_DELAY;
    }
    //Button FSM
    if (ENABLE_DELAY == state) {
      PT_YIELD_UNTIL(pt, !BUTTON_IS_PRESSED());
      state = WAIT_PRESSED;
    } 
    else if (WAIT_PRESSED == state) {
      PT_WAIT_UNTIL(pt, BUTTON_IS_PRESSED());
      tstamp = tim4_tick_ext;
      state = WAIT_UNPRESSED;
    }
    else if (WAIT_UNPRESSED == state) {
      if (tstamp <= tim4_tick_ext)
        delta = tim4_tick_ext - tstamp;
      else
        delta = (TIM4_TICK_MAX - tstamp) + tim4_tick_ext;
      
      if (!BUTTON_IS_PRESSED()) {
        if (delta >= LONG_PRESS_MS) {
          state = LONG_PRESS_DET;
        } else if (delta >= SHORT_PRESS_MS) {
          state = SHORT_PRESS_DET;
        } else {
          state = WAIT_PRESSED;
        }
      } else if (delta >= LONG_PRESS_MS) {
        state = LONG_PRESS_DET;
      }
    }
    else if (SHORT_PRESS_DET == state) {
      //Short press
      if (prog_num < PROGRAMS_NUMBER)
        prog_num = prog_num + 1u;
      else
        prog_num = 1u;
      PT_SEM_SIGNAL(pt, &disable_all_sem);
      PT_SEM_SIGNAL(pt, &restart_prog_sem);
      state = ENABLE_DELAY;
    }
    else if (LONG_PRESS_DET == state) {
      //Long press
      // prog_num_next = 1u;
      prog_num = 0u;
      system_enabled = 0u;
      PT_SEM_SIGNAL(pt, &disable_all_sem);
      state = ENABLE_DELAY;
    }
    PT_YIELD(pt);
  }
  PT_END(pt);
}

static PT_THREAD(disable_all(struct pt * pt))
{
  PT_BEGIN(pt);
  for(;;) {
    PT_SEM_WAIT(pt, &disable_all_sem);
    green_led_all(DISABLE);
  }
  PT_END(pt);
}


/**
 * @brief 
 * 
 */
void main(void)
{
#ifdef __USE_RGB_DIRECT_CONTROL
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  test_rgb();
  for(;;) {}
#endif

  uint8_t pressed_cnt = 0u, unpressed_cnt = 0u;
  uint16_t dcnt = 0u, ledcnt = 0u;
  volatile uint32_t delay = 0u;
//  uint8_t st = 0u;
  // uint16_t delay;

  PT_INIT(&(led_programm_pt[0]));
  PT_INIT(&(led_programm_pt[1]));
  PT_INIT(&button_check_pt);
  PT_INIT(&disable_all_pt);
  PT_SEM_INIT(&disable_all_sem, 0);
  PT_SEM_INIT(&restart_prog_sem, 0);

  bsp_init();
  enableInterrupts();

IDLE:

  green_led_all(DISABLE);
  RGB_LEDS_PWR(0);

  for(;;) {
    AWU_IdleModeEnable();
    // FLASH_SetLowPowerMode()
    BUTTON_IRQ_EN();
    halt();
    BUTTON_IRQ_DIS();
    AWU_Init(AWU_TIMEBASE_128MS);
    for(;;) {
      halt();
      if (BUTTON_IS_PRESSED()) {
        pressed_cnt++;
        unpressed_cnt = 0u;
      } else {
        pressed_cnt = 0u;
        unpressed_cnt++;
      }
      if (pressed_cnt >= PRESSED_COUNT)
        goto RUN_PROGRAMM;
      if (unpressed_cnt >= UNPRESSED_COUNT)
        break;
    };
  }
  
RUN_PROGRAMM:
  do { //Programm cycle
    disable_all(&disable_all_pt);
    led_programm_1(&led_programm_pt[0]);
    led_programm_2(&led_programm_pt[1]);

    button_check(&button_check_pt);
  } while (system_enabled);
  goto IDLE;
}
    // //Get programm
    // switch(prog_num) {

    //   /* Programm #0 */
    //   case 0u:
    //     // for(ledcnt = 0; ledcnt < GLEDS_NUM; ledcnt++) {
    //     if (st)
    //       green_led(ledcnt, DISABLE);
    //     else
    //       green_led(ledcnt, ENABLE);
    //     for(delay = 0u; delay < 3000u; delay++) {}
    //     if (ledcnt < GLEDS_NUM) {
    //       ledcnt++;
    //     } else {
    //       ledcnt = 0u;
    //       st = !st;
    //     }
    //     break;

    //   /* Programm #1 */
    //   case 1u:
    //     if (st)
    //       green_led_all(DISABLE);
    //     else
    //       green_led_all(ENABLE);
    //     for(delay = 0u; delay < 30000u; delay++) {}
    //     st = !st;
    //     break;
      
    // default:
    //   prog_num = 0u;
    // }

    //Check if button is pressed
/*    if (is_short_pressed()) {
      if (prog_num < PROGRAMS_NUMBER)
        prog_num++;
      else
        prog_num = 0u;
    }

    if (is_long_pressed())
      goto IDLE;*/

  // } //Programm cycle
  


  // for(;;) {
  //   // test_blink();
  //   green_led_all(ENABLE);
  //   for(delay = 0u; delay < 30000u; delay++) {}
  //   green_led_all(DISABLE);
  //   for(delay = 0u; delay < 30000u; delay++) {}
  // }

  // for(;;) {
  //   for(cnt = 0; cnt < GLEDS_NUM; cnt++) {
  //     // green_led(cnt, ENABLE);
  //     // for(delay = 0u; delay < 10000u; delay++) {}
  //     // green_led(cnt, DISABLE);
  //     // for(delay = 0u; delay < 1000u; delay++) {}
  //     if (st) {
  //       green_led(cnt, DISABLE);
  //     } else {
  //       green_led(cnt, ENABLE);
  //     }
  //     for(delay = 0u; delay < 3000u; delay++) {}
  //   }
  //   st = !st;
  // }

  // RGB_LEDS_PWR(1);
  
  // /* Infinite loop */
  // for(;;)
  // {
  //   for(delay = 0u; delay < 30000u; delay++) {}
  //   configure_rgb(0u, 0u, 1u, 0u);
  //   configure_rgb(1u, 0u, 100u, 0u);
  //   send_rgb();
  //   for(delay = 0u; delay < 30000u; delay++) {}
    
  //   configure_rgb(0u, 1u, 1u, 0u);
  //   send_rgb();
  //   for(delay = 0u; delay < 30000u; delay++) {}
    
  //   configure_rgb(0u, 0u, 1u, 1u);
  //   send_rgb();
  //   for(delay = 0u; delay < 30000u; delay++) {}
    
  //   configure_rgb(0u, 0u, 1u, 0u);
  //   send_rgb();
  //   for(delay = 0u; delay < 30000u; delay++) {}
    
  //   configure_rgb(0u, 1u, 1u, 1u);
  //   send_rgb();
  //   for(delay = 0u; delay < 30000u; delay++) {}
  // }
// }

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
