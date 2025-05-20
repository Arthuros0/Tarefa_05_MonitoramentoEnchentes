#ifndef MATRIX_LEDS_H
#define MATRIX_LEDS_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pio_matrix.pio.h"


extern uint sm;

uint8_t obter_index(uint8_t i);

void desenha_frame(const uint32_t matriz[][25],uint8_t frame);

void apaga_matriz();

void setup_led_matrix();

void init_leds();

extern const uint32_t estados[4][25];
#endif