// Copyright 2022 GEIST @geigeigeist
// Copyright 2026 josericardodainese
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

/* Freshly generated UID. The Allien is a different device from the original
 * TOTEM split (different matrix, pinout, key count and controller), so it must
 * NOT reuse the upstream TOTEM UID -- Vial caches its keyboard definition by
 * this value and would otherwise load the wrong 38-key layout.
 * Regenerate with:  python3 -c "import os;print(', '.join('0x%02X'%b for b in os.urandom(8)))"
 */
#define VIAL_KEYBOARD_UID \
    { 0x93, 0x98, 0x9C, 0xF2, 0x7A, 0xF6, 0x9C, 0x29 }

/* Security unlock combo: the two outermost keys of the top row,
 * "1" (row 0, col 1) and "R" (row 0, col 4).
 */
#define VIAL_UNLOCK_COMBO_ROWS \
    { 0, 0 }
#define VIAL_UNLOCK_COMBO_COLS \
    { 1, 4 }

#define TAPPING_TERM 170

// Auto Shift
#define NO_AUTO_SHIFT_ALPHA
#define AUTO_SHIFT_TIMEOUT TAPPING_TERM

#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE

/* Five profiles -- keep in sync with rules.mk and the keymaps[] array. */
#define DYNAMIC_KEYMAP_LAYER_COUNT 5

/* RGB matrix. The 120 ceiling comes from keyboard.json (max_brightness) and
 * exists for a reason: 20 WS2812 at full white draw well over 1 A, far past
 * the 500 mA USB budget.
 */
#define RGB_MATRIX_DEFAULT_VAL 80
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR
#define RGB_MATRIX_KEYPRESSES
