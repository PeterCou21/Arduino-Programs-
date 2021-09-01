#ifndef TIMERS_H
#define TIMERS_H
#include "MKL25Z4.h"

void set_PIT(unsigned period);
void init_PIT(unsigned period);
void start_PIT(void);
void stop_PIT(void);

extern volatile unsigned PIT_interrupt_counter;
#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
