#ifndef __CHECKSUM_H__
#define __CHECKSUM_H__
#include "stm32f4xx_hal.h"

uint16_t Checksum_16 (uint8_t * pointer, uint16_t headerlenght, uint32_t startval);

#endif
