#include <MKL25Z4.H>
#include "led.h"

void init_LED()
{
	// Set up clock gating registers
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	
	PORTB->PCR[RED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_PIN] = PORT_PCR_MUX(1);
	FPTB->PDDR |= (1UL << RED_PIN);
	
	PORTB->PCR[GREEN_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_PIN] = PORT_PCR_MUX(1);
	FPTB->PDDR |= (1UL << GREEN_PIN);
	
	PORTD->PCR[BLUE_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_PIN] = PORT_PCR_MUX(1);
	FPTD->PDDR |= (1UL << BLUE_PIN);
	
	FPTB->PSOR = (1UL << RED_PIN | 1UL << GREEN_PIN);
	FPTD->PSOR = 1UL << BLUE_PIN;
}

void set_LED(int r, int g, int b)
{
	if (r == 1) FPTB->PCOR = (1UL << RED_PIN); else FPTB->PSOR = (1UL << RED_PIN);
	if (g == 1) FPTB->PCOR = (1UL << GREEN_PIN); else FPTB->PSOR = (1UL << GREEN_PIN);
	if (b == 1) FPTD->PCOR = (1UL << BLUE_PIN); else FPTD->PSOR = (1UL << BLUE_PIN);
}
