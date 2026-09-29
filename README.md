# AVR-Clock-Calibration for ATmega328P

A bare-metal **clock calibration tool** written in C for the ATmega328P microcontroller. Designed to measure the **real frequency** of the internal RC oscillator against an external reference signal, and to tune it in yours projects.


## Table of Contents

- [Overview](#overview)
- [Project Structure](#project-structure)
- [Prerequisites & Development Environment](#prerequisites--development-environment)
  - [Fuse Configuration (Clock Prescaler)](#fuse-configuration-clock-prescaler)
- [Wiring](#wiring)
- [Quick Start](#quick-start)
- [Configuration](#configuration)



## Overview

The internal RC oscillator of an ATmega328P can be off by up to ±10 % from its nominal frequency, which causes UART errors and timing drift. This project measures the real CPU clock with input capture against a precise external reference, then finds the best `OSCCAL` value with a binary search to lock the oscillator on the target frequency (8 MHz for the atmega328P).

In my setup, the reference is an **external 1 kHz clock**, but the reference frequency is **configurable** (see [Configuration](#configuration)).

Once calibrated, the real frequency is simply:

```text
Real F_CPU = measured ticks × reference frequency
```

| Feature | Description |
| :--- | :--- |
| **Tick measurement** | input capture on PB0 (ICP1), averaged over 10 periods to filter jitter |
| **OSCCAL search** | Binary search on the 8-bit register (7 to 8 iterations instead of 256) |


## Project Structure

```text
.
├── .devcontainer
│   ├── devcontainer.json
│   └── Dockerfile
├── HAL
│   ├── clock
│   │   ├── clock.c
│   │   └── include
│   │       └── clock.h
│   ├── gpio
│   │   ├── gpio.c
│   │   └── include
│   │       └── gpio.h
│   └── uart
│       ├── include
│       │   └── uart.h
│       └── uart.c
├── main
│   └── main.c
├── Makefile
└── README.md

```

I use my own HAL for UART and GPIO. More information [here](https://github.com/Matteo-o-o/AVR-HAL-for-ATmega328P).

## Prerequisites & Development Environment

### DevContainer (Recommended)

This repository includes a pre-configured **VS Code DevContainer** with the entire AVR GNU toolchain pre-installed (`avr-gcc`, `avr-libc`, `avrdude`, `make`). Opening this workspace in VS Code with the *Dev Containers* extension gives you an instant, zero-setup build environment.

### Toolchain & Hardware Needed

| Requirement | Details |
| :--- | :--- |
| **Compiler** | `avr-gcc` |
| **Flashing Software** | `avrdude` |
| **Programmer** | USBasp |
| **Target MCU** | ATmega328P |
| **Reference Signal** | External clock, **1 kHz in my setup** (configurable), from a function generator, crystal-based oscillator... |
| **Telemetry** | USB-to-TTL serial adapter |

### Fuse Configuration (Clock Prescaler)

By default, a factory-fresh ATmega328P has the **CKDIV8** fuse programmed, which divides the internal 8 MHz RC oscillator by 8, giving an effective `F_CPU` of **1 MHz**. This project is built for the full oscillator speed (`F_CPU = 8000000UL`), if CKDIV8 is left enabled, the calibration target will never be reached and every timer tick and UART baud rate will be **off by a factor of 8**.

Before flashing for the first time, check the current fuse values:

```bash
make fuses-read
```

If `lfuse` reads `0x62` (factory default), CKDIV8 is active. Disable it while keeping the internal 8 MHz oscillator as the clock source:

```bash
make fuses-8mhz
```

## Wiring

| Signal | ATmega328P Pin | Connects to |
| :--- | :--- | :--- |
| **PB0 (ICP1)** | 14 | Reference clock output (1 kHz in my setup) |
| **PD1 (TXD)** | 3 | USB-TTL **RX** |
| **GND** | 8, 22 | Common ground |

> **Note:** all devices (MCU, reference clock, USB-TTL adapter) must share the same ground

## Quick Start

### 1. Build the Firmware

Compile the project:

```bash
make
```

### 2. Flash to ATmega328P

Upload the generated `.hex` binary via USBasp ([help for wiring the usbasp ](https://github.com/Matteo-o-o/AVR-HAL-for-ATmega328P)):

```bash
make flash
```

### 3. Read the Result

Open a serial monitor on the USB-to-TTL adapter. The calibration runs at every boot:



### 4. Clean Build Artifacts

```bash
make clean
```

## Configuration

In my setup, the external reference clock is **1 kHz**, but this is not fixed: set the frequency of your own reference signal in the `Makefile`.

| Variable | Default | Description |
| :--- | :--- | :--- |
| `F_CPU` | `8000000UL` | Target CPU frequency (Hz) |
| `F_CLOCK_REF` | `1000UL` | Frequency of the external reference signal (Hz), **change it to match your clock** |

The number of ticks the firmware expects per reference period is computed automatically:

```text
TARGET_TICKS = F_CPU / F_CLOCK_REF
In my case 8 000 000 / 1 000 = 8000 ticks (1 kHz reference)
```
Timer 1 is 16-bit, so `TARGET_TICKS` must stay below 65 536. At 8 MHz, the reference frequency must therefore be **at least ~123 Hz**.