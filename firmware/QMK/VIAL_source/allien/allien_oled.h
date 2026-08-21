/* Allien -- OLED menu, public interface
 *
 * Copyright 2026 josericardodainese
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "quantum.h"

/* ---- driven by allien.c ---------------------------------------------- */

/* Encoder turned. Returns true if the UI consumed it, false if the rotation
 * should fall through to its keycode function. */
bool allien_oled_rotate(bool clockwise);

/* Encoder button held past ENCODER_LONG_PRESS_MS: open menu / confirm item. */
void allien_oled_long_press(void);

/* Called every matrix scan: handles the idle timeout. */
void allien_oled_tick(void);

/* Called on every key press: feeds the APM counter and wakes the panel. */
void allien_oled_note_keypress(void);

/* Called from oled_task_kb(). */
void allien_oled_render(void);

/* ---- provided by allien.c, used by the menu -------------------------- */

void    allien_profile_switch_next(void);
void    allien_profile_set(uint8_t profile);
uint8_t allien_profile_current(void);
const char *allien_profile_name(uint8_t profile);
