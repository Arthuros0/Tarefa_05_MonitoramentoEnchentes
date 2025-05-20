#include "hardware/adc.h"

#define JOY_X 26
#define JOY_Y 27

#define X_CHANNEL 0
#define Y_CHANNEL 1

#define MAX_JOY 3700
#define MIDDLE_MAX_JOY 2600

#define MIDDLE_MIN_JOY 1400
#define MIN_JOY 400



void joystick_setup();

uint16_t joystick_X_read();

uint16_t joystick_Y_read();



