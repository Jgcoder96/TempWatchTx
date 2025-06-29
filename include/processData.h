#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include "systemConfig.h"

double voltageToTemperature(double voltage);
static void updateSingleSystemState(SystemStruct *system, const HysteresisConfig *hyst);
void updateSystemStatesWithHysteresis(SystemCollectionStruct *systems);

#endif 