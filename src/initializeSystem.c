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
  snprintf(systemCollection.system1.id, sizeof(systemCollection.system1.id), "36af59dc-fb08-471b-9246-0cda2b3147b2");
  snprintf(systemCollection.system1.name, sizeof(systemCollection.system1.name), "system 1");

  systemCollection.system2 = defaultSystem;
  snprintf(systemCollection.system2.id, sizeof(systemCollection.system2.id), "6203df17-11fc-4530-9468-18bdd9d18f1b");
  snprintf(systemCollection.system2.name, sizeof(systemCollection.system2.name), "system 2");
    

  systemCollection.system3 = defaultSystem;
  snprintf(systemCollection.system3.id, sizeof(systemCollection.system3.id), "7e4043ed-c71a-42b2-92e3-0fa6aeeb1fdc");
  snprintf(systemCollection.system3.name, sizeof(systemCollection.system3.name), "system 3");

  systemCollection.system4 = defaultSystem;
  snprintf(systemCollection.system4.id, sizeof(systemCollection.system4.id), "e6eefc31-2b25-4aa6-9008-34592a173672");
  snprintf(systemCollection.system4.name, sizeof(systemCollection.system4.name), "system 4");

  snprintf(systemCollection.device, sizeof(systemCollection.device), "dispositivo");

  systemCollection.hysteresis = (HysteresisConfig){
    .preventive_threshold = 85.0,
    .emergency_threshold = 110.0,
    .hysteresis_range = 5.0
  };
}