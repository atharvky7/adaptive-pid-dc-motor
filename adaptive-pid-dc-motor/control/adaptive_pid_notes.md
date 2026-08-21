# Adaptive PID Logic

The recovered implementations contain two adaptive approaches.

## ESP32 Gain Scheduling

Nominal gains:

- Kp = 1.0
- Ki = 0.5
- Kd = 0.2

When measured RPM falls below the target:

- Kp = 1.5
- Ki = 0.8
- Kd = 0.3

## MicroPython Error-Based Adaptation

The second prototype changes the gains continuously according to the observed speed error:

- Large error → increase Kp, Ki and Kd.
- Rapidly decreasing error → reduce the gains slightly.
- Gains are constrained to predefined ranges.

## MATLAB/UART Prototype

A separate UART interface receives RPM, torque and temperature and returns updated PID parameters.

These files represent recovered development versions rather than a single finalized implementation.
