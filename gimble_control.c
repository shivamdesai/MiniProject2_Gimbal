/*
	Elecanisms Mini-Project II using a PIC18F
	
	Shivam Desai and Asa Eckert-Erdheim
	September 21st, 2013
	
	Questions? 		   [Shivam@students.olin.edu; Asa@students.olin.edu]
*/

/***************************************************** 
		Includes
**************************************************** */

// Include files
#include <p24FJ128GB206.h> // PIC
#include "config.h"
#include "common.h"
#include "oc.h"			   // output compare
#include "pin.h"
#include "timer.h"
#include "uart.h"
#include "ui.h"
#include "usb.h"
#include <stdio.h>

// Define vendor requests
#define SET_VALS    1   // Vendor request that receives 2 unsigned integer values
#define GET_VALS    2   // Vendor request that returns  2 unsigned integer values
#define PRINT_VALS  3   // Vendor request that prints   2 unsigned integer values 

// Define names for pins
#define SERVO_PAN  &D[0] // servo pin
#define SERVO_TILT &D[1] // servo pin

// Define constants for servo motors
#define interval  20e-3
#define min_width .8e-3
#define max_width 2.2e-3
#define pos		  0

/***************************************************** 
		Function Prototypes & Variables
**************************************************** */ 

void initChip(void);

uint16_t PAN_VAL;
uint16_t TILT_VAL;

/*************************************************
			Initialize the PIC
**************************************************/

void initChip(){
	
    init_clock();
    init_uart();
    init_pin(); 	// initialize the pins for POTENTIOMETER
    init_ui();		// initialize the user interface for BLINKY LIGHT
    init_timer();	// initialize the timer for BLINKY LIGHT
    init_oc(); 		// initialize the output compare module for SERVO

	pin_digitalOut(SERVO_PAN);	// configure SERVO as output
	pin_digitalOut(SERVO_TILT);	// configure SERVO as output

    oc_servo(&oc1, SERVO_PAN,  &timer3, interval, min_width, max_width, pos); // configure servo control
	oc_servo(&oc2, SERVO_TILT, &timer4, interval, min_width, max_width, pos); // configure servo control

}

/*************************************************
			Vendor Requests
**************************************************/

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case SET_VALS:
            PAN_VAL = USB_setup.wValue.w;
            TILT_VAL = USB_setup.wIndex.w;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_VALS:
            temp.w = PAN_VAL;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            temp.w = TILT_VAL;
            BD[EP0IN].address[2] = temp.b[0];
            BD[EP0IN].address[3] = temp.b[1];
            BD[EP0IN].bytecount = 4;    // set EP0 IN byte count to 4
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;            
        case PRINT_VALS:
            printf("PAN_VAL = %u, TILT_VAL = %u\n", PAN_VAL, TILT_VAL);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main(void) {
	
	initChip();

    PAN_VAL  = 0;
    TILT_VAL = 0;

    InitUSB();                      // initialize the USB registers and serial interface engine

    led_on(&led1);					// initial state for BLINKY LIGHT
    led_on(&led2);					// initial state for BLINKY LIGHT
    timer_setPeriod(&timer1, 1);	// timer for BLINKY LIGHT
    timer_start(&timer1);
    timer_setPeriod(&timer2, 1);	// timer for BLINKY LIGHT
    timer_start(&timer2);

    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        
        ServiceUSB();                       // ...service USB requests

        if (timer_flag(&timer1)) {	// when the timer trips
            timer_lower(&timer1);
            led_toggle(&led1);		// toggle the BLINKY LIGHT
        }
        
    }

    while (1) {
        
        ServiceUSB();               // service any pending USB requests

        if (timer_flag(&timer2)) {	// when the timer trips
            timer_lower(&timer2);
            led_toggle(&led2);		// toggle the BLINKY LIGHT
        }
        
        pin_write(SERVO_PAN,  PAN_VAL);  // control SERVO_PAN  with LEFT/RIGHT KEYS
        pin_write(SERVO_TILT, TILT_VAL); // control SERVO_TILT with UP/DOWN	 KEYS

    }
}
