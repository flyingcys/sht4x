/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-07-15     flyingcys    first version
 */
#ifndef __SHT4X_H__
#define __SHT4X_H__

struct sht4x_device
{
    struct rt_i2c_bus_device *i2c_bus;
    rt_mutex_t lock;
};
typedef struct sht4x_device *sht4x_device_t;

/**
 * This function reads relative humidity by sht4x sensor measurement
 *
 * @param dev the pointer of device driver structure
 *
 * @return the relative humidity converted to float data.
 */
float sht4x_read_humidity(sht4x_device_t dev);

/**
 * This function reads temperature by sht4x sensor measurement
 *
 * @param dev the pointer of device driver structure
 *
 * @return the relative temperature converted to float data.
 */
float sht4x_read_temperature(sht4x_device_t dev);

/**
 * This function reads sht4x serial
 *
 * @param dev the pointer of device driver structure
 *
 * @return sht4x serial number.
 */
rt_uint32_t sht4x_read_serial(sht4x_device_t dev);

/**
 * This function releases memory and deletes mutex lock
 *
 * @param dev the pointer of device driver structure
 */
void sht4x_deinit(sht4x_device_t dev);

/**
 * This function initializes sht4x registered device driver
 *
 * @param dev the name of sht4x device
 *
 * @return the sht4x device.
 */
sht4x_device_t sht4x_init(const char *i2c_bus_name);

#endif  /* __SHT4X_H__ */