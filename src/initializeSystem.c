#include "systemConfig.h"
#include "library.h"

SystemCollectionStruct systemCollection;

void initializeSystem() {
  const SystemStruct defaultSystem = {
    .id = "",               
    .name = "",              
    .voltage = 0.0,
    .temperature = 30.0,
    .previous_state = STATE_NORMAL,
    .current_state = STATE_NORMAL,
    .speed_level = OFF
  };

  systemCollection.system1 = defaultSystem;
  snprintf(systemCollection.system1.id, sizeof(systemCollection.system1.id), "f5deb1d4-3eb3-41b7-a4bb-cba02954ae8d");
  snprintf(systemCollection.system1.name, sizeof(systemCollection.system1.name), "system 1");

  systemCollection.system2 = defaultSystem;
  snprintf(systemCollection.system2.id, sizeof(systemCollection.system2.id), "593740d7-5284-4226-abe9-b19c42dce662");
  snprintf(systemCollection.system2.name, sizeof(systemCollection.system2.name), "system 2");
    

  systemCollection.system3 = defaultSystem;
  snprintf(systemCollection.system3.id, sizeof(systemCollection.system3.id), "fdc21238-2c88-4df6-9759-c89da74aef3e");
  snprintf(systemCollection.system3.name, sizeof(systemCollection.system3.name), "system 3");

  systemCollection.system4 = defaultSystem;
  snprintf(systemCollection.system4.id, sizeof(systemCollection.system4.id), "e5ec1e01-8fb4-47f3-9510-af2b2be20c86");
  snprintf(systemCollection.system4.name, sizeof(systemCollection.system4.name), "system 4");

  snprintf(systemCollection.device, sizeof(systemCollection.device), "dispositivo");

  systemCollection.hysteresis = (HysteresisConfig){
    .preventive_threshold = 85.0,
    .emergency_threshold = 110.0,
    .hysteresis_range = 5.0
  };
}