
/* Autores:
 * Copyright (c) 2017 Gunar Schorcht <https://github.com/gschorcht>
 * Copyright (c) 2019 Ruslan V. Uss <unclerus@gmail.com>
 *
 * Actualizaciones:
 * Ivan Vargas A. ( INTECX S.A.C. => https://intecx.com.pe )
 */

//ANSI C
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

//ESP-IDF
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_err.h"

//********************************************
#define BME680_DEV_ADDR_0    0x76
#define BME680_DEV_ADDR_1    0x77
#define BME680_DEV_FREQ_HZ   400000  // Up to 3.4MHz, but esp-idf only supports 400Khz

//********************************************
#define BME680_MAX_OVERFLOW_VAL      INT32_C(0x40000000) // overflow value used in pressure calculation (bme680_convert_pressure)

#define BME680_HEATER_TEMP_MIN         200  //!< min. 200 degree Celsius
#define BME680_HEATER_TEMP_MAX         400  //!< max. 200 degree Celsius
#define BME680_HEATER_PROFILES         10   //!< max. 10 heater profiles 0 ... 9
#define BME680_HEATER_NOT_USED         -1   //!< heater not used profile

/**
 * Fixed point sensor values (fixed THPG values)
 */
typedef struct
{
    int16_t temperature;     //!< temperature in degree C * 100 (Invalid value INT16_MIN)
    uint32_t pressure;       //!< barometric pressure in Pascal (Invalid value 0)
    uint32_t humidity;       //!< relative humidity in % * 1000 (Invalid value 0)
    uint32_t gas_resistance; //!< gas resistance in Ohm         (Invalid value 0)
} bme680_values_fixed_t;

/**
 * Floating point sensor values (real THPG values)
 */
typedef struct
{
    float temperature;    //!< temperature in degree C        (Invalid value -327.68)
    float pressure;       //!< barometric pressure in hPascal (Invalid value 0.0)
    float humidity;       //!< relative humidity in %         (Invalid value 0.0)
    float gas_resistance; //!< gas resistance in Ohm          (Invalid value 0.0)
} BME680_values_float_t;

/**
 * Filter size
 */
typedef enum {
    BME680_IIR_SIZE_0 = 0, //!< Filter is not used
    BME680_IIR_SIZE_1,
    BME680_IIR_SIZE_3,
    BME680_IIR_SIZE_7,
    BME680_IIR_SIZE_15,
    BME680_IIR_SIZE_31,
    BME680_IIR_SIZE_63,
    BME680_IIR_SIZE_127
} bme680_filter_size_t;

/**
 * Oversampling rate
 */
typedef enum {
    BME680_OSR_NONE = 0, //!< Measurement is skipped, output values are invalid
    BME680_OSR_1X,       //!< Default oversampling rates
    BME680_OSR_2X,
    BME680_OSR_4X,
    BME680_OSR_8X,
    BME680_OSR_16X
} bme680_oversampling_rate_t;

/**
 * @brief Sensor parameters that configure the TPHG measurement cycle
 *
 *  T - temperature measurement
 *  P - pressure measurement
 *  H - humidity measurement
 *  G - gas measurement
 */
typedef struct
{
    bme680_oversampling_rate_t osr_temperature; //!< T oversampling rate (default `BME680_OSR_1X`)
    bme680_oversampling_rate_t osr_pressure;    //!< P oversampling rate (default `BME680_OSR_1X`)
    bme680_oversampling_rate_t osr_humidity;    //!< H oversampling rate (default `BME680_OSR_1X`)
    bme680_filter_size_t filter_size;           //!< IIR filter size (default `BME680_IIR_SIZE_3`)

    int8_t heater_profile;                      //!< Heater profile used (default 0)
    uint16_t heater_temperature[10];            //!< Heater temperature for G (default 320)
    uint16_t heater_duration[10];               //!< Heater duration for G (default 150)

    int8_t ambient_temperature;                 //!< Ambient temperature for G (default 25)
} bme680_settings_t;

/**
 * @brief   Data structure for calibration parameters
 *
 * These calibration parameters are used in compensation algorithms to convert
 * raw sensor data to measurement results.
 */
typedef struct
{
    uint16_t par_t1;         //!< calibration data for temperature compensation
    int16_t  par_t2;
    int8_t   par_t3;

    uint16_t par_p1;         //!< calibration data for pressure compensation
    int16_t  par_p2;
    int8_t   par_p3;
    int16_t  par_p4;
    int16_t  par_p5;
    int8_t   par_p7;
    int8_t   par_p6;
    int16_t  par_p8;
    int16_t  par_p9;
    uint8_t  par_p10;

    uint16_t par_h1;         //!< calibration data for humidity compensation
    uint16_t par_h2;
    int8_t   par_h3;
    int8_t   par_h4;
    int8_t   par_h5;
    uint8_t  par_h6;
    int8_t   par_h7;

    int8_t   par_gh1;        //!< calibration data for gas compensation
    int16_t  par_gh2;
    int8_t   par_gh3;

    int32_t  t_fine;         //!< temperature correction factor for P and G
    uint8_t  res_heat_range;
    int8_t   res_heat_val;
    int8_t   range_sw_err;
} bme680_calib_data_t;

/**
 * BME680 sensor device data structure type
 */
typedef struct
{
	i2c_master_dev_handle_t dev_handle; //!< Device handle
    bool meas_started;                  //!< Indicates whether measurement started
    uint8_t meas_status;                //!< Last sensor status (for internal use only)
    bme680_settings_t settings;     	//!< Sensor settings
    bme680_calib_data_t calib_data; 	//!< Calibration data of the sensor
} BME680_t;

/**
 * @brief Initialize device descriptor
 *
 * @param master bus
 * @param dev Device descriptor
 * @param addr BME680 address
 * @return `ESP_OK` on success
 */
esp_err_t bme680_init(i2c_master_bus_handle_t master_bus, BME680_t *dev, uint8_t addr);

/**
 * @brief   Initialize a BME680 sensor
 *
 * The function initializes the sensor device data structure, probes the
 * sensor, soft resets the sensor, and configures the sensor with the
 * the following default settings:
 *
 * - Oversampling rate for temperature, pressure, humidity is osr_1x
 * - Filter size for pressure and temperature is iir_size 3
 * - Heater profile 0 with 320 degree C and 150 ms duration
 *
 * The sensor must be connected to an I2C bus.
 *
 * @param dev Device descriptor
 * @return `ESP_OK` on success
 */
esp_err_t bme680_init_sensor(BME680_t *dev);

/**
 * @brief   Force one single TPHG measurement
 *
 * The function triggers the sensor to start one THPG measurement cycle.
 * Parameters for the measurement like oversampling rates, IIR filter sizes
 * and heater profile can be configured before.
 *
 * Once the TPHG measurement is started, the user task has to wait for the
 * results. The duration of the TPHG measurement can be determined with
 * function *bme680_get_measurement_duration*.
 *
 * @param dev Device descriptor
 * @return `ESP_OK` on success
 */
esp_err_t bme680_force_measurement(BME680_t *dev);

/**
 * @brief   Get estimated duration of a TPHG measurement
 *
 * The function returns an estimated duration of the TPHG measurement cycle
 * in RTOS ticks for the current configuration of the sensor.
 *
 * This duration is the time required by the sensor for one TPHG measurement
 * until the results are available. It strongly depends on which measurements
 * are performed in the THPG measurement cycle and what configuration
 * parameters were set. It can vary from 1 RTOS (10 ms) tick up to 4500 RTOS
 * ticks (4.5 seconds).
 *
 * If the measurement configuration is not changed, the duration can be
 * considered as constant.
 *
 * @param dev Device descriptor
 * @param[out] duration Duration of TPHG measurement cycle in ticks or 0 on error
 * @return `ESP_OK` on success
 */
esp_err_t bme680_get_measurement_duration(const BME680_t *dev, uint32_t *duration);

/**
 * @brief   Get the measurement status
 *
 * The function can be used to test whether a measurement that was started
 * before is still running.
 *
 * @param dev Device descriptor
 * @param[out] busy true if measurement is still running or false otherwise
 * @return `ESP_OK` on success
 */
esp_err_t bme680_is_measuring(BME680_t *dev, bool *busy);

/**
 * @brief   Get results of a measurement in fixed point representation
 *
 * The function returns the results of a TPHG measurement that has been
 * started before. If the measurement is still running, the function fails
 * and returns invalid values (see type declaration).
 *
 * @param dev Device descriptor
 * @param[out] results pointer to a data structure that is filled with results
 * @return `ESP_OK` on success
 */
esp_err_t bme680_get_results_fixed(BME680_t *dev, bme680_values_fixed_t *results);

/**
 * @brief   Get results of a measurement in floating point representation
 *
 * The function returns the results of a TPHG measurement that has been
 * started before. If the measurement is still running, the function fails
 * and returns invalid values (see type declaration).
 *
 * @param dev Device descriptor
 * @param[out] results pointer to a data structure that is filled with results
 * @return `ESP_OK` on success
 */
esp_err_t bme680_get_results_float(BME680_t *dev, BME680_values_float_t *results);

/**
 * @brief   Start a measurement, wait and return the results (fixed point)
 *
 * This function is a combination of functions above. For convenience it
 * starts a TPHG measurement using ::bme680_force_measurement(), then it waits
 * the measurement duration for the results using `vTaskDelay()` and finally it
 * returns the results using function ::bme680_get_results_fixed().
 *
 * Note: Since the calling task is delayed using function `vTaskDelay()`, this
 * function must not be used when it is called from a software timer callback
 * function.
 *
 * @param dev Device descriptor
 * @param[out] results pointer to a data structure that is filled with results
 * @return `ESP_OK` on success
 */
esp_err_t bme680_measure_fixed(BME680_t *dev, bme680_values_fixed_t *results);

/**
 * @brief   Start a measurement, wait and return the results (floating point)
 *
 * This function is a combination of functions above. For convenience it
 * starts a TPHG measurement using ::bme680_force_measurement(), then it waits
 * the measurement duration for the results using `vTaskDelay` and finally it
 * returns the results using function ::bme680_get_results_float().
 *
 * Note: Since the calling task is delayed using function `vTaskDelay()`, this
 * function must not be used when it is called from a software timer callback
 * function.
 *
 * @param dev Device descriptor
 * @param[out] results pointer to a data structure that is filled with results
 * @return `ESP_OK` on success
 */
esp_err_t bme680_measure_float(BME680_t *dev, BME680_values_float_t *results);

/**
 * @brief   Set the oversampling rates for measurements
 *
 * The BME680 sensor allows to define individual oversampling rates for
 * the measurements of temperature, pressure and humidity. Using an
 * oversampling rate of *osr*, the resolution of raw sensor data can be
 * increased by ld(*osr*) bits.
 *
 * Possible oversampling rates are 1x (default), 2x, 4x, 8x, 16x, see type
 * ::bme680_oversampling_rate_t. The default oversampling rate is 1.
 *
 * Please note: Use ::BME680_OSR_NONE to skip the corresponding measurement.
 *
 * @param dev Device descriptor
 * @param osr_t oversampling rate for temperature measurements
 * @param osr_p oversampling rate for pressure measurements
 * @param osr_h oversampling rate for humidity measurements
 * @return `ESP_OK` on success
 */
esp_err_t bme680_set_oversampling_rates(BME680_t *dev, bme680_oversampling_rate_t osr_t,
                                        bme680_oversampling_rate_t osr_p, bme680_oversampling_rate_t osr_h);

/**
 * @brief   Set the size of the IIR filter
 *
 * The sensor integrates an internal IIR filter (low pass filter) to reduce
 * short-term changes in sensor output values caused by external disturbances.
 * It effectively reduces the bandwidth of the sensor output values.
 *
 * The filter can optionally be used for pressure and temperature data that
 * are subject to many short-term changes. Using the IIR filter, increases the
 * resolution of pressure and temperature data to 20 bit. Humidity and gas
 * inside the sensor does not fluctuate rapidly and does not require such a
 * low pass filtering.
 *
 * The default filter size is 3 (::BME680_IIR_SIZE_3).
 *
 * Please note: If the size of the filter is 0, the filter is not used.
 *
 * @param dev Device descriptor
 * @param size IIR filter size
 * @return `ESP_OK` on success
 */
esp_err_t bme680_set_filter_size(BME680_t *dev, bme680_filter_size_t size);

/**
 * @brief   Set a heater profile for gas measurements
 *
 * The sensor integrates a heater for the gas measurement. Parameters for this
 * heater are defined by so called heater profiles. The sensor supports up to
 * 10 heater profiles, which are numbered from 0 to 9. Each profile consists of
 * a temperature set-point (the target temperature) and a heating duration.
 *
 * This function sets the parameters for one of the heater profiles 0 ... 9.
 * To activate the gas measurement with this profile, use function
 * ::bme680_use_heater_profile(), see below.
 *
 * Please note: According to the data sheet, a target temperatures of between
 * 200 and 400 degrees Celsius are typical and about 20 to 30 ms are necessary
 * for the heater to reach the desired target temperature.
 *
 * @param dev Device descriptor
 * @param profile heater profile 0 ... 9
 * @param temperature target temperature in degree Celsius
 * @param duration heating duration in milliseconds
 * @return `ESP_OK` on success
 */
esp_err_t bme680_set_heater_profile(BME680_t *dev, uint8_t profile, uint16_t temperature, uint16_t duration);

/**
 * @brief   Activate gas measurement with a given heater profile
 *
 * The function activates the gas measurement with one of the heater
 * profiles 0 ... 9 or deactivates the gas measurement completely when
 * -1 or ::BME680_HEATER_NOT_USED is used as heater profile.
 *
 * Parameters of the activated heater profile have to be set before with
 * function ::bme680_set_heater_profile() otherwise the function fails.
 *
 * If several heater profiles have been defined with function
 * ::bme680_set_heater_profile(), a sequence of gas measurements with different
 * heater parameters can be realized by a sequence of activations of different
 * heater profiles for successive TPHG measurements using this function.
 *
 * @param dev Device descriptor
 * @param profile 0 ... 9 to activate or -1 to deactivate gas measure
 * @return `ESP_OK` on success
 */
esp_err_t bme680_use_heater_profile(BME680_t *dev, int8_t profile);

/**
 * @brief   Set ambient temperature
 *
 * The heater resistance calculation algorithm takes into account the ambient
 * temperature of the sensor. This function can be used to set this ambient
 * temperature. Either values determined from the sensor itself or from
 * another temperature sensor can be used. The default ambient temperature
 * is 25 degree Celsius.
 *
 * @param dev Device descriptor
 * @param temperature ambient temperature in degree Celsius
 * @return `ESP_OK` on success
 */
esp_err_t bme680_set_ambient_temperature(BME680_t *dev, int16_t temperature);


//***************************************************************************************
//*****************************Funciones I2C*********************************************

esp_err_t bme_i2c_init(i2c_master_bus_handle_t master_bus, BME680_t *dev, uint8_t addr);
esp_err_t bme_i2c_dev_read_reg(const BME680_t *dev, uint8_t reg, uint8_t *in_data, size_t in_size);
esp_err_t bme_i2c_dev_write_reg(const BME680_t *dev, uint8_t reg, const uint8_t *out_data, size_t out_size);







