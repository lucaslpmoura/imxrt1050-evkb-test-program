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

volatile uint32_t g_systickCounter; // Counter used by the delay function

//----- LED Functions ------
// 0U turs it on and 1U turns it off (thanks NXP...)
void Led_TurnOn(void){
	GPIO_PinWrite(LED0_GPIO, LED0_GPIO_PIN, 0U);
	led_state = true;
}

void Led_TurnOff(void){
	GPIO_PinWrite(LED0_GPIO, LED0_GPIO_PIN, 1U);
	led_state = false;
}

void Led_Toggle(bool led_state){
	if(!led_state){
		Led_TurnOn();
		return;
	}else{
		Led_TurnOff();
		return;

	}

}

//------- SysTick Functions -----------
/*
 * @brief Decrements the counter every time the interrupt is called
 */
void SysTick_Handler(void)
{
    if (g_systickCounter != 0U)
    {
        g_systickCounter--;
    }
}

/*
 * @brief Delays execution by the specified number of ticks
 * @params uint32_t n -> number of ticks
 */
void SysTick_DelayTicks(uint32_t n)
{
    g_systickCounter = n;
    while (g_systickCounter != 0U)
    {
    }
}

/*
 * @brief Configuress the Systick interrupt to happen every n milliseconds
 * @params uint32_t n -> number of milliseconds
 */
void SysTick_ConfigureInterrupt(uint32_t n){
	if (SysTick_Config(SystemCoreClock / 1000U))
	{
		while (1)
		{
		}
	}
}
