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

#define COMBO_TERM_MIN         26
#define COMBO_TERM_MINUS       27
#define COMBO_TERM_PAREN       28
#define COMBO_TERM_RCBR        32
#define COMBO_TERM_LCBR        34
#define COMBO_TERM_CROSS_HAND  38
#define NAV_TAPPING_TERM      160
#define SYS_TAPPING_TERM      170
#define LAYER_INDICATOR_DELAY 90

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

#ifdef RGB_MATRIX_ENABLE
enum indicator_leds {
    LED_LEFT_SHIFT  = 2,
    LED_NAV_THUMB   = 20,
    LED_RIGHT_SHIFT = 38,
    LED_SYS_THUMB   = 40,
};

enum indicator_brightness {
    INDICATOR_OFF = 0,
    INDICATOR_DIM = 20,
    INDICATOR_MID = 48,
    INDICATOR_ON  = 96,
};
#endif

#ifdef RGB_MATRIX_ENABLE
static uint32_t nav_layer_timer = 0;
static uint32_t sys_layer_timer = 0;
#endif

static const key_override_t delete_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_BSPC, KC_DEL);
static const key_override_t tilde_esc_override  = ko_make_basic(MOD_MASK_SHIFT, KC_ESC, S(KC_GRV));

const key_override_t *const key_overrides[] = {
    &delete_key_override,
    &tilde_esc_override,
};

static uint8_t active_layer(void) {
    return get_highest_layer(layer_state | default_layer_state);
}

static bool is_thumb_layer_key(uint16_t keycode) {
    switch (keycode) {
        case NAV_TAB:
        case SYS_CAP:
            return true;
        default:
            return false;
    }
}

static uint16_t thumb_layer_tapping_term(uint16_t keycode) {
    switch (keycode) {
        case NAV_TAB:
            return NAV_TAPPING_TERM;
        case SYS_CAP:
            return SYS_TAPPING_TERM;
        default:
            return TAPPING_TERM;
    }
}

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

    // Keep all custom timings at or above 26 ms, then add room only where the chord
    // is physically harder or unusually valuable.
    switch (combo_index) {
        case C_W_E_MINS:
            return COMBO_TERM_MINUS;
        case C_V_B_LCBR:
            return COMBO_TERM_LCBR;
        case C_N_M_RCBR:
            return COMBO_TERM_RCBR;
        case C_E_R_PLUS:
        case C_U_I_UNDS:
        case C_I_O_PIPE:
        case C_S_D_GRV:
        case C_K_L_TILD:
        case C_D_F_QUOT:
        case C_J_K_DQUO:
            return COMBO_TERM_MIN;
        case C_R_T_LPRN:
            return COMBO_TERM_PAREN;
        case C_F_J_EQL:
            return COMBO_TERM_CROSS_HAND;
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
    return active_layer() == L_BASE;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [L_BASE] = LAYOUT_split_3x6_3(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
        KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_QUOT, KC_SCLN,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
        KC_LGUI, NAV_TAB, KC_SPC,  KC_ENT,  SYS_CAP, KC_BSPC
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
        RM_TOGG, RM_HUEU, RM_SATU, RM_VALU, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KO_TOGG, CM_TOGG,
        RM_NEXT, RM_HUED, RM_SATD, RM_VALD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        _______, _______, _______, _______, _______, _______
    ),
};

layer_state_t layer_state_set_user(layer_state_t state) {
    state = update_tri_layer_state(state, L_NAV, L_SYS, L_KEYBOARD);

#ifdef RGB_MATRIX_ENABLE
    static bool nav_was_on = false;
    static bool sys_was_on = false;

    bool nav_is_on = layer_state_cmp(state, L_NAV);
    bool sys_is_on = layer_state_cmp(state, L_SYS);

    if (nav_is_on && !nav_was_on) {
        nav_layer_timer = timer_read32();
    }
    if (sys_is_on && !sys_was_on) {
        sys_layer_timer = timer_read32();
    }

    nav_was_on = nav_is_on;
    sys_was_on = sys_is_on;
#endif

    return state;
}

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    (void)record;
    return thumb_layer_tapping_term(keycode);
}

uint16_t get_quick_tap_term(uint16_t keycode, keyrecord_t *record) {
    (void)record;

    return is_thumb_layer_key(keycode) ? 0 : QUICK_TAP_TERM;
}

bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    (void)record;
    return is_thumb_layer_key(keycode);
}

#ifdef RGB_MATRIX_ENABLE
static void set_indicator_color(uint8_t index, uint8_t red, uint8_t green, uint8_t blue) {
    rgb_matrix_set_color(index, red, green, blue);
}

static void set_thumb_pair(
    uint8_t left_red,
    uint8_t left_green,
    uint8_t left_blue,
    uint8_t right_red,
    uint8_t right_green,
    uint8_t right_blue
) {
    set_indicator_color(LED_NAV_THUMB, left_red, left_green, left_blue);
    set_indicator_color(LED_SYS_THUMB, right_red, right_green, right_blue);
}

bool rgb_matrix_indicators_user(void) {
    uint8_t layer = active_layer();

    if (is_caps_word_on()) {
        set_indicator_color(LED_LEFT_SHIFT, 0, INDICATOR_ON, 40);
        set_indicator_color(LED_RIGHT_SHIFT, 0, INDICATOR_ON, 40);
    }

#ifdef COMBO_ENABLE
    if (layer == L_BASE && !is_combo_enabled()) {
        set_thumb_pair(INDICATOR_MID, 0, INDICATOR_MID, INDICATOR_MID, 0, INDICATOR_MID);
    }
#endif

    switch (layer) {
        case L_NAV:
            if (timer_elapsed32(nav_layer_timer) >= LAYER_INDICATOR_DELAY) {
                set_thumb_pair(0, INDICATOR_MID, INDICATOR_ON, 0, INDICATOR_DIM, INDICATOR_DIM);
            }
            break;
        case L_SYS:
            if (timer_elapsed32(sys_layer_timer) >= LAYER_INDICATOR_DELAY) {
                set_thumb_pair(INDICATOR_DIM, INDICATOR_DIM / 2, INDICATOR_OFF, INDICATOR_ON, INDICATOR_MID, INDICATOR_OFF);
            }
            break;
        case L_KEYBOARD:
            set_thumb_pair(INDICATOR_ON, 0, 0, INDICATOR_ON, 0, 0);
            break;
        default:
            break;
    }

    return true;
}
#endif
