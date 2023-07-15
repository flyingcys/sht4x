# SHT4x 软件包

中文页 | [English](README.md)

## 1 介绍

SHT4x 软件包提供了使用温度与湿度传感器 `sht4x` 基本功能，并且提供了软件平均数滤波器可选功能 。本文介绍该软件包的基本功能，以及 `Finsh/MSH` 测试命令等。

基本功能主要由传感器 `sht4x` 决定：在输入电压为 `1.8v-3.3v` 范围内，测量温度与湿度的量程、精度如下表所示 

| 功能 | 量程          | 精度    |
| ---- | ------------- | ------- |
| 温度 | `-40℃ - 125℃` | `±0.3℃` |
| 湿度 | `0% - 100%`   | `±3%`   |

### 1.1 目录结构

| 名称 | 说明 |
| ---- | ---- |
| sht4x.h | 传感器使用头文件 |
| sht4x.c | 传感器使用源代码 |
| SConscript | RT-Thread 默认的构建脚本 |
| README.md | 软件包使用说明 |
| sht4x_datasheet.pdf| 官方数据手册 |

### 1.2 许可证

SHT4x 软件包遵循  Apache-2.0 许可，详见 LICENSE 文件。

### 1.3 依赖

依赖 `RT-Thread I2C` 设备驱动框架。

## 2 获取软件包

使用 `sht4x` 软件包需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```
RT-Thread online packages
    peripheral libraries and drivers  --->
        [*] sht4x: digital humidity and temperature sensor sht4x driver library  --->                                          
            Version (latest)  --->
```


每个功能的配置说明如下：

- `sht4x: digital humidity and temperature sensor sht4x driver library`：选择使用 `sht4x` 软件包；
- `Version`：配置软件包版本，默认最新版本。

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。

## 3 使用 sht4x 软件包

按照前文介绍，获取 `sht4x` 软件包后，就可以按照 下文提供的 API 使用传感器 `sht4x` 与 `Finsh/MSH` 命令进行测试，详细内容如下。

### 3.1 API

#### 3.1.1  初始化 

```c
sht4x_device_t sht4x_init(const char *i2c_bus_name)
```

根据总线名称，自动初始化对应的 sht4x 设备，具体参数与返回说明如下表

| 参数    | 描述                      |
| :----- | :----------------------- |
| name   | i2c 设备名称 |
| **返回** | **描述** |
| != NULL | 将返回 sht4x 设备对象 |
| = NULL | 查找失败 |

#### 3.1.2  反初始化

```c
void sht4x_deinit(sht4x_device_t dev)
```

如果设备不再使用，反初始化将回收 sht4x 设备的相关资源，具体参数说明如下表

| 参数 | 描述           |
| :--- | :------------- |
| dev  | sht4x 设备对象 |

#### 3.1.3  读取序列号

```c
rt_uint32_t sht4x_read_serial(sht4x_device_t dev);
```

读取 `sht4x` 传感器序列号，返回序列号，具体参数与返回说明如下表

| 参数 | 描述           |
| :--- | :------------- |
| dev  | sht4x 设备对象 |
| **返回** | **描述**       |
| != 0   | sht4序列号     |
| =0.0     | 读取序列号失败       |


#### 3.1.4 读取温度

```c
float sht4x_read_temperature(sht4x_device_t dev)
```

通过 `sht4x` 传感器读取温度测量值，返回浮点型温度值，具体参数与返回说明如下表

| 参数     | 描述           |
| :------- | :------------- |
| dev      | sht4x 设备对象 |
| **返回** | **描述**       |
| != 0.0   | 测量温度值     |
| =0.0     | 测量失败       |

#### 3.1.5 读取湿度

```c
float sht4x_read_humidity(sht4x_device_t dev)
```

通过 `sht4x` 传感器读取湿度测量值，返回浮点型湿度值，具体参数与返回说明如下表

| 参数     | 描述            |
| :------- | :-------------- |
| dev      | `sht4x`设备对象 |
| **返回** | **描述**        |
| != 0.0   | 测量湿度值      |
| =0.0     | 测量失败        |


### 3.2 Finsh/MSH 测试命令

sht4x 软件包提供了丰富的测试命令，项目只要在 RT-Thread 上开启 Finsh/MSH 功能即可。在做一些基于 `sht4x` 的应用开发、调试时，这些命令会非常实用，它可以准确的读取指传感器测量的温度与湿度。具体功能可以输入 `sht4x` ，可以查看完整的命令列表

```shell
msh />sht4x
Usage:
sht4x probe <dev_name>   - probe sensor by given name
sht4x read               - read sensor sht4x data
msh />
```

#### 3.2.1 在指定的 i2c 总线上探测传感器 

当第一次使用 `sht4x` 命令时，直接输入 `sht4x probe <dev_name>` ，其中 `<dev_name>` 为指定的 i2c 总线，例如：`i2c0`。如果有这个传感器，就不会提示错误；如果总线上没有这个传感器，将会显示提示找不到相关设备，日志如下：

```
msh />sht4x probe i2c1      #探测成功，没有错误日志
msh />
msh />sht4x probe i2c88     #探测失败，提示对应的 I2C 设备找不到
[E/sht4x] can't find sht4x device on 'i2c88'
msh />
```

#### 3.2.2 读取数据

探测成功之后，输入 `sht4x read` 即可获取温度与湿度，包括提示信息，日志如下： 

```
msh />sht4x read
read sht4x sensor humidity   : 54.7 %
read sht4x sensor temperature: 27.3 ℃
msh />
```

## 4 注意事项

暂无。

## 5 联系方式

* 维护：[flyingcys](https://github.com/flyingcys)
* 主页：https://github.com/RT-Thread-packages/peripherals/sensors/sht4x

