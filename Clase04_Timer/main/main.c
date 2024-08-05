// Librerias Ansi C
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

// Librerias ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"	// para el manejo del controlador GPTimer
#include "esp_log.h"

// Macros
#define LED2_PIN			GPIO_NUM_2
#define LED4_PIN			GPIO_NUM_4
#define PIN_MASK_OUTPUT 	((1ULL << LED2_PIN) | (1ULL << LED4_PIN))

// Definicion de tipos de datos

// Variables de aplicacion
static const char *TAG = "main-timer";
gptimer_handle_t timer0_handle = NULL;
gptimer_handle_t timer1_handle = NULL;
bool led2_state = false;
bool tiempo5seg_flag = false;
bool tiempo10seg_flag = false;
uint8_t timer_count = 0;

// Prototipos de funciones
void gpio_init(void);
void timer0_init(void);
void timer1_init(void);

// Rutinas de Interrupcion (ISR)
bool IRAM_ATTR timer0_event_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
	timer_count++;
	if(timer_count == 5)
	{
		tiempo5seg_flag = true;
	}
	else if(timer_count == 10)
	{
		tiempo10seg_flag = true;
		timer_count = 0;
	}
	return true;
}

bool IRAM_ATTR timer1_event_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
	led2_state = !led2_state;
	gpio_set_level(LED2_PIN, led2_state);
	return true;
}

void app_main(void)
{
	gpio_init();
	timer0_init(); //1s
	timer1_init(); //500ms

    while(1)
    {
    	if(tiempo5seg_flag == true)
    	{
    		tiempo5seg_flag = false;
    		ESP_LOGE(TAG, "Han pasado 5 segundos");
    	}
    	else if(tiempo10seg_flag == true)
    	{
    		tiempo10seg_flag = false;
    		ESP_LOGI(TAG, "Han pasado 10 segundos");
    	}

    	vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void gpio_init(void)
{
	// Configuracion de salidas
	gpio_config_t gpio_cfg = {
			.pin_bit_mask 	= PIN_MASK_OUTPUT,
			.mode 			= GPIO_MODE_OUTPUT,
			.pull_up_en 	= GPIO_PULLUP_DISABLE,
			.pull_down_en 	= GPIO_PULLDOWN_DISABLE,
			.intr_type 		= GPIO_INTR_DISABLE
	};
	gpio_config(&gpio_cfg);
}

void timer0_init(void)
{
	// Configuracion del timer0
	gptimer_config_t timer0_config = {
			.clk_src = GPTIMER_CLK_SRC_DEFAULT, //GPTIMER_CLK_SRC_APB	APB_CLK = 80MHz
			.direction = GPTIMER_COUNT_UP,
			.resolution_hz = 1000000,	// tick = 1us
			.intr_priority = 0, // 0 es default
			.flags.intr_shared = 0
	};

	// Setear la configuracion
	gptimer_new_timer(&timer0_config, &timer0_handle);

	// Configuracion del evento de alarma
	gptimer_event_callbacks_t timer0_config_cb = {
			.on_alarm = timer0_event_alarm_cb
	};

	// Setear la configuracion del callback
	gptimer_register_event_callbacks(timer0_handle, &timer0_config_cb, NULL);

	// Habilitar el Timer
	gptimer_enable(timer0_handle);

	// Configuracion del evento de alarma
	gptimer_alarm_config_t timer0_alarm_cfg = {
			.alarm_count = 1000000, // 1s dispara evento de alarma
			.reload_count = 0,
			.flags.auto_reload_on_alarm = 1
	};

	// Setear la configuración de la alarma
	gptimer_set_alarm_action(timer0_handle, &timer0_alarm_cfg);

	// Iniciar el timer
	gptimer_start(timer0_handle);
}

void timer1_init(void)
{
	// Configuracion del timer0
	gptimer_config_t timer1_config = {
			.clk_src = GPTIMER_CLK_SRC_DEFAULT, //GPTIMER_CLK_SRC_APB	APB_CLK = 80MHz
			.direction = GPTIMER_COUNT_UP, .resolution_hz = 1000000,// tick = 1us
			.intr_priority = 0, // 0 es default
			.flags.intr_shared = 0 };

	// Setear la configuracion
	gptimer_new_timer(&timer1_config, &timer1_handle);

	// Configuracion del evento de alarma
	gptimer_event_callbacks_t timer1_config_cb = {
			.on_alarm = timer1_event_alarm_cb
	};

	// Setear la configuracion del callback
	gptimer_register_event_callbacks(timer1_handle, &timer1_config_cb, NULL);

	// Habilitar el Timer
	gptimer_enable(timer1_handle);

	// Configuracion del evento de alarma
	gptimer_alarm_config_t timer1_alarm_cfg = {
			.alarm_count = 500000, // 500ms dispara evento de alarma
			.reload_count = 0, .flags.auto_reload_on_alarm = 1 };

	// Setear la configuración de la alarma
	gptimer_set_alarm_action(timer1_handle, &timer1_alarm_cfg);

	// Iniciar el timer
	gptimer_start(timer1_handle);
}
