// Copyright 2026 0xse.reshka
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#if defined(SPLIT_KEYBOARD) && defined(RGB_MATRIX_ENABLE)
#include "transactions.h"
#endif

enum layer_names {
    L_BASE,
    L_NAV,
    L_NUMSYS,
    L_KEYBOARD,
};

enum custom_keycodes {
    NAV_FIND = SAFE_RANGE,
    NAV_FIND_GLOBAL,
    NAV_FIND_NEXT,
    NAV_FIND_PREV,
    MAC_GLOBE,
};

enum combo_names {
    C_R_T_LPRN,
    C_Y_U_RPRN,
    C_F_G_LBRC,
    C_H_J_RBRC,
    C_V_B_LCBR,
    C_N_M_RCBR,
    C_D_F_MINS,
    C_D_K_UNDS,
    C_F_J_EQL,
    C_J_K_PLUS,
    C_S_D_GRV,
};

#define COMBO_TERM_TIGHT       26
#define COMBO_TERM_RT_LPRN     28
#define COMBO_TERM_NM_RCBR     32
#define COMBO_TERM_FJ_EQL      34
#define COMBO_TERM_DK_UNDS     38
#define NAV_TAPPING_TERM      160
#define NUMSYS_TAPPING_TERM   170
#define LAYER_INDICATOR_DELAY 90
#define APPLE_GLOBE_USAGE     AC_NEXT_KEYBOARD_LAYOUT_SELECT

#define NAV_CAPS   LT(L_NAV, KC_CAPS)
#define NUMSYS_TAB LT(L_NUMSYS, KC_TAB)

#define MAC_UNDO   G(KC_Z)
#define MAC_COPY   G(KC_C)
#define MAC_PASTE  G(KC_V)
#define MAC_DELETE_WORD_LEFT A(KC_BSPC)
#define MAC_KILL_TO_END      C(KC_K)
#define NAV_WORD_NEXT  A(KC_RGHT)
#define NAV_WORD_PREV  A(KC_LEFT)
#define NAV_LINE_START G(KC_LEFT)
#define NAV_LINE_END   G(KC_RGHT)
#define NAV_DOC_END    G(KC_DOWN)

#ifdef RGB_MATRIX_ENABLE
enum indicator_brightness {
    INDICATOR_OFF = 0,
    INDICATOR_DIM = 20,
    INDICATOR_MID = 48,
    INDICATOR_ON  = 96,
};

enum indicator_key_position {
    INDICATOR_LEFT_SHIFT_ROW       = 2,
    INDICATOR_LEFT_SHIFT_COL       = 0,
    INDICATOR_RIGHT_SHIFT_ROW      = 6,
    INDICATOR_RIGHT_SHIFT_COL      = 0,
    INDICATOR_LEFT_LAYER_THUMB_ROW = 3,
    INDICATOR_LEFT_LAYER_THUMB_COL = 4,
    INDICATOR_RIGHT_LAYER_THUMB_ROW = 7,
    INDICATOR_RIGHT_LAYER_THUMB_COL = 4,
};
#endif

#ifdef RGB_MATRIX_ENABLE
static uint32_t nav_layer_timer    = 0;
static uint32_t numsys_layer_timer = 0;
static uint8_t  last_indicator_layer = L_BASE;
#endif

#if defined(SPLIT_KEYBOARD) && defined(RGB_MATRIX_ENABLE)
enum indicator_flags {
    INDICATOR_CAPS_LOCK_ON  = 1 << 0,
    INDICATOR_CAPS_WORD_ON  = 1 << 1,
    INDICATOR_COMBOS_ACTIVE = 1 << 2,
};

// Layer state is synced by QMK core. Keep a tiny custom payload only for
// indicator state that QMK does not mirror automatically.
static uint8_t indicator_state = INDICATOR_COMBOS_ACTIVE;
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

#ifdef RGB_MATRIX_ENABLE
static void refresh_indicator_timers(uint8_t current_layer) {
    if (last_indicator_layer != L_NAV && current_layer == L_NAV) {
        nav_layer_timer = timer_read32();
    }
    if (last_indicator_layer != L_NUMSYS && current_layer == L_NUMSYS) {
        numsys_layer_timer = timer_read32();
    }

    last_indicator_layer = current_layer;
}

static bool local_combos_enabled(void) {
#ifdef COMBO_ENABLE
    return is_combo_enabled();
#else
    return true;
#endif
}

static bool local_caps_lock_on(void) {
    return host_keyboard_led_state().caps_lock;
}

static bool local_caps_word_on(void) {
    return is_caps_word_on();
}
#endif

#if defined(SPLIT_KEYBOARD) && defined(RGB_MATRIX_ENABLE)
static uint8_t current_indicator_state(void) {
    uint8_t state = 0;

    if (local_caps_lock_on()) {
        state |= INDICATOR_CAPS_LOCK_ON;
    }
    if (local_caps_word_on()) {
        state |= INDICATOR_CAPS_WORD_ON;
    }
    if (local_combos_enabled()) {
        state |= INDICATOR_COMBOS_ACTIVE;
    }

    return state;
}

static bool synced_indicator_state(uint8_t flag) {
    return (indicator_state & flag) != 0;
}

static bool effective_indicator_state(uint8_t flag, bool local_state) {
    return is_keyboard_master() ? local_state : synced_indicator_state(flag);
}

static void indicator_sync_slave(uint8_t initiator2target_buffer_size, const void *initiator2target_buffer, uint8_t target2initiator_buffer_size, void *target2initiator_buffer) {
    (void)target2initiator_buffer_size;
    (void)target2initiator_buffer;

    if (initiator2target_buffer_size == sizeof(indicator_state)) {
        indicator_state = *(const uint8_t *)initiator2target_buffer;
    }
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(RPC_ID_USER_INDICATOR_SYNC, indicator_sync_slave);
}

void housekeeping_task_user(void) {
    if (!is_keyboard_master()) {
        return;
    }

    uint8_t next_state = current_indicator_state();

    if (indicator_state != next_state) {
        indicator_state = next_state;
        transaction_rpc_send(RPC_ID_USER_INDICATOR_SYNC, sizeof(next_state), &next_state);
    }
}
#endif

static bool is_thumb_layer_key(uint16_t keycode) {
    switch (keycode) {
        case NAV_CAPS:
        case NUMSYS_TAB:
            return true;
        default:
            return false;
    }
}

static uint16_t thumb_layer_tapping_term(uint16_t keycode) {
    switch (keycode) {
        case NAV_CAPS:
            return NAV_TAPPING_TERM;
        case NUMSYS_TAB:
            return NUMSYS_TAPPING_TERM;
        default:
            return TAPPING_TERM;
    }
}

static uint8_t active_mods(void) {
    return get_mods() | get_weak_mods() | get_oneshot_mods() | get_oneshot_locked_mods();
}

static bool shift_active(void) {
    return (active_mods() & MOD_MASK_SHIFT) != 0;
}

static void remember_semantic_repeat_key(uint16_t keycode) {
    if (get_repeat_key_count() == 0) {
        set_last_keycode(keycode);
        set_last_mods(0);
    }
}

static void run_semantic_shortcut(uint16_t keycode, uint16_t shortcut) {
    tap_code16(shortcut);
    remember_semantic_repeat_key(keycode);
}

static bool mods_match_undo_pair(uint8_t mods) {
    return (mods & MOD_MASK_GUI) != 0 && (mods & ~(MOD_MASK_GUI | MOD_MASK_SHIFT)) == 0;
}

static bool mods_match_tab_pair(uint8_t mods) {
    return (mods & ~MOD_MASK_SHIFT) == 0;
}

static uint16_t alternate_tab_keycode(uint8_t mods) {
    return (mods & MOD_MASK_SHIFT) ? KC_TAB : S(KC_TAB);
}

static uint16_t alternate_undo_keycode(uint8_t mods) {
    return (mods & MOD_MASK_SHIFT) ? G(KC_Z) : G(S(KC_Z));
}

static const uint16_t PROGMEM combo_rt_lprn[] = {KC_R, KC_T, COMBO_END};
static const uint16_t PROGMEM combo_yu_rprn[] = {KC_Y, KC_U, COMBO_END};
static const uint16_t PROGMEM combo_fg_lbrc[] = {KC_F, KC_G, COMBO_END};
static const uint16_t PROGMEM combo_hj_rbrc[] = {KC_H, KC_J, COMBO_END};
static const uint16_t PROGMEM combo_vb_lcbr[] = {KC_V, KC_B, COMBO_END};
static const uint16_t PROGMEM combo_nm_rcbr[] = {KC_N, KC_M, COMBO_END};
static const uint16_t PROGMEM combo_df_mins[] = {KC_D, KC_F, COMBO_END};
static const uint16_t PROGMEM combo_dk_unds[] = {KC_D, KC_K, COMBO_END};
static const uint16_t PROGMEM combo_fj_eql[]  = {KC_F, KC_J, COMBO_END};
static const uint16_t PROGMEM combo_jk_plus[] = {KC_J, KC_K, COMBO_END};
static const uint16_t PROGMEM combo_sd_grv[]  = {KC_S, KC_D, COMBO_END};

combo_t key_combos[] = {
    [C_R_T_LPRN] = COMBO(combo_rt_lprn, KC_LPRN),
    [C_Y_U_RPRN] = COMBO(combo_yu_rprn, KC_RPRN),
    [C_F_G_LBRC] = COMBO(combo_fg_lbrc, KC_LBRC),
    [C_H_J_RBRC] = COMBO(combo_hj_rbrc, KC_RBRC),
    [C_V_B_LCBR] = COMBO(combo_vb_lcbr, KC_LCBR),
    [C_N_M_RCBR] = COMBO(combo_nm_rcbr, KC_RCBR),
    [C_D_F_MINS] = COMBO(combo_df_mins, KC_MINS),
    [C_D_K_UNDS] = COMBO(combo_dk_unds, KC_UNDS),
    [C_F_J_EQL]  = COMBO(combo_fj_eql, KC_EQL),
    [C_J_K_PLUS] = COMBO(combo_jk_plus, KC_PLUS),
    [C_S_D_GRV]  = COMBO(combo_sd_grv, KC_GRV),
};

uint16_t get_combo_term(uint16_t combo_index, combo_t *combo) {
    (void)combo;

    // Keep the fast combos on one conservative baseline, then name the slower
    // outliers explicitly once a chord needs its own timing.
    switch (combo_index) {
        case C_V_B_LCBR:
        case C_D_F_MINS:
        case C_J_K_PLUS:
        case C_S_D_GRV:
            return COMBO_TERM_TIGHT;
        case C_D_K_UNDS:
            return COMBO_TERM_DK_UNDS;
        case C_F_J_EQL:
            return COMBO_TERM_FJ_EQL;
        case C_N_M_RCBR:
            return COMBO_TERM_NM_RCBR;
        case C_R_T_LPRN:
            return COMBO_TERM_RT_LPRN;
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

uint16_t get_alt_repeat_key_keycode_user(uint16_t keycode, uint8_t mods) {
    switch (keycode) {
        case KC_TAB:
            if (mods_match_tab_pair(mods)) {
                return alternate_tab_keycode(mods);
            }
            return KC_TRNS;
        case KC_Z:
            if (mods_match_undo_pair(mods)) {
                return alternate_undo_keycode(mods);
            }
            return KC_TRNS;
        case NAV_FIND:
            return NAV_FIND_GLOBAL;
        case NAV_FIND_GLOBAL:
            return NAV_FIND;
        // Keep search navigation symmetrical even though it is implemented as
        // custom keycodes rather than plain modded basic keycodes.
        case NAV_FIND_NEXT:
            return NAV_FIND_PREV;
        case NAV_FIND_PREV:
            return NAV_FIND_NEXT;
        default:
            return KC_TRNS;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MAC_GLOBE:
            host_consumer_send(record->event.pressed ? APPLE_GLOBE_USAGE : 0);
            return false;
        default:
            break;
    }

    if (!record->event.pressed) {
        return true;
    }

    switch (keycode) {
        case NAV_FIND:
            if (shift_active()) {
                // Preserve the semantic key so Alt Repeat can flip between
                // local and global search instead of repeating raw chords.
                run_semantic_shortcut(NAV_FIND_GLOBAL, G(S(KC_F)));
            } else {
                run_semantic_shortcut(NAV_FIND, G(KC_F));
            }
            return false;
        case NAV_FIND_GLOBAL:
            run_semantic_shortcut(NAV_FIND_GLOBAL, G(S(KC_F)));
            return false;
        case NAV_FIND_NEXT:
            // Preserve the semantic key so Repeat/Alt Repeat continue to
            // operate on "search next/prev" instead of the raw Cmd+G chord.
            run_semantic_shortcut(NAV_FIND_NEXT, G(KC_G));
            return false;
        case NAV_FIND_PREV:
            run_semantic_shortcut(NAV_FIND_PREV, G(S(KC_G)));
            return false;
        default:
            return true;
    }
}

bool remember_last_key_user(uint16_t keycode, keyrecord_t *record, uint8_t *remembered_mods) {
    (void)record;
    (void)remembered_mods;

    switch (keycode) {
        // Keep Repeat/Alt Repeat biased toward motions, search, and the
        // deliberate undo/redo pair, not toward clipboard actions or a bare
        // forward delete.
        case KC_DEL:
        case MAC_COPY:
        case MAC_PASTE:
            return false;
        default:
            return true;
    }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [L_BASE] = LAYOUT_split_3x6_3(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
        KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
        MAC_GLOBE, NAV_CAPS, KC_SPC,  KC_ENT,  NUMSYS_TAB, KC_BSPC
    ),

    [L_NAV] = LAYOUT_split_3x6_3(
        OSM(MOD_LCTL), QK_LLCK,       NAV_WORD_NEXT, NAV_WORD_NEXT, XXXXXXX, XXXXXXX,    MAC_COPY,      MAC_UNDO,   NAV_LINE_START, XXXXXXX,   MAC_PASTE, KC_DEL,
        OSM(MOD_LALT), NAV_LINE_END,  MAC_DELETE_WORD_LEFT, MAC_KILL_TO_END, XXXXXXX, NAV_DOC_END, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, QK_REP, QK_AREP,
        OSM(MOD_LGUI), XXXXXXX,       XXXXXXX,        XXXXXXX,       XXXXXXX, NAV_WORD_PREV, NAV_FIND_NEXT, XXXXXXX, KC_PGUP,        KC_PGDN,  NAV_FIND,   XXXXXXX,
        _______, _______,     _______,      _______,      _______,   _______
    ),

    [L_NUMSYS] = LAYOUT_split_3x6_3(
        QK_LLCK, KC_1,      KC_2,     KC_3,     KC_4,      KC_5,      KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_DEL,
        KC_BRID, KC_BRIU,   XXXXXXX,  XXXXXXX,  XXXXXXX,   XXXXXXX,   KC_MPRV, KC_MPLY, KC_MNXT, KC_MUTE, KC_VOLD, KC_VOLU,
        KC_F1,   KC_F2,     KC_F3,    KC_F4,    KC_F5,     KC_F6,     KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,
        KC_LGUI, KC_LSFT,   KC_SPC,    _______,   _______,   _______
    ),

    [L_KEYBOARD] = LAYOUT_split_3x6_3(
        QK_BOOT, EE_CLR,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        RM_TOGG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KO_TOGG, CM_TOGG,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        _______, _______, _______, _______, _______, _______
    ),
};

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, L_NAV, L_NUMSYS, L_KEYBOARD);
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
static uint8_t led_index_for_key(uint8_t row, uint8_t col) {
    return g_led_config.matrix_co[row][col];
}

static void set_indicator_color(uint8_t led_min, uint8_t led_max, uint8_t index, uint8_t red, uint8_t green, uint8_t blue) {
    if (index != NO_LED && index >= led_min && index < led_max) {
        rgb_matrix_set_color(index, red, green, blue);
    }
}

static void set_thumb_pair(
    uint8_t led_min,
    uint8_t led_max,
    uint8_t left_red,
    uint8_t left_green,
    uint8_t left_blue,
    uint8_t right_red,
    uint8_t right_green,
    uint8_t right_blue
) {
    set_indicator_color(led_min, led_max, led_index_for_key(INDICATOR_LEFT_LAYER_THUMB_ROW, INDICATOR_LEFT_LAYER_THUMB_COL), left_red, left_green, left_blue);
    set_indicator_color(led_min, led_max, led_index_for_key(INDICATOR_RIGHT_LAYER_THUMB_ROW, INDICATOR_RIGHT_LAYER_THUMB_COL), right_red, right_green, right_blue);
}

static void set_shift_pair(uint8_t led_min, uint8_t led_max, uint8_t red, uint8_t green, uint8_t blue) {
    set_indicator_color(led_min, led_max, led_index_for_key(INDICATOR_LEFT_SHIFT_ROW, INDICATOR_LEFT_SHIFT_COL), red, green, blue);
    set_indicator_color(led_min, led_max, led_index_for_key(INDICATOR_RIGHT_SHIFT_ROW, INDICATOR_RIGHT_SHIFT_COL), red, green, blue);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer = active_layer();

    refresh_indicator_timers(layer);

    bool caps_lock_on  = local_caps_lock_on();
    bool caps_word_on  = local_caps_word_on();
    bool combos_enabled = local_combos_enabled();

#if defined(SPLIT_KEYBOARD)
    caps_lock_on = effective_indicator_state(INDICATOR_CAPS_LOCK_ON, caps_lock_on);
    caps_word_on = effective_indicator_state(INDICATOR_CAPS_WORD_ON, caps_word_on);
    combos_enabled = effective_indicator_state(INDICATOR_COMBOS_ACTIVE, combos_enabled);
#endif

    if (caps_word_on) {
        set_shift_pair(led_min, led_max, 0, INDICATOR_ON, 40);
    } else if (caps_lock_on) {
        set_shift_pair(led_min, led_max, INDICATOR_ON, INDICATOR_MID, INDICATOR_OFF);
    }

#ifdef COMBO_ENABLE
    if (layer == L_BASE && !combos_enabled) {
        set_thumb_pair(led_min, led_max, INDICATOR_MID, 0, INDICATOR_MID, INDICATOR_MID, 0, INDICATOR_MID);
    }
#endif

    switch (layer) {
        case L_NAV:
            if (timer_elapsed32(nav_layer_timer) >= LAYER_INDICATOR_DELAY) {
                set_thumb_pair(led_min, led_max, 0, INDICATOR_MID, INDICATOR_ON, INDICATOR_OFF, INDICATOR_OFF, INDICATOR_OFF);
            }
            break;
        case L_NUMSYS:
            if (timer_elapsed32(numsys_layer_timer) >= LAYER_INDICATOR_DELAY) {
                set_thumb_pair(led_min, led_max, INDICATOR_DIM, INDICATOR_DIM / 2, INDICATOR_OFF, INDICATOR_ON, INDICATOR_MID, INDICATOR_OFF);
            }
            break;
        case L_KEYBOARD:
            set_thumb_pair(led_min, led_max, INDICATOR_ON, 0, 0, INDICATOR_ON, 0, 0);
            break;
        default:
            break;
    }

    return true;
}
#endif
