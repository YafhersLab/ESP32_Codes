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
#define BTN1_INTR	GPIO_NUM_25
#define BTN2_INTR	GPIO_NUM_27
#define LED2_PIN	GPIO_NUM_2
#define LED4_PIN	GPIO_NUM_4

// Definicion de tipos de datos
typedef struct
{
	uint32_t 	pulsos;
	bool		flag;
}button_t;

// Variables de aplicacion
static const char *TAG = "main-app";
volatile button_t btn1_data;
volatile button_t btn2_data;

// Prototipos de funciones
void gpio_init(void);

// Rutinas de Interrupcion (ISR)
void IRAM_ATTR button1_handler_isr (void *arg)
{
	btn1_data.pulsos++;
	if((btn1_data.pulsos >= 20) && (btn1_data.pulsos <= 50))
	{
		btn1_data.flag = true;
	}
	else if(btn1_data.pulsos > 50)
	{
		btn1_data.pulsos = 0;
		btn1_data.flag = false;
	}
}

void IRAM_ATTR button2_handler_isr (void *arg)
{
	btn2_data.pulsos++;
	if(btn2_data.pulsos > 10)
	{
		btn2_data.pulsos = 0;
		btn2_data.flag = true;
	}
}

void app_main(void)
{
	gpio_init();								// configurar el GPIO

    while(1)
    {
    	ESP_LOGW(TAG, "BTN1 Pulsos: %lu", btn1_data.pulsos);
    	ESP_LOGW(TAG, "BTN2 Pulsos: %lu", btn2_data.pulsos);
    	if(btn1_data.flag == true)
    	{
    		ESP_LOGE(TAG, "LED4 = ON");
    	}

    	if(btn2_data.flag == true)
    	{
    		btn2_data.flag = false;
    		gpio_set_level(LED2_PIN, 1);
    		ESP_LOGE(TAG, "LED2 = ON");
    		vTaskDelay(pdMS_TO_TICKS(1000));
    		gpio_set_level(LED2_PIN, 0);
    		ESP_LOGE(TAG, "LED2 = OFF");
    	}
    	vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void gpio_init(void)
{
	// Configuracion de entradas
	gpio_config_t gpio_cfg = {
			.pin_bit_mask 	= (1ULL << BTN1_INTR) | (1ULL << BTN2_INTR),
			.mode 			= GPIO_MODE_INPUT,
			.pull_up_en 	= GPIO_PULLUP_ENABLE,
			.pull_down_en 	= GPIO_PULLDOWN_DISABLE,
			.intr_type 		= GPIO_INTR_POSEDGE
	};
	gpio_config(&gpio_cfg);

	// Configuracion de salidas
	gpio_cfg.pin_bit_mask 	= (1ULL << LED2_PIN) | (1ULL << LED4_PIN);
	gpio_cfg.mode			= GPIO_MODE_OUTPUT;
	gpio_cfg.pull_up_en 	= GPIO_PULLUP_DISABLE;
	gpio_cfg.pull_down_en 	= GPIO_PULLDOWN_DISABLE;
	gpio_cfg.intr_type		= GPIO_INTR_DISABLE;
	gpio_config(&gpio_cfg);

	// Configuracion de interrupciones GPIO
	gpio_install_isr_service(0);									// habilita interrupciones globales para GPIO
	gpio_isr_handler_add(BTN1_INTR, button1_handler_isr, NULL);
	gpio_isr_handler_add(BTN2_INTR, button2_handler_isr, NULL);
}
