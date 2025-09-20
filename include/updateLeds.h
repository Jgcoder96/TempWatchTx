#ifndef UPDATE_LEDS_H
#define UPDATE_LEDS_H

void shift_register_init();
void shift_out_string_16(const char* bit_string);

#endif