#ifndef ANALOG_DIGITAL_CONVERTER_H
#define ANALOG_DIGITAL_CONVERTER_H

#include <stdint.h>

#define NUM_ADC_CHANNELS 4

static const float ADC_DEFAULT_OFFSETS[NUM_ADC_CHANNELS] = {
  0.035f,   
  0.035f,   
  0.035f,   
  0.035f  
};

typedef struct {
  float voltage;
  uint32_t raw_value;
  const char *name;
} adcReadingStruct;

void adcReaderInit(void);
void readAllAdcChannels(adcReadingStruct results[NUM_ADC_CHANNELS]);
void adc_set_channel_offset(uint8_t channel, float offset);

#endif 

