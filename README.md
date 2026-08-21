# Adaptive Self-Tuning PID Controller for DC Motor

An embedded closed-loop DC motor speed-control project developed around an ESP32, encoder feedback, PID control, adaptive gain adjustment, and a Wi-Fi dashboard for real-time monitoring.

## Project Motivation

Manual PID tuning can become difficult when the operating condition of a motor changes, for example when a load is applied and the motor speed drops.

The project explores an adaptive approach in which the PID gains are adjusted during operation based on the observed motor response.

## System Overview

```text
                 ┌──────────────────────┐
                 │    Web Dashboard     │
                 │ Target RPM / PID     │
                 └──────────┬───────────┘
                            │ Wi-Fi
                            ↓
┌──────────┐        ┌─────────────────────┐
│ Encoder  │───────→│        ESP32        │
└──────────┘        │                     │
                    │ RPM estimation      │
┌──────────┐        │ PID controller      │
│ MPU6050  │───────→│ Adaptive gain logic │
└──────────┘        └──────────┬──────────┘
                               │ PWM
                               ↓
                         Motor Driver
                               │
                               ↓
                           DC Motor
                               │
                               └──── feedback ───→ Encoder
```

## Hardware

- ESP32 / ESP32-S3
- Small DC motor (~120 RPM)
- Rotary encoder
- MPU6050 IMU
- Motor driver
- External motor supply

The MPU6050 was part of the experimental setup. The recovered ESP32 firmware initializes the sensor, while the complete original IMU-processing portion was not recovered.

## Main Features

- Closed-loop DC motor speed control
- Encoder-based RPM measurement
- PID controller with Kp, Ki and Kd
- Online adaptive PID gain adjustment
- PWM motor actuation
- Target RPM control
- Wi-Fi connectivity
- Real-time web dashboard
- Live RPM and PID parameter monitoring

## Adaptive PID

The recovered ESP32 implementation uses gain scheduling.

Nominal gains:

```text
Kp = 1.0
Ki = 0.5
Kd = 0.2
```

When the measured RPM falls below the target:

```text
Kp = 1.5
Ki = 0.8
Kd = 0.3
```

The idea is to make the controller more aggressive when the motor experiences a speed drop.

A separate MicroPython prototype in this repository explores continuous gain adjustment based on speed error and error change.

## Software

- Arduino / ESP32
- C++
- MicroPython
- PID control
- Wi-Fi
- ESPAsyncWebServer
- MPU6050 library

## Repository Structure

```text
adaptive-pid-dc-motor/
│
├── firmware/
│   ├── esp32/
│   │   └── esp32_adaptive_pid.ino
│   └── micropython/
│       ├── adaptive_pid.py
│       └── uart_pid_interface.py
│
├── control/
│   └── adaptive_pid_notes.md
│
├── hardware/
│   └── hardware.md
│
├── dashboard/
│   └── dashboard.md
│
├── docs/
│   └── system_architecture.md
│
└── images/
```

## Development Versions

### ESP32 controller

The ESP32 implementation combines encoder feedback, PID speed control, adaptive gain scheduling, PWM motor actuation, and a Wi-Fi dashboard.

### MicroPython adaptive PID prototype

The MicroPython implementation demonstrates:

- encoder pulse counting
- RPM estimation
- PID computation
- PWM motor control
- adaptive changes to Kp, Ki and Kd

### UART / MATLAB interface

The UART prototype exchanges RPM, torque and temperature values with MATLAB and sends updated PID parameters back over serial communication.

## Dashboard

The ESP32 web interface provides:

- Target RPM input
- Motor start/stop controls
- Current RPM
- Kp
- Ki
- Kd

The dashboard periodically refreshes the displayed RPM and PID parameters.

## Experimental Concept

The intended operating sequence was:

1. Start the motor near its nominal speed.
2. Apply a physical load.
3. Detect the resulting speed change through feedback.
4. Adjust the controller response.
5. Restore the motor toward the desired RPM.
6. Observe the controller behaviour through the web dashboard.

## Results

The recovered project material establishes a working closed-loop motor-control prototype with adaptive PID gain adjustment and real-time monitoring.

Original experimental plots and measurement logs were not recovered, so quantitative performance claims are intentionally not included.

## Future Work

- Integrate a regression model for direct PID-gain prediction.
- Incorporate complete MPU6050 feature extraction into the control loop.
- Add automatic system identification.
- Compare fixed PID, adaptive PID and ML-based tuning experimentally.
- Add acceleration/deceleration profiles.
- Add systematic performance metrics such as settling time, overshoot and steady-state error.

## Note

This repository reconstructs the project from surviving source code and project material. The files under `firmware/` preserve the recovered implementations; they should not be interpreted as a single final firmware version.
