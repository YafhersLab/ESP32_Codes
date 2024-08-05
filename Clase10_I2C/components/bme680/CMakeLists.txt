idf_component_register(SRCS "bme680.c" "bme680_i2c.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver freertos)

