/*
Copyright (C) 2018,2019 Jim Jiang <jim@lotlab.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "keymap_common.h"
#include "keyboard_fn.h"

const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* 0: qwerty */
    KEYMAP_ANSI( \
        ESC,  F1,   F2,   F3,   F4,   F5,   F6,   F7,   F8,   F9,   F10,  F11,  F12,        \
        GRV,  1,    2,    3,    4,    5,    6,    7,    8,    9,    0,    MINS, EQL,        \
        TAB,  Q,    W,    E,    R,    T,    Y,    U,    I,    O,    P,    LBRC, RBRC,       \
        CAPS, A,    S,    D,    F,    G,    H,    J,    K,    L,    SCLN, QUOT, ENT,        \
        UP,   LSFT,       Z,    X,    C,    V,    B,    N,    M,    COMM, DOT,  SLSH, RSFT, \
        LEFT, DOWN,       LCTL, LALT, LGUI,       SPC,        FN0,  RCTL, RALT, RGHT, RGUI, \
                                            PGUP,             HOME, INS,  BSPC,             \
                          DEL,        PGDN, END,              PAUS, SLCK, PSCR,       BSLS),
    /* 1: qwerty with out left win */
    KEYMAP_ANSI( \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,       \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,       \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,       \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,       \
        TRNS, TRNS,       TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, \
        TRNS, TRNS,       TRNS, TRNS,   NO,       TRNS,       TRNS, TRNS, TRNS, TRNS, TRNS, \
                                            TRNS,             TRNS, TRNS, TRNS,             \
                          TRNS,       TRNS, TRNS,             TRNS, TRNS, TRNS,       TRNS  ),
    /* 2: Poker Fn */
    KEYMAP_ANSI( \
        TRNS, MUTE, VOLD, VOLU, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,       \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,       \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,       \
        TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS,       \
        TRNS, TRNS,       TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, \
        TRNS, TRNS,       TRNS, TRNS,  FN1,       TRNS,       TRNS, TRNS, TRNS, TRNS, TRNS, \
                                            TRNS,             TRNS, TRNS, TRNS,             \
                          TRNS,       TRNS, TRNS,             TRNS, TRNS, TRNS,       TRNS  ),
};
const action_t fn_actions[] = {
    /* Poker Layout */
    ACTION_LAYER_MOMENTARY(2),  // to Fn overlay
    ACTION_LAYER_TOGGLE(1), // to Fn overlay
};

