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

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void main(void)
{
#ifdef __USE_RGB_DIRECT_CONTROL
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  test_rgb();
  for(;;) {}
#endif

  uint16_t cnt = 0u;
  volatile uint32_t delay = 0u;
  volatile uint8_t rgbcnt = 0u;
  uint8_t st = 0u;

  bsp_init();

  enableInterrupts();
  halt();
  
  // for(;;) {
  //   // test_blink();
  //   green_led_all(ENABLE);
  //   for(delay = 0u; delay < 30000u; delay++) {}
  //   green_led_all(DISABLE);
  //   for(delay = 0u; delay < 30000u; delay++) {}
  // }

  for(;;) {
    for(cnt = 0; cnt < GLEDS_NUM; cnt++) {
      // green_led(cnt, ENABLE);
      // for(delay = 0u; delay < 10000u; delay++) {}
      // green_led(cnt, DISABLE);
      // for(delay = 0u; delay < 1000u; delay++) {}
      if (st) {
        green_led(cnt, DISABLE);
      } else {
        green_led(cnt, ENABLE);
      }
      for(delay = 0u; delay < 3000u; delay++) {}
    }
    st = !st;
  }

  RGB_LEDS_PWR(1);
  
  /* Infinite loop */
  for(;;)
  {
    for(delay = 0u; delay < 30000u; delay++) {}
    configure_rgb(0u, 0u, 1u, 0u);
    configure_rgb(1u, 0u, 100u, 0u);
    send_rgb();
    for(delay = 0u; delay < 30000u; delay++) {}
    
    configure_rgb(0u, 1u, 1u, 0u);
    send_rgb();
    for(delay = 0u; delay < 30000u; delay++) {}
    
    configure_rgb(0u, 0u, 1u, 1u);
    send_rgb();
    for(delay = 0u; delay < 30000u; delay++) {}
    
    configure_rgb(0u, 0u, 1u, 0u);
    send_rgb();
    for(delay = 0u; delay < 30000u; delay++) {}
    
    configure_rgb(0u, 1u, 1u, 1u);
    send_rgb();
    for(delay = 0u; delay < 30000u; delay++) {}
  }
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
