//Librerias ANSI C
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

//ESP IDF
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"

//Componentes
#include "ssd1306.h"
#include "bme680.h"

//Definiciones
#define I2C0_SCL 22
#define I2C0_SDA 21
#define I2C1_SCL 14
#define I2C1_SDA 27
#define OLED_ADDR 0x3C
#define OLED_ALTO 64
#define OLED_ANCHO 128 
#define BME_ADDR 0x77

typedef struct
{
	char temp[40];
	char humd[40];
	char press[40];
	char gas_res[40];	
}bme_values_string_t;

//Variables Globales
static const char *TAG = "app-main";
i2c_master_bus_handle_t i2c0_bus_handle = NULL;
i2c_master_bus_handle_t i2c1_bus_handle = NULL;
SSD1306_t oled_device;
BME680_t bme_device;
BME680_values_float_t values;
uint32_t duracion_medicion = 0;
bme_values_string_t values_str;

//Funciones
void i2c_master_init(void);
void oled_device_init(void);
void bme_device_init(void);

void app_main(void)
{	
	i2c_master_init();
	oled_device_init();
	bme_device_init();
	
    while (1) 
    {	
		/*ssd1306_display_text(&oled_device, 0, "Hola1 :D", 7, false);
		ssd1306_display_text(&oled_device, 1, "Hola2 :D", 7, false);
		ssd1306_display_text(&oled_device, 2, "Hola3 :D", 7, false);
		ssd1306_display_text(&oled_device, 3, "Hola4 :D", 7, false);
		ssd1306_display_text(&oled_device, 4, "Hola5 :D", 7, false);
		vTaskDelay(pdMS_TO_TICKS(100));*/
		
		//Lectura del sensor BME680
		if(ESP_OK == bme680_force_measurement(&bme_device))
		{
			vTaskDelay(duracion_medicion);
			if(ESP_OK == bme680_get_results_float(&bme_device, &values))
			{
				ESP_LOGI(TAG, 	"Sensor: %.2f C | %.2f %% | %.2f hPa | %.2f Ohm\n",
								values.temperature, values.humidity, values.pressure,
								values.gas_resistance);
			}
			
			//Convertires valores flotantes a string
			sprintf(values_str.temp, "Temp: %.2f C", values.temperature);
			sprintf(values_str.humd, "Humd: %.2f RH", values.humidity);
			sprintf(values_str.press, "Press: %.2f hPA", values.pressure);
			sprintf(values_str.gas_res, "Gas: %.2f Ohms", values.gas_resistance);
			
			//Imprimimos los valores en la pantalla oled
			ssd1306_display_text(&oled_device, 0, values_str.temp, strlen(values_str.temp), false);
			ssd1306_display_text(&oled_device, 2, values_str.humd, strlen(values_str.humd), false);
			ssd1306_display_text(&oled_device, 4, values_str.press, strlen(values_str.press), false);
			ssd1306_display_text(&oled_device, 6, values_str.gas_res, strlen(values_str.gas_res), false);	
		}
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
}

void i2c_master_init(void)
{
	//Configurar el controlador I2C0
	i2c_master_bus_config_t i2c0_bus_conf = {
		.i2c_port = I2C_NUM_0,
		.sda_io_num = I2C0_SDA,
		.scl_io_num = I2C0_SCL,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.intr_priority = 0,
		.trans_queue_depth = 0,
		.flags.enable_internal_pullup = false,
	};
	
	//Creamos la instancia global para I2C0
	i2c_new_master_bus(&i2c0_bus_conf, &i2c0_bus_handle);	
	
	//Configurar el controlador I2C1
	i2c_master_bus_config_t i2c1_bus_conf = {
		.i2c_port = I2C_NUM_1,
		.sda_io_num = I2C1_SDA,
		.scl_io_num = I2C1_SCL,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.intr_priority = 0,
		.trans_queue_depth = 0,
		.flags.enable_internal_pullup = false,
	};
	
	//Creamos la instancia global para I2C1
	i2c_new_master_bus(&i2c1_bus_conf, &i2c1_bus_handle);	
}

void oled_device_init(void)
{
	//Inicialización del OLED
	ssd1306_init(i2c0_bus_handle, &oled_device, OLED_ADDR, OLED_ANCHO, OLED_ALTO);
	
	//Limpiamos la pantalla
	ssd1306_clear_screen(&oled_device, false);
	
	//Ajustar el contraste
	ssd1306_contrast(&oled_device, 0xFF);
}

void bme_device_init(void)
{
	//Configuracion del dispositivo I2C
	bme680_init(i2c1_bus_handle, &bme_device, BME_ADDR);
	
	//Iniciar propiedades del sensor
	bme680_init_sensor(&bme_device);
	
	//Cambiar las tasas de sobremuestreo
	bme680_set_oversampling_rates(&bme_device, BME680_OSR_4X, BME680_OSR_2X, BME680_OSR_2X);
	
	//Cambiar el tamaño del filtro IIR para temperatura y presion
	bme680_set_filter_size(&bme_device, BME680_IIR_SIZE_7);
	
	//Cambiar el perfil del calentador de 0 a 200 grados durante 100ms
	bme680_set_heater_profile(&bme_device, 0, 200, 100);
	bme680_use_heater_profile(&bme_device, 0);
	
	//Establecer la temperatura ambiente en 25 grados
	bme680_set_ambient_temperature(&bme_device, 25);
	
	//Mientras no se cambie la configuracion del sensor, la duracion es constante
	bme680_get_measurement_duration(&bme_device, &duracion_medicion);
}