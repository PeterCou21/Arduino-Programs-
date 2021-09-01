#include <MKL25Z4.H>
//const uint32_t led_mask = 1UL << 1; // LED is port D pin1
//const uint32_t led_mask = 1UL << 18; // LED is port B pin 18
//const uint32_t led_mask = 1UL << 19; // LED is port B pin 19

const uint32_t led_mask[3]={1UL << 18,1UL << 19,1UL << 1}; // LED is port B pin 19


#define LOOP_COUNT 0x61A80

void Delay(uint32_t nCount)
{
	while(nCount--)
	{
		;
	}
}
 int main (void)
 {
	 /* 
	SIM->SCGC5|=(1UL << 12); // Enable Clock to PORT D
	PORTD->PCR[1] = (1UL << 8); //Pin PTD1 is GPIO 1
	FPTD->PDOR=led_mask; // Switch Blue LED off
	FPTD->PDDR=led_mask; // enable PTD1 as output 
	 */
	 
	/* 
	SIM->SCGC5|=(1UL << 10); // Enable Clock to PORT B
	PORTB->PCR[19] = (1UL << 8);  //Pin PTB19 is GPIO 1 
	FPTB->PDOR=led_mask;  //Switch Green LED off
	FPTB->PDDR=led_mask;  // enable PTB1 as output
	 */
	
	 SIM->SCGC5|=(1UL << 12 |1UL << 10); // Enable Clock to PORT B

	 
	//PORTD->PCR[led_mask[0]] = (1UL << 8); 
	 
	PORTB->PCR[19] = (1UL << 8);
	PORTB->PCR[18] = (1UL << 8);
	PORTD->PCR[1] = (1UL << 8);


	//FPTD->PDOR=(led_mask[0]);
	
	FPTB->PDOR=(led_mask[0]|led_mask[1]);
	FPTD->PDOR=led_mask [2];
		//Switch Red LED off
	 
	 
  //FPTD->PDDR=(led_mask[0]);
	
	FPTB->PDDR=(led_mask[0]|led_mask[1]);
	FPTD->PDDR=led_mask [2];	
 
	while(1)
	{
		/*
		FPTD->PCOR=led_mask;
		Delay(LOOP_COUNT);
		FPTD->PSOR = led_mask;
		Delay(LOOP_COUNT);
		*/
		
		//FPTB->PCOR=(led_mask[0]);
		//Delay(LOOP_COUNT);
		
		//FPTB->PSOR=(led_mask[0]);
		//FPTB->PCOR=(led_mask[1]);		
		//Delay(LOOP_COUNT);
		
		//FPTB->PTOR=(led_mask[0]|led_mask[1]);
		//Delay(LOOP_COUNT);
		
		//FPTB->PSOR=(led_mask[1]);
		//FPTD->PCOR = (led_mask[2]);
		
		
		FPTB->PCOR=(led_mask[0]);
    Delay(1000);		
		
		FPTB->PTOR=(led_mask[0]|led_mask[1]);
		

		
		
		//FPTD->PSOR=(led_mask[2]);
	}
}
