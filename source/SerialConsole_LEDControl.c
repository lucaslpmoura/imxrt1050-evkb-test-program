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
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
#define CARRIAGE_RETURN PUTCHAR('\r')
#define NOT_IMPLEMENTED PRINTF("\r\nNot Implemented!\n");

typedef enum{
	USER,
	BLINKING
} led_control_state_t;


volatile uint32_t g_systickCounter; // Counter used by the delay function
volatile bool led_state = false; // current led state: false is OFF, true is ON

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

void Global_Led_Toggle(){
	led_state = Led_Toggle(led_state);
}

//------- SysTick Functions -----------
/*
 * @brief Decrements the counter every time the interrupt is called
 */ TODO: Fix this, not working at all
volatile uint32_t i = 0;
void SysTick_Handler(void)
{
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
        PRINTF("%d\n\r", i);
        i++;
    }
}

/*
 * @brief Configures the Systick interrupt to happen every n milliseconds
 * @params uint32_t n -> number of milliseconds
 */
void SysTick_ConfigureInterrupt(uint32_t n){
	if (SysTick_Config(SystemCoreClock / n))
	{
		while (1)
		{
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

	uint32_t led_blink_interval = 1000000; // Led Blinking interval in Milliseconds?
	led_control_state_t led_control_state = USER;

	BOARD_InitHardware();
	Led_TurnOff();


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
    			PRINTF("\n Stopped blinking Led.\n");
    		}
    		if(led_control_state == BLINKING){
    			SysTick_ConfigureInterrupt(led_blink_interval);
    		}

    		break;
    	case '3':
    		running = false;
    		break;
    	default:
    		PRINTF("\r\nPlease enter an valid option.\n");
    		break;
    	}
    }

    PRINTF(farewellMessage);
    Led_TurnOff();


}
