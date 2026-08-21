# Hardware

| Component | Role |
|---|---|
| ESP32 / ESP32-S3 | Main controller |
| DC motor (~120 RPM) | Plant under control |
| Rotary encoder | Speed feedback |
| MPU6050 | Rotational motion / IMU sensing |
| Motor driver | PWM motor actuation |
| External supply | Motor power |

The project used a small DC motor and sensor feedback to study speed regulation under changing operating conditions.

The MPU6050 was included in the experimental setup. The complete original IMU data-processing code was not recovered in the available source files.
