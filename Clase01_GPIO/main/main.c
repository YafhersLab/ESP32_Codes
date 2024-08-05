// Librerias Ansi C
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

// Librerias de ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Macros o etiquetas
#define LED_PIN 2

// Prototipos de funciones
void gpio_init(void);

void app_main(void)
{
	gpio_init();

    for(;;)
    {
    	gpio_set_level(GPIO_NUM_2, 1);
    	printf("LED = ON\r\n");
    	vTaskDelay(pdMS_TO_TICKS(1000));

    	gpio_set_level(GPIO_NUM_2, 0);
    	printf("LED = OFF\r\n");
    	vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void gpio_init(void)
{
	gpio_reset_pin(GPIO_NUM_2);
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
}
