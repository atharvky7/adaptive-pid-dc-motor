# System Architecture

```text
                    Wi-Fi
                      |
                      v
              +---------------+
              | Web Dashboard  |
              | Target RPM     |
              | RPM / Kp Ki Kd  |
              +-------+-------+
                      |
                      v
+---------+     +-------------+     +-------------+     +---------+
| Encoder | --> | RPM / Error | --> | Adaptive    | --> |  PWM    |
+---------+     | Calculation |     | PID         |     +----+----+
                +-------------+     +-------------+          |
                                                            v
                                                       +---------+
                                                       |  Motor  |
                                                       +----+----+
                                                            |
                                                            +---- feedback

                 MPU6050
                    |
                    v
              Rotational-motion
                 sensing
```

The recovered ESP32 firmware implements the encoder → PID → PWM loop and adaptive gain scheduling.

The recovered source initializes the MPU6050, while the complete final IMU processing logic is not available.
