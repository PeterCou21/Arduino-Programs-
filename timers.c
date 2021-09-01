#include "timers.h"
#include "MKL25Z4.h"
#include "queue.h"

volatile unsigned PIT_interrupt_counter = 0;
extern Q_T queue;

void init_PIT(unsigned period_us) {
	// Enable clock to PIT module
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;
	PIT->MCR |= PIT_MCR_FRZ_MASK;
	
	// Initialize PIT0 to count down from argument 
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(period_us*24); // 24 MHz clock frequency

	// No chaining
	PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN_MASK;
	
	// Generate interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;

	/* Enable Interrupts */
	NVIC_SetPriority(PIT_IRQn, 128); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(PIT_IRQn); 
	NVIC_EnableIRQ(PIT_IRQn);	
}

void set_PIT(unsigned period_us) {
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(period_us*24); // 24 MHz clock frequency
}

void start_PIT(void) {
// Enable counter
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void stop_PIT(void) {
// Enable counter
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}


void PIT_IRQHandler() {
	unsigned short out_data=0;
	
	//clear pending IRQ
	NVIC_ClearPendingIRQ(PIT_IRQn);
	
	// check to see which channel triggered interrupt 
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 0
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		
		// Do ISR work - move next sample from buffer to DAC
		if (!Q_Empty(&queue)) {
			out_data = Q_Dequeue(&queue);	
			DAC0->DAT[0].DATH = DAC_DATH_DATA1(out_data >> 8);
			DAC0->DAT[0].DATL = DAC_DATL_DATA0(out_data);
		}
	} else if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 1
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
	} 
}

// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
