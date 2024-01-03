#include <stdio.h>
#include <esp_heap_caps.h>
#include <esp_lcd_types.h>
#include <esp_lcd_panel_commands.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_rgb.h>
#include <esp_lcd_panel_interface.h>
#include <driver/i2c.h>
#include <init_vars.h>
#include <esp_log.h>

#define IO_ADDR 0x3F
#define GPIO_ADDR 0x01
#define DELAY 0x80
#define PICO_MASK 0x80
#define CLK_MASK 0x01
#define CS_MASK 0x02

static uint8_t gpio_status = 0;
static const char *I2C_ERROR = "I2C";

// Stolen functions from CircuitPython library mentioned later
// (ioexpander_bus_send and pin_change. Although it could be said that all of
// the initialization for the LCD is stolen from them)
esp_err_t pin_change(uint8_t set_pins, uint8_t clear_pins) {
    uint8_t data[] = {GPIO_ADDR, (gpio_status & ~clear_pins) | set_pins};

    esp_err_t err =
        i2c_master_write_to_device(I2C_NUM_0, IO_ADDR, (uint8_t *)&data, 2, 50);
    if (err == ESP_OK)
        gpio_status = data[1];
    else
        ESP_LOGW(I2C_ERROR, "Could not send pin change data properly");
    return err;
}

esp_err_t ioexpander_bus_send(int is_command, uint8_t *data, int data_len) {
    int dc_mask = is_command ? 0 : 0x100;
    esp_err_t ret = 0;
    for (int i = 0; i < data_len; i++) {
        uint16_t bits = dc_mask | data[i];

        for (int j = 0; j < 9; j++) {
            if (bits & 0x100) {
                ret |= pin_change(PICO_MASK, CLK_MASK | CS_MASK);
            } else {
                ret |= pin_change(0, PICO_MASK | CLK_MASK | CS_MASK);
            }

            pin_change(CLK_MASK, 0);
            bits <<= 1;
        }
    }

    return ret;
}

void app_main(void) {
    // I2C initialization
    i2c_config_t i2c_config = {.mode = I2C_MODE_MASTER,
                               .scl_io_num = 18,
                               .sda_io_num = 8,
                               .scl_pullup_en = GPIO_PULLUP_ENABLE,
                               .sda_pullup_en = GPIO_PULLUP_ENABLE,
                               .master.clk_speed = 400000,
                               .clk_flags = 0};
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, ESP_INTR_FLAG_LEVEL3);
    i2c_param_config(I2C_NUM_0, &i2c_config);

    // IO Expander initialization
    i2c_cmd_handle_t i2c_handle = i2c_cmd_link_create();
    if (i2c_handle)
        // This process is stolen straight from CircuitPython
        // https://github.com/adafruit/circuitpython/blob/7715ff09cc663b80403f8907fe90c00fd89c9706/shared-module/dotclockframebuffer/__init__.c#L17

        for (int i = 0; i < sizeof(i2c_init);) {
            char write_len = i2c_init[i];
            printf("Writing bytes: ");
            for (int j = 1; j < write_len + 1; j++) {
                printf("%02X ", i2c_init[i + j]);
            }
            printf("\n");
            esp_err_t err = i2c_master_write_to_device(
                I2C_NUM_0, IO_ADDR, (uint8_t *)&i2c_init[i + 1], write_len, 50);
            if (err == ESP_OK)
                i += write_len + 1;
        }

    // LCD initialization using SPI over the IO expander
    /* Who decided that the order of the initialization bytes was going to
     * differ????? Why do the I2C initialization codes have the data length,
     * then the command, then the data, but the LCD initialization has the
     * command, then the data length, and then the data??? PICK ONE
     */
    for (int i = 0; i < sizeof(lcd_init);) {
        uint8_t write_len = lcd_init[i + 1];
        uint8_t cmd = lcd_init[i];
        uint8_t *data = (uint8_t *)&lcd_init[i + 2];
        uint8_t delay = (write_len & DELAY) != 0;
        write_len &= ~DELAY;
        
        ioexpander_bus_send(true, &cmd, 1);
        ioexpander_bus_send(false, data, write_len);
        
        pin_change(0, CLK_MASK);  // Idle clock
        pin_change(CS_MASK, 0);   // Disconnect chip select
    
        if (delay) {
          write_len++;
          uint16_t delay_len = lcd_init[i + 1 + write_len];
          // Judging by my initialization this is never the case but better safe
          // than sorry or whatever
          if (delay_len == 255) {
            delay_len = 500;
          }

        }
        i += write_len + 2;
    }

    i2c_cmd_link_delete(i2c_handle);

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
    // colors = heap_caps_malloc(480 * 480 * sizeof(uint16_t),
    // MALLOC_CAP_SPIRAM);
    esp_lcd_rgb_panel_get_frame_buffer(handle, 1, &colors);
    for (int i = 0; i < 480 * 480; i++) {
        ((uint16_t *)colors)[i] = 0xffff;
    }
    esp_lcd_panel_draw_bitmap(handle, 0, 0, 480, 480, colors);
}
