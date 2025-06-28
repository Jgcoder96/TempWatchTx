#include "analogDigitalConverter.h"
#include "library.h"
#include "temperatureSensor.h"

extern QueueHandle_t sensor_queue;

extern volatile bool wifiConnected;

void senseTemperature(void *pvParam) {
  
  SensorDataCollectionStruct sensorData;
  
  while (1) {

    adcReadingStruct adc_results[NUM_ADC_CHANNELS];
    
    readAllAdcChannels(adc_results);
    
    sensorData.sensor1 = (SensorDataStruct){adc_results[2].voltage};
    sensorData.sensor2 = (SensorDataStruct){adc_results[3].voltage};
    sensorData.sensor3 = (SensorDataStruct){adc_results[0].voltage};
    sensorData.sensor4 = (SensorDataStruct){adc_results[1].voltage};

    xQueueSend(sensor_queue, &sensorData, pdMS_TO_TICKS(1000));
    
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void systemControl(void *pvParam) {

  SensorDataCollectionStruct receivedData;
  
    while (1) {
      if (xQueueReceive(sensor_queue, &receivedData, pdMS_TO_TICKS(portMAX_DELAY)) == pdTRUE) {
        printf("Sensor 1: %.2f V\n", receivedData.sensor1.voltage);
        printf("Sensor 2: %.2f V\n", receivedData.sensor2.voltage);
        printf("Sensor 3: %.2f V\n", receivedData.sensor3.voltage);
        printf("Sensor 4: %.2f V\n", receivedData.sensor4.voltage);
      }  
    }
}

