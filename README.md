# Blink Template for STM32 Blue Pill (Zephyr RTOS)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Zephyr](https://img.shields.io/badge/Zephyr-4.2+-blue.svg)](https://www.zephyrproject.org/)
[![Platform](https://img.shields.io/badge/Platform-STM32F103C8T6-green.svg)]()

A minimal, production-ready template for Zephyr RTOS on the STM32F103C8T6 "Blue Pill" board. Features one-click building and flashing from VS Code, automatic reset handling, and proper hardware configuration.

✨ **Perfect for beginners** | 🚀 **Ready for production** | 🛠️ **VS Code integrated**

## Prerequisites

Before using this template, make sure you have:

- ✅ Zephyr RTOS installed and configured
- ✅ West build system
- ✅ STM32 toolchain (arm-zephyr-eabi)
- ✅ OpenOCD for flashing
- ✅ ST-Link V2 programmer
- ✅ Visual Studio Code (optional, but recommended)

💡 **New to Zephyr?** Check out our installation guide: [Manual Zephyr RTOS Installation on Windows](https://www.hackster.io/gkiryaziev/manual-zephyr-rtos-installation-on-windows-0cc2a5)

## Quick Start

1. **Clone or download this template**
```bash
   git clone https://github.com/gkiryaziev/Zephyr_STM32F103C8T6.git
   cd Zephyr_STM32F103C8T6
   Activate the virtual environment for west. Instructions on how to create it are described in the Manual Zephyr RTOS Installation on Windows guide.
```

2. **Connect your Blue Pill board**
   - Connect ST-Link to Blue Pill SWD pins (SWDIO, SWCLK, GND, 3.3V)
   - Plug ST-Link into your computer

3. **Build the project**
   - In VS Code: `Terminal → Run Task → Build`
   - Or from command line: `west build -b stm32_min_dev@blue/stm32f103xb`

4. **Flash the board**
   - In VS Code: `Terminal → Run Task → Flash (64k)`
   - Or from command line: `west flash`

5. **Success!** The LED on PC13 should blink every second.

---

## 1. Hardware Configuration

All hardware-specific settings for the board are located in the `bluepill_f103c8.overlay` file.

### LED on PC13

The board's built-in LED is connected to pin **PC13**. In the overlay, it is configured with the following parameters:
- An alias `my-led` is created to reference the LED from the code.
- The `gpioc 13` pin is assigned the `GPIO_ACTIVE_LOW` flag, as the LED on the board lights up when a low voltage level is applied.

```dts
// bluepill_f103c8.overlay

/ {
    aliases {
        my-led = &pc_13;
    };

    leds {
        compatible = "gpio-leds";
        pc_13: pc-13 {
            gpios = <&gpioc 13 GPIO_ACTIVE_LOW>;
        };
    };
};
```

### Disabling JTAG and Using SWD

The SWD (Serial Wire Debug) interface is used for debugging and flashing. To free up the pins that are used by JTAG by default, the following configuration has been added to the overlay:

```dts
// bluepill_f103c8.overlay

&pinctrl {
    // Disables JTAG-DP but keeps SW-DP Enabled.
    // This frees up the following pins: PA15, PB3, PB4.
    swj-cfg = "jtag-disable";
};
```
This allows the pins `PA15`, `PB3`, and `PB4` to be used for other purposes.

## 2. Project Configuration

The main project configuration file is `prj.conf`.

### Flashing without Pressing the Reset Button

The `CONFIG_DEBUG=y` option has been added to `prj.conf`. This enables debugging features in the Zephyr kernel, which allows the programmer (e.g., ST-Link) to automatically reset the microcontroller before flashing. You no longer need to manually press the `RESET` button on the board.

```
# prj.conf
CONFIG_DEBUG=y
```

## 3. VS Code Tasks

The `.vscode/tasks.json` file contains pre-configured tasks for building, flashing, and cleaning the project. You can run them via the `Terminal -> Run Task...` menu.

- **`Build`**: Compiles the project. The `west build` command uses the `prj.conf` and `bluepill_f103c8.overlay` files for the correct build configuration.

- **`Flash (64k)`**: Flashes microcontrollers with **64 KB** of flash memory. This is the standard option for most `STM32F103C8T6` boards.

- **`Flash (128k)`**: Flashes microcontrollers with **128 KB** of flash memory. Some "C8T6" chips are actually "CBT6" chips with more memory. This task uses the `stm32f1x_128k.cfg` configuration file to inform OpenOCD of the correct flash size.

- **`Clean`**: Deletes the `build` directory, clearing out the results of previous builds.

```json
// .vscode/tasks.json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build",
            "type": "shell",
            "command": "west",
            "args": [
                "build",
				// "-p","always",
                "-b", "stm32_min_dev@blue/stm32f103xb",
                "--",
                "-DOVERLAY_CONFIG=prj.conf",
                "-DDTC_OVERLAY_FILE=bluepill_f103c8.overlay"
            ],
            "group": "build",
            "problemMatcher": []
        },
        {
            "label": "Flash (64k)",
            "type": "shell",
            "command": "west flash",
            "group": "build",
            "problemMatcher": []
        },
        {
            "label": "Flash (128k)",
            "type": "shell",
            "command": "west flash --config ./stm32f1x_128k.cfg",
            "group": "build",
            "problemMatcher": []
        },
        {
            "label": "Clean",
            "type": "shell",
            "command": "rm -rf build",
            "group": "build",
            "problemMatcher": []
        }
    ]
}
```

## 4. C/C++ IntelliSense Configuration

The `.vscode/c_cpp_properties.json` file is configured to provide the best possible IntelliSense (code completion, navigation) for the project via the VS Code C/C++ extension.

```json
// .vscode/c_cpp_properties.json
{
    "configurations": [
        {
            "name": "Zephyr",
            "compileCommands": "${workspaceFolder}/build/compile_commands.json"
        }
    ],
    "version": 4
}
```

It works by pointing the C/C++ extension to the `compile_commands.json` file. This file is **generated automatically by Zephyr's build system** when you run the `Build` task.

**Important:** For the code autocompletion to work correctly, you must run the **`Build`** task at least once after cloning the project or after a `Clean` task.

## Troubleshooting

### Problem: "Unable to reset target"

**Cause:** Microcontroller is running and OpenOCD can't halt it.

**Solution:**
1. Press and hold the RESET button on the board
2. Run the flash task
3. Release RESET when flashing starts

Ensure that `CONFIG_DEBUG=y` is set in `prj.conf` (already included in this template).

---

### Problem: "Flash write failed" or "Invalid flash size"

**Cause:** Your chip has 128KB flash, not 64KB.

**Solution:** Use the "Flash (128k)" task instead of "Flash (64k)".

---

### Problem: LED doesn't blink

**Possible causes:**
1. Check wiring (SWDIO, SWCLK, GND, 3.3V)
2. Verify ST-Link drivers are installed
3. Try pressing RESET after flashing
4. Check if LED is on PC13 (some clones use different pins)

---

### Problem: "west: command not found"

**Cause:** Zephyr environment not activated.

**Solution:** Run your `zephyr-env.cmd` script first (Windows) or source the Zephyr environment (Linux/Mac).