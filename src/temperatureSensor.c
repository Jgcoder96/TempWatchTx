#include "analogDigitalConverter.h"
#include "httpClient.h"
#include "library.h"
#include "parseDataToJSON.h"
#include "parsedDataToJSONWithSystemState.h"
#include "processData.h"
#include "systemConfig.h"
#include "temperatureSensor.h"
#include "systemControl.h"

extern QueueHandle_t sensor_queue;
extern QueueHandle_t sensor_queue_30s;

extern SystemCollectionStruct systemCollection;

extern volatile bool wifiConnected;

void senseTemperature(void *pvParam) {
  SensorDataCollectionStruct sensorData;
  const TickType_t fastSamplingPeriod = pdMS_TO_TICKS(200);  
  const TickType_t slowSamplingPeriod = pdMS_TO_TICKS(30000); 
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
          printf("[ERROR] Cola rápida llena. Datos descartados.\n");
          vTaskDelay(pdMS_TO_TICKS(50)); 
      }

      TickType_t currentTime = xTaskGetTickCount();
      if ((currentTime - lastSlowSendTime) >= slowSamplingPeriod) {
          if (xQueueSend(sensor_queue_30s, &systemCollection, 0) != pdPASS) {
              printf("[ERROR] Cola lenta (30s) llena. Datos descartados.\n");
          }
          lastSlowSendTime = currentTime;
      }
  }
}

void systemControl(void *pvParam) {

  SystemCollectionStruct receivedData;
  
    while (1) {
      if (xQueueReceive(sensor_queue, &receivedData, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) {
        char* dataInJSONWithSystemState = parseDataToJSONWithSystemState(&receivedData);
        if (wifiConnected && dataInJSONWithSystemState != NULL) {
          printf("Enviando datos al servidor: %s\n", dataInJSONWithSystemState);
          esp_err_t err = sendJsonPost("http://192.168.18.221:3000/api/data", dataInJSONWithSystemState);
          if (err != ESP_OK) {
            ESP_LOGW("HTTP", "No se pudo enviar datos al servidor");
          }
        }
        systemControlMotorAndLeds (&receivedData);
        
      }  
    }
}

void sendDataToApi(void *pvParam) {

  SystemCollectionStruct receivedData;
  
    while (1) {
      if (xQueueReceive(sensor_queue_30s, &receivedData, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) {
        char* dataInJSON = parseDataToJSON(&receivedData);
        if (wifiConnected && dataInJSON != NULL) {
          printf("Enviando datos al servidor: %s\n", dataInJSON);
          esp_err_t err = sendJsonPost("http://192.168.18.221:3000/api/data", dataInJSON);
          if (err != ESP_OK) {
            ESP_LOGW("HTTP", "No se pudo enviar datos al servidor");
          }
        }
        
      }
    }
}




