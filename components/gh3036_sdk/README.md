# GH3036 SDK 移植指南

## 1. SDK 结构介绍

GH3036 SDK 是汇顶科技(Goodix)提供的健康监测芯片软件开发套件，主要用于心率(HR)、血氧(SPO2)、心率变异性(HRV)等健康指标的监测。

### 目录结构

```
gh3036_sdk/
├── gh_algo/                    # 算法库
│   ├── adt/                   # 活动检测算法
│   ├── hr/                    # 心率算法
│   ├── hrv/                   # 心率变异性算法
│   ├── nadt/                  # 非活动检测算法
│   ├── spo2/                  # 血氧算法
│   └── common/                # 通用算法组件
├── gh_app/                    # 应用层
│   ├── inc/                   # 应用层头文件
│   │   ├── config/            # 配置文件目录
│   │   │   └── gh_global_config.h  # 全局配置文件
│   │   └── gh_app_user.h      # 应用用户接口
│   └── src/                   # 应用层源文件
│       ├── app_fusion/        # 数据融合
│       ├── app_manager/       # 应用管理
│       ├── app_mtss/          # 多任务状态管理
│       └── gh_app_user.c      # 应用用户实现
├── gh_hal/                    # 硬件抽象层
│   ├── hw_interface/          # 硬件接口
│   ├── hw_libs/               # 硬件库文件
│   └── hw_service/            # 硬件服务
│       ├── inc/
│       │   ├── config/        # HAL配置文件目录
│       │   │   └── gh_hal_config.h  # HAL配置文件
│       │   └── gh_hal_user.h  # HAL用户接口
│       └── src/
│           └──gh_hal_user.c   # HAL用户实现
└── gh_protocol/               # 通信协议
    ├── gh_protocol_user.h     # 协议用户接口
    └── gh_protocol_user.c     # 协议用户实现
└── port/                     # 移植示例目录
    ├── config/                # 配置文件示例
    │   ├── gh_global_config.h
    │   └── gh_hal_config.h
    ├── gh_app_user.c          # 应用层用户实现示例
    ├── gh_demo.c              # 示例代码
    ├── gh_demo.h
    ├── gh_hal_user.c          # HAL层用户实现示例
    ├── gh_protocol_user.c     # 协议层用户实现示例
    ├── gh_reg_lists.c         # 寄存器配置列表示例
    └── gh_reg_lists.h
```

### 重要提示：避免污染SDK

**强烈建议不要直接修改以下目录中的任何文件：**
- `gh_algo/` - 算法库目录
- `gh_app/` - 应用层目录
- `gh_hal/` - 硬件抽象层目录
- `gh_protocol/` - 通信协议目录

**正确做法：**
1. 将需要修改的文件从上述目录复制到您的项目工程中
2. 参考SDK中的`port/`目录，该目录包含了需要复制的文件示例：
   - `port/config/gh_global_config.h` - 全局配置文件
   - `port/config/gh_hal_config.h` - HAL配置文件
   - `port/gh_app_user.c` - 应用层用户实现
   - `port/gh_hal_user.c` - HAL层用户实现
   - `port/gh_protocol_user.c` - 协议层用户实现
   - `port/gh_reg_lists.c` - 寄存器配置列表
   - `port/gh_demo.c` - 示例代码

3. 在您的项目中修改这些复制的文件，而不是原始SDK文件
4. 在编译时确保包含这些复制的文件，而不是原始SDK文件

这样做可以：
- 保持SDK的原始状态，便于后续更新
- 避免意外修改关键算法或接口文件
- 使您的移植代码更加模块化和可维护

### 关键配置文件路径

1. **全局配置文件**: `port/config/gh_global_config.h` (从`gh_app/inc/config/gh_global_config.h`复制)
   - 芯片类型选择
   - 功能模块开关配置
   - 算法适配器配置

2. **HAL配置文件**: `port/config/gh_hal_config.h` (从`gh_hal/hw_service/inc/config/gh_hal_config.h`复制)
   - 通信接口选择(I2C/SPI)
   - 中断/轮询模式配置
   - FIFO配置
   - 日志配置

3. **算法配置文件**: 各算法目录下的配置文件（保持默认，不要修改）
   - `gh_algo/hr/basic/goodix_hba_config.c`
   - `gh_algo/spo2/basic/goodix_spo2_config_for_gh3036_a43992.c`
   - 其他算法配置文件

## 2. 关键移植函数

### HAL层移植函数

在 `gh_hal/hw_service/inc/gh_hal_user.h` 中定义的函数需要用户实现：

#### 仅HAL层调用流程图

![HAL层调用流程](img/hal层调用流程.jpg)

1. **SPI接口函数**:
   ```c
    /* 使用GPIO模拟CS时需要实现的函数 */
   uint32_t gh_hal_spi_init(void);                    // SPI初始化
   uint32_t gh_hal_spi_write(uint8_t* p_buffer, uint16_t len);  // SPI写数据
   uint32_t gh_hal_spi_read(uint8_t* p_buffer, uint16_t len);   // SPI读数据
   uint32_t gh_hal_spi_cs_ctrl(uint8_t level);        // SPI片选控制
```

   ```c
    /* 使用SPI硬件CS时需要实现的函数 */
   uint32_t gh_hal_spi_init(void);                    // SPI初始化
   uint32_t gh_hal_spi_write(uint8_t* p_buffer, uint16_t len);  // SPI写数据
   uint32_t gh_hal_spi_write_read(uint8_t* p_tx_buffer, uint8_t* p_rx_buffer, uint16_t len);  // SPI写读数据
   ```

2. **I2C接口函数**:
   ```c
   uint32_t gh_hal_i2c_init(void);                    // I2C初始化
   uint32_t gh_hal_i2c_write(uint8_t i2c_slaver_id, uint8_t* p_buffer, uint16_t len);  // I2C写数据
   uint32_t gh_hal_i2c_read(uint8_t i2c_slaver_id, uint8_t* p_buffer, uint16_t len);   // I2C读数据
   ```

3. **延时函数**:
   ```c
   uint32_t gh_hal_delay_us(uint16_t us);             // 微秒级延时
   uint32_t gh_hal_delay_ms(uint16_t ms);             // 毫秒级延时
   ```

4. **中断相关函数**:
   ```c
   uint32_t gh_hal_int_pin_init(void);               // 中断初始化
   ```

5. **时间戳函数**:
   ```c
   uint32_t gh_hal_get_timestamp(void);              // 获取时间戳
   ```

6. **复位相关函数**:
   ```c
   uint32_t gh_hal_reset_pin_init(void);              // 复位引脚初始化
   uint32_t gh_hal_reset_pin_ctrl(uint8_t level);    // 复位引脚控制
   ```

7. **LOG打印函数**:
   ```c
   int gh_hal_log_user(char *p_str);
   ```

### 应用层移植函数

在 `gh_app/inc/gh_app_user.h` 中定义的函数需要用户实现：

#### 应用层调用流程图

![应用层调用流程](img/app层调用流程.jpg)

1. **移动检测定时器**:
   ```c
   uint32_t gh_move_det_timer_start(void);              // 移动检测定时器启动
   uint32_t gh_move_det_timer_stop(void);              // 移动检测定时器停止
   ```


## 3. 移植流程

### 步骤1: 环境准备

1. 获取GH3036 SDK源代码
2. 准备目标平台开发环境
3. 确认目标平台与GH3036芯片的硬件连接方式(I2C或SPI)

### 步骤2: 配置修改

**可以将这两个config头文件复制到工程的单独目录进行管理，避免污染SDK**

1. **修改全局配置**:
   
   - 编辑 `port/config/gh_global_config.h` (从`gh_app/inc/config/gh_global_config.h`复制)
   - 根据需求启用/禁用相应功能模块
- 配置算法参数
  
2. **修改HAL配置**:
   - 编辑 `port/config/gh_hal_config.h` (从`gh_hal/hw_service/inc/config/gh_hal_config.h`复制)
   
   - 选择通信接口类型(I2C或SPI)
   
   - 配置中断/轮询模式
   
   - 配置FIFO参数

3. **添加寄存器配置数组**:
   - 参考文档生成符合自己硬件的配置数组：[Chelsea A 快速生成专属配置数组](https://alidocs.dingtalk.com/i/nodes/gpG2NdyVX32N52zwuq5wGkqqWMwvDqPk?utm_scene=team_space)
   
   - 将生成的寄存器数组按如下格式进行存储到新文件中，方便调用；如有多个寄存器数组，则需要创建多个不同名的变量，方便切换配置。
   
     ```c
     const gh_config_reg_t g_reg_map[] =
     {
         /* adt_hr_spo2_nadt_hrv */
         {0x0016,0x001f},// FASTEST_SAMPLE_RATE_DIVIDER:31,
         {0x0020,0x29d2},// FIFO_WATER_LINE:210, RG_FIFO_READ_INT_TIMER:0.4s,
         {0x0080,0x01b1},// RG_VCM_PULSEMODE:1,
     	/* .... */
         {0xffff,0x0001},// virtual reg,
     
     };
     
     const uint16_t g_reg_map_szie = sizeof(g_reg_map) / sizeof(g_reg_map[0]);
     ```
   

### 步骤3: HAL层移植

**对应的函数可以单独文件上进行实现，可以避免污染SDK**

**SDK内部需要实现的函数都是弱函数（weak）**

1. 实现`port/gh_hal_user.c`文件中的SPI/I2C接口函数
2. 实现延时函数
3. 实现中断相关函数(如使用中断模式)
4. 实现GPIO控制函数(如复位、片选等)

### 步骤4: 应用层移植

**对应的函数可以单独文件上进行实现，可以避免污染SDK**

**SDK内部需要实现的函数都是弱函数（weak）**

1. 实现`port/gh_app_user.c`文件中的内存管理函数
2. 实现日志输出函数
3. 实现时间戳获取函数
4. 根据需要实现其他平台相关函数

### 步骤5: 协议层集成

**对应的函数可以单独文件上进行实现，可以避免污染SDK**

**SDK内部需要实现的函数都是弱函数（weak）**

1. 在主程序中初始化协议:
   ```c
   void gh_protocol_init();
   void gh_protocal_lock(void);    //互斥锁函数
void gh_protocal_unlock(void);  //互斥锁函数
   void gh_protocol_delay();       //延时函数
   void gh_protocol_data_send(void *data, int32_t size); //数据发送函数
   ```
   
2. 实现`port/gh_protocol_user.c`文件中的数据接收处理:
   ```c
   void gh_protocol_data_recevice(uint8_t *p_rx_buffer, uint8_t rx_len);
   ```

### 步骤6: 测试与验证

1. 编译SDK，确保无编译错误
2. 运行基础功能测试，验证硬件接口
3. 测试各项健康监测功能
4. 进行性能和稳定性测试

### 步骤7: 优化与调试

1. 根据测试结果优化配置参数
2. 使用日志功能进行问题调试
3. 根据实际应用场景调整算法参数

## 4. 参考资源

- [Chelsea A 相关Q&A（GH3036/8）](https://alidocs.dingtalk.com/i/nodes/1zknDm0WRaMv5M2wHB4deG1o8BQEx5rG?utm_scene=team_space)
- [Chelsea A 相关资料（GH3036/8）](https://alidocs.dingtalk.com/i/nodes/0eMKjyp81379p7YGuQZ6435vVxAZB1Gv?utm_scene=team_space)

## 5. 常见问题

1. **编译错误**: 检查是否正确实现了所有必需的HAL函数
2. **通信失败**: 验证SPI/I2C配置和硬件连接
3. **算法初始化失败**: 检查配置是否一致
4. **数据不准确**: 调整算法参数和传感器配置

## 6. 技术支持

如需技术支持，请联系汇顶科技技术支持团队或访问官方开发者社区。
