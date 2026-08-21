/* Allien -- unibody 20-key macropad, no encoder, no OLED
 *
 * Copyright 2022 GEIST @geigeigeist
 * Copyright 2026 josericardodainese
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Board-level behaviour shared by every keymap:
 *   - the RGB matrix layout (LED chain order);
 *   - a small chord engine: two-key combinations, each held for one second,
 *     that cycle the profile and open the RGB control layer;
 *   - the profile colour indicator.
 *
 * The original TOTEM hand_swap_config lived here. It is gone: the board is a
 * single unit, so SWAP_HANDS has nothing to swap.
 *
 * All tunables live in config.h.
 */

#include "quantum.h"

// ┌─────────────────────────────────────────────────┐
// │ R G B   m a t r i x   l a y o u t               │
// └─────────────────────────────────────────────────┘

#ifdef RGB_MATRIX_ENABLE
/* The LED chain must be routed DIN -> DOUT following SW1..SW20, i.e. the same
 * order as the LAYOUT macro: top row left-to-right, then each row below.
 * If the PCB ends up serpentining instead, only the first block changes.
 */
led_config_t g_led_config = {
    {   // matrix position -> LED index
        { NO_LED,      0,      1,      2,      3, NO_LED },   // row 0 (ROW4)
        {      4,      5,      6,      7,      8, NO_LED },   // row 1 (ROW3)
        {      9,     10,     11,     12,     13,     14 },   // row 2 (ROW2)
        {     15,     16, NO_LED,     17,     18,     19 },   // row 3 (ROW1)
    },
    {   // LED index -> physical position { x, y }
        {  69,  7 },   // LED  0  SW1  "1"
        {  97,  0 },   // LED  1  SW2  "2"
        { 125, 10 },   // LED  2  SW3  "3"
        { 153, 10 },   // LED  3  SW4  "R"
        {  42, 14 },   // LED  4  SW5  "Q"
        {  69, 21 },   // LED  5  SW6  "A"
        {  97, 14 },   // LED  6  SW7  "W"
        { 125, 23 },   // LED  7  SW8  "D"
        { 153, 23 },   // LED  8  SW9  "F"
        {  31, 34 },   // LED  9  SW10 "SHIFT"
        {  66, 34 },   // LED 10  SW11 "Z"
        {  97, 27 },   // LED 11  SW12 "S"
        { 139, 37 },   // LED 12  SW13 "X"
        { 185, 33 },   // LED 13  SW14 "T"
        { 224, 48 },   // LED 14  SW15 "E"
        {   0, 39 },   // LED 15  SW16 "G"
        {  38, 48 },   // LED 16  SW17 "CTRL"
        { 139, 56 },   // LED 17  SW18 "C"
        { 174, 53 },   // LED 18  SW19 "SPACE"
        { 205, 64 },   // LED 19  SW20 "B"
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

/* Only these three rotate through the G+B chord:
 *     Jogo -> 3ds Max -> Macros -> Jogo
 * RESERVED stays out of the cycle and is reached by assigning a layer keycode
 * in Vial; RGB is reached with the G+E chord. Cycling while on either of them
 * returns to jogo.
 */
static const uint8_t profile_cycle_order[] = {
    ALLIEN_PROFILE_GAME,
    ALLIEN_PROFILE_MAX3DS,
    ALLIEN_PROFILE_MACROS,
};

/* jogo = vermelho, 3ds Max = azul, macros = verde,
 * reservada = roxo, RGB = amarelo
 */
static const uint8_t profile_hue[ALLIEN_PROFILE_COUNT] = {
    [ALLIEN_PROFILE_GAME]     = 0,     // red
    [ALLIEN_PROFILE_MAX3DS]   = 170,   // blue
    [ALLIEN_PROFILE_MACROS]   = 85,    // green
    [ALLIEN_PROFILE_RESERVED] = 191,   // purple
    [ALLIEN_PROFILE_RGB]      = 43,    // yellow
};

static void profile_indicate(uint8_t profile) {
#ifdef RGB_MATRIX_ENABLE
    if (profile >= ALLIEN_PROFILE_COUNT) return;
    /* Respect an explicit "RGB off": never light up on our own. */
    if (!rgb_matrix_is_enabled()) return;
    /* The profile colour only means anything on the solid effect. On any
     * animation the hue belongs to the user, so we keep our hands off it --
     * otherwise every layer change would stomp on the palette they just set
     * from the RGB layer. Brightness and effect are never touched either way.
     */
    if (rgb_matrix_get_mode() != RGB_MATRIX_SOLID_COLOR) return;
    /* _noeeprom: never overwrite saved settings, never wear out the flash. */
    rgb_matrix_sethsv_noeeprom(profile_hue[profile], 255, rgb_matrix_get_val());
#else
    (void)profile;
#endif
}

static void profile_switch_next(void) {
    uint8_t current = get_highest_layer(default_layer_state);
    uint8_t next    = profile_cycle_order[0];

    for (uint8_t i = 0; i < ARRAY_SIZE(profile_cycle_order); i++) {
        if (profile_cycle_order[i] == current) {
            next = profile_cycle_order[(i + 1) % ARRAY_SIZE(profile_cycle_order)];
            break;
        }
    }
    /* Persists in EEPROM, so the profile survives a reboot or replug. */
    set_single_persistent_default_layer(next);
}

static void rgb_layer_toggle(void) {
    layer_invert(ALLIEN_PROFILE_RGB);
}

// ┌─────────────────────────────────────────────────┐
// │ c h o r d   e n g i n e                         │
// └─────────────────────────────────────────────────┘

/* Every key that takes part in a chord is tracked here. A chord key is
 * withheld from the host for up to CHORD_SYNC_MS so that a recognised chord
 * never leaks its normal keycode; if no partner shows up in that window the
 * key is forwarded and behaves completely normally.
 */
enum chord_keys {
    CK_ANCHOR = 0,   // "G"
    CK_PROFILE,      // "B"
    CK_RGB,          // "E"
    CK_COUNT,
};

typedef struct {
    uint8_t  row, col;
    bool     down;
    bool     held_back;   // pressed, keycode not yet sent to the host
    bool     forwarded;   // keycode currently registered on the host
    bool     consumed;    // a chord fired -- swallow the release
    uint16_t keycode;
    uint16_t press_time;
} chord_key_t;

static chord_key_t chord_key[CK_COUNT] = {
    [CK_ANCHOR]  = { .row = CHORD_ANCHOR_ROW,  .col = CHORD_ANCHOR_COL  },
    [CK_PROFILE] = { .row = CHORD_PROFILE_ROW, .col = CHORD_PROFILE_COL },
    [CK_RGB]     = { .row = CHORD_RGB_ROW,     .col = CHORD_RGB_COL     },
};

typedef struct {
    uint8_t a, b;            // indices into chord_key[]
    void  (*action)(void);
} chord_def_t;

/* Checked in order, so if all three keys somehow go down together the first
 * entry wins. */
static const chord_def_t chords[] = {
    { CK_ANCHOR, CK_PROFILE, profile_switch_next },
    { CK_ANCHOR, CK_RGB,     rgb_layer_toggle    },
};

static int8_t  armed_chord = -1;
static uint16_t armed_since;

static int8_t chord_key_index(keypos_t key) {
    for (uint8_t i = 0; i < CK_COUNT; i++) {
        if (chord_key[i].row == key.row && chord_key[i].col == key.col) return i;
    }
    return -1;
}

/* Release a withheld key to the host as a normal hold. */
static void chord_key_forward(uint8_t i) {
    chord_key[i].held_back = false;
    chord_key[i].forwarded = true;
    register_code16(chord_key[i].keycode);
}

/* Is this key half of the chord currently armed? */
static bool in_armed_chord(uint8_t i) {
    if (armed_chord < 0) return false;
    return chords[armed_chord].a == i || chords[armed_chord].b == i;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user(keycode, record)) return false;

    int8_t idx = chord_key_index(record->event.key);
    if (idx < 0) return true;   // not a chord key -- nothing to do

    if (record->event.pressed) {
        chord_key[idx].down       = true;
        chord_key[idx].held_back  = true;
        chord_key[idx].forwarded  = false;
        chord_key[idx].consumed   = false;
        chord_key[idx].keycode    = keycode;
        chord_key[idx].press_time = timer_read();

        /* Arm a chord only if both of its keys are down AND still withheld,
         * i.e. the two presses landed within CHORD_SYNC_MS of each other.
         * Pressing G, waiting, then pressing B will never arm anything.
         */
        if (armed_chord < 0) {
            for (uint8_t c = 0; c < ARRAY_SIZE(chords); c++) {
                const chord_key_t *a = &chord_key[chords[c].a];
                const chord_key_t *b = &chord_key[chords[c].b];
                if (a->down && a->held_back && b->down && b->held_back) {
                    armed_chord = c;
                    armed_since = timer_read();
                    break;
                }
            }
        }
        return false;   // withhold; matrix_scan_kb decides what happens next
    }

    chord_key[idx].down = false;

    if (chord_key[idx].consumed) {
        /* A chord already fired on this press -- the release is silent. */
        chord_key[idx].consumed  = false;
        chord_key[idx].held_back = false;
        return false;
    }

    if (in_armed_chord(idx)) {
        /* Broken before the hold completed: treat it as ordinary typing. The
         * partner that is still down becomes a normal hold.
         */
        uint8_t partner = (chords[armed_chord].a == idx) ? chords[armed_chord].b
                                                         : chords[armed_chord].a;
        armed_chord = -1;
        if (chord_key[partner].down && chord_key[partner].held_back) {
            chord_key_forward(partner);
        }
    }

    if (chord_key[idx].held_back) {
        chord_key[idx].held_back = false;
        tap_code16(chord_key[idx].keycode);   // quick tap that never reached the host
    } else if (chord_key[idx].forwarded) {
        chord_key[idx].forwarded = false;
        unregister_code16(chord_key[idx].keycode);
    }
    return false;
}

void matrix_scan_kb(void) {
    if (armed_chord >= 0) {
        if (timer_elapsed(armed_since) >= CHORD_HOLD_MS) {
            uint8_t a = chords[armed_chord].a;
            uint8_t b = chords[armed_chord].b;
            void (*action)(void) = chords[armed_chord].action;

            armed_chord = -1;
            chord_key[a].held_back = chord_key[b].held_back = false;
            chord_key[a].consumed  = chord_key[b].consumed  = true;
            action();
        }
        /* While a chord is armed, none of its keys may reach the host. */
        matrix_scan_user();
        return;
    }

    for (uint8_t i = 0; i < CK_COUNT; i++) {
        if (chord_key[i].down && chord_key[i].held_back &&
            timer_elapsed(chord_key[i].press_time) >= CHORD_SYNC_MS) {
            chord_key_forward(i);
        }
    }
    matrix_scan_user();
}

// ┌─────────────────────────────────────────────────┐
// │ i n d i c a t o r   h o o k s                   │
// └─────────────────────────────────────────────────┘

layer_state_t default_layer_state_set_kb(layer_state_t state) {
    profile_indicate(get_highest_layer(state));
    return default_layer_state_set_user(state);
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    /* Entering the RGB layer turns the board yellow (on the solid effect), so
     * you can tell the layer is live before touching anything. Adjustments
     * made from there stick, because profile_indicate() bows out on
     * animations and nothing re-fires until the layer changes again.
     */
    profile_indicate(get_highest_layer(state | default_layer_state));
    return layer_state_set_user(state);
}

void keyboard_post_init_kb(void) {
    profile_indicate(get_highest_layer(default_layer_state));
    keyboard_post_init_user();
}
