#include "nvs_flash.h"
#include "wifiManager.h"
#include "library.h"
#include "temperatureSensor.h"
#include "analogDigitalConverter.h"
#include "initializeSystem.h"
#include "systemConfig.h"

QueueHandle_t sensor_queue = NULL;

void app_main(void) {
    
  esp_err_t ret = nvs_flash_init();

  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  myWifiStartConnection();

  adcReaderInit();

  initializeSystem();

  sensor_queue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);

  xTaskCreatePinnedToCore(senseTemperature, "sensorTemp", 8192, NULL, 12, NULL, 1);
  xTaskCreatePinnedToCore(systemControl, "controlTemp", 8192, NULL, 12, NULL, 1);
}