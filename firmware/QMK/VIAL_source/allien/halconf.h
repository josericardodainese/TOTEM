// Copyright 2026 josericardodainese
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

/* The SSD1306 OLED talks I2C, which ChibiOS leaves out of the build unless
 * asked for. The peripheral itself (I2C1) is already enabled by the stock
 * RP2040 mcuconf.h, so no mcuconf override is needed here.
 */
#define HAL_USE_I2C TRUE

#include_next <halconf.h>
