#include "ssd1306.h"

static const char *TAG = "ssd1206-i2c";


esp_err_t ssd_i2c_init(i2c_master_bus_handle_t master_bus, SSD1306_t *dev, uint8_t addr, int width, int height) {

	//Variables locales
	esp_err_t ret;
	uint8_t buff_send[30];

	//Configuracion del modulos DS3231 (Dispositivo I2C)
	i2c_device_config_t dev_conf = {
			.dev_addr_length = I2C_ADDR_BIT_LEN_7,
			.device_address  = addr,
			.scl_speed_hz    = SSD1306_DEV_FREQ_HZ,
	};
	//Agregar un dispositivo al bus I2C
	i2c_master_bus_add_device(master_bus, &dev_conf, &dev->dev_handle);

	vTaskDelay(pdMS_TO_TICKS(50));

	dev->_flip   = false;
	dev->_width  = width;
	dev->_height = height;
	dev->_pages  = 8;
	if (dev->_height == 32) {
		dev->_pages = 4;
	}

	//Set buffer
	buff_send[0] = OLED_CONTROL_BYTE_CMD_STREAM;
	buff_send[1] = OLED_CMD_DISPLAY_OFF;             // AE
	buff_send[2] = OLED_CMD_SET_MUX_RATIO;           // A8
	if (dev->_height == 64) buff_send[3] = 0x3F;     // 3F
	if (dev->_height == 32) buff_send[3] = 0x1F;     // 1F
	buff_send[4] = OLED_CMD_SET_DISPLAY_OFFSET;	     // D3
	buff_send[5] = 0x00;
	//buff_send[6] = OLED_CONTROL_BYTE_DATA_STREAM;  // 40
	buff_send[6] = OLED_CMD_SET_DISPLAY_START_LINE;	 // 40
	//buff_send[7] = OLED_CMD_SET_SEGMENT_REMAP;     // A1
	if (dev->_flip) {
		buff_send[7] = OLED_CMD_SET_SEGMENT_REMAP_0; // A0
	} else {
		buff_send[7] = OLED_CMD_SET_SEGMENT_REMAP_1; // A1
	}
	buff_send[8]  = OLED_CMD_SET_COM_SCAN_MODE;		 // C8
	buff_send[9]  = OLED_CMD_SET_DISPLAY_CLK_DIV;	 // D5
	buff_send[10] = 0x80;                            // 08
	buff_send[11] = OLED_CMD_SET_COM_PIN_MAP;		 // DA
	if (dev->_height == 64) buff_send[12] = 0x12;    // 12
	if (dev->_height == 32) buff_send[12] = 0x02;    // 02
	buff_send[13] = OLED_CMD_SET_CONTRAST;			 // 81
	buff_send[14] = 0xFF;                            // FF
	buff_send[15] = OLED_CMD_DISPLAY_RAM;			 // A4
	buff_send[16] = OLED_CMD_SET_VCOMH_DESELCT;		 // DB
	buff_send[17] = 0x40;                            // 40
	buff_send[18] = OLED_CMD_SET_MEMORY_ADDR_MODE;   // 20
	//buff_send[19] = OLED_CMD_SET_HORI_ADDR_MODE;	 // 00
	buff_send[19] = OLED_CMD_SET_PAGE_ADDR_MODE;	 // 02
	// Set Lower Column Start Address for Page Addressing Mode
	buff_send[20] = 0x00;                            // 00
	// Set Higher Column Start Address for Page Addressing Mode
	buff_send[21] = 0x10;                            // 10
	buff_send[22] = OLED_CMD_SET_CHARGE_PUMP;		 // 8D
	buff_send[23] = 0x14;                            // 14
	buff_send[24] = OLED_CMD_DEACTIVE_SCROLL;		 // 2E
	buff_send[25] = OLED_CMD_DISPLAY_NORMAL;		 // A6
	buff_send[26] = OLED_CMD_DISPLAY_ON;      		 // AF

	//Iniciar transaccion i2c
	ret = i2c_master_transmit(dev->dev_handle, buff_send, 27, 500); // timeout = 500ms

	if (ret == ESP_OK) {
		ESP_LOGI(TAG, "OLED configurado exitosamente");
	} else {
		ESP_LOGE(TAG, "OLED configuracion fallida. code: 0x%.2X", ret);
	}

	return ret;
}


esp_err_t ssd_i2c_display_image(SSD1306_t *dev, int page, int seg, uint8_t * images, int width) {

	//Variables locales
	esp_err_t ret;
	uint8_t buff_send[100];
	int _page = page;
	int _seg  = seg + CONFIG_OFFSETX;
	uint8_t columLow  = _seg & 0x0F;
	uint8_t columHigh = (_seg >> 4) & 0x0F;
	uint8_t count = 0;

	if (page >= dev->_pages) return ESP_FAIL;
	if (seg  >= dev->_width) return ESP_FAIL;
	if (dev->_flip) {
		_page = (dev->_pages - page) - 1;
	}

	//Set buffer
	buff_send[0] = OLED_CONTROL_BYTE_CMD_STREAM;
	// Set Lower Column Start Address for Page Addressing Mode
	buff_send[1] = (0x00 + columLow);
	// Set Higher Column Start Address for Page Addressing Mode
	buff_send[2] = (0x10 + columHigh);
	// Set Page Start Address for Page Addressing Mode
	buff_send[3] = (0xB0 | _page);

	//Iniciar transaccion i2c
	ret = i2c_master_transmit(dev->dev_handle, buff_send, 4, 100); // 100ms
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "OLED transmision fallida. code: 0x%.2X", ret);
		return ret;
	}

	buff_send[0] = OLED_CONTROL_BYTE_DATA_STREAM;
	count = 1;
	for(uint8_t i = 0; i < width; i++) {
		if(sizeof(buff_send) < i) {
			ret = ESP_FAIL;
			break;
		}
		buff_send[i+1] = images[i];
		count++;
	}

	//Iniciar transaccion i2c
	ret = i2c_master_transmit(dev->dev_handle, buff_send, count, 500); // 500ms
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "OLED transmision fallida. code: 0x%.2X", ret);
	}

	return ret;
}

esp_err_t ssd_i2c_contrast(SSD1306_t * dev, int contrast) {
	//Variables locales
	esp_err_t ret;
	uint8_t buff_send[5];
	int _contrast = contrast;

	if (contrast < 0x0) _contrast = 0;
	if (contrast > 0xFF) _contrast = 0xFF;

	//Set buff
	buff_send[0] = OLED_CONTROL_BYTE_CMD_STREAM;
	buff_send[1] = OLED_CMD_SET_CONTRAST;
	buff_send[2] = (uint8_t)_contrast;

	//Iniciar transaccion i2c
	ret = i2c_master_transmit(dev->dev_handle, buff_send, 3, 100); // 100ms
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "OLED transmision fallida. code: 0x%.2X", ret);
	}

	return ret;
}


esp_err_t ssd_i2c_hardware_scroll(SSD1306_t *dev, ssd1306_scroll_type_t scroll) {

	//Variables locales
	esp_err_t ret = ESP_FAIL;
	uint8_t buff_send[20];

	//Set buffer
	buff_send[0] = OLED_CONTROL_BYTE_CMD_STREAM;

	if (scroll == SCROLL_RIGHT) {
		buff_send[1] = OLED_CMD_HORIZONTAL_RIGHT;	// 26
		buff_send[2] = 0x00; // Dummy byte
		buff_send[3] = 0x00; // Define start page address
		buff_send[4] = 0x07; // Frame frequency
		buff_send[5] = 0x07; // Define end page address
		buff_send[6] = 0x00; //
		buff_send[7] = 0xFF; //
		buff_send[8] = OLED_CMD_ACTIVE_SCROLL;		// 2F

		//Iniciar transaccion i2c
		ret = i2c_master_transmit(dev->dev_handle, buff_send, 9, 100); // 100ms
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "OLED Scroll fallida. code: 0x%.2X", ret);
		}
		else {
			ESP_LOGD(TAG, "OLED Scroll existoso");
		}
	}
	else if (scroll == SCROLL_LEFT) {
		buff_send[1] = OLED_CMD_HORIZONTAL_LEFT;    // 27
		buff_send[2] = 0x00; // Dummy byte
		buff_send[3] = 0x00; // Define start page address
		buff_send[4] = 0x07; // Frame frequency
		buff_send[5] = 0x07; // Define end page address
		buff_send[6] = 0x00; //
		buff_send[7] = 0xFF; //
		buff_send[8] = OLED_CMD_ACTIVE_SCROLL;		 // 2F

		//Iniciar transaccion i2c
		ret = i2c_master_transmit(dev->dev_handle, buff_send, 9, 100); // 100ms
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "OLED Scroll fallida. code: 0x%.2X", ret);
		}
		else {
			ESP_LOGD(TAG, "OLED Scroll existoso");
		}
	}
	else if (scroll == SCROLL_DOWN) {
		buff_send[1] = OLED_CMD_CONTINUOUS_SCROLL;	// 29
		buff_send[2] = 0x00; // Dummy byte
		buff_send[3] = 0x00; // Define start page address
		buff_send[4] = 0x07; // Frame frequency
		//buff_send[5] = 0x01; // Define end page address
		buff_send[5] = 0x00; // Define end page address
		buff_send[6] = 0x3F; // Vertical scrolling offset

		buff_send[7] = OLED_CMD_VERTICAL;			// A3
		buff_send[8] = 0x00;
		if (dev->_height == 64)
		{
			//buff_send[9] = 0x7F;
			buff_send[9] = 0x40;
		}
		
		if (dev->_height == 32)
		{
			buff_send[10] = 0x20;
			buff_send[11] = OLED_CMD_ACTIVE_SCROLL;		// 2F	
		}

		//Iniciar transaccion i2c
		ret = i2c_master_transmit(dev->dev_handle, buff_send, 12, 100); // 100ms
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "OLED Scroll fallida. code: 0x%.2X", ret);
		}
		else {
			ESP_LOGD(TAG, "OLED Scroll existoso");
		}
	}
	else if (scroll == SCROLL_UP) {
		buff_send[1] = OLED_CMD_CONTINUOUS_SCROLL;	// 29
		buff_send[2] = 0x00; // Dummy byte
		buff_send[3] = 0x00; // Define start page address
		buff_send[4] = 0x07; // Frame frequency
		//buff_send[5] = 0x01, true); // Define end page address
		buff_send[5] = 0x00; // Define end page address
		buff_send[6] = 0x01; // Vertical scrolling offset

		buff_send[7] = OLED_CMD_VERTICAL;			// A3
		buff_send[8] = 0x00;
		if (dev->_height == 64)
		{
			//buff_send[9] = 0x7F, true);
			buff_send[9] = 0x40;
		}
		if (dev->_height == 32)
		{
			buff_send[10] = 0x20;
			buff_send[11] = OLED_CMD_ACTIVE_SCROLL;		// 2F
		}
		

		//Iniciar transaccion i2c
		ret = i2c_master_transmit(dev->dev_handle, buff_send, 12, 100); // 100ms
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "OLED Scroll fallida. code: 0x%.2X", ret);
		}
		else {
			ESP_LOGD(TAG, "OLED Scroll existoso");
		}
	}
	else if (scroll == SCROLL_STOP) {
		buff_send[1] = OLED_CMD_DEACTIVE_SCROLL;	// 2E

		//Iniciar transaccion i2c
		ret = i2c_master_transmit(dev->dev_handle, buff_send, 2, 10); // 10ms
		if (ret != ESP_OK) {
			ESP_LOGE(TAG, "OLED Scroll fallida. code: 0x%.2X", ret);
		}
		else {
			ESP_LOGD(TAG, "OLED Scroll existoso");
		}
	}

	return ret;
}

