/*
 * button.h
 *
 *  Created on: 3. 4. 2025
 *      Author: xlouzal00
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "fsl_gpio.h"
#include "board.h"
#include "fsl_common_arm.h"

#define SW8_IRQ BOARD_USER_BUTTON_IRQ
#define SW8_IRQHandler BOARD_USER_BUTTON_IRQ_HANDLER
#define Button_OnPressed SW8_IRQHandler //IRQ Handler define by the user

#define SW8_GPIO BOARD_USER_BUTTON_GPIO // GPIO5
#define SW8_GPIO_PIN BOARD_USER_BUTTON_GPIO_PIN // 0U


void Button_Init(void);
uint32_t Button_Read(void);

#endif /* BUTTON_H_ */
