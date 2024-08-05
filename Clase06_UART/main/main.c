// Librerias ANSI C
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

// Librerias ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "hal/gpio_types.h"
#include "hal/uart_types.h"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"
#include "esp_log.h"

// Etiquetas
#define UART2_BAUD_RATE	115200
#define UART2_PORT 		UART_NUM_2
#define UART2_TX 		GPIO_NUM_17
#define UART2_RX		GPIO_NUM_16
#define UART0_BAUD_RATE	115200
#define UART0_PORT 		UART_NUM_0
#define UART0_TX 		GPIO_NUM_1
#define UART0_RX		GPIO_NUM_3
#define UART_BUFF_SIZE 	1024

// Prototipos de Funciones
void uart2_init();
void uart0_init();
void gpio_init();

// Etiqueta de modulo
static const char *TAG = "app-uart";

// Variables de Aplicación
char buffer_tx2[] = "Enviando mensaje desde la ESP32 \r\n";
char buffer_tx0[] = "Enviando mensaje por ESP32 \r\n";
char buffer_rx2[50];
char buffer_rx0[50];
int len_buff_rx2;
int len_buff_rx0;

// Funcion Principal
void app_main(void)
{
	gpio_init();
	uart2_init();
	uart0_init();
	
    while (1) 
    {
		// Transmision
        uart_write_bytes(UART2_PORT, buffer_tx2, strlen(buffer_tx2));
        uart_write_bytes(UART0_PORT, buffer_tx0, strlen(buffer_tx0));
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        //Recepcion
        len_buff_rx2 = uart_read_bytes(UART2_PORT, buffer_rx0, 50, pdMS_TO_TICKS(100));
        if(len_buff_rx2 > 0)
        {
			buffer_rx2[len_buff_rx2] = '\0';
			ESP_LOGE(TAG, "DATA leida : %s", buffer_rx0);
		} 
		
		len_buff_rx0 = uart_read_bytes(UART0_PORT, buffer_rx0, 50, pdMS_TO_TICKS(100));
        if(len_buff_rx0 > 0)
        {
			buffer_rx2[len_buff_rx0] = '\0';
			ESP_LOGE(TAG, "DATA leida : %s", buffer_rx0);
		} 
    }
}

// Desarrollo de Funciones
void gpio_init(void)
{
	gpio_reset_pin(GPIO_NUM_2);
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_DEF_OUTPUT);
}

void uart2_init(void)
{
	// Configuracion de UART
	uart_config_t uart2_config = {
		.baud_rate = UART2_BAUD_RATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 0,
		.source_clk = UART_SCLK_DEFAULT
	};
	
	// Establecer los parametros al HW
	uart_param_config(UART2_PORT, &uart2_config);
	
	// UART2 Pinout
	uart_set_pin(UART2_PORT, UART2_TX, UART2_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	
	// Instalar el controlador UART
	uart_driver_install(UART2_PORT, UART_BUFF_SIZE, UART_BUFF_SIZE, 0, NULL, 0);
}

void uart0_init(void)
{
	// Configuracion de UART
	uart_config_t uart0_config = {
		.baud_rate = UART0_BAUD_RATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 0,
		.source_clk = UART_SCLK_DEFAULT
	};
	
	// Establecer los parametros al HW
	uart_param_config(UART0_PORT, &uart0_config);
	
	// UART2 Pinout
	uart_set_pin(UART0_PORT, UART0_TX, UART0_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	
	// Instalar el controlador UART
	uart_driver_install(UART0_PORT, UART_BUFF_SIZE, UART_BUFF_SIZE, 0, NULL, 0);
}