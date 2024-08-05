// Librerias Ansi C
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

// Librerias ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Macros
#define LED1	GPIO_NUM_2
#define LED2 	GPIO_NUM_4
#define	BUTTON1	GPIO_NUM_34
#define	BUTTON2	GPIO_NUM_26

// Variables
static const char *TAG = "main-gpio";
esp_log_level_t nivel_registro;

// Prototipos de funciones
void gpio_init(void);

void app_main(void)
{
	gpio_init();								// configurar el GPIO
	esp_log_level_set(TAG, ESP_LOG_VERBOSE);	// configuracion del nivel de detalle
	nivel_registro = esp_log_level_get(TAG);	// obtener el nivel de detalle

    while(1)
    {
    	// leer el boton 1
    	if(0 == gpio_get_level(BUTTON1))
    	{
    		gpio_set_level(LED1, 1);
    	}
    	else
    	{
    		gpio_set_level(LED1, 0);
    	}

    	// leer el boton 1
    	if(0 == gpio_get_level(BUTTON2))
    	{
    		gpio_set_level(LED2, 1);
    	}
    	    else
    	{
    	    gpio_set_level(LED2, 0);
    	}

    	printf("\r\n\r\nNivel de detalle: %d \r\n", nivel_registro);

    	// usamos macros de registro
    	ESP_LOGE(TAG, "Este mensaje es un error!!!");
    	ESP_LOGW(TAG, "Este mensaje es una advertencia!!!");
    	ESP_LOGI(TAG, "Este mensaje es informativo!!!");
    	ESP_LOGD(TAG, "Este mensaje es de debug!!!");
    	ESP_LOGV(TAG, "Este mensaje es de verbose!!!");
    	vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void gpio_init(void)
{
	// Configuracion de entradas
	gpio_config_t gpio_cfg = {
			.pin_bit_mask 	= (1ULL << BUTTON1) | (1ULL << BUTTON2),
			.mode 			= GPIO_MODE_INPUT,
			.pull_up_en 	= GPIO_PULLUP_DISABLE,
			.pull_down_en 	= GPIO_PULLDOWN_DISABLE,
			.intr_type 		= GPIO_INTR_DISABLE,
	};
	gpio_config(&gpio_cfg);

	// Configuracion de salidas
	gpio_cfg.pin_bit_mask 	= (1ULL << LED1) | (1ULL << LED2);
	gpio_cfg.mode			= GPIO_MODE_OUTPUT;
	gpio_cfg.pull_up_en 	= GPIO_PULLUP_DISABLE;
	gpio_cfg.pull_down_en 	= GPIO_PULLDOWN_DISABLE;
	gpio_cfg.intr_type		= GPIO_INTR_DISABLE;
	gpio_config(&gpio_cfg);
}
