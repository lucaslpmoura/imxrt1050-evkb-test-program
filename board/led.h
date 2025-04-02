/*
 * led.h
 *
 *  Created on: 2. 4. 2025
 *      Author: xlouzal00
 */

#ifndef LED_H_
#define LED_H_

//Led Functions
void Led_TurnOn(void);
void Led_TurnOff(void);
void Led_Toggle(void);

//SysTick Functions
void SysTick_Handler(void);
void SysTick_DelayTicks(uint32_t n);
void SysTick_ConfigureInterrupt(uint32_t n);

#endif /* LED_H_ */
