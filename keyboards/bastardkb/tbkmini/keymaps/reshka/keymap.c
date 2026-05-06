// Copyright 2026 0xse.reshka
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layer_names {
    L_BASE,
    L_NAV,
    L_SYS,
    L_KEYBOARD,
};

enum combo_names {
    C_R_T_LPRN,
    C_Y_U_RPRN,
    C_F_G_LBRC,
    C_H_J_RBRC,
    C_V_B_LCBR,
    C_N_M_RCBR,
    C_F_J_EQL,
    C_W_E_MINS,
    C_E_R_PLUS,
    C_U_I_UNDS,
    C_I_O_PIPE,
    C_D_F_QUOT,
    C_J_K_DQUO,
    C_S_D_GRV,
    C_K_L_TILD,
};

#define COMBO_TERM_ROLL   22
#define COMBO_TERM_TIGHT  24
#define COMBO_TERM_CROSS  38

#define NAV_TAB LT(L_NAV, KC_TAB)
#define SYS_CAP LT(L_SYS, KC_CAPS)

#define MAC_UNDO   G(KC_Z)
#define MAC_CUT    G(KC_X)
#define MAC_COPY   G(KC_C)
#define MAC_PASTE  G(KC_V)
#define MAC_REDO   G(S(KC_Z))
#define MAC_FIND   G(KC_F)
#define MAC_SPOT   G(KC_SPC)
#define MAC_MCTL   C(KC_UP)
#define MAC_DESK_L C(KC_LEFT)
#define MAC_DESK_R C(KC_RGHT)
#define MAC_SSHOT  G(S(KC_4))
#define TAB_PREV   G(S(KC_LBRC))
#define TAB_NEXT   G(S(KC_RBRC))

static const key_override_t delete_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);
static const key_override_t tilde_esc_override  = ko_make_basic(MOD_MASK_SHIFT, KC_ESC, S(KC_GRV));

const key_override_t *key_overrides[] = {
    &delete_key_override,
    &tilde_esc_override,
};

static const uint16_t PROGMEM combo_rt_lprn[] = {KC_R, KC_T, COMBO_END};
static const uint16_t PROGMEM combo_yu_rprn[] = {KC_Y, KC_U, COMBO_END};
static const uint16_t PROGMEM combo_fg_lbrc[] = {KC_F, KC_G, COMBO_END};
static const uint16_t PROGMEM combo_hj_rbrc[] = {KC_H, KC_J, COMBO_END};
static const uint16_t PROGMEM combo_vb_lcbr[] = {KC_V, KC_B, COMBO_END};
static const uint16_t PROGMEM combo_nm_rcbr[] = {KC_N, KC_M, COMBO_END};
static const uint16_t PROGMEM combo_fj_eql[]  = {KC_F, KC_J, COMBO_END};
static const uint16_t PROGMEM combo_we_mins[] = {KC_W, KC_E, COMBO_END};
static const uint16_t PROGMEM combo_er_plus[] = {KC_E, KC_R, COMBO_END};
static const uint16_t PROGMEM combo_ui_unds[] = {KC_U, KC_I, COMBO_END};
static const uint16_t PROGMEM combo_io_pipe[] = {KC_I, KC_O, COMBO_END};
static const uint16_t PROGMEM combo_df_quot[] = {KC_D, KC_F, COMBO_END};
static const uint16_t PROGMEM combo_jk_dquo[] = {KC_J, KC_K, COMBO_END};
static const uint16_t PROGMEM combo_sd_grv[]  = {KC_S, KC_D, COMBO_END};
static const uint16_t PROGMEM combo_kl_tild[] = {KC_K, KC_L, COMBO_END};

combo_t key_combos[] = {
    [C_R_T_LPRN] = COMBO(combo_rt_lprn, KC_LPRN),
    [C_Y_U_RPRN] = COMBO(combo_yu_rprn, KC_RPRN),
    [C_F_G_LBRC] = COMBO(combo_fg_lbrc, KC_LBRC),
    [C_H_J_RBRC] = COMBO(combo_hj_rbrc, KC_RBRC),
    [C_V_B_LCBR] = COMBO(combo_vb_lcbr, KC_LCBR),
    [C_N_M_RCBR] = COMBO(combo_nm_rcbr, KC_RCBR),
    [C_F_J_EQL]  = COMBO(combo_fj_eql, KC_EQL),
    [C_W_E_MINS] = COMBO(combo_we_mins, KC_MINS),
    [C_E_R_PLUS] = COMBO(combo_er_plus, KC_PLUS),
    [C_U_I_UNDS] = COMBO(combo_ui_unds, KC_UNDS),
    [C_I_O_PIPE] = COMBO(combo_io_pipe, KC_PIPE),
    [C_D_F_QUOT] = COMBO(combo_df_quot, KC_QUOT),
    [C_J_K_DQUO] = COMBO(combo_jk_dquo, KC_DQUO),
    [C_S_D_GRV]  = COMBO(combo_sd_grv, KC_GRV),
    [C_K_L_TILD] = COMBO(combo_kl_tild, KC_TILD),
};

uint16_t get_combo_term(uint16_t combo_index, combo_t *combo) {
    (void)combo;

    // Tighten timings on more roll-prone pairs; loosen the cross-hand equals combo slightly.
    switch (combo_index) {
        case C_W_E_MINS:
        case C_E_R_PLUS:
        case C_U_I_UNDS:
        case C_I_O_PIPE:
        case C_S_D_GRV:
        case C_K_L_TILD:
            return COMBO_TERM_ROLL;
        case C_D_F_QUOT:
        case C_J_K_DQUO:
        case C_R_T_LPRN:
            return COMBO_TERM_TIGHT;
        case C_F_J_EQL:
            return COMBO_TERM_CROSS;
        default:
            return COMBO_TERM;
    }
}

bool get_combo_must_tap(uint16_t combo_index, combo_t *combo) {
    (void)combo_index;
    (void)combo;
    return true;
}

bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    (void)combo_index;
    (void)combo;
    (void)keycode;
    (void)record;

    // Symbol combos are a base-layer typing primitive, not a layer-local feature.
    return get_highest_layer(layer_state | default_layer_state) == L_BASE;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [L_BASE] = LAYOUT_split_3x6_3(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
        KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_QUOT, KC_SCLN,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
        KC_LGUI, NAV_TAB, KC_SPC,  KC_ENT,  KC_BSPC, SYS_CAP
    ),

    [L_NAV] = LAYOUT_split_3x6_3(
        QK_LLCK, KC_1,       KC_2,       KC_3,       KC_4,       KC_5,       KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_DEL,
        G(KC_A), MAC_UNDO,   MAC_CUT,    MAC_COPY,   MAC_PASTE,  MAC_REDO,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, QK_REP,  QK_AREP,
        QK_LLCK, A(KC_LEFT), A(KC_RGHT), G(KC_LEFT), G(KC_RGHT), MAC_FIND,   KC_HOME, KC_PGDN, KC_PGUP, KC_END,  TAB_PREV, TAB_NEXT,
        _______, _______,    _______,     _______,    _______,    _______
    ),

    [L_SYS] = LAYOUT_split_3x6_3(
        KC_F1,   KC_F2,     KC_F3,     KC_F4,      KC_F5,      KC_F6,      KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
        CW_TOGG, MAC_SPOT,  MAC_MCTL,  MAC_DESK_L, MAC_DESK_R, MAC_SSHOT,  KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU,
        QK_LLCK, KC_BRID,   KC_BRIU,   KC_WBAK,    KC_WFWD,    XXXXXXX,    TAB_PREV, TAB_NEXT, G(KC_W), G(KC_T), G(KC_R), XXXXXXX,
        _______, _______,   _______,    _______,    _______,    _______
    ),

    [L_KEYBOARD] = LAYOUT_split_3x6_3(
        QK_BOOT, EE_CLR,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        RM_TOGG, RM_HUEU, RM_SATU, RM_VALU, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KO_TOGG, CW_TOGG,
        RM_NEXT, RM_HUED, RM_SATD, RM_VALD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        _______, _______, _______, _______, _______, _______
    ),
};

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, L_NAV, L_SYS, L_KEYBOARD);
}

bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    (void)record;

    switch (keycode) {
        case NAV_TAB:
        case SYS_CAP:
            return true;
        default:
            return false;
    }
}
