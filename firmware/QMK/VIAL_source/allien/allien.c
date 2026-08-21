/* Allien -- unibody 20-key macropad + encoder + OLED (Waveshare RP2040-Zero)
 *
 * Copyright 2022 GEIST @geigeigeist
 * Copyright 2026 josericardodainese
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Board-level behaviour shared by every keymap:
 *   - the RGB matrix layout (LED chain order);
 *   - the two-key, one-second-hold profile chord;
 *   - the encoder: short click switches profile, long click drives the menu;
 *   - the profile colour indicator.
 *
 * The original TOTEM hand_swap_config lived here. It is gone: the board is a
 * single unit, so SWAP_HANDS has nothing to swap.
 *
 * All tunables live in config.h.
 */

#include "quantum.h"
#include "allien_oled.h"

// ┌─────────────────────────────────────────────────┐
// │ R G B   m a t r i x   l a y o u t               │
// └─────────────────────────────────────────────────┘

#ifdef RGB_MATRIX_ENABLE
/* The LED chain must be routed DIN -> DOUT following SW1..SW20, i.e. the same
 * order as the LAYOUT macro: top row left-to-right, then each row below.
 * If the PCB ends up serpentining instead, only the first block changes.
 * [0,5] is the encoder button -- a switch with no LED, hence NO_LED.
 */
led_config_t g_led_config = {
    {   // matrix position -> LED index
        { NO_LED,      0,      1,      2,      3, NO_LED },   // row 0 (ROW4), [0,5] = encoder button
        {      4,      5,      6,      7,      8, NO_LED },   // row 1 (ROW3)
        {      9,     10,     11,     12,     13,     14 },   // row 2 (ROW2)
        {     15,     16, NO_LED,     17,     18,     19 },   // row 3 (ROW1)
    },
    {   // LED index -> physical position { x, y }
        {  66,  5 },   // LED  0  SW1  "1"
        {  96,  0 },   // LED  1  SW2  "2"
        { 126,  4 },   // LED  2  SW3  "3"
        { 156,  6 },   // LED  3  SW4  "R"
        {  36, 28 },   // LED  4  SW5  "Q"
        {  66, 23 },   // LED  5  SW6  "A"
        {  96, 18 },   // LED  6  SW7  "W"
        { 126, 22 },   // LED  7  SW8  "D"
        { 156, 25 },   // LED  8  SW9  "F"
        {  36, 47 },   // LED  9  SW10 "Shift"
        {  66, 41 },   // LED 10  SW11 "Z"
        {  96, 37 },   // LED 11  SW12 "S"
        { 126, 40 },   // LED 12  SW13 "X"
        { 192, 31 },   // LED 13  SW14 "T"
        { 224, 46 },   // LED 14  SW15 "E"
        {   0, 49 },   // LED 15  SW16 "G"
        {  48, 62 },   // LED 16  SW17 "Ctrl"
        { 126, 60 },   // LED 17  SW18 "C"
        { 192, 59 },   // LED 18  SW19 "Espaco"
        { 224, 64 },   // LED 19  SW20 "B"
    },
    {   // LED index -> flags
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT,
    }
};
#endif

// ┌─────────────────────────────────────────────────┐
// │ p r o f i l e s                                 │
// └─────────────────────────────────────────────────┘

/* Only these three rotate through the chord and the short click:
 *     Jogo -> 3ds Max -> Macros -> Jogo
 * RESERVED and CONFIG are reached from the OLED menu (Perfil) or by a layer
 * keycode assigned in Vial. Cycling while on one of them returns to jogo.
 */
static const uint8_t profile_cycle_order[] = {
    ALLIEN_PROFILE_GAME,
    ALLIEN_PROFILE_MAX3DS,
    ALLIEN_PROFILE_MACROS,
};

/* jogo = vermelho, 3ds Max = azul, macros = verde,
 * reservada = roxo, configuracao = amarelo
 */
static const uint8_t profile_hue[ALLIEN_PROFILE_COUNT] = {
    [ALLIEN_PROFILE_GAME]     = 0,     // red
    [ALLIEN_PROFILE_MAX3DS]   = 170,   // blue
    [ALLIEN_PROFILE_MACROS]   = 85,    // green
    [ALLIEN_PROFILE_RESERVED] = 191,   // purple
    [ALLIEN_PROFILE_CONFIG]   = 43,    // yellow
};

const char *allien_profile_name(uint8_t profile) {
    switch (profile) {
        case ALLIEN_PROFILE_GAME:     return "JOGO";
        case ALLIEN_PROFILE_MAX3DS:   return "3DS MAX";
        case ALLIEN_PROFILE_MACROS:   return "MACROS";
        case ALLIEN_PROFILE_RESERVED: return "RESERVADA";
        case ALLIEN_PROFILE_CONFIG:   return "CONFIG";
        default:                       return "?";
    }
}

uint8_t allien_profile_current(void) {
    return get_highest_layer(default_layer_state);
}

static void profile_indicate(uint8_t profile) {
#ifdef RGB_MATRIX_ENABLE
    if (profile >= ALLIEN_PROFILE_COUNT) return;
    /* Respect an explicit "RGB off": never light up on our own. */
    if (!rgb_matrix_is_enabled()) return;
    /* Only the hue moves. The effect and brightness the user picked -- in Vial
     * or in the OLED menu -- are left alone, and _noeeprom keeps us from
     * overwriting their saved settings or wearing out the flash.
     */
    rgb_matrix_sethsv_noeeprom(profile_hue[profile], 255, rgb_matrix_get_val());
#else
    (void)profile;
#endif
}

void allien_profile_set(uint8_t profile) {
    if (profile >= ALLIEN_PROFILE_COUNT) return;
    /* Persists in EEPROM, so the profile survives a reboot or replug. */
    set_single_persistent_default_layer(profile);
}

void allien_profile_switch_next(void) {
    uint8_t current = allien_profile_current();
    uint8_t next    = profile_cycle_order[0];

    for (uint8_t i = 0; i < ARRAY_SIZE(profile_cycle_order); i++) {
        if (profile_cycle_order[i] == current) {
            next = profile_cycle_order[(i + 1) % ARRAY_SIZE(profile_cycle_order)];
            break;
        }
    }
    allien_profile_set(next);
}

// ┌─────────────────────────────────────────────────┐
// │ c h o r d   s t a t e   m a c h i n e           │
// └─────────────────────────────────────────────────┘

typedef struct {
    bool     down;       // key is physically pressed
    bool     held_back;  // pressed, but its keycode has not reached the host
    bool     forwarded;  // its keycode is currently registered on the host
    bool     consumed;   // chord fired -- swallow the matching release
    uint16_t keycode;
    uint16_t press_time;
} profile_key_t;

static profile_key_t profile_key[2];
static uint16_t      chord_start_time;
static bool          chord_armed;

static int8_t profile_key_index(keypos_t key) {
    if (key.row == PROFILE_KEY_A_ROW && key.col == PROFILE_KEY_A_COL) return 0;
    if (key.row == PROFILE_KEY_B_ROW && key.col == PROFILE_KEY_B_COL) return 1;
    return -1;
}

static void profile_key_forward(uint8_t i) {
    profile_key[i].held_back = false;
    profile_key[i].forwarded = true;
    register_code16(profile_key[i].keycode);
}

// ┌─────────────────────────────────────────────────┐
// │ e n c o d e r   b u t t o n                     │
// └─────────────────────────────────────────────────┘

static bool     enc_btn_down;
static bool     enc_btn_long_fired;
static uint16_t enc_btn_press_time;

static bool is_encoder_button(keypos_t key) {
    return key.row == ENCODER_BTN_ROW && key.col == ENCODER_BTN_COL;
}

// ┌─────────────────────────────────────────────────┐
// │ e v e n t   h a n d l i n g                     │
// └─────────────────────────────────────────────────┘

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user(keycode, record)) return false;

    if (record->event.pressed) {
        allien_oled_note_keypress();
    }

    /* ---- encoder push switch: an ordinary matrix key, owned by us ---- */
    if (is_encoder_button(record->event.key)) {
        if (record->event.pressed) {
            enc_btn_down       = true;
            enc_btn_long_fired = false;
            enc_btn_press_time = timer_read();
        } else {
            enc_btn_down = false;
            /* A long press already acted while the button was still held. */
            if (!enc_btn_long_fired) {
                allien_profile_switch_next();
            }
        }
        return false;   // never emits a keycode
    }

    /* ---- profile chord ---------------------------------------------- */
    int8_t idx = profile_key_index(record->event.key);
    if (idx < 0) return true;

    uint8_t other = idx ^ 1;

    if (record->event.pressed) {
        profile_key[idx].down       = true;
        profile_key[idx].held_back  = true;
        profile_key[idx].forwarded  = false;
        profile_key[idx].consumed   = false;
        profile_key[idx].keycode    = keycode;
        profile_key[idx].press_time = timer_read();

        /* Armed only if the partner is also down AND still withheld, i.e. the
         * two presses landed within PROFILE_CHORD_SYNC_MS of each other.
         */
        if (profile_key[other].down && profile_key[other].held_back) {
            chord_armed      = true;
            chord_start_time = timer_read();
        }
        return false;
    }

    profile_key[idx].down = false;

    if (profile_key[idx].consumed) {
        profile_key[idx].consumed  = false;
        profile_key[idx].held_back = false;
        return false;
    }

    if (chord_armed) {
        /* Broken before the hold completed: treat it as ordinary typing. */
        chord_armed = false;
        if (profile_key[other].down && profile_key[other].held_back) {
            profile_key_forward(other);
        }
    }

    if (profile_key[idx].held_back) {
        profile_key[idx].held_back = false;
        tap_code16(profile_key[idx].keycode);
    } else if (profile_key[idx].forwarded) {
        profile_key[idx].forwarded = false;
        unregister_code16(profile_key[idx].keycode);
    }
    return false;
}

void matrix_scan_kb(void) {
    /* ---- encoder long press ------------------------------------------ */
    if (enc_btn_down && !enc_btn_long_fired &&
        timer_elapsed(enc_btn_press_time) >= ENCODER_LONG_PRESS_MS) {
        enc_btn_long_fired = true;
        allien_oled_long_press();
    }

    /* ---- chord -------------------------------------------------------- */
    if (chord_armed) {
        if (timer_elapsed(chord_start_time) >= PROFILE_SWITCH_HOLD_MS) {
            chord_armed = false;
            for (uint8_t i = 0; i < 2; i++) {
                profile_key[i].held_back = false;
                profile_key[i].consumed  = true;
            }
            allien_profile_switch_next();
        }
        /* While armed, neither key may be forwarded to the host. */
        allien_oled_tick();
        matrix_scan_user();
        return;
    }

    for (uint8_t i = 0; i < 2; i++) {
        if (profile_key[i].down && profile_key[i].held_back &&
            timer_elapsed(profile_key[i].press_time) >= PROFILE_CHORD_SYNC_MS) {
            profile_key_forward(i);
        }
    }

    allien_oled_tick();
    matrix_scan_user();
}

#ifdef ENCODER_ENABLE
bool encoder_update_kb(uint8_t index, bool clockwise) {
    if (!encoder_update_user(index, clockwise)) return false;

    /* The menu gets first refusal. It declines only when the encoder is set
     * to "Teclas" and the status screen is showing.
     */
    if (allien_oled_rotate(clockwise)) return false;

    /* Fallback function when the menu is not driving. Change these two
     * keycodes to give the encoder a different job.
     */
    tap_code(clockwise ? KC_VOLU : KC_VOLD);
    return false;
}
#endif

#ifdef OLED_ENABLE
oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    return oled_init_user(rotation);
}

bool oled_task_kb(void) {
    if (!oled_task_user()) return false;
    allien_oled_render();
    return false;
}
#endif

// ┌─────────────────────────────────────────────────┐
// │ i n d i c a t o r   h o o k s                   │
// └─────────────────────────────────────────────────┘

layer_state_t default_layer_state_set_kb(layer_state_t state) {
    profile_indicate(get_highest_layer(state));
    return default_layer_state_set_user(state);
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    profile_indicate(get_highest_layer(state | default_layer_state));
    return layer_state_set_user(state);
}

void keyboard_post_init_kb(void) {
    profile_indicate(get_highest_layer(default_layer_state));
    keyboard_post_init_user();
}
