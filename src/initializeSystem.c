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
  snprintf(systemCollection.system1.id, sizeof(systemCollection.system1.id), "03136b1f-65e1-4d92-ae19-8ba71876cdb5");
  snprintf(systemCollection.system1.name, sizeof(systemCollection.system1.name), "system 1");

  systemCollection.system2 = defaultSystem;
  snprintf(systemCollection.system2.id, sizeof(systemCollection.system2.id), "f418cf2e-22fb-40e3-9f31-2578d79db87f");
  snprintf(systemCollection.system2.name, sizeof(systemCollection.system2.name), "system 2");
    

  systemCollection.system3 = defaultSystem;
  snprintf(systemCollection.system3.id, sizeof(systemCollection.system3.id), "90f7ec5d-56c9-423d-92ea-9721b685f9f9");
  snprintf(systemCollection.system3.name, sizeof(systemCollection.system3.name), "system 3");

  systemCollection.system4 = defaultSystem;
  snprintf(systemCollection.system4.id, sizeof(systemCollection.system4.id), "74ede588-0fdc-4a85-8232-6455e163dbfb");
  snprintf(systemCollection.system4.name, sizeof(systemCollection.system4.name), "system 4");

  snprintf(systemCollection.device, sizeof(systemCollection.device), "dispositivo");

  systemCollection.hysteresis = (HysteresisConfig){
    .preventive_threshold = 85.0,
    .emergency_threshold = 110.0,
    .hysteresis_range = 5.0
  };
}