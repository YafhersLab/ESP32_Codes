//Librerias ANSI C
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

//Librerias ESP-IDF
#include "driver/adc_types_legacy.h"
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

//Definiciones
#define VMAX 3.3f
#define DMAX 4095

//Prototipos de Funciones
void ADC1_Init(void);
void ADC1_read_multisampling(adc_channel_t canal, int *adc_raw_promediado);

//Tipos de Datos
typedef struct
{
	int raw;
	float vout;
}ADC_resultados_t;

//Crear un identificador
adc_oneshot_unit_handle_t adc1_handle = NULL;
static const char *TAG = "main-app";

//Variables de Aplicacion
ADC_resultados_t pot = {0};

void app_main(void)
{	
	//Iniciamos perifericos
	ADC1_Init();
	
    while(1)
    {
        /*//Leer el valor del ADC
        adc_oneshot_read(adc1_handle, ADC1_CHANNEL_6, &pot.raw);
        pot.vout =((float)pot.raw * VMAX) / DMAX;
        
        ESP_LOGI(TAG, "Valor ADC: %d", pot.raw);
        ESP_LOGI(TAG, "Voltaje ADC: %.2f \r\n", pot.vout);
        vTaskDelay(pdMS_TO_TICKS(1000));*/
        
        ADC1_read_multisampling(ADC_CHANNEL_6, &pot.raw);
        pot.vout =((float)pot.raw * VMAX) / DMAX;
        ESP_LOGI(TAG, "Valor ADC: %d", pot.raw);
        ESP_LOGI(TAG, "Voltaje ADC: %.2f \r\n", pot.vout);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void ADC1_Init(void)
{
	//Configuracion del ADC1
	adc_oneshot_unit_init_cfg_t adc1_config = {
		.unit_id = ADC_UNIT_1,
		.ulp_mode = ADC_ULP_MODE_DISABLE,
		.clk_src = ADC_RTC_CLK_SRC_DEFAULT
	};
	
	//Asignar la configuracion al ADC1_Handle y Hardware
	adc_oneshot_new_unit(&adc1_config,&adc1_handle);
	
	//Configuracion del canal ADC
	adc_oneshot_chan_cfg_t adc1_channel_config = {
		.atten = ADC_ATTEN_DB_11,
		.bitwidth = ADC_BITWIDTH_12,
	};
	
	//Cargamos la configuracion del canal
	adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_6, &adc1_channel_config);
	adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_7, &adc1_channel_config);
}

void ADC1_read_multisampling(adc_channel_t canal, int *adc_raw_promediado)
{
	int32_t adc_acumulacion = 0;
	int adc_value = 0;
	
	for(int muestras = 0; muestras < 64; muestras++)
	{
		adc_oneshot_read(adc1_handle,canal, &adc_value);
		adc_acumulacion = adc_acumulacion + (int32_t)adc_value;
	}
	
	*adc_raw_promediado = (int)(adc_acumulacion / (int32_t)64); 
}