/*
 * Copyright 2016-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    SerialConsole_LEDControl.c
 * @brief   An introduction to programming the IMXRT1050 EVKB evalutaion board. .
 */


#include <stdio.h>
#include <stdint.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

// Custom Libraries
#include "hardware_init.h"
#include "led.h"
#include "button.h"


#define CARRIAGE_RETURN PUTCHAR('\r') // Macro for the carriage return char (i dont know why i did this in this way)
#define NOT_IMPLEMENTED PRINTF("\r\nNot Implemented!\n"); // For development purposes


// See Systick_ConfigureInterrupt() function for explanation.
#define INTERRUPTS_PER_SECOND 32


/* Enum defining the modes of operation of the built in Led.
* In USER mode, the user can manually toggle the Led through the serial console interface.
* In BLINKING mode, activated through the user menu, the Led blinks, and its speed can be adjusted
* by pressing the user switch on the board (SW8)
*/
typedef enum{
	USER,
	BLINKING
} led_control_state_t;



volatile bool led_state = false; // current led state: false is OFF, true is ON
led_control_state_t led_control_state = USER; // Informs if Led is blinking or on User (Manual) mode

volatile bool button_state = false;

volatile uint32_t systick_cycle_target = 0; // The number of Systicks that the handler should wait to toggle the Led
volatile uint32_t systick_cycle_counter = 0; // Counts how many Systicks have occoured so far

#define NUM_OF_INTERVALS 5
#define INTERVAL_DEFAULT_VALUE 1000
uint32_t led_blink_possible_intervals[] = {1000,750, 500, 250, 100}; // Predefined led blinking intervals
uint32_t led_blink_index = 0; // Current interval selected
uint32_t led_blink_interval = INTERVAL_DEFAULT_VALUE; // Current interval value



/*
 * @brief Prints the options menu on the serial console
 */
void Print_Menu(led_control_state_t led_control_state){
	CARRIAGE_RETURN;
	PRINTF("-----MENU-----\n");
	CARRIAGE_RETURN;

	PRINTF("(1) Toggle the Led State\n");

	CARRIAGE_RETURN;

	if(led_control_state == USER){
		PRINTF("(2) Start Blinking LED\n");
	}
	if(led_control_state == BLINKING){
		PRINTF("(2) Stop Blinking LED\n");
	}


	CARRIAGE_RETURN;
	PRINTF("(3) Quit Program\n");
	CARRIAGE_RETURN;
}

/*
 * @brief Toggles the current led control state
 */
void LedControlState_Toggle(led_control_state_t *led_control_state){
	if(*led_control_state == USER){
		*led_control_state = BLINKING;
		return;
	}
	if(*led_control_state == BLINKING){
		*led_control_state = USER;
		return;
	}
}


//TODO: Add Debouncing
/*
 * @brief Function called by the IRQ when the user button is pressed
 */

void Button_OnPressed(void){
	// Clears interrupt flags
	GPIO_PortClearInterruptFlags(SW8_GPIO, 1U << SW8_GPIO_PIN);


	// If the Led is in Blinking mode, cycles trough its intervals values
	if(led_control_state == BLINKING) {
		led_blink_index++;
		led_blink_interval = led_blink_possible_intervals[(led_blink_index) % NUM_OF_INTERVALS];

		PRINTF("Blinking interval changed to %d ms\n\r", led_blink_interval);

		led_state = Led_TurnOff();
		systick_cycle_target = led_blink_interval/INTERRUPTS_PER_SECOND;

	}

	//I dont know what that does, but it was in the example so i'm keeping it.
	SDK_ISR_EXIT_BARRIER;
}

//------- SysTick Functions -----------
/*
 * @brief Function called when the Systick interrupt is called.
 *
 */
//volatile uint32_t i = 0;

void SysTick_Handler(void)
{
	// Incremets the systick counter
	systick_cycle_counter++;

	// If it reaches the target value, and Led is in blinking mode, toggles it
	if(systick_cycle_counter >= systick_cycle_target) {
		if(led_control_state == BLINKING){
			led_state = Led_Toggle(led_state);
		}

		systick_cycle_counter = 0;
	}
}

/*
 * @brief Configures the Systick interrupt to happen almost 32 seconds per second (31.47)
 * The Systick_Config() function is an built-in function of the SDK, and its parameter is the number
 * of ticks that the processor waits to trigger the interrupt. By setting it to the maximum value possible,
 * (see function definition) the interrupt occurs almost every 32 ticks (528000000 Hz / 0xFFFFFF = 31.47)
 *
 */
void SysTick_ConfigureInterrupt(uint32_t n){
	if (SysTick_Config(0xFFFFFFUL) == 0) // Started interrupt successfully
	{
		systick_cycle_target = n/INTERRUPTS_PER_SECOND;

	}else { // Maybe it is good to add some sort of error handling
		while(1) {

		}
	}
}

/*
 * @brief   Application entry point.
 */
int main(void) {

	bool running = true; //Running flag
	char option = '0'; // Option selected by the user
	char *farewellMessage = "\r\n Terminating program.";




	BOARD_InitHardware(); // Initializes GPIO, Led and interrupts
	Button_Init(); // Initializes the interrupt for the user button (SW8)
	led_state = Led_TurnOff(); // Start execution with the Led turned off

	SysTick_ConfigureInterrupt(led_blink_interval); // Configures the blinking interrupt

    Print_Menu(led_control_state);

    while(running){
    	option = GETCHAR(); // Reads from serial input

    	PUTCHAR('\r');
    	PUTCHAR('\n');
    	PUTCHAR(option);
    	switch(option){
    	case '1': // Toggle the Led
        	Print_Menu(led_control_state);
        	led_state = Led_Toggle(led_state);

        	CARRIAGE_RETURN;
        	if(led_state){
        		PRINTF("\nTurning Led On...\n");
        	}else{
        		PRINTF("\nTurning Led Off.. \n");
        	}
        	CARRIAGE_RETURN;

    		break;
    	case '2': // Toggle Led blinking
       		LedControlState_Toggle(&led_control_state);
    		Print_Menu(led_control_state);

    		if(led_control_state == USER){
    			led_state = Led_TurnOff();
    			led_blink_interval = 1000;
    			PRINTF("\n Stopped blinking Led.\n\r");
    		}
    		if(led_control_state == BLINKING){
    			PRINTF("\n Started blinking Led with an interval of %d ms.\n\r", led_blink_interval);
    		}

    		break;
    	case '3': // Stops execution
    		running = false;
    		break;
    	default:
    		PRINTF("\r\nPlease enter an valid option.\n");
    		CARRIAGE_RETURN;
    		break;
    	}
    }

    PRINTF(farewellMessage);
    Led_TurnOff();


}
