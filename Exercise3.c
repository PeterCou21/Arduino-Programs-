#include <MKL25Z4.H>
#include <math.h>

#define DAC_POS (30)
#define MASK(x) (1UL << (x))
#define MAX_PERIOD (10000)
#define MIN_PERIOD (100)
#define SQUARE (0)
#define RAMP (1)
#define SINE (2)
#define MAX_DAC_CODE (4095)
#define NUM_STEPS (64)

unsigned SineTable[NUM_STEPS];

void Init_DAC(void)
{
	//-----Enable Clock For DAC and PORT E-------
	
	SIM->SCGC6 |=SIM_SCGC6_DAC0_MASK;
  SIM->SCGC5 |=SIM_SCGC5_PORTE_MASK;
	
	//--------------------------------------------
	
	//-----------Selecting Analogue pins----------
	
	PORTE ->PCR[DAC_POS] &=~PORT_PCR_MUX_MASK;
  PORTE ->PCR[DAC_POS] &=~PORT_PCR_MUX(0);
	
  //---------------------------------------------
	
	//------------Disabling Buffer---------------
	
	DAC0 -> C1= 0;
  DAC0 -> C2= 0;
	
	//----------------------------------------------
	
	//---enable DAC, select VDDA as reference voltage----
	
	DAC0 -> C0 = DAC_C0_DACEN_MASK| DAC_C0_DACRFS_MASK;
	
	//----------------------------------------------------
	

}

void Delay_us(volatile unsigned int time_del) 
{
	time_del = 9*time_del + time_del/2;
	while (time_del--) 
	{
		;
	}
}

void Play_Tone(unsigned int period, unsigned int num_cycles, unsigned wave_type)	
{
	unsigned step, out_data;
    
	while (num_cycles>0) 
	{
		num_cycles--;
		for (step = 0; step < NUM_STEPS; step++)
		{
			switch (wave_type) 
			{
				case SQUARE: 
					if (step < NUM_STEPS)
						out_data = 0;
					else
						out_data = MAX_DAC_CODE;
					break;
					
				case RAMP:
						out_data = (step*MAX_DAC_CODE)/NUM_STEPS;
					break;
				
				case SINE:
					out_data = SineTable[step];
				
				  break;
				   default:
				  break;					
			}
			
			DAC0->DAT[0].DATL = DAC_DATL_DATA0(out_data);	
			DAC0->DAT[0].DATH = DAC_DATH_DATA1(out_data >> 8);
			
			Delay_us(period/NUM_STEPS);	
		}
	}	
}

void Init_SineTable(void)
{
	
	for (int n=0; n<NUM_STEPS; n++)
	{
		SineTable[n] = (MAX_DAC_CODE/2)*(1+sin(n*2*3.1415927/NUM_STEPS));
	}
}

int main (void)
{
	Init_DAC();
	Init_SineTable();
	
	
	while (1) 
	{		
		Play_Tone(1000, 5, SQUARE);
		//Play_Tone(1000, 5, RAMP);
		Play_Tone(1000, 5, SINE);
	}
}