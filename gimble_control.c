/*
	Elecanisms Mini-Project I using a PIC18F
	
	Shivam Desai and Asa Eckert-Erdheim
	September 10th, 2013
	
	Questions? 		   [Shivam@students.olin.edu; Asa@students.olin.edu]
*/

/***************************************************** 
		Includes
**************************************************** */

// Include files
#include <p24FJ128GB206.h> // PIC
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"			   // output compare

// Define names for pins
#define POT   &A[0] // potentiometer pin
#define SERVO &D[0] // servo pin

// Define constants for servo
#define interval  20e-3
#define min_width .8e-3
#define max_width 2.2e-3
#define pos		  0

/***************************************************** 
		Function Prototypes & Variables
**************************************************** */ 

void initChip(void);

uint16_t pot_val;

/*************************************************
			Initialize the PIC
**************************************************/

void initChip(){
	
    init_clock();
    init_ui();		// initialize the user interface for BLINKY LIGHT
    init_timer();	// initialize the timer for BLINKY LIGHT
    init_pin(); 	// initialize the pins for POTENTIOMETER
    init_oc(); 		// initialize the output compare module for SERVO

	pin_analogIn(POT);		// configure POT as input
	pin_digitalOut(SERVO);	// configure SERVO as output
    oc_servo(&oc1, SERVO, &timer3, interval, min_width, max_width, pos); // configure servo control

}

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main(void) {
	
	initChip();
	
    led_on(&led1);					// initial state for BLINKY LIGHT
    timer_setPeriod(&timer2, 1);	// timer for BLINKY LIGHT
    timer_start(&timer2);
		
    while (1) {
        if (timer_flag(&timer2)) {	// when the timer trips
            timer_lower(&timer2);
            led_toggle(&led1);		// toggle the BLINKY LIGHT
        }
        
        pot_val = pin_read(POT);   // read POT value
        led_write(&led2, pot_val); // make sure POT is read
        pin_write(SERVO, pot_val); // control SERVO with POT
    }
}
