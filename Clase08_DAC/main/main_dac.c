// Librerias ANSI C
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

// Librerias ESP IDF
#include "driver/dac_cosine.h"
#include "driver/dac_types_legacy.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/dac_oneshot.h"
#include "hal/dac_types.h"
#include "soc/clk_tree_defs.h"

// Variables Globales
dac_cosine_handle_t dac_cos_ch0_handle = NULL;
dac_cosine_handle_t dac_cos_ch1_handle = NULL;
dac_oneshot_handle_t dac_ch0_handle = NULL;
dac_oneshot_handle_t dac_ch1_handle = NULL;

// Funciones
void DAC_Init();
void DAC_COS_Init();

void app_main(void)
{
	// Iniciamos perifericos
	DAC_COS_Init();
	
    while (1) 
    {
		vTaskDelay(pdMS_TO_TICKS(100));	
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

void DAC_COS_Init()
{
	// Configuracion - Canal 0
	dac_cosine_config_t dac_cos_ch0_conf = {
		.chan_id = DAC_CHAN_0,
		.freq_hz = 10000, //10KHz,
		.clk_src = DAC_COSINE_CLK_SRC_RTC_FAST,
		.atten = DAC_COSINE_ATTEN_DB_0,
		.phase = DAC_COSINE_PHASE_0,
		.offset = 0,
		.flags.force_set_freq = false,
	};
	
	// Creamos la instancia - Canal 0
	dac_cosine_new_channel(&dac_cos_ch0_conf,&dac_cos_ch0_handle);
	dac_cosine_start(dac_cos_ch0_handle);
	
	// Configuracion - Canal 1
	dac_cosine_config_t dac_cos_ch1_conf = {
		.chan_id = DAC_CHAN_1,
		.freq_hz = 5000, //5KHz,
		.clk_src = DAC_COSINE_CLK_SRC_RTC_FAST,
		.atten = DAC_COSINE_ATTEN_DB_0,
		.phase = DAC_COSINE_PHASE_0,
		.offset = 0,
		.flags.force_set_freq = false,
	};
	
	// Creamos la instancia - Canal 1
	dac_cosine_new_channel(&dac_cos_ch1_conf,&dac_cos_ch1_handle);
	dac_cosine_start(dac_cos_ch1_handle);
	
}