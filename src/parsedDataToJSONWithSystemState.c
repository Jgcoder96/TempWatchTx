#include "library.h"
#include "systemConfig.h"

static const char* systemStateToString(SystemState state) {
  switch(state) {
    case STATE_NORMAL:      return "normal";
    case STATE_PREVENTIVE:  return "preventive";
    case STATE_EMERGENCY:   return "emergency";
    default:                return "unknown";
  }
}

static const char* speedLevelToString(SpeedLevel speed) {
  switch(speed) {
    case OFF:           return "off";
    case NORMAL_SPEED:  return "normal";
    case HIGH_SPEED:    return "high";
    default:            return "unknown";
  }
}

static cJSON* systemToJsonWithStatus(const SystemStruct* system) {
  cJSON* json_system = cJSON_CreateObject();
  cJSON_AddStringToObject(json_system, "id_sensor", system->name);
  cJSON_AddNumberToObject(json_system, "voltage", system->voltage);
  cJSON_AddNumberToObject(json_system, "temperature", system->temperature);
  cJSON_AddStringToObject(json_system, "status", systemStateToString(system->current_state));
  cJSON_AddStringToObject(json_system, "speed_motor", speedLevelToString(system->speed_level));
  return json_system;
}

char* parseDataToJSONWithSystemState(const SystemCollectionStruct* systems) {
  cJSON* root = cJSON_CreateObject();
  cJSON* data_array = cJSON_CreateArray();
  
  const SystemStruct* systemArray[] = {
    &systems->system1,
    &systems->system2,
    &systems->system3,
    &systems->system4
  };
    
  for (int i = 0; i < 4; i++) {
    if (systemArray[i]->previous_state != systemArray[i]->current_state) { 
      cJSON_AddItemToArray(data_array, systemToJsonWithStatus(systemArray[i]));
    }
  }
    
  if (cJSON_GetArraySize(data_array) == 0) {
    cJSON_Delete(root);
    return NULL;
  }
    
  cJSON_AddItemToObject(root, "data", data_array);
    
  char* json_string = cJSON_Print(root);
  cJSON_Delete(root);
  return json_string;
}