/*
 * led.c
 *
 *  Created on: 2. 4. 2025
 *      Author: Lucas Moura
 */


#include "pin_mux.h"
#include "board.h"
#include <stdint.h>
#include "fsl_gpio.h"

// Redefining Built-in LED constants with more friendly names
#define LED0_GPIO     GPIO1
#define LED0_GPIO_PIN 9U


//----- LED Functions ------
// 0U turs it on and 1U turns it off (thanks NXP...)
bool Led_TurnOn(void){
	GPIO_PinWrite(LED0_GPIO, LED0_GPIO_PIN, 0U);
	return true;
}

bool Led_TurnOff(void){
	GPIO_PinWrite(LED0_GPIO, LED0_GPIO_PIN, 1U);
	return false;
}

bool Led_Toggle(bool led_state){
	if(!led_state){
		Led_TurnOn();
		return true;
	}else{
		Led_TurnOff();
		return false;

	}

}


