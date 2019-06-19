/* tng-di8
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * main.c: Initialization for TNG-DI8
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "configs/config.h"

#include "bricklib2/logging/logging.h"

const uint32_t boot_info __attribute__ ((section(".boot_info"))) = 0;

typedef void (* firmware_start_func_t)(void);

static void jump_to_firmware() {
	register firmware_start_func_t firmware_start_func;
	const uint32_t stack_pointer_address = STM32F0_FIRMWARE_ACTIVE_POS_STACK_POINTER;
	const uint32_t reset_pointer_address = STM32F0_FIRMWARE_ACTIVE_POS_STACK_POINTER + 4;

	// Set stack pointer with the first word of the run mode program
	// Vector table's first entry is the stack pointer value
	__set_MSP((*(uint32_t *)stack_pointer_address));

	// Set the program counter to the application start address
	// Vector table's second entry is the system reset value
	firmware_start_func = * ((firmware_start_func_t *)reset_pointer_address);
	firmware_start_func();
}

int main(void) {
//	logging_init();
//	logd("Start TNG Bootloader\n\r");

	jump_to_firmware();

	while(true) {
	}
}
