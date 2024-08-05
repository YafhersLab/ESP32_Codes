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
#define LED2_PIN			GPIO_NUM_2
#define LED4_PIN			GPIO_NUM_4
#define PIN_MASK_OUTPUT 	((1ULL << LED2_PIN) | (1ULL << LED4_PIN))

// Definicion de tipos de datos

// Variables de aplicacion
static const char *TAG = "main-timer";


// Prototipos de funciones
void ledc_pwm_low_speed(void);

// Rutinas de Interrupcion (ISR)

void app_main(void)
{
	ledc_pwm_low_speed();

    while(1)
    {

    	// Canal PWM1
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 2000); // 0 a 4095
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

		// Canal PWM2
		ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, 2000); // 0 a 4095
		ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);

		vTaskDelay(pdMS_TO_TICKS(100));

		for(uint32_t duty_cycle = 0; duty_cycle <= 4095; duty_cycle = duty_cycle + 10)
		{

		}

    }
}

void ledc_pwm_low_speed(void)
{
	// Configuracion del l_timer0
	ledc_timer_config_t l_timer0_cfg = {
			.speed_mode = LEDC_LOW_SPEED_MODE,
			.duty_resolution = LEDC_TIMER_12_BIT, // Duty 0 a 4095
			.timer_num = LEDC_TIMER_0,
			.freq_hz = 10000,
			.clk_cfg = LEDC_APB_CLK
	};

	// Setear la configuracion
	ledc_timer_config(&l_timer0_cfg);

	// Configuracion del canal 1
	ledc_channel_config_t l_channel1_cfg = {
			.gpio_num = GPIO_NUM_2,
			.speed_mode = LEDC_LOW_SPEED_MODE,
			.channel = LEDC_CHANNEL_1,
			.intr_type = 0,
			.timer_sel = LEDC_TIMER_0,
			.duty = 0,
			.hpoint = 0,
			.flags.output_invert = 0
	};

	// Setear la configuracion
	ledc_channel_config(&l_channel1_cfg);

	// Configuracion del canal 2
	ledc_channel_config_t l_channel2_cfg = {
			.gpio_num = GPIO_NUM_4,
			.speed_mode = LEDC_LOW_SPEED_MODE,
			.channel = LEDC_CHANNEL_2,
			.intr_type = 0,
			.timer_sel = LEDC_TIMER_0,
			.duty = 0,
			.hpoint = 0,
			.flags.output_invert = 1
	};

	// Setear la configuracion
	ledc_channel_config(&l_channel2_cfg);
}
