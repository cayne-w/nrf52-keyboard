### 前置准备

- **工具链**: 已经能在这个工程里正常用 `make`、`nrfjprog`（或 `pyocd`）烧录。
- **芯片型号**: `keyboard/ikbc-cayne87/Makefile` 已改为 `NRF_CHIP := nrf52832`。
- **SoftDevice**: 默认用 `S112`（如果你改成 `S132`，下面命令不变，只要保证 app/bootloader 一致即可）。

#### Docker环境编译（可选）

```bash
cd xxx/nrf52-keyboard

docker run --rm -it -v $PWD:/work lotlab/nrf52-keyboard
```

下面所有命令都在目录中执行：

```bash
cd xxx/nrf52-keyboard/keyboard/ikbc-cayne87
```

---

### 步骤一：编译主程序（键盘固件）

1. 进入键盘目录：

```bash
cd /Users/kayne/Workspace/mcu/nrf52-keyboard/keyboard/ikbc-cayne87
```

2. 编译主程序（和 CH554 固件）：

```bash
make
```

完成后，主程序 HEX 在：

- `_build/nrf52_kbd.hex`

---

### 步骤二：生成 DFU 配置和签名固件（用于 BLE DFU）

1. 生成 DFU setting（`nrf52_settings.hex`）：

```bash
make setting
```

2. 生成 DFU 包（用于以后手机/PC 通过 BLE 升级）：

```bash
make package
```

会在 `_build/` 下生成类似：

- `nrf52_kbd_XXXX.zip`（BLE DFU 升级包）
- 同时 `setting` 步骤会生成 `_build/nrf52_settings.hex`

3. 生成 **签名应用 + SoftDevice** 的合并文件：

```bash
make merge_all
```

这一步会生成：

- `_build/nrf52_kbd_sign_with_sd.hex`

---

### 步骤三：编译 Bootloader

1. 仍在 `ikbc-cayne87` 目录下，执行：

```bash
make bootloader
```

这会调用 `application/bootloader/project/Makefile`，生成：

- `_build/nrf52_bootloader.hex`
  这是 Nordic 的 **secure_ble bootloader**，支持 BLE DFU。

---

### 步骤四：合并 Bootloader + 应用 + SoftDevice（一次性全镜像）

1. 在 `ikbc-cayne87` 目录执行：

```bash
make merge_bootloader_all
```

`application/main/project/bl.mk` 里定义了该目标，会生成：

- `_build/nrf52_all.hex`

内部包含：

- SoftDevice
- Bootloader（BLE DFU）
- 带 DFU setting 的签名应用固件

---

### 步骤五：第一次烧录整机固件

1. 擦除整片（可选但推荐）：

```bash
nrfjprog -f nrf52 --eraseall
```

2. 烧录合并好的全镜像：

```bash
nrfjprog -f nrf52 --program _build/nrf52_all.hex --sectorerase
nrfjprog -f nrf52 --reset
```

到这里：

- 设备上已经有：SoftDevice + BLE DFU Bootloader + 主程序。
- 以后升级主程序，可以直接用：
  - 手机 `nRF Connect` / PC `nrfutil`，加载 `_build/nrf52_kbd_XXXX.zip`，通过 BLE DFU 升级；
  - 或在开发调试期继续用 JLink 直接刷 `_build/nrf52_kbd.hex`（此时只覆盖应用区）。

---

### 日常调试时的简化流程

- 只改应用代码、不动 bootloader 时：

```bash
cd /Users/kayne/Workspace/mcu/nrf52-keyboard/keyboard/ikbc-cayne87
make
nrfjprog -f nrf52 --program _build/nrf52_kbd.hex --sectorerase
nrfjprog -f nrf52 --reset
```

如果你想严格按现有 Makefile：

- 直接用：

```bash
make flash         # 在 app.mk 中，烧录 _build/nrf52_kbd.hex
make flash_setting # 如需重新刷 DFU setting
make flash_bootloader  # 如需单独重刷 bootloader
```

[15 tools called]

### 简单结论

在你现在这套 `ikbc-cayne87` 配置里：
- **没有**把“进入 bootloader”的功能绑到任何按键；
- **默认进入 bootloader 的方式是通过 BLE 的 Buttonless DFU 服务，由手机/PC 的 DFU 工具触发**，不需要（也无法）靠键盘组合键直接进。

---

### 常用方式：用 nRF Connect 手机 App 进入 bootloader 并升级

1. **确保设备已烧好全镜像并正常上电**
   已按你前面步骤烧了 `_build/nrf52_all.hex`，键盘能正常作为 BLE 键盘工作。

2. **用手机安装并打开 Nordic 的 nRF Connect（BLE 版）**
   - iOS / Android 都有 “nRF Connect for Mobile”。

3. **连接键盘设备**
   - 在 App 中扫描，找到你的设备名（例如 `IKBC_Cayne87`，名称来自 `config.h` 的 `PRODUCT`）。
   - 点击连接。

4. **发起 DFU 升级**（App 会自动让设备跳转到 bootloader）
   - 在 nRF Connect 里，选菜单里的 “DFU” 功能；
   - 选择你之前 `make package` 生成的 `_build/nrf52_kbd_XXXX.zip`；
   - App 会：
     - 先给应用的 Buttonless DFU 特性写入“进入 DFU”的命令；
     - 应用收到事件后，按 `ble_dfu_evt_handler` 的逻辑断开、设置寄存器并关机：

```424:444:application/main/src/ble/ble_services.c
static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event) {
    case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE: {
        ble_disconnect(NULL);
        break;
    }
    ...
    }
}
...
#ifdef BUTTONLESS_DFU
static void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void* p_context)
{
    if (state == NRF_SDH_EVT_STATE_DISABLED) {
        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }
}
#endif
```

   - 下次上电时，MCU 直接进入 Nordic secure bootloader，开始广播 DFU 名称（通常是 `DfuTarg` 或配置的名字），nRF Connect 会自动切到 bootloader 并完成升级。

整个过程对你来说就是：**在 nRF Connect 里点 DFU → 选 zip 包 → 等待完成**，无需额外按键动作。

---

### 如果你想通过键盘按键进入 bootloader（可选）

当前 `ikbc-cayne87` 的 `keymap_plain.c` 里 **没有**绑定 `CONTROL_BOOTLOADER`，只是定义了 Fn 层：

```54:57:keyboard/ikbc-cayne87/keymap_plain.c
const action_t fn_actions[] = {
    /* Poker Layout */
    ACTION_LAYER_MOMENTARY(2),  // FN0
    ACTION_LAYER_TOGGLE(1),     // FN1
};
```

而真正执行“进 bootloader”的路径在 `keyboard_fn.c`：

```51:67:application/main/src/keyboard/keyboard_fn.c
__attribute__((weak)) void action_function(keyrecord_t* record, uint8_t id, uint8_t opt)
{
    if (record->event.pressed) {
        switch (id) {
        case KEYBOARD_CONTROL:
            ...
            case CONTROL_BOOTLOADER: // 跳转到bootloader
                bootloader_jump();
                break;
```

如果你以后想**加一个按键组合来进 DFU**，可以仿照 `ergoconn` 的写法，在自己的 `fn_actions` 里加一条：

```c
ACTION_FUNCTION_OPT(KEYBOARD_CONTROL, CONTROL_BOOTLOADER)
```

再把它放到某个物理键上即可；但这不是必须，当前通过 **nRF Connect 的 DFU** 方式已经可以正常让键盘进入 bootloader 并升级。
