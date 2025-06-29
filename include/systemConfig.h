#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

typedef enum {
  STATE_NORMAL,     
  STATE_PREVENTIVE,  
  STATE_EMERGENCY    
} SystemState;

typedef enum {
  OFF,
  NORMAL_SPEED,
  HIGH_SPEED
  
} SpeedLevel;

typedef struct {
  float preventive_threshold;  
  float emergency_threshold;   
  float hysteresis_range;     
} HysteresisConfig;

typedef struct {
  char id[40];
  char name[40];
  double voltage;
  double temperature;
  SystemState previous_state; 
  SystemState current_state; 
  SpeedLevel speed_level;
} SystemStruct;

typedef struct {
  char device[40];
  SystemStruct system1;
  SystemStruct system2;
  SystemStruct system3;
  SystemStruct system4;
  HysteresisConfig hysteresis;
} SystemCollectionStruct;

#define QUEUE_LENGTH 100
#define ITEM_SIZE sizeof(SystemCollectionStruct)

#endif 