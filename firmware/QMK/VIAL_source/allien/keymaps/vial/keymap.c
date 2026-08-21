/* Allien -- unibody 20-key macropad + encoder + OLED (RP2040-Zero)
 *
 * Copyright 2022 GEIST @geigeigeist
 * Copyright 2026 josericardodainese
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Physical layout (matrix [row, col] in parentheses):
 *
 *              1(0,1)  2(0,2)  3(0,3)  R(0,4)      ENC(0,5)
 *      Q(1,0)  A(1,1)  W(1,2)  D(1,3)  F(1,4)
 *    SFT(2,0)  Z(2,1)  S(2,2)  X(2,3)        T(2,4)   E(2,5)
 *  G(3,0) CTL(3,1)             C(3,3)      SPC(3,4)   B(3,5)
 *
 * 20 keys + the encoder push switch = 21 entries per layer.
 * The three matrix positions with nothing on them -- [0,0], [1,5] and [3,2] --
 * are filled with KC_NO by the generated LAYOUT macro.
 *
 * ENC(0,5) is the encoder button. It is a normal matrix key electrically, but
 * ../../allien.c intercepts it by position, so its keycode here is never sent:
 *   short click -> next profile,  long click -> OLED menu.
 * The encoder ROTATION is owned by the menu and is not remappable in Vial.
 *
 * The G + B chord and the RGB profile indicator also live in ../../allien.c;
 * every tunable is in ../../config.h.
 */

#include QMK_KEYBOARD_H

enum allien_layers {
    _GAME     = ALLIEN_PROFILE_GAME,
    _MAX3DS   = ALLIEN_PROFILE_MAX3DS,
    _MACROS   = ALLIEN_PROFILE_MACROS,
    _RESERVED = ALLIEN_PROFILE_RESERVED,
    _CONFIG   = ALLIEN_PROFILE_CONFIG,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* 0 -- JOGO (white legends from the reference drawing) */
    [_GAME] = LAYOUT(
                  KC_1,     KC_2,     KC_3,     KC_R,     KC_NO,
        KC_Q,     KC_A,     KC_W,     KC_D,     KC_F,
        KC_LSFT,  KC_Z,     KC_S,     KC_X,     KC_T,       KC_E,
        KC_G,     KC_LCTL,            KC_C,     KC_SPC,     KC_B
    ),

    /* 1 -- 3ds MAX (Q/W/E/R are select / move / rotate / scale) */
    [_MAX3DS] = LAYOUT(
                  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_NO,
        KC_Q,     KC_W,     KC_E,     KC_R,     KC_G,
        KC_LSFT,  KC_Z,     KC_S,     KC_X,     LCTL(KC_Z), LCTL(KC_Y),
        KC_LALT,  KC_LCTL,            KC_DEL,   KC_SPC,     KC_F
    ),

    /* 2 -- MACROS / uso geral */
    [_MACROS] = LAYOUT(
                  KC_F13,   KC_F14,   KC_F15,   KC_F16,   KC_NO,
        KC_F17,   KC_F18,   KC_F19,   KC_F20,   KC_F21,
        KC_F22,   KC_F23,   KC_F24,   DM_REC1,  DM_PLY1,    DM_RSTP,
        KC_NO,    KC_LCTL,            DM_REC2,  KC_SPC,     DM_PLY2
    ),

    /* 3 -- RESERVADA (intentionally empty; assign freely in Vial) */
    [_RESERVED] = LAYOUT(
                  KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,      KC_NO,
        KC_NO,    KC_NO,              KC_NO,    KC_NO,      KC_NO
    ),

    /* 4 -- CONFIGURACAO / manutencao */
    [_CONFIG] = LAYOUT(
                  RM_TOGG,  RM_NEXT,  RM_HUEU,  RM_SATU,  KC_NO,
        QK_BOOT,  RM_VALD,  RM_VALU,  RM_HUED,  RM_SATD,
        KC_LSFT,  DM_REC1,  DM_PLY1,  DM_RSTP,  EE_CLR,     DB_TOGG,
        KC_NO,    KC_NO,              KC_NO,    KC_NO,      KC_NO
    ),
};
