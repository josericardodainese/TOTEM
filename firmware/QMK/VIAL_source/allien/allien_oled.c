/* Allien -- OLED menu and status screens (SSD1306 128x64 over I2C)
 *
 * Copyright 2026 josericardodainese
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Interaction model:
 *
 *   short click      -> next profile          (in any state)
 *   turn             -> navigate / adjust
 *   long click 500ms -> open menu, enter item, confirm, go back
 *
 * The short click is reserved for the profile, so the menu needs the long
 * click to confirm -- otherwise there would be no way to enter an item.
 *
 * With the default 6x8 font the panel is 21 columns x 8 rows.
 */

#include "allien_oled.h"
#include <stdio.h>
#include <string.h>

#define OLED_COLS 21
#define OLED_ROWS 8

// ┌─────────────────────────────────────────────────┐
// │ s t a t e                                       │
// └─────────────────────────────────────────────────┘

typedef enum { UI_IDLE, UI_MENU, UI_EDIT } ui_state_t;

typedef enum {
    SCREEN_STATUS,
    SCREEN_APM,
    SCREEN_TIMER,
    SCREEN_LOGO,
    SCREEN_COUNT,
} screen_mode_t;

/* What a turn does while the status screen is showing. */
typedef enum {
    ENC_MODE_MENU,   // open and drive the menu (default)
    ENC_MODE_KEYS,   // send keycodes instead; menu still reachable by long press
    ENC_MODE_COUNT,
} encoder_mode_t;

typedef enum {
    ITEM_PROFILE,
    ITEM_SCREEN,
    ITEM_RGB_VAL,
    ITEM_RGB_MODE,
    ITEM_ENC_MODE,
    ITEM_TIMER_RUN,
    ITEM_TIMER_RESET,
    ITEM_BOOTLOADER,
    ITEM_COUNT,
} menu_item_t;

/* Items that hold a value are entered with a long press; the rest fire
 * immediately and stay in the list. */
static bool item_is_editable(uint8_t item) {
    return item <= ITEM_ENC_MODE;
}

static ui_state_t     ui_state    = UI_IDLE;
static screen_mode_t  screen_mode = SCREEN_STATUS;
static encoder_mode_t enc_mode    = ENC_MODE_MENU;
static uint8_t        menu_sel    = 0;
static uint16_t       last_input  = 0;

// ┌─────────────────────────────────────────────────┐
// │ A P M   c o u n t e r                           │
// └─────────────────────────────────────────────────┘

/* 12 buckets of 5 s = a rolling 60 s window. */
#define APM_BUCKETS   12
#define APM_BUCKET_MS 5000

static uint16_t apm_bucket[APM_BUCKETS];
static uint8_t  apm_head;
static uint16_t apm_last_roll;

static void apm_tick(void) {
    while (timer_elapsed(apm_last_roll) >= APM_BUCKET_MS) {
        apm_last_roll += APM_BUCKET_MS;
        apm_head = (apm_head + 1) % APM_BUCKETS;
        apm_bucket[apm_head] = 0;
    }
}

static uint16_t apm_value(void) {
    uint16_t total = 0;
    for (uint8_t i = 0; i < APM_BUCKETS; i++) total += apm_bucket[i];
    return total;   // one full window == one minute
}

// ┌─────────────────────────────────────────────────┐
// │ s t o p w a t c h                               │
// └─────────────────────────────────────────────────┘

static bool     timer_running;
static uint32_t timer_accum_ms;
static uint32_t timer_started_at;

static uint32_t stopwatch_ms(void) {
    if (!timer_running) return timer_accum_ms;
    return timer_accum_ms + (timer_read32() - timer_started_at);
}

static void stopwatch_toggle(void) {
    if (timer_running) {
        timer_accum_ms += timer_read32() - timer_started_at;
        timer_running = false;
    } else {
        timer_started_at = timer_read32();
        timer_running    = true;
    }
}

static void stopwatch_reset(void) {
    timer_running  = false;
    timer_accum_ms = 0;
}

// ┌─────────────────────────────────────────────────┐
// │ h e l p e r s                                   │
// └─────────────────────────────────────────────────┘

static void note_input(void) {
    last_input = timer_read();
    oled_on();
}

static uint8_t rgb_val(void) {
#ifdef RGB_MATRIX_ENABLE
    return rgb_matrix_get_val();
#else
    return 0;
#endif
}

static const char *enc_mode_name(void) {
    return enc_mode == ENC_MODE_MENU ? "Menu" : "Teclas";
}

static const char *screen_name(uint8_t m) {
    switch (m) {
        case SCREEN_STATUS: return "Status";
        case SCREEN_APM:    return "APM";
        case SCREEN_TIMER:  return "Tempo";
        default:            return "Logo";
    }
}

static const char *item_label(uint8_t item) {
    switch (item) {
        case ITEM_PROFILE:     return "Perfil";
        case ITEM_SCREEN:      return "Tela";
        case ITEM_RGB_VAL:     return "Brilho RGB";
        case ITEM_RGB_MODE:    return "Efeito RGB";
        case ITEM_ENC_MODE:    return "Encoder";
        case ITEM_TIMER_RUN:   return "Cronometro";
        case ITEM_TIMER_RESET: return "Zerar tempo";
        default:               return "Reiniciar";
    }
}

/* Right-hand value shown next to each item in the list. */
static void item_value(uint8_t item, char *out, size_t n) {
    switch (item) {
        case ITEM_PROFILE:  snprintf(out, n, "%s", allien_profile_name(allien_profile_current())); break;
        case ITEM_SCREEN:   snprintf(out, n, "%s", screen_name(screen_mode)); break;
        case ITEM_RGB_VAL:  snprintf(out, n, "%u", (unsigned)rgb_val()); break;
#ifdef RGB_MATRIX_ENABLE
        case ITEM_RGB_MODE: snprintf(out, n, "%u", (unsigned)rgb_matrix_get_mode()); break;
#else
        case ITEM_RGB_MODE: snprintf(out, n, "-"); break;
#endif
        case ITEM_ENC_MODE: snprintf(out, n, "%s", enc_mode_name()); break;
        case ITEM_TIMER_RUN: snprintf(out, n, "%s", timer_running ? "on" : "off"); break;
        default: out[0] = '\0'; break;
    }
}

// ┌─────────────────────────────────────────────────┐
// │ i n p u t                                       │
// └─────────────────────────────────────────────────┘

static void edit_adjust(bool clockwise) {
    switch (menu_sel) {
        case ITEM_PROFILE: {
            uint8_t p = allien_profile_current();
            p = clockwise ? (p + 1) % ALLIEN_PROFILE_COUNT
                          : (p + ALLIEN_PROFILE_COUNT - 1) % ALLIEN_PROFILE_COUNT;
            /* The menu can reach every profile, including 3 and 4, which the
             * G+B chord and the short click deliberately skip. */
            allien_profile_set(p);
            break;
        }
        case ITEM_SCREEN:
            screen_mode = clockwise ? (screen_mode + 1) % SCREEN_COUNT
                                    : (screen_mode + SCREEN_COUNT - 1) % SCREEN_COUNT;
            break;
#ifdef RGB_MATRIX_ENABLE
        case ITEM_RGB_VAL:
            if (clockwise) rgb_matrix_increase_val_noeeprom();
            else           rgb_matrix_decrease_val_noeeprom();
            break;
        case ITEM_RGB_MODE:
            if (clockwise) rgb_matrix_step_noeeprom();
            else           rgb_matrix_step_reverse_noeeprom();
            break;
#endif
        case ITEM_ENC_MODE:
            enc_mode = (enc_mode + 1) % ENC_MODE_COUNT;
            break;
        default:
            break;
    }
}

bool allien_oled_rotate(bool clockwise) {
    note_input();

    switch (ui_state) {
        case UI_IDLE:
            if (enc_mode == ENC_MODE_KEYS) {
                return false;   // let the caller send a keycode instead
            }
            ui_state = UI_MENU; // first turn opens the menu
            return true;

        case UI_MENU:
            menu_sel = clockwise ? (menu_sel + 1) % ITEM_COUNT
                                 : (menu_sel + ITEM_COUNT - 1) % ITEM_COUNT;
            return true;

        case UI_EDIT:
            edit_adjust(clockwise);
            return true;
    }
    return true;
}

void allien_oled_long_press(void) {
    note_input();

    switch (ui_state) {
        case UI_IDLE:
            ui_state = UI_MENU;
            break;

        case UI_MENU:
            if (item_is_editable(menu_sel)) {
                ui_state = UI_EDIT;
            } else {
                switch (menu_sel) {
                    case ITEM_TIMER_RUN:   stopwatch_toggle(); break;
                    case ITEM_TIMER_RESET: stopwatch_reset();  break;
                    case ITEM_BOOTLOADER:  reset_keyboard();   break;
                    default: break;
                }
            }
            break;

        case UI_EDIT:
            ui_state = UI_MENU;   // confirm and step back out
            break;
    }
}

void allien_oled_note_keypress(void) {
    apm_tick();
    apm_bucket[apm_head]++;
    note_input();
}

void allien_oled_tick(void) {
    apm_tick();
    if (ui_state != UI_IDLE && timer_elapsed(last_input) > OLED_MENU_TIMEOUT_MS) {
        ui_state = UI_IDLE;
    }
}

// ┌─────────────────────────────────────────────────┐
// │ r e n d e r i n g                               │
// └─────────────────────────────────────────────────┘

static void write_at(uint8_t col, uint8_t row, const char *s, bool invert) {
    oled_set_cursor(col, row);
    oled_write(s, invert);
}

static void draw_rule(uint8_t row) {
    write_at(0, row, "---------------------", false);
}

/* A [####----] style bar, `width` characters wide including the brackets. */
static void draw_bar(uint8_t col, uint8_t row, uint8_t width, uint8_t value, uint8_t max) {
    char buf[OLED_COLS + 1];
    if (width < 3) return;
    if (width > OLED_COLS) width = OLED_COLS;
    uint8_t inner  = width - 2;
    uint8_t filled = max ? (uint8_t)(((uint32_t)value * inner) / max) : 0;
    if (filled > inner) filled = inner;

    buf[0] = '[';
    for (uint8_t i = 0; i < inner; i++) buf[1 + i] = (i < filled) ? '#' : '-';
    buf[1 + inner] = ']';
    buf[2 + inner] = '\0';
    write_at(col, row, buf, false);
}

static void render_status(void) {
    char buf[OLED_COLS + 1];

    snprintf(buf, sizeof(buf), " %-19.19s", allien_profile_name(allien_profile_current()));
    write_at(0, 0, buf, true);          // inverted banner
    draw_rule(1);

    snprintf(buf, sizeof(buf), "Enc   %.14s", enc_mode_name());
    write_at(0, 2, buf, false);

#ifdef RGB_MATRIX_ENABLE
    snprintf(buf, sizeof(buf), "RGB   ef%-3u  v%u",
             (unsigned)rgb_matrix_get_mode(), (unsigned)rgb_val());
#else
    snprintf(buf, sizeof(buf), "RGB   --");
#endif
    write_at(0, 3, buf, false);

    snprintf(buf, sizeof(buf), "APM   %u", (unsigned)apm_value());
    write_at(0, 4, buf, false);

    uint32_t ms = stopwatch_ms();
    snprintf(buf, sizeof(buf), "Tempo %02u:%02u%s",
             (unsigned)(ms / 60000), (unsigned)((ms / 1000) % 60),
             timer_running ? " >" : "");
    write_at(0, 5, buf, false);

    draw_rule(6);
    write_at(0, 7, "girar:menu  seg:ok", false);
}

static void render_apm(void) {
    char buf[OLED_COLS + 1];
    uint16_t v = apm_value();

    write_at(0, 0, "  A P M", false);
    draw_rule(1);

    snprintf(buf, sizeof(buf), "     %u", (unsigned)v);
    write_at(0, 3, buf, false);

    /* 300 APM is a generous full scale for a 20-key pad. */
    draw_bar(0, 5, 21, v > 300 ? 300 : (uint8_t)(v > 255 ? 255 : v), 255);

    snprintf(buf, sizeof(buf), "janela 60s");
    write_at(0, 7, buf, false);
}

static void render_timer(void) {
    char buf[OLED_COLS + 1];
    uint32_t ms = stopwatch_ms();

    write_at(0, 0, "  C R O N O M E T R O", false);
    draw_rule(1);

    snprintf(buf, sizeof(buf), "    %02u:%02u.%u",
             (unsigned)(ms / 60000), (unsigned)((ms / 1000) % 60),
             (unsigned)((ms / 100) % 10));
    write_at(0, 3, buf, false);

    write_at(0, 5, timer_running ? "     rodando" : "     parado", false);
    draw_rule(6);
    write_at(0, 7, "menu > Cronometro", false);
}

static void render_logo(void) {
    write_at(0, 2, "   T O T E M  2 0", false);
    draw_rule(3);
    char buf[OLED_COLS + 1];
    snprintf(buf, sizeof(buf), "      %.14s", allien_profile_name(allien_profile_current()));
    write_at(0, 4, buf, false);
}

static void render_menu(void) {
    char buf[OLED_COLS + 1];
    char val[10];

    snprintf(buf, sizeof(buf), "MENU            %u/%u",
             (unsigned)(menu_sel + 1), (unsigned)ITEM_COUNT);
    write_at(0, 0, buf, false);
    draw_rule(1);

    /* Six visible rows, scrolled to keep the selection in view. */
    const uint8_t visible = 6;
    uint8_t first = 0;
    if (menu_sel >= visible) first = menu_sel - visible + 1;
    if (first + visible > ITEM_COUNT) first = ITEM_COUNT - visible;

    for (uint8_t i = 0; i < visible; i++) {
        uint8_t item = first + i;
        item_value(item, val, sizeof(val));
        /* Precision as well as width: %-12s alone only sets a minimum, so the
         * compiler cannot prove the result fits in buf. */
        snprintf(buf, sizeof(buf), "%c%-12.12s%6.6s",
                 item == menu_sel ? '>' : ' ', item_label(item), val);
        write_at(0, 2 + i, buf, item == menu_sel);
    }
}

static void render_edit(void) {
    char buf[OLED_COLS + 1];
    char val[10];

    snprintf(buf, sizeof(buf), " %-20.20s", item_label(menu_sel));
    write_at(0, 0, buf, true);
    draw_rule(1);

    item_value(menu_sel, val, sizeof(val));
    snprintf(buf, sizeof(buf), "      %.14s", val);
    write_at(0, 3, buf, false);

    if (menu_sel == ITEM_RGB_VAL) {
        draw_bar(0, 5, 21, rgb_val(), RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    }

    draw_rule(6);
    write_at(0, 7, "girar:muda  seg:ok", false);
}

void allien_oled_render(void) {
    oled_clear();

    switch (ui_state) {
        case UI_MENU: render_menu(); return;
        case UI_EDIT: render_edit(); return;
        case UI_IDLE: break;
    }

    switch (screen_mode) {
        case SCREEN_APM:   render_apm();   break;
        case SCREEN_TIMER: render_timer(); break;
        case SCREEN_LOGO:  render_logo();  break;
        default:           render_status();break;
    }
}
