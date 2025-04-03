/*
 * button.c
 *
 *  Created on: 3. 4. 2025
 *      Author: Lucas Moura
 *
 *   Borrowed a lot from the igpio_input_interrupt SDK example
 */

#include "button.h"


gpio_pin_config_t sw8_config = {
        kGPIO_DigitalInput,
        0,
        kGPIO_IntRisingEdge,
 };

void Button_Init(void) {
	/* Init input switch GPIO. */
	EnableIRQ(SW8_IRQ);
	GPIO_PinInit(SW8_GPIO, SW8_GPIO_PIN, &sw8_config);

	/* Enable GPIO pin interrupt */
	GPIO_PortEnableInterrupts(SW8_GPIO, 1U << SW8_GPIO_PIN);
}

uint32_t Button_Read(void) {
	return GPIO_PinRead(SW8_GPIO, SW8_GPIO_PIN);
}

