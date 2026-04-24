// 1. Force the SDL define before anything else
#ifndef LV_USE_SDL
#define LV_USE_SDL 1
#endif

#include "lvgl/lvgl.h"
#include "lvgl/src/drivers/sdl/lv_sdl_window.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mouse.h"
#include <iostream>
#include <windows.h>

// 2. Import our new UI page
#include "../ui/page.h"

int main(int argc, char* argv[]) {
    // Initialize Core and Drivers
    lv_init();
    lv_display_t * disp = lv_sdl_window_create(800, 480);
    lv_indev_t * mouse = lv_sdl_mouse_create();

    // --- LOAD THE UI ---
    // This calls the function we built in t2.cpp
    create_page_t2();

    std::cout << "Success! Window open and UI loaded from t2.cpp." << std::endl;

    // The Main Loop
    while(1) {
        lv_timer_handler();
        Sleep(5);
    }
    
    return 0;
}
//g++ main.cpp api/*.cpp ui/t2/*.cpp -I. -I./lvgl -I./lvgl/src -I./lvgl/src/drivers/sdl -DLV_CONF_INCLUDE_SIMPLE -DLV_USE_SDL=1 -L. -o app.exe "-Wl,--start-group" liblvgl.a -lcurl -lSDL2 "-Wl,--end-group"