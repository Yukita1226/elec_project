// 1. Force the SDL define before anything else
#ifndef LV_USE_SDL
#define LV_USE_SDL 1
#endif

#include "lvgl/lvgl.h"
#include "lvgl/src/drivers/sdl/lv_sdl_window.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mouse.h"
#include <iostream>
#include <windows.h>

// --- THE BUTTON EVENT HANDLER ---
// LVGL will run this function every time something happens to the button
static void btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    // We only care if the action was a full "CLICK"
    if(code == LV_EVENT_CLICKED) {
        std::cout << "you click it 3 sec" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Initialize Core and Drivers
    lv_init();
    lv_display_t * disp = lv_sdl_window_create(800, 480);
    lv_indev_t * mouse = lv_sdl_mouse_create();

    // --- DECORATE THE UI ---
    
    // 1. Get the background screen and set it to a nice dark grey
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_palette_main(LV_PALETTE_BLUE_GREY), LV_PART_MAIN);

    // 2. Create the Button
    lv_obj_t * btn = lv_btn_create(screen);
    lv_obj_set_size(btn, 150, 60);                  // Make it 150px wide, 60px tall
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);       // Put it dead center
    
    // 3. Attach our Event Handler to the button
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);

    // 4. Create a Label (Text) and put it inside the Button
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Click Me!");
    lv_obj_center(label);                           // Center text inside the button

    std::cout << "Success! Window open and UI loaded." << std::endl;

    // The Main Loop
    while(1) {
        lv_timer_handler();
        Sleep(5);
    }
    
    return 0;
}

//g++ main.cpp api/*.cpp -I. -I./lvgl -I./lvgl/src -I./lvgl/src/drivers/sdl -DLV_CONF_INCLUDE_SIMPLE -DLV_USE_SDL=1 -L. -o app.exe "-Wl,--start-group" liblvgl.a -lcurl -lSDL2 "-Wl,--end-group"