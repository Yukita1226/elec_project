#include "../ui/page.h"
#include "lvgl/lvgl.h"
#include "../../api/api.h"
#include <string>
#include <cstdio>   // snprintf

// ─────────────────────────────────────────────────────────────────────────────
//  PALETTE
// ─────────────────────────────────────────────────────────────────────────────
#define CLR_BG          lv_color_hex(0x0D1117)
#define CLR_CARD        lv_color_hex(0x161B22)
#define CLR_CARD_SOLAR  lv_color_hex(0xFFB300)   // amber  – voltage
#define CLR_CARD_EV     lv_color_hex(0x00BCD4)   // cyan   – current
#define CLR_CARD_CAR    lv_color_hex(0x66BB6A)   // green  – watt
#define CLR_TEXT        lv_color_hex(0xE6EDF3)
#define CLR_SUBTEXT     lv_color_hex(0x8B949E)
#define CLR_DIVIDER     lv_color_hex(0x21262D)
#define CLR_POSITIVE    lv_color_hex(0x3FB950)
#define CLR_NEGATIVE    lv_color_hex(0xF85149)

// ─────────────────────────────────────────────────────────────────────────────
//  CHART
// ─────────────────────────────────────────────────────────────────────────────
#define CHART_POINTS    20

static lv_chart_series_t * ser_voltage = nullptr;
static lv_chart_series_t * ser_amp     = nullptr;
static lv_chart_series_t * ser_watt    = nullptr;
static lv_obj_t           * chart      = nullptr;

// ─────────────────────────────────────────────────────────────────────────────
//  LIVE WIDGETS
// ─────────────────────────────────────────────────────────────────────────────
struct CardWidgets {
    lv_obj_t * val_ptotal;
    lv_obj_t * val_ptransfer;
    lv_obj_t * flow_dot;
    lv_obj_t * bar;
};

static CardWidgets w_voltage, w_amp, w_watt;
static lv_obj_t  * lbl_last_update = nullptr;
static lv_timer_t * refresh_timer  = nullptr;

// ─────────────────────────────────────────────────────────────────────────────
//  HELPERS
// ─────────────────────────────────────────────────────────────────────────────
static void set_grid_value(const CardWidgets & w,
                           float value, const char* unit,
                           float max_scale,
                           lv_color_t accent)
{
    char buf[64];

    // Main value
    snprintf(buf, sizeof(buf), "%.2f %s", value, unit);
    lv_label_set_text(w.val_ptotal, buf);

    // Hide transfer label
    lv_label_set_text(w.val_ptransfer, "");

    // Flow dot — green if value > 0
    lv_obj_set_style_bg_color(w.flow_dot,
        value > 0.0f ? CLR_POSITIVE : CLR_NEGATIVE, LV_PART_MAIN);

    // Bar
    int32_t bar_val = (int32_t)((value / max_scale) * 100.0f);
    if (bar_val > 100) bar_val = 100;
    if (bar_val < 0)   bar_val = 0;
    lv_bar_set_value(w.bar, bar_val, LV_ANIM_ON);
    lv_obj_set_style_bg_color(w.bar, accent, LV_PART_INDICATOR);
}

// ─────────────────────────────────────────────────────────────────────────────
//  DATA FETCH & UI UPDATE
// ─────────────────────────────────────────────────────────────────────────────
static void do_refresh(lv_timer_t * /*t*/ = nullptr)
{
    static Recivedata a;
    Grid grid_data = a.getGrid();

    // Update cards
    set_grid_value(w_voltage, grid_data.Voltage, "V",  300.0f, CLR_CARD_SOLAR);
    set_grid_value(w_amp,     grid_data.Amm,     "A",  30.0f,  CLR_CARD_EV);
    set_grid_value(w_watt,    grid_data.Watt,    "W",  10000.0f, CLR_CARD_CAR);

    // Push chart points
    lv_chart_set_next_value(chart, ser_voltage, (lv_coord_t)(grid_data.Voltage));
    lv_chart_set_next_value(chart, ser_amp,     (lv_coord_t)(grid_data.Amm * 10));
    lv_chart_set_next_value(chart, ser_watt,    (lv_coord_t)(grid_data.Watt / 10));
    lv_chart_refresh(chart);

    // Timestamp
    static uint32_t tick = 0;
    char ts[40];
    snprintf(ts, sizeof(ts), "Last updated: %lu s ago", (unsigned long)(lv_tick_get() / 1000 - tick));
    tick = lv_tick_get() / 1000;
    lv_label_set_text(lbl_last_update, ts);
}

// ─────────────────────────────────────────────────────────────────────────────
//  CARD BUILDER
// ─────────────────────────────────────────────────────────────────────────────
static CardWidgets build_card(lv_obj_t * parent,
                              const char * title,
                              const char * icon,
                              lv_color_t accent,
                              lv_coord_t x, lv_coord_t y,
                              lv_coord_t w, lv_coord_t h)
{
    lv_obj_t * card = lv_obj_create(parent);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, w, h);
    lv_obj_set_scrollbar_mode(card, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_style_bg_color(card,     CLR_CARD,    LV_PART_MAIN);
    lv_obj_set_style_border_color(card, accent,      LV_PART_MAIN);
    lv_obj_set_style_border_width(card, 2,           LV_PART_MAIN);
    lv_obj_set_style_border_opa(card,   120,         LV_PART_MAIN);
    lv_obj_set_style_radius(card,       10,          LV_PART_MAIN);
    lv_obj_set_style_pad_all(card,      10,          LV_PART_MAIN);
    lv_obj_set_style_shadow_width(card, 12,          LV_PART_MAIN);
    lv_obj_set_style_shadow_color(card, accent,      LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(card,   60,          LV_PART_MAIN);

    // Accent top strip
    lv_obj_t * strip = lv_obj_create(card);
    lv_obj_set_size(strip, w - 20, 4);
    lv_obj_set_pos(strip, 0, 0);
    lv_obj_set_style_bg_color(strip, accent, LV_PART_MAIN);
    lv_obj_set_style_border_width(strip, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(strip, 2, LV_PART_MAIN);

    // Flow dot
    lv_obj_t * dot = lv_obj_create(card);
    lv_obj_set_size(dot, 10, 10);
    lv_obj_align(dot, LV_ALIGN_TOP_RIGHT, 0, 8);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(dot, CLR_SUBTEXT, LV_PART_MAIN);
    lv_obj_set_style_border_width(dot, 0, LV_PART_MAIN);

    // Icon
    lv_obj_t * ico = lv_label_create(card);
    lv_label_set_text(ico, icon);
    lv_obj_align(ico, LV_ALIGN_TOP_LEFT, 0, 10);
    lv_obj_set_style_text_color(ico, accent, LV_PART_MAIN);
    lv_obj_set_style_text_font(ico, &lv_font_montserrat_20, LV_PART_MAIN);

    // Title
    lv_obj_t * ttl = lv_label_create(card);
    lv_label_set_text(ttl, title);
    lv_obj_align(ttl, LV_ALIGN_TOP_LEFT, 0, 34);
    lv_obj_set_style_text_color(ttl, CLR_SUBTEXT, LV_PART_MAIN);
    lv_obj_set_style_text_font(ttl, &lv_font_montserrat_12, LV_PART_MAIN);

    // Main value
    lv_obj_t * val = lv_label_create(card);
    lv_label_set_text(val, "-- ");
    lv_obj_align(val, LV_ALIGN_TOP_LEFT, 0, 52);
    lv_obj_set_style_text_color(val, CLR_TEXT, LV_PART_MAIN);
    lv_obj_set_style_text_font(val, &lv_font_montserrat_20, LV_PART_MAIN);

    // Transfer label (hidden for grid)
    lv_obj_t * xfer_lbl = lv_label_create(card);
    lv_label_set_text(xfer_lbl, "");
    lv_obj_align(xfer_lbl, LV_ALIGN_TOP_LEFT, 0, 90);
    lv_obj_set_style_text_color(xfer_lbl, CLR_SUBTEXT, LV_PART_MAIN);
    lv_obj_set_style_text_font(xfer_lbl, &lv_font_montserrat_10, LV_PART_MAIN);

    lv_obj_t * xfer = lv_label_create(card);
    lv_label_set_text(xfer, "");
    lv_obj_align(xfer, LV_ALIGN_TOP_LEFT, 0, 104);
    lv_obj_set_style_text_color(xfer, CLR_TEXT, LV_PART_MAIN);
    lv_obj_set_style_text_font(xfer, &lv_font_montserrat_14, LV_PART_MAIN);

    // Power bar
    lv_obj_t * bar = lv_bar_create(card);
    lv_obj_set_size(bar, w - 20, 6);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_bar_set_range(bar, 0, 100);
    lv_bar_set_value(bar, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(bar, CLR_DIVIDER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(bar, accent,      LV_PART_INDICATOR);
    lv_obj_set_style_radius(bar, 3, LV_PART_MAIN);
    lv_obj_set_style_radius(bar, 3, LV_PART_INDICATOR);

    return CardWidgets { val, xfer, dot, bar };
}

// ─────────────────────────────────────────────────────────────────────────────
//  CHART BUILDER
// ─────────────────────────────────────────────────────────────────────────────
static void build_chart(lv_obj_t * parent,
                        lv_coord_t x, lv_coord_t y,
                        lv_coord_t w, lv_coord_t h)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_pos(cont, x, y);
    lv_obj_set_size(cont, w, h);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(cont,     CLR_CARD,    LV_PART_MAIN);
    lv_obj_set_style_border_color(cont, CLR_DIVIDER, LV_PART_MAIN);
    lv_obj_set_style_border_width(cont, 1,           LV_PART_MAIN);
    lv_obj_set_style_radius(cont,       10,          LV_PART_MAIN);
    lv_obj_set_style_pad_all(cont,      10,          LV_PART_MAIN);

    // Title
    lv_obj_t * ttl = lv_label_create(cont);
    lv_label_set_text(ttl, "Grid History");
    lv_obj_align(ttl, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_color(ttl, CLR_SUBTEXT, LV_PART_MAIN);
    lv_obj_set_style_text_font(ttl, &lv_font_montserrat_12, LV_PART_MAIN);

    // Legend dots
    const struct { const char * name; lv_color_t color; lv_coord_t ox; } legend[] = {
        { "Voltage", CLR_CARD_SOLAR, 0   },
        { "Amp",     CLR_CARD_EV,    80  },
        { "Watt",    CLR_CARD_CAR,   145 },
    };
    for (auto & l : legend) {
        lv_obj_t * d = lv_obj_create(cont);
        lv_obj_set_size(d, 8, 8);
        lv_obj_set_pos(d, w - 170 + l.ox, 2);
        lv_obj_set_style_radius(d, LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_set_style_bg_color(d, l.color, LV_PART_MAIN);
        lv_obj_set_style_border_width(d, 0, LV_PART_MAIN);

        lv_obj_t * ld = lv_label_create(cont);
        lv_label_set_text(ld, l.name);
        lv_obj_set_pos(ld, w - 158 + l.ox, 0);
        lv_obj_set_style_text_color(ld, CLR_SUBTEXT, LV_PART_MAIN);
        lv_obj_set_style_text_font(ld, &lv_font_montserrat_10, LV_PART_MAIN);
    }

    // Chart widget
    chart = lv_chart_create(cont);
    lv_obj_set_size(chart, w - 20, h - 40);
    lv_obj_align(chart, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, CHART_POINTS);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 1000);

    lv_obj_set_style_bg_color(chart,     CLR_CARD,    LV_PART_MAIN);
    lv_obj_set_style_border_width(chart, 0,           LV_PART_MAIN);
    lv_obj_set_style_line_color(chart,   CLR_DIVIDER, LV_PART_MAIN);
    lv_obj_set_style_line_width(chart,   1,           LV_PART_MAIN);
    lv_chart_set_div_line_count(chart, 4, 5);

    lv_obj_set_style_width(chart,  0, LV_PART_INDICATOR);
    lv_obj_set_style_height(chart, 0, LV_PART_INDICATOR);

    ser_voltage = lv_chart_add_series(chart, CLR_CARD_SOLAR, LV_CHART_AXIS_PRIMARY_Y);
    ser_amp     = lv_chart_add_series(chart, CLR_CARD_EV,    LV_CHART_AXIS_PRIMARY_Y);
    ser_watt    = lv_chart_add_series(chart, CLR_CARD_CAR,   LV_CHART_AXIS_PRIMARY_Y);

    for (int i = 0; i < CHART_POINTS; i++) {
        lv_chart_set_next_value(chart, ser_voltage, 0);
        lv_chart_set_next_value(chart, ser_amp,     0);
        lv_chart_set_next_value(chart, ser_watt,    0);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  PAGE ENTRY POINT
// ─────────────────────────────────────────────────────────────────────────────
void create_page_t2()
{
    lv_obj_t * scr = lv_screen_active();

    lv_obj_set_style_bg_color(scr,   CLR_BG, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr,     LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(scr,   LV_SCROLLBAR_MODE_OFF);

    // Header bar
    lv_obj_t * header = lv_obj_create(scr);
    lv_obj_set_size(header, LV_HOR_RES, 40);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_scrollbar_mode(header, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(header,     CLR_CARD, LV_PART_MAIN);
    lv_obj_set_style_border_width(header, 0,        LV_PART_MAIN);
    lv_obj_set_style_radius(header,       0,        LV_PART_MAIN);
    lv_obj_set_style_pad_all(header,      0,        LV_PART_MAIN);

    lv_obj_t * title = lv_label_create(header);
    lv_label_set_text(title, LV_SYMBOL_CHARGE "  Grid Monitor");
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 12, 0);
    lv_obj_set_style_text_color(title, CLR_TEXT, LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, LV_PART_MAIN);

    lbl_last_update = lv_label_create(header);
    lv_label_set_text(lbl_last_update, "Last updated: --");
    lv_obj_align(lbl_last_update, LV_ALIGN_RIGHT_MID, -12, 0);
    lv_obj_set_style_text_color(lbl_last_update, CLR_SUBTEXT, LV_PART_MAIN);
    lv_obj_set_style_text_font(lbl_last_update, &lv_font_montserrat_10, LV_PART_MAIN);

    // Card row
    const lv_coord_t CARD_Y   = 48;
    const lv_coord_t CARD_H   = 148;
    const lv_coord_t CARD_W   = 142;
    const lv_coord_t CARD_GAP = 6;

    w_voltage = build_card(scr, "VOLTAGE", LV_SYMBOL_IMAGE,  CLR_CARD_SOLAR,
                            CARD_GAP,                         CARD_Y, CARD_W, CARD_H);
    w_amp     = build_card(scr, "CURRENT", LV_SYMBOL_CHARGE, CLR_CARD_EV,
                            CARD_GAP + CARD_W + CARD_GAP,    CARD_Y, CARD_W, CARD_H);
    w_watt    = build_card(scr, "WATT",    LV_SYMBOL_DRIVE,  CLR_CARD_CAR,
                            CARD_GAP + (CARD_W + CARD_GAP) * 2, CARD_Y, CARD_W, CARD_H);

    // Chart
    const lv_coord_t CHART_Y = CARD_Y + CARD_H + 8;
    const lv_coord_t CHART_H = LV_VER_RES - CHART_Y - 10;
    build_chart(scr, CARD_GAP, CHART_Y, LV_HOR_RES - CARD_GAP * 2, CHART_H);

    // Timer
    refresh_timer = lv_timer_create(do_refresh, 1000, nullptr);

    // First load
    do_refresh();
}
