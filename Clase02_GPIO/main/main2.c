// Librerias ANSI C
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

// Librerias ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"

// Prototipos de Funciones
void GPIO_Init(void);

// Variables Globales
uint16_t contador = 0;

//Funcion Principal
void app_main(void)
{
	GPIO_Init();
	
   while(1){
	   // Leer entradas digitales
	   if(0 == gpio_get_level(GPIO_NUM_34))
	   {
		   gpio_set_level(GPIO_NUM_2, 1);
		   gpio_set_level(GPIO_NUM_4, 1);
	   }
	   else
	   {
		   gpio_set_level(GPIO_NUM_2, 0);
		   gpio_set_level(GPIO_NUM_4, 0);
	   }
	   
	   if(0 == gpio_get_level(GPIO_NUM_26))
	   {
		   vTaskDelay(pdMS_TO_TICKS(500));
		   contador++;
		   printf("Valor de contador: %u \n", contador);
	   }
	   
   }
}

// Desarrollo de funciones
void GPIO_Init(void)
{
	// Estructura de configuracion
	gpio_config_t gpio_conf = {
		.pin_bit_mask	= (1ULL << GPIO_NUM_34) | (1ULL << GPIO_NUM_26),
		.mode 			= GPIO_MODE_INPUT,
		.pull_up_en 	= GPIO_PULLUP_DISABLE,
		.pull_down_en	= GPIO_PULLDOWN_DISABLE,
		.intr_type		= GPIO_INTR_DISABLE,
	};
	
	// Establecer la configuracion al Hardware
	gpio_config(&gpio_conf);
	
	// Configuracion de Salidas
	gpio_conf.pin_bit_mask 	= (1ULL << GPIO_NUM_4) | (1ULL << GPIO_NUM_2);
	gpio_conf.mode			= GPIO_MODE_OUTPUT;
	gpio_conf.pull_up_en	= GPIO_PULLUP_DISABLE;
	gpio_conf.pull_down_en	= GPIO_PULLDOWN_DISABLE;
	gpio_conf.intr_type		= GPIO_INTR_DISABLE;
	
	// Establecer la configuración al Hardware
	gpio_config(&gpio_conf);
}