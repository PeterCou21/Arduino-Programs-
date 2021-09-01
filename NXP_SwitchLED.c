#include <MKL25Z4.H>

#define LED1_POS (18)
#define LED2_POS (19)
#define LED3_POS (1)
#define SW1_POS (5)
#define SPKR_POS (4)

#define MASK(x) (1UL << (x))



int main (void)
{
	//------Enable CLOCK signals for ports---
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK; 
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;	
	
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;	
	
	//-------initalising GPIO PINS---------
	
	//--------------port A-------------
	PORTB->PCR[LED1_POS] &= ~PORT_PCR_MUX_MASK;            
	PORTB->PCR[LED1_POS] |= PORT_PCR_MUX(1);  
	
	PORTB->PCR[LED2_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[LED2_POS] |= PORT_PCR_MUX(1);
	
	PORTD->PCR[LED3_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTD->PCR[LED3_POS] |= PORT_PCR_MUX(1);

	PORTA->PCR[SW1_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTA->PCR[SW1_POS] |= PORT_PCR_MUX(1);
	
	//---------------PORT D---------------------
	
	//PORTD->PCR[SPKR_POS] &= ~PORT_PCR_MUX_MASK;          
	//PORTD->PCR[SPKR_POS] |= PORT_PCR_MUX(1);

	//------------------------------------------



	//PTA->PDOR &= ~MASK(LED1_POS);  //turn off LED1
	//PTA->PDOR |= MASK(LED2_POS);  // turn on LED1, turn off LED2
	
	//PTD->PDOR |= MASK(SPKR_POS);
	
	FPTB->PDOR &= ~(LED1_POS|LED2_POS);
	FPTD->PDOR &= ~(LED3_POS);
	
	//PTD->PDOR |= MASK(SPKR_POS);
	
	
	//----------setting i/0--------------------
	PTB->PDDR |= (MASK(LED1_POS) | MASK (LED2_POS)); // set LED bits to outputs
	PTD->PDDR |= (MASK(LED3_POS));
	
	PTA->PDDR &= ~MASK(SW1_POS); // clear Switch bit to input
	//------------------------------------------
	int press_count = 0;
	
	
			if (PTA->PDIR & MASK(SW1_POS)) 
		{			
			PTB->PSOR = 1UL << LED2_POS;
			PTB->PCOR = 1UL	<< LED1_POS;
			PTD->PCOR = 1UL	<< LED3_POS;
			press_count++;
		}
		else if (PTA ->PDIR & MASK(SW1_POS))		
		{
			PTB->PCOR = 1UL << LED2_POS;
			PTB->PSOR = 1UL	<< LED1_POS;
			PTD->PCOR = 1UL	<< LED3_POS;
			press_count++;
		}
		else if (PTA ->PDIR & MASK(SW1_POS))
		{
			PTB->PCOR = 1UL << LED2_POS;
			PTB->PCOR = 1UL	<< LED1_POS;
			PTD->PSOR = 1UL	<< LED3_POS;
			press_count++;
	
	while (1)
	{

		//}
		//else
		//{
			//PTB->PCOR = 1UL << LED2_POS;
			//PTB->PCOR = 1UL	<< LED1_POS;
			//PTD->PCOR = 1UL	<< LED3_POS;
			press_count++;
		}	

	}
	
}
