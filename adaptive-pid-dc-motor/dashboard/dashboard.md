# Real-Time Web Dashboard

The ESP32 firmware hosts a lightweight HTTP dashboard over Wi-Fi.

## Controls

- Set target RPM
- Start motor
- Stop motor

## Monitored Parameters

- Current RPM
- Target RPM
- Kp
- Ki
- Kd

The browser periodically requests updated RPM and PID values from the ESP32.

This allowed the tuning behaviour to be observed during motor operation.
