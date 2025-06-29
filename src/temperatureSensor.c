#include "analogDigitalConverter.h"
#include "library.h"
#include "systemConfig.h"
#include "temperatureSensor.h"
#include "processData.h"

extern QueueHandle_t sensor_queue;

extern SystemCollectionStruct systemCollection;

extern volatile bool wifiConnected;

void senseTemperature(void *pvParam) {
  
  SensorDataCollectionStruct sensorData;
  const TickType_t samplingPeriod = pdMS_TO_TICKS(200);
  
  while (1) {

    vTaskDelay(pdMS_TO_TICKS(samplingPeriod));

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
    systemCollection.system4.temperature = voltageToTemperature(sensorData.sensor4.voltage );

    updateSystemStatesWithHysteresis(&systemCollection);

    if (xQueueSend(sensor_queue, &systemCollection, 0) != pdPASS) {
      printf("[ERROR] Cola llena. Datos descartados.\n");
      vTaskDelay(pdMS_TO_TICKS(50)); 
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

