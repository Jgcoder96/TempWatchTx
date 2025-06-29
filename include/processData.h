#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include "systemConfig.h"

double roundNumber(double value);
double voltageToTemperature(double voltage);
void updateSingleSystemState(SystemStruct *system, const HysteresisConfig *hyst);
void updateSystemStatesWithHysteresis(SystemCollectionStruct *systems);

#endif 