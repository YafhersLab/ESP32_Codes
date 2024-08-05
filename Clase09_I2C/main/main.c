//Librerias Ansi C
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

//Librerias ESP-IDF
#include "driver/i2c_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "hal/i2c_types.h"
#include "soc/clk_tree_defs.h"
#include "soc/i2c_struct.h"
#include "esp_err.h"

//Definiciones
#define I2C0_SDA 21
#define I2C0_SCL 22
#define DS3231_ADDR 0b01101000
#define FORMAT_12 1
#define FORMAT_24 0
#define AM 1
#define PM 0
#define NOT 0


//Prototipos de Funciones
void i2c_master_init(void);
esp_err_t i2c_device_write(const i2c_master_dev_handle_t dev_handle, const uint8_t reg_write, const uint8_t out_data);
esp_err_t i2c_device_read(const i2c_master_dev_handle_t dev_handle, const uint8_t reg_read, uint8_t *in_data);
void DS3231_SetHora(uint8_t hrs, uint8_t min, uint8_t seg, uint8_t formt, uint8_t am_pm);
void DS3231_SetFecha(uint8_t diaSem, uint8_t diaMes, uint8_t mes, uint8_t anio);
void DS3231_ReadHora(uint8_t *pHrs, uint8_t *pMin, uint8_t *pSeg, char *pAmPm);
void DS3231_ReadFecha(uint8_t *pDiaSem, uint8_t *pDiaMes, uint8_t *pMes, uint8_t *pAnio);

//Estructuras
typedef struct
{
	uint8_t hora;
	uint8_t min;
	uint8_t seg;
	uint8_t diaSemana;
	uint8_t	diaMes;
	uint8_t mes;
	uint8_t anio;
}RTC_t;

//Variables globales
i2c_master_bus_handle_t i2c0_bus_handle = NULL;
i2c_master_dev_handle_t dev_ds3231_handle = NULL;
RTC_t dataRTC;
static const char *TAG = "app-main"; 

void app_main(void)
{
	//Iniciamos perifericos
	i2c_master_init();
	
	//DS3231
	DS3231_SetHora(20, 50, 10, FORMAT_24, NOT);
	DS3231_SetFecha(2, 17, 7, 24);
	
    while (1) 
    {
		DS3231_ReadHora(&dataRTC.hora, &dataRTC.min, &dataRTC.seg, NULL);
		DS3231_ReadFecha(&dataRTC.diaSemana, &dataRTC.diaMes ,&dataRTC.diaMes ,&dataRTC.anio);
		
		ESP_LOGI(TAG, "DATOS RTC - DS3231");
		ESP_LOGI(TAG, "%02u : %02u : %02u", dataRTC.hora, dataRTC.min, dataRTC.seg);
		switch(dataRTC.diaSemana)
		{
			case 1:
			ESP_LOGI(TAG, "%02u / %02u / 20%02u - Lunes\r\n", dataRTC.diaMes, dataRTC.mes, dataRTC.anio);
				break;
			case 2:
			ESP_LOGI(TAG, "%02u / %02u / 20%02u - Martes\r\n", dataRTC.diaMes, dataRTC.mes, dataRTC.anio);
				break;
			case 3:
			ESP_LOGI(TAG, "%02u / %02u / 20%02u - Miercoles\r\n", dataRTC.diaMes, dataRTC.mes, dataRTC.anio);
				break;
			case 4:
			ESP_LOGI(TAG, "%02u / %02u / 20%02u - Jueves\r\n", dataRTC.diaMes, dataRTC.mes, dataRTC.anio);
				break;
			case 5:
			ESP_LOGI(TAG, "%02u / %02u / 20%02u - Viernes\r\n", dataRTC.diaMes, dataRTC.mes, dataRTC.anio);
				break;
			case 6:
			ESP_LOGI(TAG, "%02u / %02u / 20%02u - Sabado\r\n", dataRTC.diaMes, dataRTC.mes, dataRTC.anio);
				break;
			case 7:
			ESP_LOGI(TAG, "%02u / %02u / 20%02u - Domingo\r\n", dataRTC.diaMes, dataRTC.mes, dataRTC.anio);
				break;
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void i2c_master_init(void){
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
	
	//Configuramos el dispositivo I2C DS3231
	i2c_device_config_t dev_config = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = DS3231_ADDR, //0x68
		.scl_speed_hz = 400000, //400Khz
	};
	
	//Agregamos el esclavo al bus
	i2c_master_bus_add_device(i2c0_bus_handle, &dev_config, &dev_ds3231_handle);	
}

esp_err_t i2c_device_write(const i2c_master_dev_handle_t dev_handle, const uint8_t reg_write, const uint8_t out_data)
{
	esp_err_t ret;
	uint8_t buff_send[2];
	
	buff_send[0] = reg_write;
	buff_send[1] = out_data;
	
	ret = i2c_master_transmit(dev_handle, buff_send, 2, -1);
	return ret; //ESP OK
}

esp_err_t i2c_device_read(const i2c_master_dev_handle_t dev_handle, const uint8_t reg_read, uint8_t *in_data)
{
	esp_err_t ret;
	ret = i2c_master_transmit_receive(dev_handle, &reg_read, 1, in_data, 1, -1);
	return ret; //ESP OK
}

//****************************************************//
//FUNCIONES RTC DS3231 (Formato BCD)

void DS3231_SetHora(uint8_t hrs, uint8_t min, uint8_t seg, uint8_t formt, uint8_t am_pm)
{
	switch(formt)
	{
		case FORMAT_12:
			//Convertimos a formato BCD
			seg = (uint8_t)( ((seg/10)<<4) | (seg%10));
			min = (uint8_t)( ((min/10)<<4) | (min%10));
			hrs = (uint8_t)( ((hrs/10)<<4) | (hrs%10));
			
			if(am_pm == AM)
			{
				//Configuramos el registro de horas para el formato 12 y AM
				hrs = (hrs & 0b00011111) | 0b01000000;
			}
			else if(am_pm == PM)
			{
				//Configuramos el registro de horas para el formato 12 y PM
				hrs = (hrs & 0b00011111) | 0b01100000;
			}
			
			//Enviamos datos por I2C
			i2c_device_write(dev_ds3231_handle, 0x00, seg);
			i2c_device_write(dev_ds3231_handle, 0x01, min);
			i2c_device_write(dev_ds3231_handle, 0x02, hrs);
		break;
		
		case FORMAT_24:
			//Convertir a formato BCD
			seg = (uint8_t)( ((seg/10)<<4) | (seg%10));
			min = (uint8_t)( ((min/10)<<4) | (min%10));
			hrs = (uint8_t)( ((hrs/10)<<4) | (hrs%10));
			hrs = (uint8_t)(hrs & 0b00111111);
			
			//Enviamos datos por I2C
			i2c_device_write(dev_ds3231_handle, 0x00, seg);
			i2c_device_write(dev_ds3231_handle, 0x01, min);
			i2c_device_write(dev_ds3231_handle, 0x02, hrs);
		break;
	}
}

void DS3231_SetFecha(uint8_t diaSem, uint8_t diaMes, uint8_t mes, uint8_t anio)
{
	//Convertimos a formato BCD
	diaSem = (uint8_t)(diaSem & 0b00000111);
	diaMes = (uint8_t)( ((diaMes/10)<<4) | (diaMes%10) );
	diaMes = (uint8_t)( ((mes/10)<<4) | (mes%10) );
	diaMes = (uint8_t)( ((anio/10)<<4) | (anio%10) );
	
	//Enviamos datos por I2C
	i2c_device_write(dev_ds3231_handle, 0x03, diaSem);	
	i2c_device_write(dev_ds3231_handle, 0x04, diaMes);
	i2c_device_write(dev_ds3231_handle, 0x05, mes);
	i2c_device_write(dev_ds3231_handle, 0x06, anio);
}

void DS3231_ReadHora(uint8_t *pHrs, uint8_t *pMin, uint8_t *pSeg, char *pAmPm)
{
	//Lectura de datos I2C
	i2c_device_read(dev_ds3231_handle, 0x00, pSeg);
	i2c_device_read(dev_ds3231_handle, 0x01, pMin);
	i2c_device_read(dev_ds3231_handle, 0x02, pHrs);
	
	//Evaluamos si estamos en el formato de 12 o 24 horas
	if( (*pHrs & (1 << 6)) != 0 )
	{
		//Formato 12 horas
		if( (*pHrs & (1 << 5)) != 0 )
		{
			//es PM
			strcpy((char *)pAmPm, "PM");
		}
		else
		{
			//es AM
			strcpy((char *)pAmPm, "AM");
		}
		
		//Enmascaramos
		*pHrs = (uint8_t)(*pHrs & 0b00011111);
		*pHrs = (uint8_t)((*pHrs >> 4)*10 + (*pHrs & 0b00001111));
	}
	else 
	{
		//Formato 24 horas
		*pHrs = (uint8_t)(*pHrs & 0b00111111);
		*pHrs = (uint8_t)((*pHrs >> 4)*10 + (*pHrs & 0b00001111));
	}
	
	*pMin = (uint8_t)((*pMin >> 4)*10) + (*pMin & 0b00001111);
	*pSeg = (uint8_t)((*pSeg >> 4)*10) + (*pSeg & 0b00001111);
}

void DS3231_ReadFecha(uint8_t *pDiaSem, uint8_t *pDiaMes, uint8_t *pMes, uint8_t *pAnio)
{
	//Lectura de datos I2C
	i2c_device_read(dev_ds3231_handle, 0x03, pDiaSem);
	i2c_device_read(dev_ds3231_handle, 0x04, pDiaMes);
	i2c_device_read(dev_ds3231_handle, 0x05, pMes);
	i2c_device_read(dev_ds3231_handle, 0x06, pAnio);
	
	//Le damos formato decimal
	*pDiaSem = (uint8_t)(*pDiaSem & 0b00000111);
	*pDiaMes = (uint8_t)((*pDiaMes >> 4)*10 + (*pDiaMes & 0b00001111));
	*pMes = (uint8_t)((*pMes >> 4)*10 + (*pMes & 0b00001111));
	*pAnio = (uint8_t)((*pAnio >> 4)*10 + (*pAnio & 0b00001111));
}
