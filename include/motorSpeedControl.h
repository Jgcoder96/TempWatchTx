#ifndef MOTOR_SPEED_CONTROL_H
#define MOTOR_SPEED_CONTROL_H

#include "systemConfig.h"

extern volatile SpeedLevel g_velocidad_motor_1;
extern volatile SpeedLevel g_velocidad_motor_2;
extern volatile SpeedLevel g_velocidad_motor_3;
extern volatile SpeedLevel g_velocidad_motor_4;

void motores_init(void);

void motores_start_control_task(void);

#endif