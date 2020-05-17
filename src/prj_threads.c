

#include "prj_threads.h"

enum fsm_state {
  ENABLE_DELAY = 0u,
  WAIT_PRESSED,
  WAIT_UNPRESSED,
  SHORT_PRESS_DET,
  LONG_PRESS_DET
};

/* Public variables ----------------------------------------------------------*/
struct pt button_check_pt;
struct pt led_programm_pt[2];
struct pt_sem disable_all_sem;
uint8_t system_enabled = 0u;
struct pt disable_all_pt;
struct pt_sem restart_prog_sem;

/* Private variables ---------------------------------------------------------*/
static struct timer delay_timer;
static uint8_t prog_num = 0u;

/* Extern variables ----------------------------------------------------------*/
extern uint16_t tim4_tick_ext;


/**
 * @brief Initilize all threads and semaphores
 * 
 */
void threads_init(void)
{
    PT_INIT(&(led_programm_pt[0]));
    PT_INIT(&(led_programm_pt[1]));
    PT_INIT(&button_check_pt);
    PT_INIT(&disable_all_pt);
    PT_SEM_INIT(&disable_all_sem, 0);
    PT_SEM_INIT(&restart_prog_sem, 0);
}


/**
 * @brief Treads for check button for programs switching and system off
 * 
 */
PT_THREAD(button_check(struct pt *pt))
{
  static uint16_t tstamp = 0u, delta = 0u;
  static enum fsm_state state = ENABLE_DELAY;

  PT_BEGIN(pt);
  for(;;) {
    
    //Restart FSM
    if (!system_enabled) {
      prog_num = 1u;
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
      prog_num = 0u;
      system_enabled = 0u;
      PT_SEM_SIGNAL(pt, &disable_all_sem);
      state = ENABLE_DELAY;
    }
    PT_YIELD(pt);
  }
  PT_END(pt);
}


/**
 * @brief Thread for disabling all LEDs
 * 
 */
PT_THREAD(disable_all(struct pt * pt))
{
  PT_BEGIN(pt);
  for(;;) {
    PT_SEM_WAIT(pt, &disable_all_sem);
    green_led_all(DISABLE);
  }
  PT_END(pt);
}


/**
 * @brief Function that checks if semaphore is bigger than 0.
 * If this is true, semaphore is decremented
 * 
 * @param s         Semaphore pointer
 * @return uint8_t  "1" if semaphore is bigger than 0
 */
static uint8_t PT_SEM_CHECK(struct pt_sem *s)
{
    if (s->count > 0) {--(s)->count; return 1u;} else {return 0u;}
}


/**
 * @brief Thread for LED programm #1
 * 
 */
PT_THREAD(led_programm_1(struct pt *pt))
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


/**
 * @brief Thread for LED programm #2
 * 
 */
PT_THREAD(led_programm_2(struct pt *pt))
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

