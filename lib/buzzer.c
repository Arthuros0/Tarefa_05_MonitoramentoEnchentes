#include "buzzer.h"

uint buzzer_init(uint8_t gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);        
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_set_enabled(slice, false);

    return slice;
}

void buzzer_turn_on(uint8_t gpio, uint slice, uint16_t freq) {
    uint clock_div = 4;                             // divisor do clock
    uint sys_clock = clock_get_hz(clk_sys);         // frequência do sistema
    uint wrap = sys_clock / (clock_div * freq) - 1; // calcula o valor de "wrap" para a frequência desejada

    pwm_set_clkdiv(slice, clock_div);              
    pwm_set_wrap(slice, wrap);                    
    pwm_set_gpio_level(gpio, (wrap) / 10);    
    pwm_set_enabled(slice, true);                   // habilita o PWM
}


void buzzer_beep(uint8_t gpio, uint slice, uint16_t freq, uint16_t duration_ms) {
    buzzer_turn_on(gpio, slice, freq); 
    sleep_ms(duration_ms);           
    pwm_set_enabled(slice, false);    
}

void buzzer_turn_off(uint8_t gpio){
  pwm_set_gpio_level(gpio, 0);
}