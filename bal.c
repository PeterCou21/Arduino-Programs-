#include <MKL25Z4.H>
#include "constants.h"
#include "timers.h"
#include "queue.h"
#include "led.h"
#include <string.h>

Q_T queue;
char kp_key, last_key;
char kp_code[5];
int kp_pos = 0;
int armed = 0, triggered = 0, last_trig = 0;
int da_trig = 0, cy_count = 0;
int pir_val = 0, last_pir = 0;

const unsigned char kp_keys[KP_ROWS][KP_COLS] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

void init_LPTMR(void)
{
	// Enable the low-power timer
	SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK;
	LPTMR0_CSR = 0;
}

void start_LPTMR(unsigned int time_s)
{
	// Set CMR to time in milliseconds
	LPTMR0_CMR = time_s * 1000;
	LPTMR0_PSR = LPTMR_PSR_PCS(1) | LPTMR_PSR_PBYP_MASK;
	// Start the timer
	LPTMR0_CSR = LPTMR_CSR_TEN_MASK;
}

void stop_LPTMR(void)
{
	// Stop the timer
	LPTMR0_CSR = 0;
}

void init_DAC(void)
{
  // Init DAC output
	
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK; 
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; 
	
	PORTE->PCR[DAC_POS] &= ~(PORT_PCR_MUX(7));	// Select analog 
		
	// Disable buffer mode
	DAC0->C1 = 0;
	DAC0->C2 = 0;
	
	// Enable DAC, select VDDA as reference voltage
	DAC0->C0 = (1UL << DAC_C0_DACEN_SHIFT) |
						 (1UL << DAC_C0_DACRFS_SHIFT);
}

void init_PIR()
{
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
  //PIR_PORT->PCR[PIR_PIN] &= ~PORT_PCR_MUX_MASK;
  PIR_PORT->PCR[PIR_PIN] = PORT_PCR_MUX(1);
  PIR_PT->PDDR &= ~(1UL << PIR_PIN);
}
	

void init_keypad()
{
	// Set up clock gating registers
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK;

	//KP_C_PORT->PCR[KP_C0] &= ~PORT_PCR_MUX_MASK;
	KP_C_PORT->PCR[KP_C0] = PORT_PCR_MUX(1);
	KP_C_PT->PDDR &= ~(1UL << KP_C0);
	//KP_C_PORT->PCR[KP_C1] &= ~PORT_PCR_MUX_MASK;
	KP_C_PORT->PCR[KP_C1] = PORT_PCR_MUX(1);
	KP_C_PT->PDDR &= ~(1UL << KP_C1);
	//KP_C_PORT->PCR[KP_C2] &= ~PORT_PCR_MUX_MASK;
	KP_C_PORT->PCR[KP_C2] = PORT_PCR_MUX(1);
	KP_C_PT->PDDR &= ~(1UL << KP_C2);
	//KP_C_PORT->PCR[KP_C3] &= ~PORT_PCR_MUX_MASK;
	KP_C_PORT->PCR[KP_C3] = PORT_PCR_MUX(1);
	KP_C_PT->PDDR &= ~(1UL << KP_C3);
		
	//KP_R_PORT->PCR[KP_R0] &= ~PORT_PCR_MUX_MASK;
	KP_R_PORT->PCR[KP_R0] = PORT_PCR_MUX(1);
	KP_R_PT->PDDR |= (1UL << KP_R0);
	//KP_R_PORT->PCR[KP_R1] &= ~PORT_PCR_MUX_MASK;
	KP_R_PORT->PCR[KP_R1] = PORT_PCR_MUX(1);
	KP_R_PT->PDDR |= (1UL << KP_R1);
	//KP_R_PORT->PCR[KP_R2] &= ~PORT_PCR_MUX_MASK;
	KP_R_PORT->PCR[KP_R2] = PORT_PCR_MUX(1);
	KP_R_PT->PDDR |= (1UL << KP_R2);
	//KP_R_PORT->PCR[KP_R3] &= ~PORT_PCR_MUX_MASK;
	KP_R_PORT->PCR[KP_R3] = PORT_PCR_MUX(1);
	KP_R_PT->PDDR |= (1UL << KP_R3);
}

char read_keypad()
{
	int c0,c1,c2,c3;
	int key_col = 4, key_row = 4;
	
	for (int p = 0; p < KP_ROWS; p++) {
		// Set the row high
		KP_R_PT->PSOR = (1UL << (KP_R0+p));
		// Read each column of the row
		c0 = (KP_C_PT->PDIR >> KP_C0) & 1;
		c1 = (KP_C_PT->PDIR >> KP_C1) & 1;
		c2 = (KP_C_PT->PDIR >> KP_C2) & 1;
		c3 = (KP_C_PT->PDIR >> KP_C3) & 1;
		// Set the row low
		KP_R_PT->PCOR = (1UL << (KP_R0+p));
		
		// Decode the information
		if (c0+c1+c2+c3 > 1) {
				// Multiple keys are pressed, return null
				return '\0';
		}
		else if (c0+c1+c2+c3 == 0) {
			// No keys are pressed, just do nothing
			;
		}
		else if (key_row != 4) {
			// Key was pressed on previous row, return null
			return '\0';
		}
		else {
			if (c0) {key_col = 0; key_row = p;}
			else if (c1) {key_col = 1; key_row = p;}
			else if (c2) {key_col = 2; key_row = p;}
			else if (c3) {key_col = 3; key_row = p;}
		}
		
	}
	if (key_row < 4 && key_col < 4) {
		// Key is pressed
		return kp_keys[key_row][key_col];
	}
	else {
		return '\0';
	}
}

void q_sqwave(unsigned int period, unsigned int num_cycles) {
	unsigned step;
	unsigned short out_data;
	
	set_PIT(period/NUM_STEPS);
	
	while (num_cycles>0) {
		num_cycles--;
		for (step = 0; step < NUM_STEPS; step++) {
					if (step < NUM_STEPS/2)
						out_data = 0;	
					else
						out_data = MAX_DAC_CODE;

			while (Q_Full(&queue)) {
				// if queue is full, then wait for it to empty
			}
			Q_Enqueue(&queue, out_data);
		}
	}
}

void q_silence(unsigned int num_cycles) {
	while (num_cycles>0) {
		num_cycles--;
		while (Q_Full(&queue)) {
				// if queue is full, then wait for it to empty
		}
		Q_Enqueue(&queue, 0);
	}
}

int main()
{
	init_LED();
	set_LED(0,1,0);
	init_DAC();
	Q_Init(&queue);
	init_PIR();
	init_LPTMR();
	
	init_keypad();
	
	while(1) {
		kp_key = read_keypad();
		if (kp_key == '\0') {
			if (last_key == '#') {
				// Clear code
				kp_pos = 0;
			}
			else if (last_key == 'A') {
				// Door alarm
				if (armed) {
					set_LED(0,1,1);
					da_trig = 1;
					start_LPTMR(10);
				}
			}
			else if (last_key == 'B') {
				// Window alarm
				if (armed) triggered = 1;
			}
			else if (last_key != '\0') {
				kp_code[kp_pos] = last_key;
				kp_pos++;
			}
		}
		last_key = kp_key;
			
		if (kp_pos == 4) {
			if (strncmp(kp_code, "1245", 4) == 0) {
				if (armed) {
					set_LED(0,1,0); // green
					armed = 0;
					// Just in case...
					stop_LPTMR();
				}
				else {
					set_LED(0,0,1);
					armed = 1;
				}
				triggered = 0;
			}
			else {
				set_LED(1,0,0); // red
			}
			kp_pos = 0;
		}
		
		// Read the PIR and trigger the alarm if it's armed
		pir_val = (PIR_PT->PDIR >> PIR_PIN) & 1;
		if (pir_val && !last_pir && armed) triggered = 1;
		last_pir = pir_val;
		
		if (da_trig) {
			init_PIT(1000/NUM_STEPS);
			start_PIT();
			if ((cy_count & 0x1F) == 1) {
				q_sqwave(1200, 32);
			}
			else {
				q_silence(32);
			}
			cy_count++;
			if (LPTMR0_CSR & LPTMR_CSR_TCF_MASK) {
				// Check the timer for door alarm
				da_trig = 0;
				stop_LPTMR();
				stop_PIT();
				triggered = 1;
				cy_count = 0;
			}
		}
		
		if (triggered) {
			if (!last_trig) {
				// Just triggered
				init_PIT(1000/NUM_STEPS);
				start_PIT();
				start_LPTMR(30);
				// This should happen anyway, just doing it to be safe
				da_trig = 0;
			}
			if ((cy_count & 0x3) < 2) {
				q_sqwave(1000, 32);
			}
			else {
				q_sqwave(800, 32);
			}
			cy_count++;
			set_LED(1,1,1); //white
			if (LPTMR0_CSR & LPTMR_CSR_TCF_MASK) {
				// Timer elapsed
				stop_LPTMR();
				triggered = 0;
				armed = 0;
				set_LED(0,1,0);
				cy_count = 0;
			}
		}
		else {
			if (last_trig) {
				// Just untriggered
				stop_PIT();
			}
		}
		
		last_trig = triggered;
	}
}
