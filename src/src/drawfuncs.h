#pragma once

#include <stdint.h>

//void blitOutlined(const uint8_t *spritesheet, const uint16_t sheetwidth, const uint8_t sheetx, const uint8_t sheety, const int16_t screenx, const int16_t screeny, const uint16_t forecolor, const uint16_t backcolor, const uint32_t flags);
void blitMasked(const uint8_t *spritesheet, const uint16_t sheetwidth, const uint8_t sheetx, const uint8_t sheety, const int16_t screenx, const int16_t screeny, const uint16_t forecolor, const uint16_t backcolor, const uint32_t flags);