#include "analogDigitalConverter.h"
#include "httpClient.h"
#include "library.h"
#include "parseDataToJSON.h"
#include "parsedDataToJSONWithSystemState.h"
#include "processData.h"
#include "systemConfig.h"
#include "temperatureSensor.h"
#include "systemControl.h"
#include "updateLeds.h"

static const char *TAG = "MAIN_TASKS";

extern QueueHandle_t sensor_queue;
extern QueueHandle_t sensor_queue_30s;

extern SystemCollectionStruct systemCollection;

extern volatile bool wifiConnected;

void senseTemperature(void *pvParam) {
  SensorDataCollectionStruct sensorData;
  const TickType_t fastSamplingPeriod = pdMS_TO_TICKS(1000);
  const TickType_t slowSamplingPeriod = pdMS_TO_TICKS(60000);
  TickType_t lastSlowSendTime = xTaskGetTickCount();

  while (1) {
      vTaskDelay(fastSamplingPeriod);  

      adcReadingStruct adc_results[NUM_ADC_CHANNELS];
      readAllAdcChannels(adc_results);
      
      sensorData.sensor1 = (SensorDataStruct){adc_results[2].voltage};
      sensorData.sensor2 = (SensorDataStruct){adc_results[3].voltage};
      sensorData.sensor3 = (SensorDataStruct){adc_results[0].voltage};
      sensorData.sensor4 = (SensorDataStruct){adc_results[1].voltage};

      systemCollection.system1.voltage = roundNumber(sensorData.sensor1.voltage);
      systemCollection.system2.voltage = roundNumber(sensorData.sensor2.voltage);
      systemCollection.system3.voltage = roundNumber(sensorData.sensor3.voltage);
      systemCollection.system4.voltage = roundNumber(sensorData.sensor4.voltage);

      systemCollection.system1.temperature = roundNumber(voltageToTemperature(sensorData.sensor1.voltage));
      systemCollection.system2.temperature = roundNumber(voltageToTemperature(sensorData.sensor2.voltage));
      systemCollection.system3.temperature = roundNumber(voltageToTemperature(sensorData.sensor3.voltage));
      systemCollection.system4.temperature = roundNumber(voltageToTemperature(sensorData.sensor4.voltage));

      updateSystemStatesWithHysteresis(&systemCollection);

      if (xQueueSend(sensor_queue, &systemCollection, 0) != pdPASS) {
          ESP_LOGW(TAG, "Cola rápida llena. Datos descartados.");
          vTaskDelay(pdMS_TO_TICKS(50)); 
      }

      TickType_t currentTime = xTaskGetTickCount();
      if ((currentTime - lastSlowSendTime) >= slowSamplingPeriod) {
          if (xQueueSend(sensor_queue_30s, &systemCollection, 0) != pdPASS) {
              ESP_LOGW(TAG, "Cola lenta (30s) llena. Datos descartados.");
          }
          lastSlowSendTime = currentTime;
      }
  }
}

void systemControl(void *pvParam) {
  SystemCollectionStruct receivedData;
  static const char *TASK_TAG = "SYSTEM_CONTROL";
  
  while (1) {
    if (xQueueReceive(sensor_queue, &receivedData, portMAX_DELAY) == pdTRUE) {
      
      char* dataInJSONWithSystemState = parseDataToJSONWithSystemState(&receivedData);
      if (wifiConnected && dataInJSONWithSystemState != NULL) {
        esp_err_t err = sendJsonPost("http://192.168.18.221:3000/api/data", dataInJSONWithSystemState);
        if (err != ESP_OK) {
          ESP_LOGW(TASK_TAG, "No se pudo enviar datos al servidor (desde control rápido)");
        }
      }
      if (dataInJSONWithSystemState != NULL) {
          free(dataInJSONWithSystemState);
      }
      

      systemControlMotorAndLeds(&receivedData);

      if (receivedData.system1.current_state != receivedData.system1.previous_state ||
          receivedData.system2.current_state != receivedData.system2.previous_state ||
          receivedData.system3.current_state != receivedData.system3.previous_state ||
          receivedData.system4.current_state != receivedData.system4.previous_state) {
          
          char stringLedSystem1[5] = "0000";
          char stringLedSystem2[5] = "0000";
          char stringLedSystem3[5] = "0000";
          char stringLedSystem4[5] = "0000";
          char stringLed[17] = "";  

          if (receivedData.system1.current_state == STATE_NORMAL) strcpy(stringLedSystem1, "1000");
          else if (receivedData.system1.current_state == STATE_PREVENTIVE) strcpy(stringLedSystem1, "0100");
          else if (receivedData.system1.current_state == STATE_EMERGENCY) strcpy(stringLedSystem1, "0010");

          if (receivedData.system2.current_state == STATE_NORMAL) strcpy(stringLedSystem2, "0100");
          else if (receivedData.system2.current_state == STATE_PREVENTIVE) strcpy(stringLedSystem2, "0010");
          else if (receivedData.system2.current_state == STATE_EMERGENCY) strcpy(stringLedSystem2, "0001");

          if (receivedData.system3.current_state == STATE_NORMAL) strcpy(stringLedSystem3, "1000");
          else if (receivedData.system3.current_state == STATE_PREVENTIVE) strcpy(stringLedSystem3, "0100");
          else if (receivedData.system3.current_state == STATE_EMERGENCY) strcpy(stringLedSystem3, "0010");

          if (receivedData.system4.current_state == STATE_NORMAL) strcpy(stringLedSystem4, "0100");
          else if (receivedData.system4.current_state == STATE_PREVENTIVE) strcpy(stringLedSystem4, "0010");
          else if (receivedData.system4.current_state == STATE_EMERGENCY) strcpy(stringLedSystem4, "0001");
          
          strcat(stringLed, stringLedSystem2);
          strcat(stringLed, stringLedSystem1);
          strcat(stringLed, stringLedSystem4);
          strcat(stringLed, stringLedSystem3);
          
          ESP_LOGI(TASK_TAG, "Actualizando estado de LEDs: %s", stringLed);
          shift_out_string_16(stringLed);
      }
    }  
  }
}

void sendDataToApi(void *pvParam) {
  SystemCollectionStruct receivedData;
  static const char *TASK_TAG = "SEND_DATA_API";
  
  while (1) {
    if (xQueueReceive(sensor_queue_30s, &receivedData, portMAX_DELAY) == pdTRUE) {
      
      char* dataInJSON = parseDataToJSON(&receivedData);
      
      if (wifiConnected && dataInJSON != NULL) {
        ESP_LOGI(TASK_TAG, "Enviando datos periódicos al servidor...");
        esp_err_t err = sendJsonPost("http://192.168.18.221:3000/api/data", dataInJSON);
        if (err != ESP_OK) {
          ESP_LOGW(TASK_TAG, "No se pudo enviar datos periódicos al servidor");
        }
      }
      
      if (dataInJSON != NULL) {
          free(dataInJSON);
      }
    }
  }
}
