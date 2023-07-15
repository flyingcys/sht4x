/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-07-15     flyingcys    first version
 */
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include <string.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME "sht4x"
#define DBG_LEVEL DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#include "sht4x.h"

#ifdef PKG_USING_SHT4X

#define SHT4X_ADDR                  0x44
#define SHT4X_GET_DATA              0xFD
#define SHT4X_CMD_READ_SERIAL       0x89

#define CRC8_POLYNOMIAL             0x31
#define CRC8_INIT                   0xFF

static rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t *data)
{
    rt_uint8_t buf[3];
    struct rt_i2c_msg msgs;
    rt_uint32_t buf_size = 1;

    buf[0] = reg;
    if (data != RT_NULL)
    {
        buf[1] = data[0];
        buf[2] = data[1];
        buf_size += 2;
    }

    msgs.addr = SHT4X_ADDR;
    msgs.flags = RT_I2C_WR;
    msgs.buf = buf;
    msgs.len = buf_size;

    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }

    return -RT_ERROR;
}

static rt_err_t read_regs(struct rt_i2c_bus_device *bus, rt_uint8_t len, rt_uint8_t *buf)
{
    struct rt_i2c_msg msgs;

    msgs.addr = SHT4X_ADDR;
    msgs.flags = RT_I2C_RD;
    msgs.buf = buf;
    msgs.len = len;

    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }

    return -RT_ERROR;
}

static rt_uint8_t check_crc(const rt_uint8_t *data, rt_uint16_t count) 
{
    rt_uint16_t i;
    rt_uint8_t crc = CRC8_INIT;
    rt_uint8_t crc_bit;

    for (i = 0; i < count; i ++)
    {
        crc ^= (data[i]);
        for (crc_bit = 0; crc_bit < 8; crc_bit ++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }
    
    return crc;
}

static rt_err_t sht4x_read_temp_humi(sht4x_device_t dev, rt_int32_t *temperature, rt_uint32_t *humidity)
{
    rt_err_t result = RT_EOK;
    rt_uint8_t buffer[6];
    rt_uint16_t words[2];

    result = write_reg(dev->i2c_bus, SHT4X_GET_DATA, RT_NULL);
    if (result != RT_EOK)
        return result;

    rt_thread_mdelay(40);
    result = read_regs(dev->i2c_bus, 6, buffer);
    if (result != RT_EOK)
        return result;

    if ((check_crc(&buffer[0], 2) != buffer[2]) || (check_crc(&buffer[3], 2) != buffer[5]))
    {
        LOG_E("sht4x check crc error");
        return -RT_ERROR;
    }
    
    /**
     * formulas for conversion of the sensor signals, optimized for fixed point
     * algebra:
     * Temperature = 175 * S_T / 65535 - 45
     * Relative Humidity = 125 * (S_RH / 65535) - 6
     */
    words[0] = buffer[0] * 256 + buffer[1];
    words[1] = buffer[3] * 256 + buffer[4];
    if (temperature)
    {
        *temperature = ((21875 * (rt_int32_t)words[0]) >> 13) - 45000;
    }

    if (humidity)
    {
        *humidity = ((15625 * (rt_uint32_t)words[1]) >> 13) - 6000;
    }

    return RT_EOK;
}

/**
 * This function reads relative humidity by sht4x sensor measurement
 *
 * @param dev the pointer of device driver structure
 *
 * @return the relative humidity converted to float data.
 */
float sht4x_read_humidity(sht4x_device_t dev)
{
    rt_err_t result = RT_EOK;
    rt_uint32_t humidity = 0;

    result = rt_mutex_take(dev->lock, RT_WAITING_FOREVER);
    if (result == RT_EOK)
    {
        result = sht4x_read_temp_humi(dev, RT_NULL, &humidity);
    }
    rt_mutex_release(dev->lock);
    
    return (float)humidity / 1000;
}

/**
 * This function reads temperature by sht20 sensor measurement
 *
 * @param dev the pointer of device driver structure
 *
 * @return the relative temperature converted to float data.
 */
float sht4x_read_temperature(sht4x_device_t dev)
{
    rt_err_t result = RT_EOK;
    rt_int32_t temperature = 0;

    result = rt_mutex_take(dev->lock, RT_WAITING_FOREVER);
    if (result == RT_EOK)
    {
        result = sht4x_read_temp_humi(dev, &temperature, RT_NULL);
    }
    rt_mutex_release(dev->lock);

    return (float)temperature / 1000;
}

rt_uint32_t sht4x_read_serial(sht4x_device_t dev)
{
    rt_err_t ret = RT_EOK;
    rt_uint32_t serial = 0;

    ret = write_reg(dev->i2c_bus, SHT4X_CMD_READ_SERIAL, RT_NULL);
    if (ret != RT_EOK)
        return 0;

    rt_thread_mdelay(10);

    ret = read_regs(dev->i2c_bus, sizeof(serial), (rt_uint8_t *)&serial);
    if (ret != RT_EOK)
        return 0;

    return serial;
}

/**
 * This function releases memory and deletes mutex lock
 *
 * @param dev the pointer of device driver structure
 */
void sht4x_deinit(sht4x_device_t dev)
{
    RT_ASSERT(dev);

    rt_mutex_delete(dev->lock);

    rt_free(dev);
}

/**
 * This function initializes sht4x registered device driver
 *
 * @param dev the name of sht4x device
 *
 * @return the sht4x device.
 */
sht4x_device_t sht4x_init(const char *i2c_bus_name)
{
    sht4x_device_t dev = RT_NULL;

    RT_ASSERT(i2c_bus_name);

    dev = rt_malloc(sizeof(struct sht4x_device));
    if (dev == RT_NULL)
    {
        LOG_E("malloc failed for sht4x device on %s", i2c_bus_name);

        return RT_NULL;
    }

    dev->i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(i2c_bus_name);
    if (dev->i2c_bus == RT_NULL)
    {
        LOG_E("find failed for sht4x devcie on %s", i2c_bus_name);

        rt_free(dev);
        return RT_NULL;
    }

    dev->lock = rt_mutex_create("mutex_sht4x", RT_IPC_FLAG_FIFO);
    if (dev->lock == RT_NULL)
    {
        LOG_E("create mutex filed for sht4x device on %s", i2c_bus_name);
        
        rt_free(dev);

        return RT_NULL;
    }

    return dev;
}

/**
 * This function initializes sht4x registered device driver
 *
 * @param dev the name of sht4x device
 *
 * @return the sht4x device.
 */
void sht4x(int argc, char *argv[])
{
    static sht4x_device_t dev = RT_NULL;
    rt_uint32_t serial = 0;
    float humidity, temperature;

    if (argc > 1)
    {
        if (strcmp(argv[1], "probe") == 0)
        {
            if (argc > 2)
            {
                if (!dev || strcmp(dev->i2c_bus->parent.parent.name, argv[2]))
                {
                    if (dev)
                    {
                        sht4x_deinit(dev);
                        dev = NULL;
                    }
                    
                    dev = sht4x_init(argv[2]);
                    if (dev == RT_NULL)
                    {
                        return;
                    }
                    
                    serial = sht4x_read_serial(dev);
                    if (serial != 0)
                    {
                        LOG_D("sht4x serial: 0x%x", serial);
                    }
                    else
                    {
                        LOG_E("sht4x read serial error");
                    }
                }
            }
            else
            {
                rt_kprintf("sht4x probe <dev_name>   - probe sensor by given name\n");
            }
        }
        else if (strcmp(argv[1], "read") == 0)
        {
            if (dev)
            {
                humidity = sht4x_read_humidity(dev);
                rt_kprintf("read sht4x sensor humidity   : %d.%d %%\n", (int)humidity, (int)(humidity * 10) % 10);
                
                temperature = sht4x_read_temperature(dev);
                rt_kprintf("read sht4x sensor temperature: %d.%d ℃\n", (int)temperature, (int)(temperature * 10) % 10);
            }
            else
            {
                rt_kprintf("Please using 'sht4x probe <dev_name>' first\n");
            }
        }
        else
        {
            rt_kprintf("Unknown command. Please enter 'sht4x' for help\n");
        }
    }
    else
    {
        rt_kprintf("Usage:\n");
        rt_kprintf("sht4x probe <dev_name>   - probe sensor by given name\n");
        rt_kprintf("sht4x read               - read sensor sht4x data\n");
    }
}
MSH_CMD_EXPORT(sht4x, sht4x sensor);

#endif /* PKG_USING_SHT4X */
