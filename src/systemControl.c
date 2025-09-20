#include "systemConfig.h"      
#include "motorSpeedControl.h" 

void systemControlMotorAndLeds(const SystemCollectionStruct* systems) {
    const SystemStruct* systemArray[] = {
        &systems->system1,
        &systems->system2,
        &systems->system3,
        &systems->system4
    };

    volatile SpeedLevel* p_velocidades_globales[] = {
        &g_velocidad_motor_1,
        &g_velocidad_motor_2,
        &g_velocidad_motor_3,
        &g_velocidad_motor_4
    };

    for (int i = 0; i < 4; i++) {
        SpeedLevel targetSpeed = systemArray[i]->speed_level;
        *(p_velocidades_globales[i]) = targetSpeed;
    }
}