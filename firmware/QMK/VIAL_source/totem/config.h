// Copyright 2022 GEIST @geigeigeist
// Copyright 2026 josericardodainese
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

/* TOTEM 20 -- unibody 20-key macropad, no encoder and no OLED.
 *
 * Everything is driven from the keys themselves: two two-key chords, each
 * requiring a one-second hold, cycle the profile and open the RGB control
 * layer. See the chord table below.
 *
 * NOT the original TOTEM split: no second half, no serial link, no handedness.
 * EE_HANDS, SERIAL_USART_*, SPLIT_* and hand_swap_config are all gone.
 *
 * Board is a Seeed XIAO RP2040. This variant needs only 11 GPIO
 * (6 cols + 4 rows + 1 RGB), which is exactly what the XIAO exposes -- and it
 * matches the project schematic pin for pin, LEDS_SIGNAL on D6 included.
 * A Waveshare RP2040-Zero also works; see readme.md for its pin mapping.
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
#define TOTEM20_PROFILE_GAME     0   // jogo
#define TOTEM20_PROFILE_MAX3DS   1   // 3ds Max
#define TOTEM20_PROFILE_MACROS   2   // macros / uso geral
#define TOTEM20_PROFILE_RESERVED 3   // reservada
#define TOTEM20_PROFILE_RGB      4   // controle de RGB / manutencao
#define TOTEM20_PROFILE_COUNT    5

// ┌─────────────────────────────────────────────────┐
// │ c h o r d s                                     │
// └─────────────────────────────────────────────────┘

/* Two chords, both anchored on "G" so there is a single key to remember:
 *
 *     G + B  held 1 s  ->  next profile   (Jogo -> 3ds Max -> Macros -> Jogo)
 *     G + E  held 1 s  ->  toggle the RGB control layer
 *
 * The three keys involved are the far corners of the board -- G at the bottom
 * left, B and E at the right end of the thumb cluster -- so an accidental
 * simultaneous press during gameplay is essentially impossible.
 *
 * Change only these coordinates to move a chord to another pair of keys.
 */
#define CHORD_ANCHOR_ROW  3
#define CHORD_ANCHOR_COL  0   // SW16 -- "G", shared by both chords

#define CHORD_PROFILE_ROW 3
#define CHORD_PROFILE_COL 5   // SW20 -- "B"

#define CHORD_RGB_ROW     2
#define CHORD_RGB_COL     5   // SW15 -- "E"

/* How long both keys must stay down before a chord fires. */
#define CHORD_HOLD_MS 1000

/* How close together the two presses must land to count as "pressed
 * together". This is also how long each chord key is withheld from the host,
 * so that a recognised chord never leaks its normal keycodes. Raising it makes
 * the chords easier to hit; lowering it cuts the latency added to G, B and E.
 * Set to 0 to disable withholding (chords then leak keycodes).
 */
#define CHORD_SYNC_MS 50

// ┌─────────────────────────────────────────────────┐
// │ m i s c                                         │
// └─────────────────────────────────────────────────┘

#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 500U
