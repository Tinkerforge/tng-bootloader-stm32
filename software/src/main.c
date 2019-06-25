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

#include <stdbool.h>
#include <stdint.h>

#include "bricklib2/tng/tng_firmware.h"

const uint32_t boot_info __attribute__ ((section(".boot_info"))) = 0;

typedef void (* firmware_start_func_t)(void);

// Dummy GetTick implementation for flash functions
// We disable timeout support by always returning 0...
uint32_t HAL_GetTick(void) {
	return 0;
}

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

static void copy_new_firmware_to_active_firmware(void) {
	uint32_t length = tng_firmware_get_length();
	if((length % 4) != 0) {
		length += (4 - (length % 4));
	}

	HAL_FLASH_Unlock();

	FLASH_EraseInitTypeDef erase_init = {
		.TypeErase = FLASH_TYPEERASE_PAGES,
		.PageAddress = STM32F0_FIRMWARE_ACTIVE_POS_START,
		.NbPages = length/FLASH_PAGE_SIZE + 1
	};
	uint32_t page_error = 0;
	if(HAL_FLASHEx_Erase(&erase_init, &page_error) != HAL_OK) {
		HAL_FLASH_Lock();
		return;
	}

	for(uint32_t i = 0; i < length; i += 4) {
		uint32_t *data = ((uint32_t*)(STM32F0_FIRMWARE_NEW_POS_START + i));
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, STM32F0_FIRMWARE_ACTIVE_POS_START + i, *data) != HAL_OK) {
			HAL_FLASH_Lock();
			return;
		}
	}
	HAL_FLASH_Lock();
}

static bool check_copied_firmware(void) {
	uint32_t length = tng_firmware_get_length();
	if((length % 4) != 0) {
		length += (4 - (length % 4));
	}

	for(uint32_t i = 0; i < length; i += 4) {
		uint32_t *data_active = ((uint32_t*)(STM32F0_FIRMWARE_ACTIVE_POS_START + i));
		uint32_t *data_new =    ((uint32_t*)(STM32F0_FIRMWARE_NEW_POS_START + i));
		if(*data_active != *data_new) {
			return false;
		}
	}

	return true;
}

int main(void) {
	const uint32_t boot_info = tng_firmware_get_boot_info();
	if(boot_info == 0) {
		// TODO: Check CRC in active firmware and flash red LED if not OK maybe?
		jump_to_firmware();
	} else {
		const uint32_t status = tng_firmware_check_all();
		if(status == TNG_FIRMWARE_COPY_STATUS_OK) {
			bool copied_firmware_ok = false;
			while(!copied_firmware_ok) {
				copy_new_firmware_to_active_firmware();
				copied_firmware_ok = check_copied_firmware();
			}
		}

		tng_firmware_set_boot_info(0);
		jump_to_firmware();
	}

	while(true) {
		__NOP();
	}
}
