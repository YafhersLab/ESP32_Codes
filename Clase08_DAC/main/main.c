// Librerias ANSI C
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

// Librerias ESP IDF
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/dac_oneshot.h"
#include "hal/dac_types.h"

// Variables Globales
dac_oneshot_handle_t dac_ch0_handle = NULL;
dac_oneshot_handle_t dac_ch1_handle = NULL;

// Funciones
void DAC_Init();

void app_main(void)
{
	// Iniciamos perifericos
	DAC_Init();
	
	// Vout_DAC = DATA * (3.3v / 255)
	dac_oneshot_output_voltage(dac_ch0_handle, 110);
	dac_oneshot_output_voltage(dac_ch1_handle, 200);
    while (1) 
    {
		vTaskDelay(pdMS_TO_TICKS(100));
		
		// Señal Triangular
		for(uint8_t onda = 0; onda < 255; onda++)
		{
			dac_oneshot_output_voltage(dac_ch0_handle, onda);
			dac_oneshot_output_voltage(dac_ch1_handle, onda);
		}
		
		for(uint8_t onda = 255; onda > 0; onda--)
		{
			dac_oneshot_output_voltage(dac_ch0_handle, onda);
			dac_oneshot_output_voltage(dac_ch1_handle, onda);
		}
		
    }
    
}

void DAC_Init()
{
	// Configuracion DAC - Canal 0
	dac_oneshot_config_t dac_ch0_conf = {
		.chan_id = DAC_CHAN_0,
	};
	
	// Creamos la instancia
	dac_oneshot_new_channel(&dac_ch0_conf, &dac_ch0_handle);
	
	// Configuramos el DAC - Canal 1
	dac_oneshot_config_t dac_ch1_conf = {
		.chan_id = DAC_CHAN_1,
	};
	
	// Creamos la instancia
	dac_oneshot_new_channel(&dac_ch1_conf, &dac_ch1_handle);
}