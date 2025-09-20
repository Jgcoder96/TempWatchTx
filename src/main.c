#include "nvs_flash.h"
#include "wifiManager.h"
#include "library.h"
#include "temperatureSensor.h"
#include "analogDigitalConverter.h"
#include "initializeSystem.h"
#include "systemConfig.h"
#include "motorSpeedControl.h"
#include "updateLeds.h"

QueueHandle_t sensor_queue = NULL;
QueueHandle_t sensor_queue_30s = NULL;

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

  motores_init();

  motores_start_control_task();

  shift_register_init();

  shift_out_string_16("0100100001001000");

  sensor_queue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);
  sensor_queue_30s = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);

  xTaskCreatePinnedToCore(senseTemperature, "senseTemperature", 8192, NULL, 12, NULL, 1);
  xTaskCreatePinnedToCore(systemControl, "systemControl", 8192, NULL, 12, NULL, 1);
  xTaskCreatePinnedToCore(sendDataToApi, "sendDataToApi", 8192, NULL, 12, NULL, 1);
}