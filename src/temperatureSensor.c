#include "analogDigitalConverter.h"
#include "library.h"
#include "systemConfig.h"
#include "temperatureSensor.h"
#include "processData.h"

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

      systemCollection.system1.voltage = sensorData.sensor1.voltage;
      systemCollection.system2.voltage = sensorData.sensor2.voltage;
      systemCollection.system3.voltage = sensorData.sensor3.voltage;
      systemCollection.system4.voltage = sensorData.sensor4.voltage;

      systemCollection.system1.temperature = voltageToTemperature(sensorData.sensor1.voltage);
      systemCollection.system2.temperature = voltageToTemperature(sensorData.sensor2.voltage);
      systemCollection.system3.temperature = voltageToTemperature(sensorData.sensor3.voltage);
      systemCollection.system4.temperature = voltageToTemperature(sensorData.sensor4.voltage);

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
        printf("Sensor 1: %.2f V\n", receivedData.system1.temperature);
        printf("Estado Anterior: %d\n", receivedData.system1.previous_state);
        printf("Estado Actual: %d\n", receivedData.system1.current_state);
        printf("Estado Motor: %d\n", receivedData.system1.speed_level);
        
      }  
    }
}

void sendDataToApi(void *pvParam) {

  SystemCollectionStruct receivedData;
  
    while (1) {
      if (xQueueReceive(sensor_queue_30s, &receivedData, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) {
        printf("holaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");   
      }
    }
}




