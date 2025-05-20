#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "joystick.h"
#include "matrix_leds.h"
#include "ssd1306.h"
#include "buzzer.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

typedef struct{
    uint8_t river_level;
    uint8_t rain;
}Sensor_data;

typedef struct{
    uint16_t x;
    uint16_t y; 
}joystick;

QueueHandle_t xQueueProcessingSensor;
QueueHandle_t xQueueMatrixLed;
QueueHandle_t xQueueDisplay;
QueueHandle_t xQueueBuzzer;

void vJoystickTask(){
    joystick_setup();

    joystick joystick_data;

    while (1)
    {
        joystick_data.y=joystick_Y_read();

        joystick_data.x=joystick_X_read(); //Corrige inversão do eixo X

        printf("X: %d, Y: %d\n", joystick_data.x, joystick_data.y);
        
        if((joystick_data.x >= 2050 || joystick_data.x <=1850)||(joystick_data.y >= 2140 || joystick_data.y <=1940)){
            xQueueSend(xQueueProcessingSensor, &joystick_data, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(150));
    }
    
}

void vProcessingSensorTask(){
    
    joystick joystick_data;
    Sensor_data sensor_data = {30, 10};

    while (1)
    {
        if(xQueueReceive(xQueueProcessingSensor, &joystick_data, 0)){
            //Processamento do dados de chuva no eixo X
            if(joystick_data.x > MAX_JOY && sensor_data.rain <= 98){
                sensor_data.rain+=2;
            }else if(joystick_data.x > MIDDLE_MAX_JOY && sensor_data.rain <= 99){
                sensor_data.rain++;
            }else if(joystick_data.x < MIN_JOY && sensor_data.rain >= 2){
                sensor_data.rain-=2;
            }else if(joystick_data.x < MIDDLE_MIN_JOY && sensor_data.rain >= 1){
                sensor_data.rain--;
            }

            //Processamento dos dados de nivel de rio no eixo Y
            if(joystick_data.y > MAX_JOY && sensor_data.river_level <= 98){
                sensor_data.river_level+=2;
            }else if(joystick_data.y > MIDDLE_MAX_JOY && sensor_data.river_level <= 99){
                sensor_data.river_level++;
            }else if(joystick_data.y < MIN_JOY && sensor_data.river_level >= 4){
                sensor_data.river_level-=2;
            }else if(joystick_data.y < MIDDLE_MIN_JOY && sensor_data.river_level >= 3){
                sensor_data.river_level--;
            }

            xQueueSend(xQueueMatrixLed, &sensor_data, 0);
            xQueueSend(xQueueDisplay, &sensor_data, 0);
            xQueueSend(xQueueBuzzer, &sensor_data, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
}

void vMatrixLedTask(){
    setup_led_matrix();
    Sensor_data sensor_data;

    while (1)
    {
        if(xQueueReceive(xQueueMatrixLed, &sensor_data, 0)){
            if(sensor_data.rain >= 80 || sensor_data.river_level >= 70){
                desenha_frame(estados,1);
            }else{
                desenha_frame(estados,2);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
}

void vDisplayTask(){

    i2c_init(I2C_PORT, 400 * 4000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display
    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    bool cor = true;

    Sensor_data sensor_data;
    char rain_str[15];
    char river_level_str[15];

    while (1)
    {
        if(xQueueReceive(xQueueDisplay, &sensor_data, 0)){
            cor = !cor;

            sprintf(rain_str, "Chuva: %d%%", sensor_data.rain);
            sprintf(river_level_str, "Nivel: %d%%", sensor_data.river_level);

            ssd1306_fill(&ssd, !cor);
            ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);      // Desenha um retângulo
            //ssd1306_line(&ssd, 3, 25, 123, 25, cor);           // Desenha uma linha

            if(sensor_data.rain >= 80 || sensor_data.river_level >= 70){
                
                ssd1306_draw_string(&ssd,"Alerta!",6,10);
                ssd1306_draw_string(&ssd,rain_str,8,25); 
                ssd1306_draw_string(&ssd,river_level_str,8,40);

            }else{
                ssd1306_draw_string(&ssd,"Nivel Normal",6,10);
                ssd1306_draw_string(&ssd,rain_str,8,25); 
                ssd1306_draw_string(&ssd,river_level_str,8,40);
            }
            ssd1306_send_data(&ssd);
        }
    }
    
}

void vBuzzerTask(){
    uint slice_buzzer=buzzer_init(BUZZER_A_PIN);
    Sensor_data sensor_data;
    bool alert=false;

    while (1)
    {
        if(xQueueReceive(xQueueBuzzer, &sensor_data, 0)){
            
            alert=(sensor_data.rain >= 80 || sensor_data.river_level >= 70);

            if(alert){
                for (uint8_t i = 0; i < 2; i++)
                {
                    buzzer_turn_on(BUZZER_A_PIN, slice_buzzer, 1000);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    buzzer_turn_off(BUZZER_A_PIN);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    xQueueReceive(xQueueBuzzer, &sensor_data, 0);
                }
                vTaskDelay(pdMS_TO_TICKS(500));
            }
        }
        buzzer_turn_off(BUZZER_A_PIN);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
}

void vLedTask(){
    init_leds();
    Sensor_data sensor_data;

    while (1)
    {
        if(xQueueReceive(xQueueBuzzer, &sensor_data, 0)){
            if(sensor_data.rain >= 80 || sensor_data.river_level >= 70){
                gpio_put(11, 0);
                for (uint8_t i = 0; i < 2; i++){
                    gpio_put(13, 1);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    gpio_put(13, 0);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
            }else{
                gpio_put(11, 1);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
    
}



int main()
{
    stdio_init_all();

    xQueueProcessingSensor = xQueueCreate(5, sizeof(joystick));
    xQueueMatrixLed = xQueueCreate(5, sizeof(Sensor_data));
    xQueueDisplay = xQueueCreate(5, sizeof(Sensor_data));
    xQueueBuzzer = xQueueCreate(5, sizeof(Sensor_data));

    xTaskCreate(vJoystickTask, "Joystick", 256, NULL, 1, NULL);
    xTaskCreate(vProcessingSensorTask, "Processing Sensor", 256, NULL, 1, NULL);
    xTaskCreate(vMatrixLedTask, "Matrix Led", 512, NULL, 1, NULL);
    xTaskCreate(vDisplayTask, "Display", 256, NULL, 1, NULL);
    xTaskCreate(vBuzzerTask, "Buzzer", 256, NULL, 1, NULL);
    xTaskCreate(vLedTask, "Led", 256, NULL, 1, NULL);

    vTaskStartScheduler();

}
