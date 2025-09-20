#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "motorSpeedControl.h" 

#define LEDC_TIMER           LEDC_TIMER_0
#define LEDC_MODE            LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RESOLUTION LEDC_TIMER_10_BIT 
#define LEDC_FREQUENCY       (5000)           


#define DUTY_MOTOR_PARADO    0                 
#define DUTY_MOTOR_VEL_NORMAL 512             
#define DUTY_MOTOR_VEL_ALTA  921              

static const char *TAG = "MOTOR_CONTROL";

typedef struct {
  int gpio_num;
  ledc_channel_t channel;
  volatile SpeedLevel* p_velocidad;
} motor_config_t;

volatile SpeedLevel g_velocidad_motor_1 = OFF;
volatile SpeedLevel g_velocidad_motor_2 = OFF;
volatile SpeedLevel g_velocidad_motor_3 = OFF;
volatile SpeedLevel g_velocidad_motor_4 = OFF;

static motor_config_t motores[] = {
  { .gpio_num = 25, .channel = LEDC_CHANNEL_0, .p_velocidad = &g_velocidad_motor_1 }, 
  { .gpio_num = 27, .channel = LEDC_CHANNEL_1, .p_velocidad = &g_velocidad_motor_2 }, 
  { .gpio_num = 14, .channel = LEDC_CHANNEL_2, .p_velocidad = &g_velocidad_motor_3 }, 
  { .gpio_num = 13, .channel = LEDC_CHANNEL_3, .p_velocidad = &g_velocidad_motor_4 }  
};
static const int NUM_MOTORES = sizeof(motores) / sizeof(motores[0]);

static void establecer_duty_motor(ledc_channel_t channel, SpeedLevel velocidad) {
  uint32_t duty_cycle = 0;
  switch (velocidad) {
    case OFF:          duty_cycle = DUTY_MOTOR_PARADO;      break;
    case NORMAL_SPEED: duty_cycle = DUTY_MOTOR_VEL_NORMAL;  break;
    case HIGH_SPEED:   duty_cycle = DUTY_MOTOR_VEL_ALTA;    break;
  }
  ledc_set_duty(LEDC_MODE, channel, duty_cycle);
  ledc_update_duty(LEDC_MODE, channel);
}

static void motor_control_task(void *pvParameters) {
  ESP_LOGI(TAG, "Tarea de control de motores iniciada. Actualizando cada 100ms.");
  while (1) {
    for (int i = 0; i < NUM_MOTORES; i++) {
      establecer_duty_motor(motores[i].channel, *motores[i].p_velocidad);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void motores_init(void) {
  ledc_timer_config_t ledc_timer = {
    .speed_mode = LEDC_MODE,
    .timer_num = LEDC_TIMER,
    .duty_resolution = LEDC_DUTY_RESOLUTION,
    .freq_hz = LEDC_FREQUENCY,
    .clk_cfg = LEDC_AUTO_CLK
  };
    
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  for (int i = 0; i < NUM_MOTORES; i++) {
    ledc_channel_config_t ledc_channel = {
      .speed_mode = LEDC_MODE,
      .channel    = motores[i].channel,
      .timer_sel  = LEDC_TIMER,
      .intr_type  = LEDC_INTR_DISABLE,
      .gpio_num   = motores[i].gpio_num,
      .duty       = 0, 
      .hpoint     = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ESP_LOGI(TAG, "Configurado Motor %d (GPIO %d, Canal %d)", i + 1, motores[i].gpio_num, motores[i].channel);
  }
}

void motores_start_control_task(void) {
  xTaskCreate(
    motor_control_task,
    "motor_control_task",
    4096, 
    NULL, 
    5,    
    NULL 
  );
}