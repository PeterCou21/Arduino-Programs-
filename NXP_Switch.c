#include <MKL25Z4.H>

#define MASK(x) (1UL << (x))

// All switches are on port E

#define SW_LT_POS (23)

#define SW_RT_POS (30)

#define SW_CR_POS (22) 
// Macro to read switches returns state of switches, active low

#define LED1_POS (18)
#define LED2_POS (19)
#define LED3_POS (1)

#define READ_SWITCHES (PTE->PDIR) 


void init_3way_switch(void) {

SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; /* enable clock for port E */
SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;	
SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;	

	
PORTB->PCR[LED1_POS] &= ~PORT_PCR_MUX_MASK;            
PORTB->PCR[LED1_POS] |= PORT_PCR_MUX(1);  
	
PORTB->PCR[LED2_POS] &= ~PORT_PCR_MUX_MASK;          
PORTB->PCR[LED2_POS] |= PORT_PCR_MUX(1);

PORTD->PCR[LED3_POS] &= ~PORT_PCR_MUX_MASK;          
PORTD->PCR[LED3_POS] |= PORT_PCR_MUX(1);	
	
/* Select GPIO and enable pull-up resistors for pins connected to switches */

PORTE->PCR[SW_LT_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;

PORTE->PCR[SW_RT_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;

PORTE->PCR[SW_CR_POS] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;

FPTB->PDOR &= ~(LED1_POS|LED2_POS);
FPTD->PDOR &= ~(LED3_POS);

PTB->PDDR |= (MASK(LED1_POS) | MASK (LED2_POS)); // set LED bits to outputs
PTD->PDDR |= (MASK(LED3_POS));


	
/* Set port C bits 30, 23, 22 to inputs */

PTE->PDDR &= ~( GPIO_PDDR_PDD(SW_LT_POS) |

GPIO_PDDR_PDD(SW_RT_POS) |

GPIO_PDDR_PDD(SW_CR_POS) );

}


void Test_Switches_And_LED(void) 
	{

	unsigned switch_code;

	while (1) 
		{


		switch_code = ~READ_SWITCHES;

			if (switch_code & MASK(SW_LT_POS))

		{
				
			PTB->PCOR = 1UL << LED1_POS;
			
			PTB->PSOR = 1UL	<< LED2_POS;
			PTD->PSOR = 1UL	<< LED3_POS;
		}

		else if(switch_code & MASK(SW_RT_POS))
		{
			PTB->PCOR = 1UL	<< LED2_POS;
			
			PTB->PSOR = 1UL << LED1_POS;
		  PTD->PSOR = 1UL	<< LED3_POS;
			
		}

		else if(switch_code & MASK(SW_CR_POS))

		{
			PTD->PCOR = 1UL	<< LED3_POS;
			
			PTB->PSOR = 1UL << LED1_POS;
			PTB->PSOR = 1UL	<< LED2_POS;
		  
			

		}		

			else

		{
			
		  PTB->PSOR = 1UL	<< LED1_POS;
			PTB->PSOR = 1UL << LED2_POS;
		  PTD->PSOR = 1UL	<< LED3_POS;

		}	

	}	

} 

int main (void)

{

//Init_LCD();

//Clear_LCD();
	
init_3way_switch();
	
//Set_Cursor(0,0);

//Print_LCD("Hello World!");
	
	
Test_Switches_And_LED();

} 
