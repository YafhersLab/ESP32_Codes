// Librerias Ansi C
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

// Librerias ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"	// para el manejo del controlador LEDC
#include "esp_log.h"

// Macros

// Definicion de tipos de datos

// Prototipos de funciones
void ledc_pwm_high_speed(void);

// Rutinas de Interrupcion (ISR)

void app_main(void)
{
	ledc_pwm_high_speed();

    while(1)
    {
    	ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_6, 4100);
    	ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_6);
    	ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_7, 4100);
		ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_7);

		// A LA MISMA FRECUENCIA PERO CON DIFERENTES TIMERS HAY UN DESFASE

		vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void ledc_pwm_high_speed(void)
{
	// Configuracion del h_timer2
	ledc_timer_config_t h_timer2_cfg = {
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.duty_resolution = LEDC_TIMER_13_BIT, // Duty 0 a 8192
			.timer_num = LEDC_TIMER_2,
			.freq_hz = 5000,
			.clk_cfg = LEDC_APB_CLK
	};

	// Setear la configuracion
	ledc_timer_config(&h_timer2_cfg);

	// Configuracion del h_timer3
	ledc_timer_config_t h_timer3_cfg = {
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.duty_resolution = LEDC_TIMER_13_BIT, // Duty 0 a 8192
			.timer_num = LEDC_TIMER_3,
			.freq_hz = 5000,
			.clk_cfg = LEDC_APB_CLK
	};

	// Setear la configuracion
	ledc_timer_config(&h_timer3_cfg);

	// Configuracion del canal 6
	ledc_channel_config_t h_channel6_cfg = {
			.gpio_num = GPIO_NUM_2,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.channel = LEDC_CHANNEL_6,
			.intr_type = LEDC_INTR_DISABLE,
			.timer_sel = LEDC_TIMER_2,
			.duty = 0,
			.hpoint = 0,
			.flags.output_invert = 0
	};

	// Setear la configuracion
	ledc_channel_config(&h_channel6_cfg);

	// Configuracion del canal 7
	ledc_channel_config_t h_channel7_cfg = {
			.gpio_num = GPIO_NUM_4,
			.speed_mode = LEDC_HIGH_SPEED_MODE,
			.channel = LEDC_CHANNEL_7,
			.intr_type = 0,
			.timer_sel = LEDC_TIMER_3,
			.duty = 0,
			.hpoint = 200, // va desde 0 a 20 bits
			.flags.output_invert = 0
	};

	// Setear la configuracion
	ledc_channel_config(&h_channel7_cfg);
}
