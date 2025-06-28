#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

typedef struct {
  double voltage;
} SensorDataStruct;

typedef struct {
  SensorDataStruct sensor1;
  SensorDataStruct sensor2;
  SensorDataStruct sensor3;
  SensorDataStruct sensor4;
} SensorDataCollectionStruct;

#define QUEUE_LENGTH 100
#define ITEM_SIZE sizeof(SensorDataCollectionStruct)

#endif 