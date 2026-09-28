# 🔥 Smart Fire Management System

An embedded fire and gas safety management system built around the **STM32F103C8T6** microcontroller. The system continuously monitors temperature and gas levels, automatically activates a water pump during fire conditions, closes the gas valve when dangerous conditions are detected, and provides password-protected manual controls through a 4×4 keypad and 16×2 LCD.

> ⚠️ **Academic / Prototype Project:** This project is intended for educational and simulation purposes. It is **not a certified fire-safety system** and must not be deployed as a real-world life-safety or industrial protection system without substantial additional engineering, testing, redundancy, and certification.

---

## 📌 Overview

The system combines:

* 🌡️ Temperature monitoring
* 🛢️ Gas concentration monitoring
* 🔥 Automatic fire detection
* 💧 Automatic water-pump activation
* 🔒 Automatic gas-valve closure
* 🔑 Password-protected manual controls
* ⌨️ 4×4 matrix keypad
* 🖥️ 16×2 character LCD
* ⚙️ PWM-based actuator control
* ⏱️ Timer-driven keypad scanning
* 🧠 Priority-based alarm handling

The firmware is implemented using **STM32 HAL** and generated/configured with **STM32CubeMX**.

---

## 🏗️ System Architecture

```text
                    ┌─────────────────────┐
                    │   STM32F103C8T6     │
                    │                     │
 Temperature ──────►│ ADC1 Channel 0      │
 Sensor             │                     │
                    │                     │
 Gas Sensor ───────►│ ADC1 Channel 1      │
                    │                     │
                    │                     │
 Keypad ───────────►│ TIM4 + GPIO         │
                    │                     │
                    │                     │
 LCD ◄──────────────│ GPIO                │
                    │                     │
                    │                     │
 Water Pump ◄───────│ TIM2 CH1 PWM        │
                    │                     │
 Gas Valve ◄────────│ TIM3 CH1 PWM        │
                    └─────────────────────┘
```

### Control flow

```text
Sensors
   │
   ▼
ADC Measurements
   │
   ▼
Threshold Evaluation
   │
   ├─────────────── Fire detected ──────────────► Pump ON
   │                                               │
   │                                               ▼
   │                                         Gas Valve CLOSED
   │
   └─────────────── Gas detected ───────────────► Gas Valve CLOSED
                                                   │
                                                   ▼
                                             Alarm State
```

Fire conditions have priority over gas-only conditions.

---

# ✨ Features

## 🔥 Automatic Fire Detection

The system continuously samples the temperature sensor through the STM32 ADC.

When the measured ADC value reaches the configured fire threshold:

```c
#define TEMP_DANGER 491
```

the system enters the fire alarm state.

During a fire alarm:

* Water pump → **ON**
* Gas valve → **CLOSED**
* Fire alarm state → **ACTIVE**

---

## 🛢️ Gas Detection

The gas sensor is connected to ADC1 channel 1.

The current prototype uses:

```c
#define GAS_DANGER 819
```

When the gas level exceeds the configured threshold:

* Gas alarm → **ACTIVE**
* Gas valve → **CLOSED**

If both fire and gas conditions occur simultaneously, the **fire alarm has priority**.

---

## 💧 Automatic Water Pump Control

The water pump is controlled using PWM through:

```text
PA15 → TIM2_CH1
```

During a fire alarm, the pump is automatically activated.

The current firmware uses a PWM compare value of:

```text
999
```

for the automatic fire response.

---

## 🔒 Gas Valve Control

The gas valve is represented by a servo controlled through:

```text
PA6 → TIM3_CH1
```

The firmware uses different PWM compare values to represent the valve positions.

The valve is automatically closed when:

* Fire is detected
* Gas is detected
* The user manually requests closure

---

# 🔑 Password-Protected Manual Control

Manual actuator control is protected by a password.

The default password is:

```text
1234
```

The user can access manual controls through the keypad.

### Available controls

| Key | Function                 |
| --- | ------------------------ |
| `B` | Gas valve open/close     |
| `C` | Water pump on/off        |
| `D` | Change password          |
| `*` | Show live temperature    |
| `#` | Show live gas percentage |
| `A` | Cancel / Exit            |

Password input is displayed using masked characters:

```text
Password:
****
```

### Important

The current password is stored **only in RAM**.

Therefore:

> Changing the password does not persist after a reset or power cycle.

---

# ⌨️ Keypad

The project uses a standard **4×4 matrix keypad**.

The implemented layout is:

```text
┌─────┬─────┬─────┬─────┐
│  7  │  8  │  9  │  A  │
├─────┼─────┼─────┼─────┤
│  4  │  5  │  6  │  B  │
├─────┼─────┼─────┼─────┤
│  1  │  2  │  3  │  C  │
├─────┼─────┼─────┼─────┤
│  *  │  0  │  #  │  D  │
└─────┴─────┴─────┴─────┘
```

Keypad scanning is handled using **TIM4 interrupts**.

The firmware includes key-release/debounce handling to prevent repeated registration of a single physical key press.

---

# 🖥️ LCD Interface

A **16×2 character LCD** is used as the primary user interface.

The LCD operates in **4-bit mode**.

### LCD connections

| LCD Signal | STM32 Pin |
| ---------- | --------- |
| RS         | PB8       |
| E          | PB9       |
| D4         | PB10      |
| D5         | PB11      |
| D6         | PB12      |
| D7         | PB13      |

The custom LCD driver is implemented in:

```text
Core/Src/lcd.c
Core/Inc/lcd.h
```

---

# 🔌 Hardware Pin Mapping

| Component          | STM32 Pin | Peripheral |
| ------------------ | --------- | ---------- |
| Temperature Sensor | PA0       | ADC1_IN0   |
| Gas Sensor         | PA1       | ADC1_IN1   |
| Gas Valve Servo    | PA6       | TIM3_CH1   |
| Water Pump         | PA15      | TIM2_CH1   |
| Keypad Row 1       | PB0       | GPIO       |
| Keypad Row 2       | PB1       | GPIO       |
| Keypad Row 3       | PB2       | GPIO       |
| Keypad Row 4       | PB3       | GPIO       |
| Keypad Col 1       | PB4       | GPIO       |
| Keypad Col 2       | PB5       | GPIO       |
| Keypad Col 3       | PB6       | GPIO       |
| Keypad Col 4       | PB7       | GPIO       |
| LCD RS             | PB8       | GPIO       |
| LCD E              | PB9       | GPIO       |
| LCD D4             | PB10      | GPIO       |
| LCD D5             | PB11      | GPIO       |
| LCD D6             | PB12      | GPIO       |
| LCD D7             | PB13      | GPIO       |

---

# 🧠 Firmware Architecture

The firmware is divided into several logical components.

```text
Core/
├── Inc/
│   ├── main.h
│   ├── hmi.h
│   ├── lcd.h
│   ├── adc.h
│   ├── gpio.h
│   ├── tim.h
│   ├── usart.h
│   └── stm32f1xx_it.h
│
└── Src/
    ├── main.c
    ├── hmi.c
    ├── lcd.c
    ├── adc.c
    ├── gpio.c
    ├── tim.c
    ├── usart.c
    └── stm32f1xx_it.c
```

### Main modules

#### `main.c`

Responsible for:

* HAL initialization
* Peripheral initialization
* PWM startup
* ADC measurements
* Alarm evaluation
* Actuator updates
* Main control loop

The main application follows a **super-loop architecture** rather than using an RTOS.

---

#### `hmi.c`

Responsible for:

* Menu/state management
* Password handling
* Key processing
* Manual actuator control
* Alarm UI
* Temperature display
* Gas display

The HMI is implemented as a state machine.

Example states include:

```c
SYS_NORMAL
SYS_ALARM_FIRE
SYS_ALARM_GAS
SYS_PASS_GAS_CLOSE
SYS_PASS_GAS_OPEN
SYS_PASS_WATER_ON
SYS_PASS_WATER_OFF
SYS_CHANGE_PASS_OLD
SYS_CHANGE_PASS_NEW
SYS_SHOW_TEMP
SYS_SHOW_GAS
```

---

#### `lcd.c`

Contains the custom 4-bit LCD driver.

It provides functions for:

* LCD initialization
* Sending commands
* Sending characters
* Printing strings
* Cursor positioning
* Clearing the display

---

#### `stm32f1xx_it.c`

Contains interrupt-related functionality.

TIM4 is used to periodically trigger keypad scanning.

---

# ⚙️ Alarm Logic

The system uses a priority-based alarm model.

### Normal state

```text
Temperature < fire threshold
        AND
Gas < gas threshold
        │
        ▼
    NORMAL
```

Outputs:

```text
Water Pump → OFF
Gas Valve  → OPEN / normal position
```

---

### Gas alarm

```text
Gas ≥ GAS_DANGER
        │
        ▼
   GAS ALARM
        │
        ├──► Gas Valve CLOSED
        └──► Pump remains under fire/manual logic
```

---

### Fire alarm

```text
Temperature ≥ TEMP_DANGER
        │
        ▼
   FIRE ALARM
        │
        ├──► Water Pump ON
        └──► Gas Valve CLOSED
```

---

### Fire priority

If both conditions are detected:

```text
Fire + Gas
   │
   ▼
FIRE ALARM
   │
   ├── Pump ON
   └── Gas Valve CLOSED
```

This prevents the gas alarm state from overriding the more critical fire response.

---

# 📊 Sensor Conversion

The current implementation uses ADC-based calculations intended for the **Proteus simulation / project configuration**.

### Temperature

The firmware currently calculates temperature using:

```text
Temperature ≈ ADC × 500 / 4095
```

implemented with integer arithmetic.

### Gas

The displayed gas percentage is calculated approximately as:

```text
Gas % ≈ ADC × 100 / 4095
```

These formulas are **simulation-oriented mappings**, not universal physical calibration equations.

For a real deployment, the sensor characteristics, ADC reference, conditioning circuit, calibration curve, and environmental conditions would need to be considered.

---

# 🧪 Simulation

The project includes a **Proteus simulation**.

Relevant project files include:

```text
New Project.pdsprj
```

The simulation can be used to observe:

* Temperature sensor behavior
* Gas sensor behavior
* LCD output
* Keypad input
* Pump activation
* Servo/gas-valve behavior
* Alarm transitions

---

# 🛠️ Development Environment

| Component         | Technology        |
| ----------------- | ----------------- |
| Microcontroller   | STM32F103C8T6     |
| MCU Family        | STM32F1           |
| Framework         | STM32 HAL         |
| Configuration     | STM32CubeMX       |
| Firmware Language | C                 |
| IDE / Toolchain   | Keil MDK / ARM    |
| Simulation        | Proteus           |
| RTOS              | None              |
| ADC               | ADC1              |
| PWM               | TIM2 / TIM3       |
| Interrupt Timer   | TIM4              |
| Display           | 16×2 LCD          |
| Input             | 4×4 Matrix Keypad |

The project was configured using **STM32Cube Firmware Package F1 v1.8.6**.

---

# 🚀 Getting Started

## 1. Clone the repository

```bash
git clone https://github.com/TheRealMoeid/stm32_Smart_Fire_System_Management.git
cd stm32_Smart_Fire_System_Management
```

## 2. Open the firmware project

Open the project using the appropriate STM32/Keil development environment.

The CubeMX configuration is included:

```text
Smart Fire Management System Project.ioc
```

This allows the configured peripherals and pin assignments to be inspected or regenerated.

## 3. Build the firmware

Compile the project using the configured ARM toolchain.

After a successful build, generate the required firmware image for the target or simulator.

## 4. Run the Proteus simulation

Open:

```text
New Project.pdsprj
```

Configure the simulated MCU with the generated firmware image.

Start the simulation and interact with:

* Temperature sensor
* Gas sensor
* Keypad
* LCD
* Pump
* Gas-valve servo

---

# 🔄 Typical Operating Flow

```text
                 ┌───────────────┐
                 │ System Start  │
                 └───────┬───────┘
                         │
                         ▼
                ┌─────────────────┐
                │ Initialize HAL  │
                │ ADC / Timers    │
                │ LCD / HMI       │
                └────────┬────────┘
                         │
                         ▼
                ┌─────────────────┐
                │ Read Sensors    │
                └────────┬────────┘
                         │
                         ▼
                ┌─────────────────┐
                │ Check Fire      │
                └────────┬────────┘
                         │
                ┌────────┴────────┐
                │                 │
               YES                NO
                │                 │
                ▼                 ▼
        ┌───────────────┐  ┌───────────────┐
        │ Fire Alarm    │  │ Check Gas     │
        │ Pump ON       │  └───────┬───────┘
        │ Valve CLOSED  │          │
        └───────────────┘    ┌─────┴─────┐
                             │           │
                            YES          NO
                             │           │
                             ▼           ▼
                       ┌───────────┐ ┌─────────┐
                       │Gas Alarm  │ │ Normal  │
                       │Valve CLOSE│ │ State   │
                       └───────────┘ └─────────┘
```

---

# 📁 Repository Structure

```text
.
├── Core/
│   ├── Inc/
│   │   ├── adc.h
│   │   ├── gpio.h
│   │   ├── hmi.h
│   │   ├── lcd.h
│   │   ├── main.h
│   │   ├── stm32f1xx_hal_conf.h
│   │   ├── stm32f1xx_it.h
│   │   ├── tim.h
│   │   └── usart.h
│   │
│   └── Src/
│       ├── adc.c
│       ├── gpio.c
│       ├── hmi.c
│       ├── lcd.c
│       ├── main.c
│       ├── stm32f1xx_it.c
│       ├── syscalls.c
│       ├── sysmem.c
│       ├── tim.c
│       └── usart.c
│
├── Drivers/
│   └── STM32F1xx_HAL_Driver/
│
├── MDK-ARM/
│
├── Smart Fire Management System Project.ioc
├── New Project.pdsprj
├── README.md
└── ...
```

---

# ⚠️ Current Limitations

This project is an academic prototype and has several limitations that should be understood before using or extending it.

### 1. No persistent password storage

The password exists in RAM and is lost after reset.

A production implementation should use appropriate non-volatile storage such as:

* Internal Flash
* EEPROM
* External non-volatile memory

---

### 2. Simulation-oriented sensor calibration

The current temperature and gas calculations are designed around the project simulation.

Real sensors require:

* Datasheet-based conversion
* Calibration
* ADC reference characterization
* Signal conditioning
* Noise filtering

---

### 3. No redundant sensing

A safety-critical system should not depend on a single sensor.

A real system could use:

* Multiple temperature sensors
* Multiple gas sensors
* Sensor plausibility checks
* Cross-validation

---

### 4. No hardware fault-detection architecture

The current firmware does not provide comprehensive detection of:

* Sensor disconnection
* ADC failure
* Actuator failure
* Stuck valve
* Pump failure
* Power failure

---

### 5. No watchdog-based recovery

A production safety controller should normally include appropriate watchdog and fault-recovery mechanisms.

---

### 6. Actuator driver circuitry is required

The STM32 GPIO/timer outputs should **not directly drive high-current pumps or unsuitable loads**.

A real implementation would require appropriate hardware such as:

* MOSFET/transistor drivers
* Relays where appropriate
* Flyback protection
* Separate power supplies
* Proper grounding
* Current protection

The servo also requires a suitable power supply and electrical interface.

---

### 7. Not a certified safety system

This project does not implement the hardware, software assurance, redundancy, diagnostics, validation, certification, or fail-safe engineering required for an actual fire protection system.

---

# 🔮 Future Improvements

Possible future development directions include:

* [ ] Persistent password storage
* [ ] EEPROM/Flash configuration storage
* [ ] Watchdog integration
* [ ] Sensor fault detection
* [ ] Sensor filtering and debouncing
* [ ] Multiple temperature/gas sensors
* [ ] More robust alarm state machine
* [ ] Event logging
* [ ] RTC-based event timestamps
* [ ] Buzzer/siren alarm
* [ ] Status LEDs
* [ ] Emergency override
* [ ] Manual reset mechanism
* [ ] Communication interface
* [ ] UART debugging
* [ ] GSM/Wi-Fi notification
* [ ] Remote monitoring
* [ ] Hardware fault monitoring
* [ ] Improved fail-safe actuator design
* [ ] PCB implementation
* [ ] Real hardware validation

---

# 📚 Project Documentation

Additional project documentation is included in the repository, including:

* Final project report
* Project presentation
* STM32CubeMX configuration
* Proteus simulation
* Source code

These materials document the system design, implementation, simulation, and project methodology.

---

# 🎓 Academic Context

This project was developed as an **Embedded Systems / Real-Time Systems academic project**.

The primary objectives were to demonstrate practical implementation of:

* STM32 microcontroller programming
* ADC sensor acquisition
* GPIO control
* PWM actuator control
* Timer interrupts
* Matrix keypad scanning
* LCD interfacing
* Embedded state machines
* Alarm prioritization
* Hardware simulation with Proteus

---

# 🧑‍💻 Development Notes

The firmware intentionally uses a relatively simple architecture:

```text
HAL Initialization
       │
       ▼
Peripheral Initialization
       │
       ▼
Timer / PWM / HMI Startup
       │
       ▼
      Loop
       │
       ├── Read Temperature
       ├── Read Gas
       ├── Evaluate Alarms
       ├── Update Outputs
       ├── Update Display
       └── Repeat
```

This keeps the implementation understandable and appropriate for an academic embedded-systems project.

The current version does **not** use FreeRTOS or another RTOS.

---

# 📜 License

This project does not currently specify a formal open-source license.

Unless a license is added to the repository, the default copyright rules apply and others should not assume they have permission to redistribute or modify the project.

---

# 👤 Author

**Moeid Ghiady** & **Navid Rostami**

---

# ⚠️ Disclaimer

This project is provided for **educational, experimental, and simulation purposes**.

It must not be used as the sole protection mechanism for:

* Buildings
* Industrial facilities
* Gas systems
* Human life
* Property
* Critical infrastructure

A real fire/gas safety system requires professional electrical, mechanical, embedded, safety, and regulatory engineering, together with appropriate testing and certification.

---

## ⭐ Project Summary

**Smart Fire Management System** is an STM32-based embedded safety prototype that demonstrates how sensor acquisition, alarm logic, PWM actuator control, keypad input, LCD interaction, and password-protected manual control can be combined into a small embedded control system.

```text
STM32F103C8T6
      │
      ├── 🌡️ Temperature Sensor
      ├── 🛢️ Gas Sensor
      ├── ⌨️ 4×4 Keypad
      ├── 🖥️ 16×2 LCD
      ├── 💧 Water Pump
      └── 🔒 Gas Valve
```

**Detect → Decide → Act → Display**

---
