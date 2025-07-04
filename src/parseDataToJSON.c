#include "library.h"
#include "systemConfig.h"
#include "parseDataToJSON.h"

cJSON* systemToJson(const SystemStruct* system) {
  cJSON* json_system = cJSON_CreateObject();
  cJSON_AddStringToObject(json_system, "id_sensor", system->id);
  cJSON_AddNumberToObject(json_system, "voltage", system->voltage);
  cJSON_AddNumberToObject(json_system, "temperature", system->temperature);
  return json_system;
}

char* parseDataToJSON(const SystemCollectionStruct* systems) {
  cJSON* root = cJSON_CreateObject();
  cJSON* data_array = cJSON_CreateArray();
  
  cJSON_AddItemToArray(data_array, systemToJson(&systems->system1));
  cJSON_AddItemToArray(data_array, systemToJson(&systems->system2));
  cJSON_AddItemToArray(data_array, systemToJson(&systems->system3));
  cJSON_AddItemToArray(data_array, systemToJson(&systems->system4));
  
  cJSON_AddItemToObject(root, "data", data_array);
  
  char* json_string = cJSON_Print(root);
  cJSON_Delete(root);
  return json_string;
}