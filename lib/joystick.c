#include "joystick.h"


void joystick_setup(){
  adc_init();
  adc_gpio_init(JOY_Y);
  adc_gpio_init(JOY_X);
}

uint16_t joystick_X_read(){
  adc_select_input(X_CHANNEL);
  return adc_read();
}

uint16_t joystick_Y_read(){
  adc_select_input(Y_CHANNEL);
  return adc_read();
}
