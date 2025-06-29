#include "systemConfig.h"      // Contiene la definición de SystemCollectionStruct
#include "motorSpeedControl.h" // Necesario para acceder a g_velocidad_motor_X

/**
 * @brief Sincroniza el estado de los sistemas con los actuadores (motores y LEDs).
 *
 * Esta función es el "puente" entre la lógica de estado del sistema y el módulo de control del motor.
 * Lee el campo 'speed_level' de cada sistema (que se asume ya ha sido calculado por otra lógica)
 * y lo asigna a la variable global correspondiente para que la tarea del motor actúe.
 *
 * Se debe llamar a esta función periódicamente desde el bucle principal o una tarea de lógica.
 *
 * @param systems Puntero a la colección de todos los sistemas.
 */
void systemControlMotorAndLeds(const SystemCollectionStruct* systems) {
    // Array de punteros a los 4 sistemas para iterar fácilmente sobre ellos.
    const SystemStruct* systemArray[] = {
        &systems->system1,
        &systems->system2,
        &systems->system3,
        &systems->system4
    };

    // Array de punteros a las variables globales de velocidad.
    // Este mapeo permite actualizar la variable correcta dentro de un bucle limpio.
    volatile SpeedLevel* p_velocidades_globales[] = {
        &g_velocidad_motor_1,
        &g_velocidad_motor_2,
        &g_velocidad_motor_3,
        &g_velocidad_motor_4
    };

    // Recorremos cada uno de los 4 sistemas
    for (int i = 0; i < 4; i++) {
        // Leemos la velocidad deseada directamente del struct del sistema.
        // La lógica que calcula este valor está en otra parte del código.
        SpeedLevel targetSpeed = systemArray[i]->speed_level;
        
        // Asignamos esa velocidad a la variable de control del motor.
        // La tarea 'motor_control_task' detectará este cambio y ajustará el motor.
        *(p_velocidades_globales[i]) = targetSpeed;
    }

    // Lógica adicional para los LEDs (opcional).
    // Aquí puedes usar la condición de cambio de estado si, por ejemplo,
    // quieres que un LED parpadee solo en la transición de un estado a otro.
    /*
    for (int i = 0; i < 4; i++) {
        if (systemArray[i]->previous_state != systemArray[i]->current_state) {
            // Lógica para el LED del sistema 'i': encender, apagar, parpadear...
        }
    }
    */
}