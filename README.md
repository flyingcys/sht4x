# SHT4x package

[中文页](README_zh.md) | English

## 1 Introduction

The SHT4x software package provides basic functionality for using the temperature and humidity sensor `sht4x`, as well as an optional software averaging filter. This article introduces the basic features of the software package, as well as testing commands using `Finsh/MSH`.

The basic functionality is mainly determined by the `sht4x` sensor, which measures temperature and humidity within the input voltage range of `1.8v-3.3v`. The measurement range and accuracy are shown in the table below:

| Feature | Range           | Accuracy |
| ------- | --------------- | -------- |
| Temperature | `-40℃ - 125℃` | `±0.3℃` |
| Humidity    | `0% - 100%`    | `±3%`   |

### 1.1 Directory Structure

| Name       | Description                              |
| ---------- | ---------------------------------------- |
| sht4x.h    | Header file for using the sensor          |
| sht4x.c    | Source code for using the sensor          |
| SConscript | Default build script for RT-Thread        |
| README.md  | Package usage instructions               |
| sht4x_datasheet.pdf | Official data sheet                  |

### 1.2 License

The SHT4x software package is licensed under Apache-2.0. See the LICENSE file for details.

### 1.3 Dependencies

Depends on the RT-Thread I2C device driver framework.

## 2 Getting the Software Package

To use the `sht4x` software package, select it in the RT-Thread package manager. The specific path is:

```
RT-Thread online packages
    peripheral libraries and drivers  --->
        [*] sht4x: digital humidity and temperature sensor sht4x driver library  --->                                            
            Version (latest)  --->
```

The configuration instructions for each feature are as follows:
- `sht4x: digital humidity and temperature sensor sht4x driver library`: Select to use the `sht4x` software package.
- `Version`: Configure the software package version. The default is the latest version.

Then allow the RT-Thread package manager to update automatically, or use the `pkgs --update` command to update the package to the BSP.

## 3 Using the sht4x Software Package

After obtaining the `sht4x` software package as described above, you can use the sensor `sht4x` and test it using the API and `Finsh/MSH` commands provided in the following sections.

### 3.1 API

#### 3.1.1 Initialization

```c
sht4x_device_t sht4x_init(const char *i2c_bus_name)
```
Automatically initialize the corresponding `sht4x` device based on the bus name. The specific parameters and return information are described in the following table:

| Parameter | Description                 |
| --------- | --------------------------- |
| i2c_bus_name | Name of the I2C device |
| **Returns** | **Description** |
| != NULL   | Returns the sht4x device object |
| = NULL    | The search fails |

#### 3.1.2 Deinitialization

```c
void sht4x_deinit(sht4x_device_t dev)
```
If the device is no longer in use, deinitialization will reclaim the relevant resources of the sht4x device. The specific parameter is described in the table below:

| Parameter | Description          |
| --------- | -------------------- |
| dev       | sht4x device object  |

#### 3.1.3 Read Serial Number

```c
rt_uint32_t sht4x_read_serial(sht4x_device_t dev);
```

Reads the serial number of the `sht4x` sensor and returns the serial number as an `rt_uint32_t`. The specific parameters and return information are described in the following table:

| Parameter | Description          |
| --------- | -------------------- |
| dev       | sht4x device object  |
| **Returns** | **Description**      |
| != 0      | SHT4 serial number   |
| = 0       | Failed to read       |

#### 3.1.4 Read Temperature

```c
float sht4x_read_temperature(sht4x_device_t dev)
```

Reads the temperature measurement value from the `sht4x` sensor and returns the temperature value as a float. The specific parameters and return information are described in the following table:

| Parameter | Description              |
| --------- | ------------------------ |
| dev       | sht4x device object      |
| **Returns** | **Description**          |
| != 0.0    | Measured temperature value |
| = 0.0     | Measurement failed        |

#### 3.1.5 Read Humidity

```c
float sht4x_read_humidity(sht4x_device_t dev)
```

Reads the humidity measurement value from the `sht4x` sensor and returns the humidity value as a float. The specific parameters and return information are described in the following table:

| Parameter | Description              |
| --------- | ------------------------ |
| dev       | sht4x device object      |
| **Returns** | **Description**          |
| != 0.0    | Measured humidity value   |
| = 0.0     | Measurement failed        |


### 3.2 Finsh/MSH Testing Commands

The sht4x software package provides a variety of testing commands, which can be used by enabling the Finsh/MSH feature in RT-Thread. These commands are very useful for applications development and debugging based on `sht4x` as they accurately read the temperature and humidity measured by the sensor. The complete list of commands can be viewed by entering `sht4x`.

```shell
msh />sht4x
Usage:
sht4x probe <dev_name>   - probe sensor by given name
sht4x read               - read sensor sht4x data
msh />
```

#### 3.2.1 Detecting the Sensor on a Specified I2C Bus

When using the `sht4x` command for the first time, simply enter `sht4x probe <dev_name>`, where `<dev_name>` is the specified I2C bus, such as `i2c0`. If the sensor is present, no error message will be displayed. If the sensor is not found on the bus, a prompt will be displayed indicating that the corresponding I2C device could not be found. The following is an example log:

```
msh />sht4x probe i2c1   # Successful detection, no error message.
msh />
msh />sht4x probe i2c88  # Detection failed, prompt for not finding the corresponding I2C device.
[E/sht4x] can't find sht4x device on 'i2c88'
msh />
```

#### 3.2.2 Reading Data

After successful detection, entering `sht4x read` can obtain the temperature and humidity, including the prompt information. The following is an example log:

```
msh />sht4x read
read sht4x sensor humidity   : 54.7 %
read sht4x sensor temperature: 27.3 ℃
msh />
```

## 4 Notes

None.

## 5 Contact Information

- Maintainer: [flyingcys](https://github.com/flyingcys)
- Homepage: https://github.com/RT-Thread-packages/peripherals/sensors/sht4x