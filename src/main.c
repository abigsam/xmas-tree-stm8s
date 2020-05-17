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
#include "prj_threads.h"

/* Private defines -----------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
extern uint16_t tim4_tick_ext;

extern struct pt button_check_pt;
extern struct pt led_programm_pt[2];
extern struct pt_sem disable_all_sem;
extern uint8_t system_enabled;
extern struct pt disable_all_pt;
extern struct pt_sem restart_prog_sem;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


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

  threads_init();
  // PT_INIT(&(led_programm_pt[0]));
  // PT_INIT(&(led_programm_pt[1]));
  // PT_INIT(&button_check_pt);
  // PT_INIT(&disable_all_pt);
  // PT_SEM_INIT(&disable_all_sem, 0);
  // PT_SEM_INIT(&restart_prog_sem, 0);

  bsp_init();
  enableInterrupts();

IDLE:

  green_led_all(DISABLE);
  RGB_LEDS_PWR(0);

  for(;;) {
    AWU_IdleModeEnable();
    // FLASH_SetLowPowerMode();
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
