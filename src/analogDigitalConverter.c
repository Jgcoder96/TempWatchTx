#include "library.h"
#include "analogDigitalConverter.h"

static const char *TAG = "ADC_READER";

#define NO_OF_SAMPLES       64
#define ADC_ATTEN           ADC_ATTEN_DB_12

static float adc_current_offsets[NUM_ADC_CHANNELS];

typedef struct {
  gpio_num_t gpio_num;
  adc1_channel_t adc_channel;
  const char *name;
} adc_channel_config_t;

static const adc_channel_config_t adc_channels[NUM_ADC_CHANNELS] = {
  {GPIO_NUM_32, ADC1_CHANNEL_4, "GPIO32"},
  {GPIO_NUM_33, ADC1_CHANNEL_5, "GPIO33"},
  {GPIO_NUM_34, ADC1_CHANNEL_6, "GPIO34"},
  {GPIO_NUM_35, ADC1_CHANNEL_7, "GPIO35"}
};

static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t cali_handle = NULL;
static bool adc_calibration_ok = false;

static float read_single_channel(adc1_channel_t channel, uint32_t *raw_value) {
  uint32_t adc_reading = 0;
  int raw = 0;

  for (int i = 0; i < NO_OF_SAMPLES; i++) {
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, channel, &raw));
    adc_reading += raw;
  }
  *raw_value = adc_reading / NO_OF_SAMPLES;
  
  float voltage = 0.0f;
  if (adc_calibration_ok) {
    int voltage_mv = 0;
    esp_err_t ret = adc_cali_raw_to_voltage(cali_handle, *raw_value, &voltage_mv);
    if (ret == ESP_OK) {
      voltage = (float)voltage_mv / 1000.0f;
    } else {
      ESP_LOGE(TAG, "Error en la conversión de calibración para canal %d", channel);
    }
  } else {
    ESP_LOGW(TAG, "Calibración no disponible. El voltaje será 0.");
  }
  
  uint8_t channel_idx = channel - ADC1_CHANNEL_4; // Convierte a índice 0-3
  return (voltage > adc_current_offsets[channel_idx]) 
            ? (voltage - adc_current_offsets[channel_idx]) 
            : 0.0f;
}

void adcReaderInit(void) {
  for (int i = 0; i < NUM_ADC_CHANNELS; i++) {
    adc_current_offsets[i] = ADC_DEFAULT_OFFSETS[i];
  }

  adc_oneshot_unit_init_cfg_t init_config1 = {
    .unit_id = ADC_UNIT_1,
    .ulp_mode = ADC_ULP_MODE_DISABLE,
  };
  
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

  adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_12,
    .atten = ADC_ATTEN,
  };
  for (int i = 0; i < NUM_ADC_CHANNELS; i++) {
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, adc_channels[i].adc_channel, &config));
  }

  adc_cali_line_fitting_config_t cali_config = {
    .unit_id = ADC_UNIT_1,
    .atten = ADC_ATTEN,
    .bitwidth = ADC_BITWIDTH_12,
  };

  esp_err_t ret = adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle);

  if (ret == ESP_OK) {
    adc_calibration_ok = true;
    ESP_LOGI(TAG, "Calibración ADC creada exitosamente (Line Fitting).");
  } else {
    ESP_LOGW(TAG, "No se pudo crear la calibración Line Fitting, error: %s", esp_err_to_name(ret));
  }
}

void readAllAdcChannels(adcReadingStruct results[NUM_ADC_CHANNELS]) {
  for (int i = 0; i < NUM_ADC_CHANNELS; i++) {
    results[i].name = adc_channels[i].name;
    results[i].voltage = read_single_channel( adc_channels[i].adc_channel, &results[i].raw_value);
  }
}

void adc_set_channel_offset(uint8_t channel, float offset) {
  if (channel < NUM_ADC_CHANNELS) {
    adc_current_offsets[channel] = offset;
  }
}