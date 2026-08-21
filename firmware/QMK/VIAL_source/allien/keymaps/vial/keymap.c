/* Allien -- unibody 20-key macropad, no encoder, no OLED
 *
 * Copyright 2022 GEIST @geigeigeist
 * Copyright 2026 josericardodainese
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Physical layout (matrix [row, col] in parentheses):
 *
 *              1(0,1)  2(0,2)  3(0,3)  R(0,4)
 *      Q(1,0)  A(1,1)  W(1,2)  D(1,3)  F(1,4)
 *    SFT(2,0)  Z(2,1)  S(2,2)  X(2,3)        T(2,4)   E(2,5)
 *  G(3,0) CTL(3,1)             C(3,3)      SPC(3,4)   B(3,5)
 *
 * The four matrix positions without a switch -- [0,0], [0,5], [1,5] and
 * [3,2] -- are filled with KC_NO by the generated LAYOUT macro.
 *
 * Two chords, both anchored on G (see ../../config.h):
 *     G + B  held 1 s  ->  next profile
 *     G + E  held 1 s  ->  toggle the RGB layer below
 *
 * G, B and E are intercepted by position in ../../allien.c. Pressed alone they
 * behave completely normally; only a recognised chord swallows them.
 */

#include QMK_KEYBOARD_H

enum allien_layers {
    _GAME     = ALLIEN_PROFILE_GAME,
    _MAX3DS   = ALLIEN_PROFILE_MAX3DS,
    _MACROS   = ALLIEN_PROFILE_MACROS,
    _RESERVED = ALLIEN_PROFILE_RESERVED,
    _RGB      = ALLIEN_PROFILE_RGB,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* 0 -- JOGO (white legends from the reference drawing) */
    [_GAME] = LAYOUT(
                  KC_1,     KC_2,     KC_3,     KC_R,
        KC_Q,     KC_A,     KC_W,     KC_D,     KC_F,
        KC_LSFT,  KC_Z,     KC_S,     KC_X,     KC_T,       KC_E,
        KC_G,     KC_LCTL,            KC_C,     KC_SPC,     KC_B
    ),

    /* 1 -- 3ds MAX (Q/W/E/R are select / move / rotate / scale) */
    [_MAX3DS] = LAYOUT(
                  KC_F1,    KC_F2,    KC_F3,    KC_F4,
        KC_Q,     KC_W,     KC_E,     KC_R,     KC_G,
        KC_LSFT,  KC_Z,     KC_S,     KC_X,     LCTL(KC_Z), LCTL(KC_Y),
        KC_LALT,  KC_LCTL,            KC_DEL,   KC_SPC,     KC_F
    ),

    /* 2 -- MACROS / uso geral */
    [_MACROS] = LAYOUT(
                  KC_F13,   KC_F14,   KC_F15,   KC_F16,
        KC_F17,   KC_F18,   KC_F19,   KC_F20,   KC_F21,
        KC_F22,   KC_F23,   KC_F24,   DM_REC1,  DM_PLY1,    DM_RSTP,
        KC_NO,    KC_LCTL,            DM_REC2,  KC_SPC,     DM_PLY2
    ),

    /* 3 -- RESERVADA (intentionally empty; assign freely in Vial) */
    [_RESERVED] = LAYOUT(
                  KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,
        KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,      KC_NO,
        KC_NO,    KC_NO,              KC_NO,    KC_NO,      KC_NO
    ),

    /* 4 -- RGB / manutencao.  Reached with G + E held for 1 s; the same chord
     * turns it back off. Effects change instantly, so you see what you get.
     *
     *          efeito-  efeito+  liga/desl  BOOT
     *   brilho-  brilho+  matiz-   matiz+   satur+
     *   satur-   veloc-   veloc+   EE_CLR   debug    [E]
     *   [G]      gravar           tocar     parar    [B]
     *
     * G, B and E are left dead here so a stray single press does nothing --
     * they exist on this layer only as chord partners.
     */
    [_RGB] = LAYOUT(
                  RM_PREV,  RM_NEXT,  RM_TOGG,  QK_BOOT,
        RM_VALD,  RM_VALU,  RM_HUED,  RM_HUEU,  RM_SATU,
        RM_SATD,  RM_SPDD,  RM_SPDU,  EE_CLR,   DB_TOGG,    KC_NO,
        KC_NO,    DM_REC1,            DM_PLY1,  DM_RSTP,    KC_NO
    ),
};
