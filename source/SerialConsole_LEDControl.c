/*
 * Copyright 2016-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    SerialConsole_LEDControl.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include <stdint.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

#include "hardware_init.h"
#include "led.h"
#include "button.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
#define CARRIAGE_RETURN PUTCHAR('\r')
#define NOT_IMPLEMENTED PRINTF("\r\nNot Implemented!\n");

#define INTERRUPTS_PER_SECOND 32

typedef enum{
	USER,
	BLINKING
} led_control_state_t;



volatile bool led_state = false; // current led state: false is OFF, true is ON
led_control_state_t led_control_state = USER; // Informs if Led is blinking or on User (Manual) mode

volatile bool button_state = false;

volatile uint32_t systick_cycle_target = 0; // The number of Systicks that the handler should wait to toggle the Led
volatile uint32_t systick_cycle_counter = 0; // Counts how many Systicks have occoured so far


uint32_t led_blink_possible_intervals[] = {1000,750, 500, 250, 100};
uint32_t led_blink_index = 0;
uint32_t led_blink_interval = 1000;

#define NUM_OF_INTERVALS 5

void SysTick_ConfigureInterrupt(uint32_t n);

/*
 * @brief Prints the options menu
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
void Button_OnPressed(void){
	GPIO_PortClearInterruptFlags(SW8_GPIO, 1U << SW8_GPIO_PIN);


	if(led_control_state == BLINKING) {
		led_blink_index++;
		led_blink_interval = led_blink_possible_intervals[(led_blink_index) % NUM_OF_INTERVALS];
		PRINTF("Blinking interval changed to %d ms\n\r", led_blink_interval);
		led_state = Led_TurnOff();
		systick_cycle_target = led_blink_interval/INTERRUPTS_PER_SECOND;

	}


	SDK_ISR_EXIT_BARRIER;
}

//------- SysTick Functions -----------
/*
 * @brief Decrements the counter every time the interrupt is called
 * TODO: Fix this, not working at all
 */
volatile uint32_t i = 0;

void SysTick_Handler(void)
{
	systick_cycle_counter++;
	if(systick_cycle_counter >= systick_cycle_target) {
		if(led_control_state == BLINKING){
			led_state = Led_Toggle(led_state);
		}
		systick_cycle_counter = 0;
	}
}

/*
 * @brief Configures the Systick interrupt to happen every n milliseconds
 * @params uint32_t n -> number of milliseconds
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

	bool running = true;
	char option = '0';
	char *farewellMessage = "\r\n Terminating program.";




	BOARD_InitHardware();
	Button_Init();
	led_state = Led_TurnOff();

	SysTick_ConfigureInterrupt(led_blink_interval);

    Print_Menu(led_control_state);

    while(running){
    	option = GETCHAR();

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
    	case '3':
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
