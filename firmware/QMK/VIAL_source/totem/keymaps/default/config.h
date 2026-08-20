// Copyright 2026 josericardodainese
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

/* The TOTEM 20 is a single-piece board, so the split handedness settings the
 * original TOTEM keymap carried here (MASTER_LEFT / MASTER_RIGHT) are gone.
 */

#define TAPPING_TERM 170

// Auto Shift
#define NO_AUTO_SHIFT_ALPHA
#define AUTO_SHIFT_TIMEOUT TAPPING_TERM
#define AUTO_SHIFT_NO_SETUP

#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE
#define NO_ACTION_ONESHOT

/* See keymaps/vial/config.h for why the brightness ceiling matters. */
#define RGB_MATRIX_DEFAULT_VAL 80
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR
#define RGB_MATRIX_KEYPRESSES
