#include "processData.h"
#include "systemConfig.h"

double voltageToTemperature(double voltage) {
  double temp_min = 30.0;   
  double temp_max = 140.0;  
  
  if (voltage < 0.0) voltage = 0.0;

  return temp_min + (voltage / 3.0) * (temp_max - temp_min);
}

static void updateSingleSystemState(SystemStruct *system, const HysteresisConfig *hyst) {
  system->previous_state = system->current_state;

  switch (system->current_state) {
    case STATE_NORMAL:
      if (system->temperature > hyst->preventive_threshold + hyst->hysteresis_range) {
        system->current_state = STATE_PREVENTIVE;
        system->speed_level = NORMAL_SPEED;
      }
      break;

    case STATE_PREVENTIVE:
      if (system->temperature > hyst->emergency_threshold + hyst->hysteresis_range) {
        system->current_state = STATE_EMERGENCY;
        system->speed_level = HIGH_SPEED;
      } else if (system->temperature < hyst->preventive_threshold - hyst->hysteresis_range) {
        system->current_state = STATE_NORMAL;
        system->speed_level = OFF;
      }
      break;

    case STATE_EMERGENCY:
      if (system->temperature < hyst->emergency_threshold - hyst->hysteresis_range) {
        system->current_state = STATE_PREVENTIVE;
        system->speed_level = NORMAL_SPEED;
      }
      break;
  }
}

void updateSystemStatesWithHysteresis(SystemCollectionStruct *systems) {
  updateSingleSystemState(&systems->system1, &systems->hysteresis);
  updateSingleSystemState(&systems->system2, &systems->hysteresis);
  updateSingleSystemState(&systems->system3, &systems->hysteresis);
  updateSingleSystemState(&systems->system4, &systems->hysteresis);
}
