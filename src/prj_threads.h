
#ifndef __PRJ_THREADS_H
#define __PRJ_THREADS_H

#include "bsp.h"
#include "pt.h"
#include "pt-sem.h"

#define PROGRAMS_NUMBER         (2u)
#define LONG_PRESS_MS           (3000u)
#define SHORT_PRESS_MS          (100u)
#define LED_P1_DELAY_MS         (30u)


/* Private function prototypes -----------------------------------------------*/
static uint8_t PT_SEM_CHECK(struct pt_sem *s);

/* Public function prototypes ------------------------------------------------*/
void threads_init(void);
PT_THREAD(button_check(struct pt *pt));
PT_THREAD(disable_all(struct pt * pt));
PT_THREAD(led_programm_1(struct pt *pt));
PT_THREAD(led_programm_2(struct pt *pt));


#endif //__PRJ_THREADS_H