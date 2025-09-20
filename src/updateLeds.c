#include <stdio.h>
#include <string.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "rom/ets_sys.h"

#define DS_PIN      GPIO_NUM_19
#define SHCP_PIN    GPIO_NUM_18
#define STCP_PIN    GPIO_NUM_5

static const char *TAG = "DUAL_STRING_CONTROL";

void shift_register_init();
void shift_out_16bits(uint8_t data_chip2, uint8_t data_chip1);
void shift_out_string_16(const char* bit_string);


void shift_register_init() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << DS_PIN) | (1ULL << SHCP_PIN) | (1ULL << STCP_PIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);
    gpio_set_level(DS_PIN, 0);
    gpio_set_level(SHCP_PIN, 0);
    gpio_set_level(STCP_PIN, 0);
}

void shift_out_16bits(uint8_t data_chip2, uint8_t data_chip1) {
    gpio_set_level(STCP_PIN, 0);
    for (int i = 7; i >= 0; i--) {
        gpio_set_level(DS_PIN, (data_chip2 >> i) & 1);
        gpio_set_level(SHCP_PIN, 1); ets_delay_us(5); gpio_set_level(SHCP_PIN, 0);
    }
    for (int i = 7; i >= 0; i--) {
        gpio_set_level(DS_PIN, (data_chip1 >> i) & 1);
        gpio_set_level(SHCP_PIN, 1); ets_delay_us(5); gpio_set_level(SHCP_PIN, 0);
    }
    gpio_set_level(STCP_PIN, 1); ets_delay_us(5); gpio_set_level(STCP_PIN, 0);
}

void shift_out_string_16(const char* bit_string) {
    if (bit_string == NULL || strlen(bit_string) != 16) {
        ESP_LOGE(TAG, "Error: El string debe tener 16 caracteres.");
        return;
    }

    uint8_t data_chip1 = 0; 
    uint8_t data_chip2 = 0; 

    for (int i = 0; i < 8; i++) {
        if (bit_string[i] == '1') {
            data_chip2 |= (1 << i);
        }
    }

    for (int i = 0; i < 8; i++) {
        if (bit_string[i + 8] == '1') {
            data_chip1 |= (1 << i);
        }
    }
    
    ESP_LOGI(TAG, "String '%s' convertido a:", bit_string);
    ESP_LOGI(TAG, "  - Chip 2 (U4): 0x%02X", data_chip2);
    ESP_LOGI(TAG, "  - Chip 1 (U3): 0x%02X", data_chip1);
    
    shift_out_16bits(data_chip2, data_chip1);
}