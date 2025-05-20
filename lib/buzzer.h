#ifndef BUZZER_H
#define BUZZER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// Defines
#define BUZZER_A_PIN 21

// Cabeçalhos das funções
uint buzzer_init(uint8_t gpio);
void buzzer_turn_on(uint8_t gpio, uint slice, uint16_t freq);
void buzzer_beep(uint8_t gpio, uint slice, uint16_t freq, uint16_t duration_ms);
void buzzer_turn_off(uint8_t gpio);

#endif