# CLAUDE.md

## 强制规则：所有回答必须使用中文

**所有 AI 回答必须使用中文（简体中文）。** 这是强制规则，适用于与本仓库相关的所有对话。
- 代码中的注释和文档也尽量使用中文
- 变量名、函数名、类型名等标识符保持英文不变
- 回复用户时，直接以中文回答，不需要输出英文再翻译

---

本文件为 Claude Code (claude.ai/code) 在此仓库中工作时提供指导。

## 项目概览

**ESP32-C3 (RISC-V) 宠物项圈固件**，搭载 **Goodix GH3036** PPG（光电容积描记）传感器。实现心率 (HR)、血氧 (SpO2)、心率变异性 (HRV) 及运动/佩戴检测 (ADT/NADT)。

- **MCU**: ESP32-C3（单核 RISC-V，32位）
- **传感器**: Goodix GH3036（又名 Chelsea A）— SPI 接口 @ 2MHz，软件 CS
- **SDK**: Goodix GH3036 SDK — 4层架构（HAL → 算法 → 应用 → 协议），预编译 RISC-V `.a` 算法库
- **框架**: ESP-IDF v5.5.3（CMake + Ninja 构建系统）

## 编译与烧录命令

通过 ESP-IDF 的 `idf.py` 工具执行：

```powershell
# 编译项目（在项目根目录）
idf.py build

# 完整流程：编译 + 烧录 + 串口监视器
idf.py -p COM14 flash monitor

# 仅烧录（跳过编译）
idf.py -p COM14 flash

# 仅监视器（打开串口，不烧录）
idf.py -p COM14 monitor

# 清理编译产物
idf.py fullclean

# 打开 Kconfig 菜单配置
idf.py menuconfig

# 设置目标芯片（已配好 esp32c3）
idf.py set-target esp32c3
```

**串口**: `COM14`（配置在 `.vscode/settings.json` 中）  
**ESP-IDF 路径**: `C:\esp\v5.5.3\esp-idf`（通过 `IDF_PATH` 环境变量设置）  
**ESP-IDF 工具链**: `e:\ESP-IDF\tools\esp`

> 注意：`sdkconfig` 已被 `.gitignore` 忽略 — 执行 `idf.py clean` 或 `idf.py reconfigure` 后需要重新配置。

## 关键 Kconfig 选项（来自 `sdkconfig`）

通过 `idf.py menuconfig` → "GH3036 SDK Configuration" 设置：

| 配置项 | 值 | 含义 |
|---|---|---|
| `CONFIG_GH_CHIP_TYPE` | 3 | GH3036 传感器 |
| `CONFIG_GH_INTERFACE_TYPE` | 1 | SPI 软件 CS |
| `CONFIG_GH_ISR_MODE` | 0 | 中断模式（非轮询） |
| `CONFIG_GH_USE_SDK_APP` | y | 使用 SDK 应用层 |
| `CONFIG_GH_ALGO_LEVEL` | 4 | EXCLUSIVE 级别算法 |
| `CONFIG_GH_ALGO_EN` | y | 启用算法 |
| `CONFIG_GH_PROTOCOL_EN` | y | 启用 RPC 协议层 |
| `CONFIG_GH_FUSION_MODE_SEL` | 1 | 同步融合模式 |
| `CONFIG_GH_MTSS_EN` | y | 多任务状态管理 |
| `CONFIG_GH_LOG_DEBUG_ENABLE` | y | 调试日志 |
| `GH_FUNC_SLOT_EN` | 全开 | 所有功能开启（HR/SpO2/HRV/ADT/NADT） |

算法库前缀：`riscv32_gcc-14.2.0-esp_os+soft_`（适配 ESP-IDF 的 RISC-V GCC 预编译库）。

## 硬件引脚映射

| 信号 | GPIO | 说明 |
|---|---|---|
| SPI SCLK | GPIO 6 | 2 MHz，模式 0 |
| SPI MOSI | GPIO 7 | |
| SPI MISO | GPIO 2 | |
| SPI CS | GPIO 10 | 软件控制 |
| INT | GPIO 4 | 上拉输入，上升沿触发 |
| RESET | GPIO 5 | 推挽输出 |

## 架构：SDK 4层 + 移植层

```
┌──────────────────────────────────────────────────────────┐
│  main/ppg_main.c — app_main() 入口                        │
│  5ms 轮询循环: gh_app_demo_int_process()                  │
├──────────────────────────────────────────────────────────┤
│  main/user/src/ — 移植层（弱函数覆盖）                     │
│  gh_demo_api.c       初始化 / 启动 / 停止 / 配置切换       │
│  gh_hal_user.c       SPI、GPIO、定时器、ISR、日志实现       │
│  gh_app_user.c       数据发布、事件处理、运动定时器         │
│  gh_protocol_user.c  RPC 锁/发送（send 为空占位）          │
│  gh_demo_reg_lists.c 传感器寄存器初始化映射                │
├──────────────────────────────────────────────────────────┤
│  components/gh3036_sdk/ — Goodix SDK（请勿修改）           │
│  gh_hal/         硬件接口 + 服务层（SPI/I2C，FIFO）       │
│  gh_algo/        预编译 .a 库：HR、SpO2、HRV、ADT、NADT  │
│                  （本项目使用 EXCLUSIVE 级别）             │
│  gh_app/         数据融合、算法适配器、MTSS 状态管理       │
│  gh_protocol/    RPC 协议框架                             │
└──────────────────────────────────────────────────────────┘
```

### 重要原则：不要修改 SDK 文件

根据 SDK 文档要求：**切勿修改** `gh_algo/`、`gh_app/`、`gh_hal/`、`gh_protocol/` 中的任何文件。这些是供应商提供的核心文件，需保持原始状态以便后续更新。

正确的做法：在 `main/user/src/` 中实现弱函数覆盖。SDK 提供了 `components/gh3036_sdk/port/` 模板目录，说明需要复制和自定义哪些文件。

### 算法库

预编译的 RISC-V `.a` 文件位于 `components/gh3036_sdk/gh_algo/common/`：
- `gh3036drv.a`（驱动 HAL，2KB）
- `common_dsp.a`（1.1MB），`common_dl.a`（41-64KB）
- `hr_*.a`（81-145KB，依级别而定）
- `spo2_*.a`（84-127KB）
- `hrv.a`（67KB），`nadt.a`（88KB）

同时包含了 ARM Cortex-M4 的 `.lib` 文件，但本项目使用 RISC-V 架构，用不到这些。

### 主循环

在 [`main/ppg_main.c:24-34`](main/ppg_main.c#L24-L34) 中：

```c
void app_main(void) {
    gh_app_demo_init();          // SDK 初始化 → HAL → 协议 → 配置下载
    gh_app_demo_start(GH_FUNC_SLOT_EN);  // 启用所有功能
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5));    // 5 ms 间隔
        gh_app_demo_int_process();       // 轮询 ISR → FIFO 读取 → 算法处理 → 数据发布
    }
}
```

主循环每 5ms 运行一次。`gh_hal_isr()` 处理中断事件和 FIFO 数据，然后通过 `gh_app_user.c` 中的回调函数发布结果。

### 协议层（开发中）

`gh_protocol` 提供了基于 RPC 的通信框架，用于 BLE/UART/Wi-Fi 数据传输。  
**当前状态**：[`gh_protocol_user.c`](main/user/src/gh_protocol_user.c) 中的 `gh_protocol_data_send()` 是一个空实现的占位符，带有 `TODO` 注释 — 外部传输通道尚未接入。

## 移植层 — 用户实现的弱函数

SDK 使用 **弱符号（weak symbol）** 机制。[`main/user/src/`](main/user/src/) 中的实现会在链接时覆盖 SDK 默认实现。

### HAL 层函数（`gh_hal_user.h`）— [`main/user/src/gh_hal_user.c`](main/user/src/gh_hal_user.c)：
- `gh_hal_spi_init/write/read/cs_ctrl` — SPI 总线配置和数据传输
- `gh_hal_delay_us/ms` — `esp_rom_delay_us()` / `vTaskDelay()`
- `gh_hal_get_timestamp` — `esp_timer_get_time() / 1000`
- `gh_hal_int_pin_init / reset_pin_init / reset_pin_ctrl` — GPIO 配置
- `gh_hal_log_user` — `ESP_LOGI` 封装

### 应用层函数（`gh_app_user.h`）— [`main/user/src/gh_app_user.c`](main/user/src/gh_app_user.c)：
- `gh_demo_int_process` — ISR 轮询入口
- `gh_demo_config_ctrl` — 低功耗模式 + 配置下载
- `gh_demo_sample_ctrl` — 启动/停止采样 + 通道使能
- `gh_demo_data_publish` — 协议处理 + 数据日志
- `gh_demo_action_event_publish` — 佩戴/摘下事件和运动事件
- `gh_move_det_timer_start/stop` — FreeRTOS 定时器

### 寄存器配置 — [`main/user/src/gh_demo_reg_lists.c`](main/user/src/gh_demo_reg_lists.c)：
- `g_reg_list0` — 活动配置（约 36 个寄存器写入）
- `g_reg_list1/2` — 空配置占位（预留给未来使用）

## SDK 配置文件

位于 `components/gh3036_sdk/` 中（项目搭建时复制）：
- **全局配置**: `gh_app/inc/config/gh_global_config.h` — 芯片类型、功能开关、算法适配器
- **HAL 配置**: `gh_hal/hw_service/inc/config/gh_hal_config.h` — 接口类型、中断/轮询模式、FIFO 设置
- **自动生成**: `gh_auto_config/gh_auto_config_kconfig.h` — 从 Kconfig 选择派生

## 开发注意事项

- **没有测试基础设施** — 无单元测试、无 CI/CD、无测试脚本。
- **顶层的 `README.md` 已过时** — 描述的是 ESP-IDF 模板中的 SPI EEPROM 示例，与本项目无关。
- **SDK 移植文档** 在 `components/gh3036_sdk/README.md`（中文）。
- **供应商数据手册** 在 `docs/Chelsea_A(GH3036)/`（已被 gitignore）。
- **`sdkconfig` 和 `sdkconfig.old` 被 gitignore** — 它们是 `idf.py menuconfig` 生成的构建产物。
- **clangd** 配置在 `.clangd` 和 `.vscode/settings.json` 中，指向 `e:\ESP-IDF\tools\esp\tools\esp-clang\...`。
- **`gh_protocol_data_send` 是空函数** — 外部通信的数据管道尚未连接。
