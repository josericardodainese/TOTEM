// Copyright 2022 GEIST @geigeigeist
// Copyright 2026 josericardodainese
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

/* Allien -- unibody 20-key macropad + rotary encoder + SSD1306 OLED,
 * on a Waveshare RP2040-Zero.
 *
 * NOT the original TOTEM split: no second half, no serial link, no handedness.
 * EE_HANDS, SERIAL_USART_*, SPLIT_* and hand_swap_config are all gone.
 *
 * The board changed from the Seeed XIAO RP2040 because the XIAO exposes only
 * 11 GPIO and the full feature set needs 15:
 *     6 cols + 4 rows + 1 RGB + 2 encoder + 2 I2C = 15
 * The encoder push switch costs nothing -- it is an ordinary matrix key at
 * [0,5], one of the positions the 4x6 matrix leaves empty.
 *
 * MATRIX_ROWS / MATRIX_COLS are derived from matrix_pins in keyboard.json.
 */

// ┌─────────────────────────────────────────────────┐
// │ p r o f i l e s                                 │
// └─────────────────────────────────────────────────┘

/* Five configurable profiles. Must stay in sync with
 * DYNAMIC_KEYMAP_LAYER_COUNT in keymaps/vial/{config.h,rules.mk}
 * and with the keymaps[] array in every keymap.c.
 */
#define ALLIEN_PROFILE_GAME     0   // jogo
#define ALLIEN_PROFILE_MAX3DS   1   // 3ds Max
#define ALLIEN_PROFILE_MACROS   2   // macros / uso geral
#define ALLIEN_PROFILE_RESERVED 3   // reservada
#define ALLIEN_PROFILE_CONFIG   4   // configuracao / manutencao
#define ALLIEN_PROFILE_COUNT    5

// ┌─────────────────────────────────────────────────┐
// │ p r o f i l e   s w i t c h   c h o r d         │
// └─────────────────────────────────────────────────┘

/* Two-key chord that cycles profiles: the outer ends of the bottom row,
 * "G" at the far left and "B" at the far right of the thumb cluster.
 * Still available even though the encoder click now does the same thing --
 * it works with the encoder unplugged, and costs nothing to keep.
 * Change only these four coordinates to move the chord elsewhere.
 */
#define PROFILE_KEY_A_ROW 3
#define PROFILE_KEY_A_COL 0   // SW16 -- "G"
#define PROFILE_KEY_B_ROW 3
#define PROFILE_KEY_B_COL 5   // SW20 -- "B"

#define PROFILE_SWITCH_HOLD_MS 1000   // both keys held this long -> switch
#define PROFILE_CHORD_SYNC_MS    50   // presses must land this close together

// ┌─────────────────────────────────────────────────┐
// │ e n c o d e r   +   O L E D                     │
// └─────────────────────────────────────────────────┘

/* Encoder push switch, wired into the matrix like any other key.
 * Short click  -> next profile
 * Long  click  -> open the OLED menu / confirm the selected item
 */
#define ENCODER_BTN_ROW 0
#define ENCODER_BTN_COL 5
#define ENCODER_LONG_PRESS_MS 500

/* Most EC11 encoders emit 4 pulses per detent. Lower this to 2 if the menu
 * skips two items per click; raise it if it takes two clicks to move one.
 */
#define ENCODER_RESOLUTION 4

/* Menu returns to the status screen after this long without input. */
#define OLED_MENU_TIMEOUT_MS 6000

/* OLED on I2C1, pins GP10 (SDA) / GP11 (SCL).
 *
 * The pins are NOT arbitrary: the RP2040 muxes I2C onto fixed pin groups, and
 * GP10/GP11 is a valid I2C1 SDA/SCL pair. I2C1 specifically, because ChibiOS
 * ships with RP_I2C_USE_I2C1 TRUE and RP_I2C_USE_I2C0 FALSE -- picking I2C0
 * would additionally require a custom mcuconf.h to switch the peripheral on.
 *
 * Despite the "I2C1_" prefix, these two macro names are what QMK's chibios
 * i2c_master driver reads regardless of which peripheral I2C_DRIVER selects.
 */
#undef I2C_DRIVER
#define I2C_DRIVER I2CD1
#undef I2C1_SDA_PIN
#define I2C1_SDA_PIN GP10
#undef I2C1_SCL_PIN
#define I2C1_SCL_PIN GP11

#define OLED_DISPLAY_128X64
/* OLEDs burn in. Blank the panel after 60 s idle; any key or turn wakes it. */
#define OLED_TIMEOUT 60000
#define OLED_BRIGHTNESS 160

// ┌─────────────────────────────────────────────────┐
// │ m i s c                                         │
// └─────────────────────────────────────────────────┘

#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 500U
