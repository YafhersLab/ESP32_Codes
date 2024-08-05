/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ruslan V. Uss <unclerus@gmail.com>
 *
 * Actualizaciones:
 * Ivan Vargas A. ( INTECX S.A.C. => https://intecx.com.pe )
 */

#include "bme680.h"

static const char *TAG = "bme680_i2c";

//Desarrollo de funciones
esp_err_t bme_i2c_init(i2c_master_bus_handle_t master_bus, BME680_t *dev, uint8_t addr)
{
	//Variables locales
	esp_err_t ret;

	//Configuracion del dispositivo
	i2c_device_config_t dev_conf = {
			.dev_addr_length = I2C_ADDR_BIT_LEN_7,
			.device_address  = addr,
			.scl_speed_hz    = BME680_DEV_FREQ_HZ,
	};
	//Agregar un dispositivo al bus I2C
	ret = i2c_master_bus_add_device(master_bus, &dev_conf, &dev->dev_handle);
	if (ret == ESP_OK) {
		ESP_LOGI(TAG, "BME agredado al bus exitosamente");
	} else {
		ESP_LOGE(TAG, "BME error al agregar al bus. code: 0x%.2X", ret);
	}

	return ret;
}


esp_err_t bme_i2c_dev_read_reg(const BME680_t *dev, uint8_t reg, uint8_t *in_data, size_t in_size)
{
	esp_err_t ret;

	if (!dev || !in_data || !in_size)
		return ESP_ERR_INVALID_ARG;

	//Transmitir y leer dato
	ret = i2c_master_transmit_receive(dev->dev_handle, &reg, 1, in_data, in_size, 10); //10ms

	return ret; // ESP_OK
}

esp_err_t bme_i2c_dev_write_reg(const BME680_t *dev, uint8_t reg, const uint8_t *out_data, size_t out_size)
{
	esp_err_t ret;
	uint8_t buff_send[2];

	if (!dev || !out_data || !out_size)
		return ESP_ERR_INVALID_ARG;

	//Set buffer
	buff_send[0] = reg;
	buff_send[1] = *out_data;

	ret = i2c_master_transmit(dev->dev_handle, buff_send, 2, 10); //10ms

	return ret; // ESP_OK
}
