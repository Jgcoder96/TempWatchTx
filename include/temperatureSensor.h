#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include "sensorData.h" 

void senseTemperature(void *pvParam);
void systemControl(void *pvParam);
void sendDataToApi(void *pvParam);

#endif 