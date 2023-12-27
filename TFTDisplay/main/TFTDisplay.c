#include <stdio.h>
// #include <soc/soc_caps.h>
#include <esp_heap_caps.h>
#include <esp_lcd_types.h>
#include <esp_lcd_panel_commands.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>
#include <esp_lcd_panel_interface.h>

void app_main(void) {
    esp_lcd_panel_handle_t handle = NULL;
    esp_lcd_rgb_panel_config_t config = {
        .data_width = 16,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .de_gpio_num = 2,
        .hsync_gpio_num = 41,
        .vsync_gpio_num = 42,
        .pclk_gpio_num = 1,
        .disp_gpio_num = -1,
        .data_gpio_nums = {11, 10, 9, 46, 3, 48, 47, 21, 14, 13, 12, 40, 39, 38,
                           0, 45},
        .bits_per_pixel = 0,
        .num_fbs = 1,
        .timings = {.pclk_hz = 16000000,
                    .h_res = 480,
                    .v_res = 480,
                    .hsync_pulse_width = 20,
                    .vsync_pulse_width = 40,
                    .hsync_front_porch = 40,
                    .hsync_back_porch = 40,
                    .vsync_front_porch = 40,
                    .vsync_back_porch = 40,
                    .flags =
                        {
                            .hsync_idle_low = 0,
                            .vsync_idle_low = 0,
                            .de_idle_high = 0,
                            .pclk_idle_high = 0,
                            .pclk_active_neg = 0,

                        }},
        .flags = {
            .fb_in_psram = true,
        }};

    esp_lcd_new_rgb_panel(&config, &handle);
    ESP_ERROR_CHECK(esp_lcd_panel_reset(handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(handle));
    void *colors = NULL;
    // colors = heap_caps_malloc(480 * 480 * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
    esp_lcd_rgb_panel_get_frame_buffer(handle, 1, &colors);
    for (int i = 0; i < 480 * 480; i++) {
        ((uint16_t *)colors)[i] = 0xffff;
    }
    esp_lcd_panel_draw_bitmap(handle, 0, 0, 480, 480, colors);
}
